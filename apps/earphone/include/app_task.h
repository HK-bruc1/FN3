#ifndef APP_TASK_H
#define APP_TASK_H

#include "typedef.h"
#define NULL_VALUE 0

enum {
    APP_BT_TASK,
    APP_MUSIC_TASK,
    APP_PC_TASK,
    APP_HEARING_AID_TASK,
    APP_LINEIN_TASK,
};

extern u8 app_curr_task;
extern u8 app_next_task;
extern u8 app_prev_task;

int app_task_switch_check(u8 app_task);
int app_task_switch_to(u8 app_task, int priv);
void app_task_switch_next(void);
void app_task_switch_prev(void);
u8 app_get_curr_task(void);

#endif
