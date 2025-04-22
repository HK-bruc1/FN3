#include "key_event_deal.h"
#include "btstack/avctp_user.h"
#include "event.h"
#include "app_power_manage.h"
#include "app_main.h"
#include "tone_player.h"
#include "audio_config.h"
#include "user_cfg.h"
#include "system/timer.h"
#include "vol_sync.h"
#include "media/includes.h"
#include "pbg_user.h"
#include "application/eq_config.h"
#include "earphone.h"
#include "ui_manage.h"
#include "bt_tws.h"
#include "audio_anc.h"
#include "application/audio_dec_app.h"
#include "media/spectrum/spectrum_fft.h"


//------------------------------------------------------ 2023-08-22 DHF GZH add. find earphoen audio

#if G_log_total
    #if 1
        #define G_log(format, ...)       printf("->>>---    GZH    ---<<<-" format "\r\n", ## __VA_ARGS__)
    #else
        #define G_log(...)
    #endif
#else
    #define G_log(...)
#endif




struct audio_dec_sine_app_hdl *dec_id = NULL; //正弦波解码句柄
static u8 find_ear_state = CANCLE_SEEK;       //下发的当前指令

// static void sync_find_state(u8 find_state);   //副->主 当前状态
static bool slave_state = 0;                  //从机的播放状态
static bool sibling_state = 0;                //主机的播放状态


#define FIND_EAR_TIME     300                 //寻找耳机响的间隔
static u16 find_ear_timer = 0;                //寻找耳机的timer句柄
void open_find_ear_time(void);                //打开寻找耳机间隔响定时器
void close_find_ear_time(void);               //关闭寻找耳机间隔响定时器

static u8 sibling_slave_flag = 0;//主从标志位

static bool close_play_flag = 0; 

void bt_tws_sync_find(u8 en); //副耳播放发消息
//正弦参数
static const struct audio_sin_param sine_test2[] = {
    { 3000 << 9, 0x7fffffff, 0, 100},
};


//获取下发的当前指令
bool get_find_ear_state(void){
    return find_ear_state;
}



// void bt_tws_find_slave_set_flag(void); //改变副耳的标志位
void bt_tws_find_sibling_set_flag(u8 find_state); //改变主耳的标志位


//更新主耳的主从播放状态标志
void updata_sibling_state(bool test)
{
    if(sibling_slave_flag == TWS_ROLE_SLAVE){
        bt_tws_find_sibling_set_flag(!!find_ear_timer);
    }
    if(test){
        // G_log("---------------------------------------     副耳点击,再次更新当前状态    ");
    }else{
        if(sibling_slave_flag == TWS_ROLE_SLAVE){
            // G_log("---------------------------------------     副耳进入同步点击,首次发送状态    ");
        }else{
            // G_log("---------------------------------------     主耳进入同步点击, 要副耳发状态   ");
        }
    }
}

// //---------------------------只能主耳调用
// void send_to_AC701x(void)
// {

//     // G_log("   最后发送    sibling_state =%d  slave_state=%d  ",sibling_state,slave_state);
//     u8 channel = 'U';
//     channel = tws_api_get_local_channel();  //主机通道
//     sibling_slave_flag = tws_api_get_role();
//     extern u8 connect_dev;
//     extern void send_crtl_phone_app_data(u8 ctrl_or_read,u8 type,u8 cmd);
//     extern void ear_send_data_to_watch(u8 type,u8 cmd,u8 val);


//     //左耳为主机   
//     if(channel == 'L'){ 
//         if(sibling_state && slave_state){ 
//             G_log("  Error          实际应不存在，点击后进入必有一边停止    ");
//             if(connect_dev == CONNECT_AC701_DEV){
//                 ear_send_data_to_watch(COMMON_SEEK_BEGIN, SEEK_STATE_READ, SEEK_ALL);
//             }else if(connect_dev == CONNECT_APP_DEV){
//                 send_crtl_phone_app_data(COMMON_READ,COMMON_SEEK_BEGIN,SEEK_ALL);
//             }
//         }else if(sibling_state && !slave_state){ 
//             // G_log("    正在寻找左耳    ");
//             if(connect_dev == CONNECT_AC701_DEV){
//                 ear_send_data_to_watch(COMMON_SEEK_BEGIN, SEEK_STATE_READ, SEEK_LEFT);
//             }else if(connect_dev == CONNECT_APP_DEV){
//                 send_crtl_phone_app_data(COMMON_READ,COMMON_SEEK_BEGIN,SEEK_LEFT);
//             }
//         }else if(!sibling_state && slave_state){ 
//             // G_log("    正在寻找右耳    ");
//             if(connect_dev == CONNECT_AC701_DEV){
//                 ear_send_data_to_watch(COMMON_SEEK_BEGIN, SEEK_STATE_READ, SEEK_RIGHT);
//             }else if(connect_dev == CONNECT_APP_DEV){
//                 send_crtl_phone_app_data(COMMON_READ,COMMON_SEEK_BEGIN,SEEK_RIGHT);
//             }
//         }else if(!sibling_state && !slave_state){ 
//             // G_log("    取消寻找    ");
//             if(connect_dev == CONNECT_AC701_DEV){
//                 ear_send_data_to_watch(COMMON_SEEK_BEGIN, SEEK_STATE_READ, CANCLE_SEEK);
//             }else if(connect_dev == CONNECT_APP_DEV){
//                 send_crtl_phone_app_data(COMMON_READ,COMMON_SEEK_BEGIN,CANCLE_SEEK);
//             }
//         }
//     //右耳为主机
//     }else if(channel == 'R'){  
//         if(sibling_state && slave_state){ 
//             G_log("  Error          实际应不存在，点击后进入必有一边停止    ");
//             if(connect_dev == CONNECT_AC701_DEV){
//                 ear_send_data_to_watch(COMMON_SEEK_BEGIN, SEEK_STATE_READ, SEEK_ALL);
//             }else if(connect_dev == CONNECT_APP_DEV){
//                 send_crtl_phone_app_data(COMMON_READ,COMMON_SEEK_BEGIN,SEEK_ALL);
//             }  
//         }else if(sibling_state && !slave_state){ 
//             // G_log("    正在寻找右耳    ");
//             if(connect_dev == CONNECT_AC701_DEV){
//                 ear_send_data_to_watch(COMMON_SEEK_BEGIN, SEEK_STATE_READ, SEEK_RIGHT);
//             }else if(connect_dev == CONNECT_APP_DEV){
//                 send_crtl_phone_app_data(COMMON_READ,COMMON_SEEK_BEGIN,SEEK_RIGHT);
//             }   
//         }else if(!sibling_state && slave_state){ 
//             // G_log("    正在寻找左耳    ");
//             if(connect_dev == CONNECT_AC701_DEV){
//                 ear_send_data_to_watch(COMMON_SEEK_BEGIN, SEEK_STATE_READ, SEEK_LEFT);
//             }else if(connect_dev == CONNECT_APP_DEV){
//                 send_crtl_phone_app_data(COMMON_READ,COMMON_SEEK_BEGIN,SEEK_LEFT);
//             }  
//         }else if(!sibling_state && !slave_state){ 
//             // G_log("    取消寻找    ");
//             if(connect_dev == CONNECT_AC701_DEV){
//                 ear_send_data_to_watch(COMMON_SEEK_BEGIN, SEEK_STATE_READ, CANCLE_SEEK);
//             }else if(connect_dev == CONNECT_APP_DEV){
//                 send_crtl_phone_app_data(COMMON_READ,COMMON_SEEK_BEGIN,CANCLE_SEEK);
//             } 
//         }
//     }else{
//         G_log("  -------   TWS 未连接   ------- ");
//         if(sibling_state){ //单耳正在寻找耳机 ---- 默认为左耳
//             G_log("  Error           实际应不存在此状态   默认上报为左耳    ");
//             if(connect_dev == CONNECT_AC701_DEV){
//                 ear_send_data_to_watch(COMMON_SEEK_BEGIN, SEEK_STATE_READ, SEEK_LEFT);
//             }else if(connect_dev == CONNECT_APP_DEV){
//                 send_crtl_phone_app_data(COMMON_READ,COMMON_SEEK_BEGIN,SEEK_LEFT);
//             } 
//         }else{
//             G_log("    取消寻找    ");
//             if(connect_dev == CONNECT_AC701_DEV){
//                 ear_send_data_to_watch(COMMON_SEEK_BEGIN, SEEK_STATE_READ, CANCLE_SEEK);
//             }else if(connect_dev == CONNECT_APP_DEV){
//                 send_crtl_phone_app_data(COMMON_READ,COMMON_SEEK_BEGIN,CANCLE_SEEK);
//             } 
//         }
//     }


// }



//单击按下后处理
void key_update_find_ear_state(void){

    //关闭定时器间隔声音
    close_find_ear_time();

    sibling_slave_flag = tws_api_get_role();
    // G_log("0主 1从 = %d   ",sibling_slave_flag);
    if(sibling_slave_flag == TWS_ROLE_MASTER){
        //主耳 直接判断发送数据
        sibling_state = !!find_ear_timer;//主耳状态更新
        // send_to_AC701x();
    }else{
        updata_sibling_state(1); //副耳给主耳更新状态 (副耳 点击关闭间隔播放定时器后，再次更新状态给主耳)
    }

}


//结束播放
int fine_ear_close(void)
{
    if (dec_id) {
        audio_dec_sine_app_play_end(dec_id);
        dec_id = NULL;
    }
    return 0;
}

//开始播放
int fine_ear_open(void)
{
    if (get_charge_online_flag()) {
        return false;
    }
    fine_ear_close();
    if(!dec_id){
        dec_id = audio_dec_sine_app_create_by_parm(sine_test2, ARRAY_SIZE(sine_test2), 1);
    }  
    if (dec_id) {
        audio_dec_sine_app_open(dec_id);
        return true;
    }
    G_log("play err \n");
    return false;
}



//间隔播放寻找手机声音,间隔时间：FIND_EAR_TIME
void find_ear_fun(void)
{
    static bool interval = 0;

    if(interval){
        fine_ear_open();
    }else{
        fine_ear_close();
    }
    interval = !interval;

    //直接在bt_tws_find_sync同步位置调用close_find_ear_time会死机，通过close_play_flag标志位关闭定时器
    if(close_play_flag){
        close_play_flag = 0;
        close_find_ear_time();
    }

}


static u16 clodse_timer = 0;
void clodse_timer_func()
{
    if(clodse_timer){
        fine_ear_close();
        sys_timeout_del(clodse_timer);
        clodse_timer = 0;
    }
    
}

//关间隔播放寻找手机声音
void close_find_ear_time(void)
{
    if(find_ear_timer){
        close_play_flag = 1;
        sys_timer_del(find_ear_timer);
        find_ear_timer = 0;

        if(!clodse_timer){
            clodse_timer = sys_timeout_add(NULL,clodse_timer_func,FIND_EAR_TIME);
        }
    }
}

//开间隔播放寻找手机声音
void open_find_ear_time(void)
{
    // close_find_ear_time();
    if(!find_ear_timer){
        close_play_flag = 0;
        find_ear_timer = sys_timer_add(NULL,find_ear_fun,FIND_EAR_TIME);
    }
}

//获取是否在播放寻找耳机声音
bool get_find_ear_timer(void){
    return !!find_ear_timer;
}



/*
fine_state:
1：寻找左耳
2: 寻找右耳
3: 寻找 左右 耳
4: 取消寻找
*/
void fine_ear_interval(u8 fine_state) //（手表下发后，主耳判断）
{
    u8 channel = 'U';
    channel = tws_api_get_local_channel();

    find_ear_state = fine_state;

    //如果正在播放歌曲，暂停播放
    G_log("fine_state = %x   get_curr_ear_data(EAR_MUSIC_STATE) = %d ",fine_state,a2dp_get_status());
    if(fine_state != CANCLE_SEEK && (a2dp_get_status()==BT_MUSIC_STATUS_STARTING)){
        G_log("1111111111111  2 ");
        user_send_cmd_prepare(USER_CTRL_AVCTP_OPID_PAUSE, 0, NULL); 
    }

    G_log("fine_state = %x   channel = %c",fine_state,channel);

    if(channel == 'L'){
        switch(fine_state){
            case SEEK_LEFT:
                open_find_ear_time();
                bt_tws_sync_find(0);
                break;
            case SEEK_ALL:
                open_find_ear_time();
                bt_tws_sync_find(1);
                break;
            case SEEK_RIGHT:
                close_find_ear_time();
                bt_tws_sync_find(1);
                break;
            case CANCLE_SEEK:
                close_find_ear_time();
                bt_tws_sync_find(0);
                break;
        }
    }else if(channel == 'R'){

        switch(fine_state){
            case SEEK_RIGHT:
                open_find_ear_time();
                bt_tws_sync_find(0);
                break;
            case SEEK_ALL:
                open_find_ear_time();
                bt_tws_sync_find(1);
                break;
            case SEEK_LEFT:
                close_find_ear_time();
                bt_tws_sync_find(1);
                break;
            case CANCLE_SEEK:
                close_find_ear_time();
                bt_tws_sync_find(0);
                break;
        }

    }else{ //对耳没有连接
        if(fine_state == CANCLE_SEEK){
            close_find_ear_time();
        }else{
            open_find_ear_time();
        }
    }
}



//------------------------------------------------------------------  同步数据给副耳

#define TWS_FUNC_ID_DIND_SYNC    TWS_FUNC_ID('F', 'I', 'N', 'D')
#define SEEK_EAR_UPLOAD_FLAG        0X62

static void bt_tws_find_sync(void *_data, u16 len, bool rx)
{
    u8 *data = (u8 *)_data;

    // G_log("副 : end -------------------------------------------------------------------------");

    if (rx) {
        // G_log("-------------------------------------  副 rx = %d    data[0] = 0x%x   data[1] = %d      ",rx,data[0],data[1]);
        if(data[0] == COMMON_SEEK_BEGIN){
            if(data[1]){
                open_find_ear_time(); //开 间隔播放找耳机
            }else{
                close_play_flag = 1;  //关 间隔播放找耳机
            }
        }else if(data[0] == SEEK_EAR_UPLOAD_FLAG){
            // G_log("111111111111111");
        }
    }else{
        // G_log("-  副 rx = 0   发送端 ");
    }

    // G_log("副 : end -------------------------------------------------------------------------");

}

REGISTER_TWS_FUNC_STUB(app_find_sync_stub) = {
    .func_id = TWS_FUNC_ID_DIND_SYNC,
    .func    = bt_tws_find_sync,
};

void bt_tws_sync_find(u8 en) //副耳播放发消息(播放状态)
{
    u8 data[2];
    data[0] = COMMON_SEEK_BEGIN;
    data[1] = en; 
    tws_api_send_data_to_slave(data, 2, TWS_FUNC_ID_DIND_SYNC);
}
// void bt_tws_find_slave_set_flag(void) //改变副耳的标志位
// {
//     u8 data[2];
//     data[0] = SEEK_EAR_UPLOAD_FLAG;
//     data[1] = 1; 
//     tws_api_send_data_to_slave(data, 2, TWS_FUNC_ID_DIND_SYNC);
// }




//------------------------------------------------------------------  同步数据给主耳（）

#define TWS_FUNC_ID_FIND_STATE_SYNC	 TWS_FUNC_ID('F', 'D', 'A', 'T')

static void find_state_sync_tws_deal(void *_data, u16 len, bool rx)
{
    u8 *data = (u8 *)_data;
    
    // G_log("主 : start -------------------------------------------------------------------------");
    
    if(rx){
        // G_log("-------  主 rx = %d    data[0] = 0x%x   data[1] = %d      ",rx,data[0],data[1]);
        if(data[0] == SEEK_EAR_UPLOAD_FLAG){
            G_log("000000000000000000000000  ");
            slave_state = data[1]; //当前副耳发送给主耳，对应状态的是副耳（主耳视角）
            sibling_state = !!find_ear_timer;//主耳状态更新
            if(sibling_slave_flag == TWS_ROLE_MASTER){//此应为主机，加判断确保
                // send_to_AC701x();
            }

            // G_log("   主  sibling_state =%d      slave_state=%d    ",sibling_state,slave_state);
        }
        
    }else{
        // G_log("-  主 rx = 0   发送端 ");
    }

    // G_log("主 : end -------------------------------------------------------------------------");
    
    
    
}

// static void sync_find_state(u8 find_state)
// {
//     u8 data[2] = {0};
//     data[0] = COMMON_SEEK_BEGIN;
//     data[1] = find_state; 
//     tws_api_send_data_to_sibling(data, 2, TWS_FUNC_ID_FIND_STATE_SYNC);
// }

void bt_tws_find_sibling_set_flag(u8 find_state) //改变主耳的主从播放标志位(更新主耳状态)
{
    u8 data[2] = {0};
    data[0] = SEEK_EAR_UPLOAD_FLAG;
    data[1] = find_state; 
    tws_api_send_data_to_sibling(data, 2, TWS_FUNC_ID_FIND_STATE_SYNC);

}


REGISTER_TWS_FUNC_STUB(find_state_sync) = {
    .func_id = TWS_FUNC_ID_FIND_STATE_SYNC,
    .func    = find_state_sync_tws_deal,
};



