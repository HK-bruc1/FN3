/*
 ***************************************************************************
 *							AUDIO AEC DEMO
 * File  : audio_aec_demo.c
 * By    : GZR
 * Notes : 1.可用内存
 *		   (1)free_ram_overlay:	静态复用内存(FREE_RAM_OVERLAY_SIZE)
 *		   (2)free_ram:	动态内存(mem_stats:physics memory size xxxx bytes)
 *		   2.demo默认将输入数据copy到输出，相关处理只需在运算函数
 *		    audio_aec_run()实现即可
 *		   3.双mic ENC开发，只需打开对应板级以下配置即可：
 *			#define TCFG_AUDIO_DUAL_MIC_ENABLE		ENABLE_THIS_MOUDLE
 *		   4.建议算法开发者使用宏定义将自己的代码模块包起来
 *		   5.算法处理完的数据，如有需要，可以增加EQ处理：AEC_UL_EQ_EN
 *		   6.开发阶段，默认使用芯片最高主频160MHz，可以通过修改AEC_CLK来修改
 			运行频率。
 ***************************************************************************
 */
#include "aec_user.h"
#include "system/includes.h"
#include "media/includes.h"
#include "application/eq_config.h"
#include "circular_buf.h"
#include "overlay_code.h"
#include "audio_config.h"
#include "debug.h"

#if defined(TCFG_CVP_DEVELOP_ENABLE) && (TCFG_CVP_DEVELOP_ENABLE == 1)

#define AEC_CLK				(160 * 1000000L)	/*模块运行时钟(MaxFre:160MHz)*/
#define AEC_FRAME_POINTS	256					/*AEC处理帧长，跟mic采样长度关联*/
#define AEC_FRAME_SIZE		(AEC_FRAME_POINTS << 1)
#if TCFG_AUDIO_DUAL_MIC_ENABLE
#define AEC_MIC_NUM         2   /*双mic*/
#else
#define AEC_MIC_NUM         1   /*单mic*/
#endif/*TCFG_AUDIO_DUAL_MIC_ENABLE*/

/*上行数据eq*/
#define AEC_UL_EQ_EN		0

#ifdef AUDIO_PCM_DEBUG
/*AEC串口数据导出*/
const u8 CONST_AEC_EXPORT = 1;
#else
const u8 CONST_AEC_EXPORT = 0;
#endif/*AUDIO_PCM_DEBUG*/

extern int audio_dac_read_reset(void);
extern int audio_dac_read(s16 points_offset, void *data, int len);
extern void esco_enc_resume(void);
extern int aec_uart_init();
extern int aec_uart_fill(u8 ch, void *buf, u16 size);
extern void aec_uart_write(void);
extern int aec_uart_close(void);

int *mux_rx_bulk_test = NULL;

#define FREE_RAM_OVERLAY_SIZE	(6 * 1024)
u8 free_ram_overlay[FREE_RAM_OVERLAY_SIZE] sec(.cvp_dev_overlay_mem);

/*AEC输入buf复用mic_adc采样buf*/
#define MIC_BULK_MAX		3
struct mic_bulk {
    struct list_head entry;
    s16 *addr;
    u16 len;
    u16 used;
};

struct audio_aec_hdl {
    u8 start;				//aec模块状态
    volatile u8 busy;
    u8 output_buf[1000];	//aec数据输出缓存
    cbuffer_t output_cbuf;
    s16 *mic;				/*主mic数据地址*/
    s16 *mic_ref;			/*参考mic数据地址*/
    s16 *free_ram;			/*当前可用内存*/
    s16 spk_ref[AEC_FRAME_POINTS];	/*扬声器参考数据*/
    s16 out[AEC_FRAME_POINTS];		/*运算输出地址*/
    OS_SEM sem;
    /*数据复用相关数据结构*/
    struct mic_bulk in_bulk[MIC_BULK_MAX];
    struct mic_bulk inref_bulk[MIC_BULK_MAX];
    struct list_head in_head;
    struct list_head inref_head;
#if AEC_UL_EQ_EN
    struct audio_eq *ul_eq;
#endif/*AEC_UL_EQ_EN*/
};
struct audio_aec_hdl *aec_hdl = NULL;
struct audio_aec_hdl  aec_hdl_mem AT(.cvp_dev_overlay_mem);

/*
*********************************************************************
*                  Audio AEC Output Read
* Description: 读取aec模块的输出数据
* Arguments  : buf  读取数据存放地址
*			   len	读取数据长度
* Return	 : 数据读取长度
* Note(s)    : None.
*********************************************************************
*/
int audio_aec_output_read(s16 *buf, u16 len)
{
    //printf("rlen:%d-%d\n",len,aec_hdl.output_cbuf.data_len);
    local_irq_disable();
    if (!aec_hdl || !aec_hdl->start) {
        printf("audio_aec close now");
        local_irq_enable();
        return -EINVAL;
    }
    u16 rlen = cbuf_read(&aec_hdl->output_cbuf, buf, len);
    if (rlen == 0) {
        //putchar('N');
    }
    local_irq_enable();
    return rlen;
}

/*
*********************************************************************
*                  Audio AEC Output Handle
* Description: AEC模块数据输出回调
* Arguments  : data 输出数据地址
*			   len	输出数据长度
* Return	 : 数据输出消耗长度
* Note(s)    : None.
*********************************************************************
*/
static int audio_aec_output(s16 *data, u16 len)
{
    u16 wlen = 0;
    if (aec_hdl && aec_hdl->start) {
        wlen = cbuf_write(&aec_hdl->output_cbuf, data, len);
        //printf("wlen:%d-%d\n",len,aec_hdl.output_cbuf.data_len);
        if (wlen != len) {
            printf("aec_out_full:%d,%d\n", len, wlen);
        }
        esco_enc_resume();
    }
    return wlen;
}

/*
 *跟踪系统内存使用情况:physics memory size xxxx bytes
 *正常的系统运行过程，应该至少有3k bytes的剩余空间给到系统调度开销
 */
static void sys_memory_trace(void)
{
    static int cnt = 0;
    if (cnt++ > 200) {
        cnt = 0;
        mem_stats();
    }
}

/*
*********************************************************************
*                  Audio AEC RUN
* Description: AEC数据处理核心
* Arguments  : in 		主mic数据
*			   inref	参考mic数据(双mic降噪有用)
*			   ref		speaker参考数据
*			   out		数据输出
* Return	 : 数据运算输出长度
* Note(s)    : 在这里实现AEC_core
*********************************************************************
*/
static int audio_aec_run(s16 *in, s16 *inref, s16 *ref, s16 *out, u16 points)
{
    int out_size = 0;
    putchar('.');
    memcpy(out, in, (points << 1));
    //memcpy(out, inref, (points << 1));
    out_size = points << 1;
#if AEC_UL_EQ_EN
    if (aec_hdl->ul_eq) {
        audio_eq_run(aec_hdl->ul_eq, out, out_size);
    }
#endif/*AEC_UL_EQ_EN*/

    sys_memory_trace();
    return out_size;
}

/*
*********************************************************************
*                  Audio AEC Task
* Description: AEC任务
* Arguments  : priv	私用参数
* Return	 : None.
* Note(s)    : None.
*********************************************************************
*/
static void audio_aec_task(void *priv)
{
    printf("==Audio AEC Task==\n");
    struct mic_bulk *bulk = NULL;
    struct mic_bulk *bulk_ref = NULL;
    u8 pend = 1;
    while (1) {
        if (pend) {
            os_sem_pend(&aec_hdl->sem, 0);
        }
        pend = 1;
        if (aec_hdl->start) {
            if (!list_empty(&aec_hdl->in_head)) {
                aec_hdl->busy = 1;
                local_irq_disable();
                /*1.获取主mic数据*/
                bulk = list_first_entry(&aec_hdl->in_head, struct mic_bulk, entry);
                list_del(&bulk->entry);
                aec_hdl->mic = bulk->addr;
#if (AEC_MIC_NUM == 2)
                /*2.获取参考mic数据*/
                bulk_ref = list_first_entry(&aec_hdl->inref_head, struct mic_bulk, entry);
                list_del(&bulk_ref->entry);
                aec_hdl->mic_ref = bulk_ref->addr;
#endif/*Dual_Microphone*/
                local_irq_enable();
                /*3.获取speaker参考数据*/
                audio_dac_read(60, aec_hdl->spk_ref, AEC_FRAME_SIZE);

                /*4.算法处理*/
                int out_len = audio_aec_run(aec_hdl->mic, aec_hdl->mic_ref, aec_hdl->spk_ref, aec_hdl->out, AEC_FRAME_POINTS);

                /*5.结果输出*/
                audio_aec_output(aec_hdl->out, out_len);

                /*6.数据导出*/
                if (CONST_AEC_EXPORT) {
                    aec_uart_fill(0, aec_hdl->mic, 512);		//主mic数据
                    aec_uart_fill(1, aec_hdl->mic_ref, 512);	//副mic数据
                    aec_uart_fill(2, aec_hdl->spk_ref, 512);	//扬声器数据
                    //aec_uart_fill(2, aec_hdl->out,out_len);	//算法运算结果
                    aec_uart_write();
                }
                bulk->used = 0;
#if (AEC_MIC_NUM == 2)
                bulk_ref->used = 0;
#endif/*Dual_Microphone*/
                aec_hdl->busy = 0;
                pend = 0;
            }
        }
    }
}

/*
*********************************************************************
*                  Audio AEC Open
* Description: 初始化AEC模块
* Arguments  : sr 			采样率(8000/16000)
* Return	 : 0 成功 其他 失败
* Note(s)    : None.
*********************************************************************
*/
int audio_aec_init(u16 sample_rate)
{
    printf("audio_aec_init,sr = %d\n", sample_rate);
    mem_stats();
    if (aec_hdl) {
        printf("audio aec is already open!\n");
        return -1;
    }

    /* aec_hdl = zalloc(sizeof(struct audio_aec_hdl)); */
    /* if (aec_hdl == NULL) { */
    /*     printf("aec_hdl malloc failed"); */
    /*     return -ENOMEM; */
    /* } */
    /* audio_overlay_load_code(OVERLAY_AEC); */

    memset(&aec_hdl_mem, 0, sizeof(aec_hdl_mem));
    aec_hdl = &aec_hdl_mem;

    printf("aec_hdl size:%d\n", sizeof(struct audio_aec_hdl));
    clk_set("sys", AEC_CLK);
    INIT_LIST_HEAD(&aec_hdl->in_head);
    INIT_LIST_HEAD(&aec_hdl->inref_head);
    cbuf_init(&aec_hdl->output_cbuf, aec_hdl->output_buf, sizeof(aec_hdl->output_buf));
    if (CONST_AEC_EXPORT) {
        aec_uart_init();
    }

#if AEC_UL_EQ_EN
    struct audio_eq_param ul_eq_param = {0};
    ul_eq_param.sr = sample_rate;
    ul_eq_param.channels = 1;
    ul_eq_param.online_en = 1;
    ul_eq_param.mode_en = 0;
    ul_eq_param.remain_en = 0;
    ul_eq_param.max_nsection = EQ_SECTION_MAX;
    ul_eq_param.cb = aec_ul_eq_filter;
    ul_eq_param.eq_name = aec_eq_mode;
    aec_hdl->ul_eq = audio_dec_eq_open(&ul_eq_param);
#endif/*AEC_UL_EQ_EN*/
    os_sem_create(&aec_hdl->sem, 0);
    task_create(audio_aec_task, NULL, "aec");
    audio_dac_read_reset();
    aec_hdl->start = 1;

    mem_stats();
#if	0
    aec_hdl->free_ram = malloc(1024 * 63);
    mem_stats();
#endif

    printf("audio_aec_open succ\n");
    return 0;
}


/*
*********************************************************************
*                  Audio AEC Close
* Description: 关闭AEC模块
* Arguments  : None.
* Return	 : None.
* Note(s)    : None.
*********************************************************************
*/
void audio_aec_close(void)
{
    printf("audio_aec_close:%x", (u32)aec_hdl);
    if (aec_hdl) {
        aec_hdl->start = 0;
        while (aec_hdl->busy) {
            os_time_dly(2);
        }
        task_kill("aec");
        if (CONST_AEC_EXPORT) {
            aec_uart_close();
        }

#if AEC_UL_EQ_EN
        if (aec_hdl->ul_eq) {
            audio_dec_eq_close(aec_hdl->ul_eq);
            aec_hdl->ul_eq = NULL;
        }
#endif/*AEC_UL_EQ_EN*/

        local_irq_disable();
        if (aec_hdl->free_ram) {
            free(aec_hdl->free_ram);
        }
        /* free(aec_hdl); */
        aec_hdl = NULL;
        local_irq_enable();
        printf("audio_aec_close succ\n");
    }
}

/*
*********************************************************************
*                  Audio AEC Status
* Description: AEC模块当前状态
* Arguments  : None.
* Return	 : 0 关闭 其他 打开
* Note(s)    : None.
*********************************************************************
*/
u8 audio_aec_status(void)
{
    if (aec_hdl) {
        return aec_hdl->start;
    }
    return 0;
}

/*
*********************************************************************
*                  Audio AEC Input
* Description: AEC源数据输入
* Arguments  : buf	输入源数据地址
*			   len	输入源数据长度
* Return	 : None.
* Note(s)    : 输入一帧数据，唤醒一次运行任务处理数据，默认帧长256点
*********************************************************************
*/
void audio_aec_inbuf(s16 *buf, u16 len)
{
    if (aec_hdl && aec_hdl->start) {
        int i = 0;
        for (i = 0; i < MIC_BULK_MAX; i++) {
            if (aec_hdl->in_bulk[i].used == 0) {
                break;
            }
        }
        if (i < MIC_BULK_MAX) {
            aec_hdl->in_bulk[i].addr = buf;
            aec_hdl->in_bulk[i].used = 0x55;
            aec_hdl->in_bulk[i].len = len;
            list_add_tail(&aec_hdl->in_bulk[i].entry, &aec_hdl->in_head);
        } else {
            printf(">>>aec_in_full\n");
            /*align reset*/
            struct mic_bulk *bulk;
            list_for_each_entry(bulk, &aec_hdl->in_head, entry) {
                bulk->used = 0;
                __list_del_entry(&bulk->entry);
            }
            return;
        }
        os_sem_set(&aec_hdl->sem, 0);
        os_sem_post(&aec_hdl->sem);
    }
}

/*
*********************************************************************
*                  Audio AEC Input Reference
* Description: AEC源参考数据输入
* Arguments  : buf	输入源数据地址
*			   len	输入源数据长度
* Return	 : None.
* Note(s)    : 双mic ENC的参考mic数据输入,单mic的无须调用该接口
*********************************************************************
*/
void audio_aec_inbuf_ref(s16 *buf, u16 len)
{
    if (aec_hdl && aec_hdl->start) {
        int i = 0;
        for (i = 0; i < MIC_BULK_MAX; i++) {
            if (aec_hdl->inref_bulk[i].used == 0) {
                break;
            }
        }
        if (i < MIC_BULK_MAX) {
            aec_hdl->inref_bulk[i].addr = buf;
            aec_hdl->inref_bulk[i].used = 0x55;
            aec_hdl->inref_bulk[i].len = len;
            list_add_tail(&aec_hdl->inref_bulk[i].entry, &aec_hdl->inref_head);
        } else {
            printf(">>>aec_inref_full\n");
            /*align reset*/
            struct mic_bulk *bulk;
            list_for_each_entry(bulk, &aec_hdl->inref_head, entry) {
                bulk->used = 0;
                __list_del_entry(&bulk->entry);
            }
            return;
        }
    }
}

/*
*********************************************************************
*                  Audio AEC Reference
* Description: AEC模块参考数据输入
* Arguments  : buf	输入参考数据地址
*			   len	输入参考数据长度
* Return	 : None.
* Note(s)    : 声卡设备是DAC，默认不用外部提供参考数据
*********************************************************************
*/
void audio_aec_refbuf(s16 *buf, u16 len)
{
    if (aec_hdl && aec_hdl->start) {

    }
}

#endif /*TCFG_CVP_DEVELOP_ENABLE*/
