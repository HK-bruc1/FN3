/*
 * @Author: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @Date: 2025-04-01 20:12:50
 * @LastEditors: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @LastEditTime: 2025-04-13 12:10:02
 * @FilePath: \FN3\apps\common\third_party_profile\jieli\JL_rcsp\adv_app_setting\adv_key_setting.h
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#ifndef __ADV_KEY_SETTING_H__
#define __ADV_KEY_SETTING_H__


#include "le_rcsp_adv_module.h"

#if RCSP_ADV_KEY_SET_ENABLE
#define ADVKEYMAX 8
void set_key_setting(u8 *key_setting_info);
void get_key_setting(u8 *key_setting_info);

#endif
#endif
