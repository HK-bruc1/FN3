#ifndef APP_MAIN_H
#define APP_MAIN_H

//寻找耳机开始符
#define COMMON_SEEK_BEGIN   0X60
//寻找耳机结束符
#define COMMON_SEEK_END     0X61
//寻找左耳
#define SEEK_LEFT           0X01
//寻找右耳
#define SEEK_RIGHT          0X02
//寻找 左右 耳
#define SEEK_ALL            0X03
//取消寻找
#define CANCLE_SEEK         0X04
//当前寻找状态
#define SEEK_STATE_READ     0X05



#define NO_DEV_CONNECT      0 
#define CONNECT_AC701_DEV   1 
#define CONNECT_APP_DEV     2 



//总开始符
#define COMMON_BEGIN  0XAA
//总结束符
#define COMMON_END    0X55
//控制符
#define COMMON_CTRL   0XF0  //(仅手表)
//上报符
#define COMMON_READ   0XF1

typedef struct _APP_VAR {
    s8 bt_volume;
    s8 dev_volume;
    s8 music_volume;
    s8 call_volume;
    s8 wtone_volume;
    u8 opid_play_vol_sync;
    u8 aec_dac_gain;
    u8 aec_mic_gain;
    u8 aec_mic1_gain;
    u8 rf_power;
    u8 goto_poweroff_flag;
    u8 goto_poweroff_cnt;
    u8 play_poweron_tone;
    u8 remote_dev_company;
    u8 siri_stu;
    u8 cycle_mode;
    u8 have_mass_storage;
    int auto_stop_page_scan_timer;     //用于1拖2时，有一台连接上后，超过三分钟自动关闭Page Scan
    volatile int auto_shut_down_timer;
    volatile int wait_exit_timer;
    u16 auto_off_time;
    u16 warning_tone_v;
    u16 poweroff_tone_v;
    u16 phone_dly_discon_time;
    u8 usb_mic_gain;
    int wait_timer_do;
    u32 start_time;
    float audio_mic_array_diff_cmp;//麦克风阵列补偿值
    u8 audio_mic_array_trim_en; //麦克风阵列校准
    float enc_degradation;//default:1,range[0:1]
} APP_VAR;

typedef struct _BT_USER_PRIV_VAR {
    //phone
    u8 phone_ring_flag: 1;
    u8 phone_num_flag: 1;
    u8 phone_income_flag: 1;
    u8 phone_call_dec_begin: 1;
    u8 phone_con_sync_num_ring: 1;
    u8 phone_con_sync_ring: 1;
    u8 reserved: 2;
    u8 get_phone_num_timecnt;

    u8 inband_ringtone;
    u8 phone_vol;
    u16 phone_timer_id;
    u8 last_call_type;
    u8 income_phone_num[30];
    u8 income_phone_len;
    s32 auto_connection_counter;
    int auto_connection_timer;
    u8 auto_connection_addr[6];
    int tws_con_timer;
    u8 tws_start_con_cnt;
    u8 tws_conn_state;
    bool search_tws_ing;
    int sniff_timer;
    bool fast_test_mode;
} BT_USER_PRIV_VAR;

typedef struct _BT_USER_COMM_VAR {
} BT_USER_COMM_VAR;

extern APP_VAR app_var;
extern BT_USER_PRIV_VAR bt_user_priv_var;

#define earphone (&bt_user_priv_var)

enum{
    EAR_MUSIC_STATE = 0,
    EAR_MUSIC_VOL,
    EAR_ANC_STATE,
    EAR_EQ_STATE,
    EAR_WORK_STATE,
    EAR_LP_EAR_STATE,
    EAR_FIND_EAR_STATE,

    EAR_STATE_MAX,
};

void fine_ear_interval(u8 fine_state);
#endif
