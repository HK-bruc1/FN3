/*
 * @Author: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @Date: 2025-04-01 20:12:50
 * @LastEditors: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @LastEditTime: 2025-04-15 21:36:22
 * @FilePath: \FN3\apps\common\third_party_profile\jieli\JL_rcsp\adv_app_setting\adv_eq_setting.h
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#ifndef __ADV_EQ_SETTING_H__
#define __ADV_EQ_SETTING_H__

#include "le_rcsp_adv_module.h"

#if defined(RCSP_ADV_EQ_SET_ENABLE) && RCSP_ADV_EQ_SET_ENABLE

struct _EQ_INFO {
    u8 mode;
    s8 gain_val[10];
};

void set_eq_setting(u8 *eq_setting);
void get_eq_setting(u8 *eq_setting);
void deal_eq_setting(u8 *eq_setting, u8 write_vm, u8 tws_sync);
u8 app_get_eq_info(u8 *get_eq_info);
u8 app_get_eq_all_info(u8 *get_eq_info);
void save_eq_mode(u8 *eq_mode);
u8 get_eq_mode(void);
#endif
#endif
