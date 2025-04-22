/*
 * @Author: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @Date: 2025-04-01 20:12:50
 * @LastEditors: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @LastEditTime: 2025-04-11 17:40:42
 * @FilePath: \FN3\apps\common\third_party_profile\jieli\JL_rcsp\adv_app_setting\adv_setting_common.h
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#ifndef __ADV_SETTING_COMMON_H__
#define __ADV_SETTING_COMMON_H__

#include "typedef.h"
#include "event.h"

#if RCSP_ADV_EN
struct t_s_info {
    u8 edr_name[32];
    u8 key_setting[24];//added bys samson on 20250411
    u8 key_extra_setting[4];
    u8 led_status[6];
    u8 mic_mode;
    u8 work_mode;
    u8 eq_mode_info[11];
};
extern struct t_s_info _s_info;

void deal_time_stamp_setting(u32 time_stamp, u8 write_vm, u8 tws_sync);
void deal_bt_name_setting(u8 *bt_name_setting, u8 write_vm, u8 tws_sync);
void deal_key_setting(u8 *key_setting_info, u8 write_vm, u8 tws_sync);
void deal_led_setting(u8 *led_setting_info, u8 write_vm, u8 tws_sync);
void deal_mic_setting(u8 mic_setting_info, u8 write_vm, u8 tws_sync);
void deal_work_setting(u8 work_setting_info, u8 write_vm, u8 tws_sync, u8 poweron);

void adv_setting_init(void);
void update_adv_setting(u16 type);
void update_info_from_adv_vm_info(void);
void modify_bt_name_and_reset(u32 msec);
#endif
#endif
