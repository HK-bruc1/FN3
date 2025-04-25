#include "dev_update.h"
#include "dev_manager.h"
#include "update/update.h"
#include "update/update_loader_download.h"
#include "app_config.h"

#if defined(CONFIG_SD_UPDATE_ENABLE) || defined(CONFIG_USB_UPDATE_ENABLE)
#define DEV_UPDATE_EN		1
#else
#define DEV_UPDATE_EN		0
#endif

#ifdef DEV_UPDATE_SUPPORT_JUMP
extern void __JUMP_TO_MASKROM();
extern void save_spi_port();
extern void update_close_hw(void *filter_name);
extern void ram_protect_close(void);
#endif      //endif DEV_UPDATE_SUPPORT_JUMP

extern bool uart_update_send_update_ready(char *file_path);
extern bool get_uart_update_sta(void);

static char update_path[48] = {0};
extern const char updata_file_name[];

struct __update_dev_reg {
    char *logo;
    int type;
    union {
        UPDATA_SD sd;
    } u;
};


#if TCFG_SD0_ENABLE
static const struct __update_dev_reg sd0_update = {
    .logo = "sd0",
    .type = SD0_UPDATA,
    .u.sd.control_type = SD_CONTROLLER_0,
#ifdef TCFG_SD0_PORT_CMD
    .u.sd.control_io_clk = TCFG_SD0_PORT_CLK,
    .u.sd.control_io_cmd = TCFG_SD0_PORT_CMD,
    .u.sd.control_io_dat = TCFG_SD0_PORT_DA0,
#else
#if (TCFG_SD0_PORTS=='A')
    .u.sd.control_io = SD0_IO_A,
#elif (TCFG_SD0_PORTS=='B')
    .u.sd.control_io = SD0_IO_B,
#elif (TCFG_SD0_PORTS=='C')
    .u.sd.control_io = SD0_IO_C,
#elif (TCFG_SD0_PORTS=='D')
    .u.sd.control_io = SD0_IO_D,
#elif (TCFG_SD0_PORTS=='E')
    .u.sd.control_io = SD0_IO_E,
#elif (TCFG_SD0_PORTS=='F')
    .u.sd.control_io = SD0_IO_F,
#endif
#endif
    .u.sd.power = 1,
};
#endif//TCFG_SD0_ENABLE

#if TCFG_SD1_ENABLE
static const struct __update_dev_reg sd1_update = {
    .logo = "sd1",
    .type = SD1_UPDATA,
    .u.sd.control_type = SD_CONTROLLER_1,
#if (TCFG_SD1_PORTS=='A')
    .u.sd.control_io = SD1_IO_A,
#else
    .u.sd.control_io = SD1_IO_B,
#endif
    .u.sd.power = 1,

};
#endif//TCFG_SD1_ENABLE

#if TCFG_UDISK_ENABLE
static const struct __update_dev_reg udisk_update = {
    .logo = "udisk0",
    .type = USB_UPDATA,
};
#endif//TCFG_UDISK_ENABLE


static const struct __update_dev_reg *update_dev_list[] = {
#if TCFG_UDISK_ENABLE
    &udisk_update,
#endif//TCFG_UDISK_ENABLE
#if TCFG_SD0_ENABLE
    &sd0_update,
#endif//
#if TCFG_SD1_ENABLE
    &sd1_update,
#endif//TCFG_SD1_ENABLE
};

void *dev_update_get_parm(int type)
{
    struct __update_dev_reg *parm = NULL;
    for (int i = 0; i < ARRAY_SIZE(update_dev_list); i++) {
        if (update_dev_list[i]->type == type) {
            parm = (struct __update_dev_reg *)update_dev_list[i];
        }
    }

    if (parm == NULL) {
        return NULL;
    }
    return (void *)&parm->u.sd;
}


struct strg_update {
    void *fd;
    char *update_path;
};
static struct strg_update strg_update = {0};
#define __this 		(&strg_update)

static u16 strg_f_open(void)
{
    if (!__this->update_path) {
        printf("file path err ");
        return false;
    }

    if (__this->fd) {
        return true;
        /* fclose(__this->fd);
        __this->fd = NULL; */
    }
    __this->fd = fopen(__this->update_path, "r");
    if (!__this->fd) {
        printf("file open err ");
        return false;
    }
    return true;
}

static u16 strg_f_read(void *fp, u8 *buff, u16 len)
{
    if (!__this->fd) {
        return (u16) - 1;
    }

    len = fread(__this->fd, buff, len);
    return len;
}

static int strg_f_seek(void *fp, u8 type, u32 offset)
{
    if (!__this->fd) {
        return (int) - 1;
    }

    int ret = fseek(__this->fd, offset, type);
    /* return 0; // 0k */
    return ret;
}
static u16 strg_f_stop(u8 err)
{
    if (__this->fd) {
        fclose(__this->fd);
        __this->fd = NULL;
    }
    return true;
}

static int strg_update_set_file_path_and_hdl(char *update_path, void *fd)
{
    __this->update_path = update_path;
    __this->fd = fd;

    return true;
}

static const update_op_api_t strg_dev_update_op = {
    .f_open = strg_f_open,
    .f_read = strg_f_read,
    .f_seek = strg_f_seek,
    .f_stop = strg_f_stop,
};

static void dev_update_param_private_handle(UPDATA_PARM *p)
{
    u16 up_type = p->parm_type;

#ifdef CONFIG_SD_UPDATE_ENABLE
    if ((up_type == SD0_UPDATA) || (up_type == SD1_UPDATA)) {
        int sd_start = (u32)p + sizeof(UPDATA_PARM);
        void *sd = NULL;
        sd = dev_update_get_parm(up_type);
        if (sd) {
            memcpy((void *)sd_start, sd, UPDATE_PRIV_PARAM_LEN);
        } else {
            memset((void *)sd_start, 0, UPDATE_PRIV_PARAM_LEN);
        }
    }
#endif

#ifdef CONFIG_USB_UPDATE_ENABLE
    if (up_type == USB_UPDATA) {
        printf("usb updata ");
        int usb_start = (u32)p + sizeof(UPDATA_PARM);
        memset((void *)usb_start, 0, UPDATE_PRIV_PARAM_LEN);
    }
#endif
    memcpy(p->file_patch, updata_file_name, strlen(updata_file_name));
}

static void dev_update_before_jump_handle(u16 up_type)
{
    printf("升级前跳转处理，升级类型: 0x%x\n", up_type);
#ifdef DEV_UPDATE_SUPPORT_JUMP
#if TCFG_BLUETOOTH_BACK_MODE			//后台模式需要把蓝牙关掉
    printf("关闭蓝牙模块\n");
    if (BT_MODULES_IS_SUPPORT(BT_MODULE_LE)) {
        printf("关闭BLE模块\n");
        ll_hci_destory();
    }
    printf("关闭蓝牙控制器\n");
    hci_controller_destory();
#endif
    printf("关闭硬件模块\n");
    update_close_hw("null");
    printf("关闭RAM保护\n");
    ram_protect_close();
    printf("保存SPI端口配置\n");
    save_spi_port();

    printf("准备跳转到MASKROM...\n");
    /* JL_UART0->CON0 = 0; */
    /* JL_UART1->CON0 = 0; */
    __JUMP_TO_MASKROM();
#else
    printf("执行CPU复位\n");
    cpu_reset();
#endif      //DEV_UPDATE_SUPPORT_JUMP
}

static void dev_update_state_cbk(int type, u32 state, void *priv)
{
    update_ret_code_t *ret_code = (update_ret_code_t *)priv;
    printf("升级状态回调: 类型=0x%x, 状态=0x%x\n", type, state);

    switch (state) {
    case UPDATE_CH_EXIT:
        printf("升级通道退出\n");
        if ((0 == ret_code->stu) && (0 == ret_code->err_code)) {
            printf("升级成功，准备跳转到升级模式\n");
            update_mode_api_v2(type,
                               dev_update_param_private_handle,
                               dev_update_before_jump_handle);
        } else {
            printf("升级失败，状态=%d, 错误码=%d, 执行CPU复位!!!\n", ret_code->stu, ret_code->err_code);
            cpu_reset();
        }
        break;
    default:
        printf("未知的升级状态: 0x%x\n", state);
        break;
    }
}
u16 dev_update_check(char *logo)
{
    printf("检查设备升级: %s\n", logo);
    if (update_success_boot_check() == true) {
        printf("已经成功升级过，不需要再次升级\n");
        return UPDATA_NON;
    }
    struct __dev *dev = dev_manager_find_spec(logo, 0);
    if (dev) {
        printf("找到设备: %s\n", logo);
#if DEV_UPDATE_EN
        //<查找设备升级配置
        printf("查找设备升级配置\n");
        struct __update_dev_reg *parm = NULL;
        for (int i = 0; i < ARRAY_SIZE(update_dev_list); i++) {
            if (0 == strcmp(update_dev_list[i]->logo, logo)) {
                parm = (struct __update_dev_reg *)update_dev_list[i];
                printf("找到设备升级配置: %s\n", logo);
            }
        }
        if (parm == NULL) {
            printf("dev update without parm err!!!\n");
            return UPDATA_PARM_ERR;
        }
        //<尝试按照路径打开升级文件
        printf("尝试按照路径打开升级文件\n");
        char *updata_file = (char *)updata_file_name;
        if (*updata_file == '/') {
            updata_file ++;
        }
        memset(update_path, 0, sizeof(update_path));
        sprintf(update_path, "%s%s", dev_manager_get_root_path(dev), updata_file);
        printf("升级文件路径: %s\n", update_path);
        FILE *fd = fopen(update_path, "r");
        if (!fd) {
            ///没有升级文件， 继续跑其他解码相关的流程
            printf("打开升级文件失败，继续运行其他流程\n");
            return UPDATA_DEV_ERR;
        }
        printf("成功打开升级文件\n");

#if(USER_UART_UPDATE_ENABLE) && (UART_UPDATE_ROLE == UART_UPDATE_MASTER)
        printf("发送升级准备信号到UART\n");
        uart_update_send_update_ready(update_path);
        while (get_uart_update_sta()) {
            printf("等待UART升级完成\n");
            os_time_dly(10);
        }
#else
        ///进行升级
        printf("设置升级文件路径和句柄\n");
        strg_update_set_file_path_and_hdl(update_path, (void *)fd);

        printf("初始化升级任务，升级类型: 0x%x\n", parm->type);
        update_mode_info_t info = {
            .type = parm->type,
            .state_cbk = dev_update_state_cbk,
            .p_op_api = &strg_dev_update_op,
            .task_en = 0,
        };
        printf("启动升级任务\n");
        app_active_update_task_init(&info);

#endif// USER_UART_UPDATE_ENABLE

#endif//DEV_UPDATE_EN
    }
    return UPDATA_READY;
}



