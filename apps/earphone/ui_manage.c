#include "ui_manage.h"
#include "asm/pwm_led.h"
#include "system/includes.h"
#include "app_config.h"
#include "user_cfg.h"
#include "classic/tws_api.h"
#include "btstack/avctp_user.h"

#define LOG_TAG_CONST       UI
#define LOG_TAG             "[UI]"
#define LOG_ERROR_ENABLE
#define LOG_DEBUG_ENABLE
#define LOG_INFO_ENABLE
/* #define LOG_DUMP_ENABLE */
#define LOG_CLI_ENABLE
#include "debug.h"

#define LED_VDDIO_KEEP            1
#define UI_MANAGE_BUF             8

typedef struct _ui_var {
    u8 ui_init_flag;
    u8 other_status;
    u8 power_status;
    u8 current_status;
    volatile u8 ui_flash_cnt;
    cbuffer_t ui_cbuf;
    u8 ui_buf[UI_MANAGE_BUF];
    int sys_ui_timer;
} ui_var;





#if (LED_IO_CHOOSE == 2)
enum  led_state
{
    ALL_OFF,              //关所有
    LED1_LED2_FLASH,      //红蓝交替快闪0.5s
    LED1_FLASH_5S1,       //红灯5s闪一次
    LED1_FLASH_SLOW,      //蓝慢闪、 白
    LED2_FLASH_SLOW,      //红慢闪、 绿


    LED1_ON,              //蓝亮 、 白
    LED2_ON,              //红亮 、 绿    

    LED2_ONE_FLASH,       //绿灯闪烁一下(特殊功能用法)
    GPIO_LED_flash_three,
    ALL_ON,



};


// #define TCFG_PWMLED_PIN0				    IO_PORTA_06					//LED使用的IO口
// #define TCFG_PWMLED_PIN1					IO_PORTA_08		

int   led_time = 0;
int   led_priv = 0;
int   led_cnt  = 0;
void ui_manage_scan(void *priv);
void  led_status_set(u8 led_mode);
void  led_scan(){
       switch(led_priv){
           case 1://红蓝0.5s交替闪
                   led_cnt++;
                   if(led_cnt>=10)led_cnt=0;
                   if(led_cnt%2){//红亮关蓝灯
                       gpio_set_direction(TCFG_PWMLED_PIN0, 0);         //0：输出  1：输入
                       gpio_set_direction(TCFG_PWMLED_PIN1 , 0);         //0：输出  1：输入
                       gpio_set_output_value(TCFG_PWMLED_PIN0,0);       //输出的状态
                       gpio_set_output_value(TCFG_PWMLED_PIN1 ,1);       //输出的状态
                   }
                   else{//蓝亮关红灯
                       gpio_set_direction(TCFG_PWMLED_PIN0, 0);         //0：输出  1：输入
                       gpio_set_direction(TCFG_PWMLED_PIN1 , 0);         //0：输出  1：输入
                       gpio_set_output_value(TCFG_PWMLED_PIN0,1);       //输出的状态
                       gpio_set_output_value(TCFG_PWMLED_PIN1 ,0);       //输出的状态
                   }
           break;
           case 2://蓝灯5s闪一次，一次维持0.5s
                   led_cnt++;
                   if(led_cnt<10){
                       gpio_set_direction(TCFG_PWMLED_PIN0, 1);         //0：输出  1：输入
                       gpio_set_direction(TCFG_PWMLED_PIN1 , 1);         //0：输出  1：输入
                   }
                   else{
                       gpio_set_direction(TCFG_PWMLED_PIN0, 0);         //0：输出  1：输入
                       gpio_set_direction(TCFG_PWMLED_PIN1 , 0);         //0：输出  1：输入
                       gpio_set_output_value(TCFG_PWMLED_PIN0,0);       //输出的状态
                       gpio_set_output_value(TCFG_PWMLED_PIN1 ,1);       //输出的状态
                       led_cnt=0;
                   }
           break;
           case 3://蓝灯1s闪一次
                   led_cnt^=1;
                   if(led_cnt){
                       gpio_set_direction(TCFG_PWMLED_PIN0, 0);         //0：输出  1：输入
                       gpio_set_direction(TCFG_PWMLED_PIN1 , 0);         //0：输出  1：输入
                       gpio_set_output_value(TCFG_PWMLED_PIN0,0);       //输出的状态
                       gpio_set_output_value(TCFG_PWMLED_PIN1 ,1);       //输出的状态
                    //    if(get_bt_connect_status()!=BT_STATUS_TAKEING_PHONE){
                    //       led_status_set(LED1_FLASH_5S1);
                    //    }
                   }
                   else{
                       gpio_set_direction(TCFG_PWMLED_PIN0, 1);         //0：输出  1：输入
                       gpio_set_direction(TCFG_PWMLED_PIN1 , 1);         //0：输出  1：输入
                   }
           break;
           case 4://红灯1s闪一次
                   led_cnt^=1;
                   if(led_cnt){
                       gpio_set_direction(TCFG_PWMLED_PIN0, 0);         //0：输出  1：输入
                       gpio_set_direction(TCFG_PWMLED_PIN1 , 0);         //0：输出  1：输入
                       gpio_set_output_value(TCFG_PWMLED_PIN0,1);       //输出的状态
                       gpio_set_output_value(TCFG_PWMLED_PIN1 ,0);       //输出的状态
                   }
                   else{
                       gpio_set_direction(TCFG_PWMLED_PIN0, 1);         //0：输出  1：输入
                       gpio_set_direction(TCFG_PWMLED_PIN1 , 1);         //0：输出  1：输入
                   }
           break;
            case 7://7
                   led_cnt ++;
                   if(led_cnt >= 3){
                        //大概亮300ms关灯
                        gpio_set_direction(TCFG_PWMLED_PIN0, 1);         //0：输出  1：输入
                        gpio_set_direction(TCFG_PWMLED_PIN1 , 1);         //0：输出  1：输入
                        led_cnt=0;
                        led_priv=0;
                        if(led_time){
                            sys_timeout_del(led_time);
                            led_time = 0;
                        }
                   }
                break;
            case 8:
                led_cnt ++;
                if (led_cnt%2) {
                    gpio_set_direction(TCFG_PWMLED_PIN0, 0);         //0：输出  1：输入
                    gpio_set_direction(TCFG_PWMLED_PIN1 , 0);         //0：输出  1：输入
                    gpio_set_output_value(TCFG_PWMLED_PIN0,1);       //输出的状态
                    gpio_set_output_value(TCFG_PWMLED_PIN1 ,0);       //输出的状态  0
                } else {
                    gpio_set_direction(TCFG_PWMLED_PIN0, 1);         //0：输出  1：输入
                    gpio_set_direction(TCFG_PWMLED_PIN1 , 1);         //0：输出  1：输入
                } 

                if (led_cnt >= 9) {
                    led_cnt=0;
                    led_priv=0;
                    sys_timer_del(led_time);
                    led_time = 0;
                    gpio_set_pull_up(TCFG_PWMLED_PIN0, 0);
                    gpio_set_pull_down(TCFG_PWMLED_PIN0, 0);
                    gpio_set_die(TCFG_PWMLED_PIN0, 0);
                    gpio_set_dieh(TCFG_PWMLED_PIN0, 0);
                    gpio_set_direction(TCFG_PWMLED_PIN0, 1);

                    gpio_set_pull_up(TCFG_PWMLED_PIN1, 0);
                    gpio_set_pull_down(TCFG_PWMLED_PIN1, 0);
                    gpio_set_die(TCFG_PWMLED_PIN1, 0);
                    gpio_set_dieh(TCFG_PWMLED_PIN1, 0);
                    gpio_set_direction(TCFG_PWMLED_PIN1, 1);


                }

                break;

       }
}




void  led_status_set(u8 led_mode)
{
    led_cnt=0;
    led_priv=0;
    if(led_time){
        sys_timeout_del(led_time);
        led_time = 0;
    }
        
    /**每次更新LED状态之前把计数器、私有参数、定时器都删除避免遗漏**/

    switch(led_mode){
        case  ALL_OFF:
               gpio_set_direction(TCFG_PWMLED_PIN0, 1);         //0：输出  1：输入
               gpio_set_direction(TCFG_PWMLED_PIN1 , 1);         //0：输出  1：输入
        break;
        case  LED1_LED2_FLASH:
               led_priv=1;
               led_time=sys_timer_add(NULL,led_scan,500);
        break;
        case  LED1_FLASH_5S1:
               led_priv=2;
               led_time=sys_timer_add(NULL,led_scan,500);
        break;
        case  LED1_FLASH_SLOW:
               led_priv=3;
               led_time=sys_timer_add(NULL,led_scan,1000);
        break;
        case  LED2_FLASH_SLOW:
               led_priv=4;
               led_time=sys_timer_add(NULL,led_scan,1000);
        break;


        case  LED1_ON://此处为蓝灯亮默认关闭所有红灯
               gpio_set_direction(TCFG_PWMLED_PIN0, 0);         //0：输出  1：输入
               gpio_set_direction(TCFG_PWMLED_PIN1 , 0);         //0：输出  1：输入
               gpio_set_output_value(TCFG_PWMLED_PIN0,0);       //输出的状态  0
               gpio_set_output_value(TCFG_PWMLED_PIN1 ,1);       //输出的状态
        break;
        case  LED2_ON://此处为红灯亮默认关闭所有蓝灯
               gpio_set_direction(TCFG_PWMLED_PIN0, 0);         //0：输出  1：输入
               gpio_set_direction(TCFG_PWMLED_PIN1 , 0);         //0：输出  1：输入
               gpio_set_output_value(TCFG_PWMLED_PIN0,1);       //输出的状态
               gpio_set_output_value(TCFG_PWMLED_PIN1 ,0);       //输出的状态  0
        break;
        case LED2_ONE_FLASH:
               gpio_set_direction(TCFG_PWMLED_PIN0, 0);         //0：输出  1：输入
               gpio_set_direction(TCFG_PWMLED_PIN1 , 0);         //0：输出  1：输入
               gpio_set_output_value(TCFG_PWMLED_PIN0,1);       //输出的状态
               gpio_set_output_value(TCFG_PWMLED_PIN1 ,0);       //输出的状态  0
               led_priv=7;
               led_time=sys_timer_add(NULL,led_scan,100);
            break;
        case GPIO_LED_flash_three:
                gpio_set_direction(TCFG_PWMLED_PIN0, 0);         //0：输出  1：输入
                gpio_set_direction(TCFG_PWMLED_PIN1, 0);         //0：输出  1：输入
                gpio_set_output_value(TCFG_PWMLED_PIN0,0);       //输出的状态 0
                gpio_set_output_value(TCFG_PWMLED_PIN1 ,0);       //输出的状态  0
                led_priv= 8;
                led_time=sys_timer_add(NULL,led_scan,200);
            break;
        case ALL_ON:
            gpio_set_direction(TCFG_PWMLED_PIN0, 0);         //0：输出  1：输入
            gpio_set_direction(TCFG_PWMLED_PIN1 , 0);         //0：输出  1：输入
            gpio_set_output_value(TCFG_PWMLED_PIN0,1);       //输出的状态
            gpio_set_output_value(TCFG_PWMLED_PIN1 ,1);       //输出的状态  0
            break;
    }
}

#endif



int poweron_led_cnt = 0;
void poweron_led_deal(void) {
    pwm_led_mode_set(PWM_LED0_LED1_FAST_FLASH);
    if(poweron_led_cnt){
        sys_timeout_del(poweron_led_cnt);
        poweron_led_cnt = 0;
    }
}




int factory_reset_led = 0;
volatile u8 factory_reset_cnt =0;
void factory_reset_deal(void)
{
    factory_reset_cnt++;
    printf("factory_reset_cnt = %d\n",factory_reset_cnt);
    if(factory_reset_cnt % 2){
#if (LED_IO_CHOOSE == 1)
        pwm_led_mode_set(PWM_LED_ALL_OFF);
#elif (LED_IO_CHOOSE == 2)
        led_status_set(ALL_OFF);
#endif
    }else{
#if (LED_IO_CHOOSE == 1)
        pwm_led_mode_set(PWM_LED1_ON);
#elif (LED_IO_CHOOSE == 2)
        // led_status_set(LED2_ON);
#endif
    }

    if(factory_reset_cnt >= 7){  //灯闪次数
#if (LED_IO_CHOOSE == 1)
        pwm_led_mode_set(PWM_LED_ALL_OFF);
#elif (LED_IO_CHOOSE == 2)
        led_status_set(ALL_OFF);
#endif
        factory_reset_cnt = 0;
        if(factory_reset_led){
            sys_timer_del(factory_reset_led);
            factory_reset_led = 0;
        }
        // cpu_reset();
        sys_enter_soft_poweroff(NULL);
        // power_set_soft_poweroff();
    }
}


void open_reset_timer(void)
{
    
#if (LED_IO_CHOOSE == 1)
        pwm_led_mode_set(PWM_LED_ALL_ON);
#elif (LED_IO_CHOOSE == 2)
        led_status_set(LED2_ON);
#endif
    if (factory_reset_led == 0) {
        //G_log("  ----------  ui  STATUS_FACTORY_RESET     start    ");
        factory_reset_led = sys_timer_add(NULL, factory_reset_deal, 200);
    }
}

static ui_var sys_ui_var = {.power_status =  STATUS_NORMAL_POWER};

extern int get_bt_tws_connect_status();

u8   get_ui_status(u8 *status)
{
    return cbuf_read(&(sys_ui_var.ui_cbuf), status, 1);
}

u8   get_ui_status_len(void)
{
    return cbuf_get_data_size(&(sys_ui_var.ui_cbuf));
}

u8 get_ui_busy_status()
{
    return sys_ui_var.ui_flash_cnt;
}

#if 0//(RCSP_ADV_EN)
u8 adv_get_led_status(void)
{
    return sys_ui_var.other_status;
}
#endif




static u16 get_phone_timer = 0;
void get_phone_timer_func()
{
   // G_log(" --------   同步手机时间 ");
    user_send_cmd_prepare(USER_CTRL_HFP_GET_PHONE_DATE_TIME, 0, NULL);
    if(get_phone_timer){
        sys_timeout_del(get_phone_timer);
        get_phone_timer = 0;
    }
}




/*
    LED1_ON   白
    LED2_ON   绿  
*/


void ui_manage_scan(void *priv)
{

    STATUS *p_led = get_led_config();

    sys_ui_var.sys_ui_timer = 0;

    log_info("ui_flash_cnt:%d cur_ui_status:%d", sys_ui_var.ui_flash_cnt, sys_ui_var.current_status);

    if (sys_ui_var.ui_flash_cnt == 0 || sys_ui_var.ui_flash_cnt == 7) {        //有特殊的闪烁状态等当前状态执行完再进入下一个状态
        if (get_ui_status(&sys_ui_var.current_status)) {
            if (sys_ui_var.current_status >= STATUS_CHARGE_START && sys_ui_var.current_status <= STATUS_NORMAL_POWER) {
                sys_ui_var.power_status = sys_ui_var.current_status;
            } else {
                sys_ui_var.other_status = sys_ui_var.current_status;
            }
        }
    }

    if (sys_ui_var.ui_flash_cnt) {
        sys_ui_var.ui_flash_cnt --;
        sys_ui_var.sys_ui_timer = usr_timeout_add(NULL, ui_manage_scan, 300, 1);
    } else if (get_ui_status_len()) {
        sys_ui_var.sys_ui_timer = usr_timeout_add(NULL, ui_manage_scan, 100, 1);
    }

#if 0//((RCSP_ADV_EN)&&(JL_EARPHONE_APP_EN))
    if (tws_api_get_role() == TWS_ROLE_SLAVE) {
#if (LED_IO_CHOOSE == 1)
        pwm_led_mode_set(PWM_LED_ALL_OFF);
#elif (LED_IO_CHOOSE == 2)
        led_status_set(ALL_OFF);
#endif
        return;
    }
#endif

    if (sys_ui_var.other_status != STATUS_POWEROFF && sys_ui_var.power_status != STATUS_NORMAL_POWER) {  //关机的状态优先级要高于电源状态
        switch (sys_ui_var.power_status) {
        case STATUS_LOWPOWER:
            log_info("[STATUS_LOWPOWER]\n");
            // pwm_led_mode_set(p_led->lowpower);
#if (LED_IO_CHOOSE == 1)
        // pwm_led_mode_set(PWM_LED0_ONE_FLASH_5S);
#elif (LED_IO_CHOOSE == 2)
        led_status_set(LED1_FLASH_SLOW);
#endif
            return;

        case STATUS_CHARGE_START:
            log_info("[STATUS_CHARGE_START]\n");
            // pwm_led_mode_set(p_led->charge_start);
#if (LED_IO_CHOOSE == 1)
        pwm_led_mode_set(PWM_LED1_ON);  //白灯
#elif (LED_IO_CHOOSE == 2)
        led_status_set(LED2_ON);
#endif
            return;

        case STATUS_CHARGE_FULL:
            log_info("[STATUS_CHARGE_FULL]\n");
            // pwm_led_mode_set(p_led->charge_full);
#if (LED_IO_CHOOSE == 1)
            pwm_led_mode_set(PWM_LED_ALL_OFF);
#elif (LED_IO_CHOOSE == 2)
            led_status_set(ALL_OFF);//全关
#endif
            return;

        case STATUS_CHARGE_ERR:
            log_info("[STATUS_CHARGE_ERR]\n");
            return;

        case STATUS_CHARGE_CLOSE:
            log_info("[STATUS_CHARGE_CLOSE]\n");
#if (LED_IO_CHOOSE == 1)
            pwm_led_mode_set(PWM_LED0_OFF);
            pwm_led_mode_set(PWM_LED1_OFF);
#elif (LED_IO_CHOOSE == 2)
            led_status_set(ALL_OFF);
#endif
            return;

        case STATUS_CHARGE_LDO5V_OFF:
            log_info("[STATUS_CHARGE_LDO5V_OFF]\n");
        case STATUS_EXIT_LOWPOWER:
        case STATUS_NORMAL_POWER:
            //pwm_led_mode_set(PWM_LED0_OFF);
            //pwm_led_mode_set(PWM_LED1_OFF);
            sys_ui_var.power_status = STATUS_NORMAL_POWER;
            break;
        }
    }

    switch (sys_ui_var.other_status) {
    case STATUS_POWERON:
        log_info("[STATUS_POWERON]\n");
#if (LED_IO_CHOOSE == 1)
        pwm_led_mode_set(PWM_LED0_ON);
        if (!poweron_led_cnt) {
            poweron_led_cnt = sys_timeout_add(NULL,poweron_led_deal,1000);
        }
#elif (LED_IO_CHOOSE == 2)
        led_status_set(LED2_ON);
#endif
        break;
    case STATUS_POWEROFF:
        log_info("[STATUS_POWEROFF]\n");
#if (LED_IO_CHOOSE == 1)
        pwm_led_mode_set(PWM_LED1_ON);
    //     if (p_led->power_off != PWM_LED1_FLASH_THREE) {
    //        pwm_led_mode_set(p_led->power_off);
    //    } else {
    //        if (sys_ui_var.ui_flash_cnt) {
    //            if (sys_ui_var.ui_flash_cnt % 2) {
    //                pwm_led_mode_set(PWM_LED1_ON);
    //            } else {
                //    pwm_led_mode_set(PWM_LED_ALL_OFF);
    //            }
    //        }
    //    }
#elif (LED_IO_CHOOSE == 2)
        // led_status_set(LED1_ON);
        if (p_led->power_off != PWM_LED1_FLASH_THREE) {
           pwm_led_mode_set(p_led->power_off);
       } else {
           if (sys_ui_var.ui_flash_cnt) {
               if (sys_ui_var.ui_flash_cnt % 2) {
                   led_status_set(ALL_OFF);
               } else {
                   led_status_set(LED1_ON);
               }
           }
       }
#endif
        break;

    case STATUS_BT_INIT_OK:
        log_info("[STATUS_BT_INIT_OK]\n");
        // pwm_led_mode_set(p_led->bt_init_ok);
#if (LED_IO_CHOOSE == 1)
        // pwm_led_mode_set(PWM_LED0_LED1_FAST_FLASH);
#elif (LED_IO_CHOOSE == 2)
        led_status_set(LED1_LED2_FLASH);
#endif
        break;

    case STATUS_BT_SLAVE_CONN_MASTER:
#if (LED_IO_CHOOSE == 1)
        pwm_led_mode_set(PWM_LED0_LED1_FAST_FLASH);
#elif (LED_IO_CHOOSE == 2)
        led_status_set(LED2_FLASH_SLOW);
#endif
        break;

    case STATUS_BT_CONN:
        log_info("[STATUS_BT_CONN]\n");
#if (LED_IO_CHOOSE == 1)
        pwm_led_mode_set(PWM_LED_ALL_OFF);
#elif (LED_IO_CHOOSE == 2)
        led_status_set(ALL_OFF);
#endif

        if(get_phone_timer){
            sys_timeout_del(get_phone_timer);
            get_phone_timer = 0;
        }
        if(!get_phone_timer){
            get_phone_timer = sys_timeout_add(NULL,get_phone_timer_func,1000);
        }

        break;

    case STATUS_BT_MASTER_CONN_ONE:
        log_info("[STATUS_BT_MASTER_CONN_ONE]\n");
#if (LED_IO_CHOOSE == 1)
        pwm_led_mode_set(PWM_LED0_LED1_FAST_FLASH);
#elif (LED_IO_CHOOSE == 2)
        led_status_set(LED1_LED2_FLASH);
#endif
        break;

    case STATUS_BT_DISCONN:
        log_info("[STATUS_BT_DISCONN]\n");
          #if TCFG_USER_TWS_ENABLE
        if(tws_api_get_tws_state() & TWS_STA_SIBLING_CONNECTED){
            if(tws_api_get_local_channel()=='L'){
#if (LED_IO_CHOOSE == 1)
                pwm_led_mode_set(PWM_LED0_LED1_FAST_FLASH);
#elif (LED_IO_CHOOSE == 2)
                led_status_set(LED1_LED2_FLASH);
#endif
            }else{
#if (LED_IO_CHOOSE == 1)
                pwm_led_mode_set(PWM_LED0_SLOW_FLASH);
#elif (LED_IO_CHOOSE == 2)
                led_status_set(LED1_FLASH_5S1);
#endif
            }
        }else
        #endif
        {
#if (LED_IO_CHOOSE == 1)
                pwm_led_mode_set(PWM_LED0_LED1_FAST_FLASH);
#elif (LED_IO_CHOOSE == 2)
                led_status_set(LED1_LED2_FLASH);
#endif
                
        }
        break;

    case STATUS_PHONE_INCOME:
        log_info("[STATUS_PHONE_INCOME]\n");        
#if (LED_IO_CHOOSE == 1)
        pwm_led_mode_set(PWM_LED_ALL_OFF);
#elif (LED_IO_CHOOSE == 2)
        led_status_set(ALL_OFF);
#endif
        break;

    case STATUS_PHONE_OUT:
        log_info("[STATUS_PHONE_OUT]\n");
#if (LED_IO_CHOOSE == 1)
        pwm_led_mode_set(PWM_LED_ALL_OFF);
#elif (LED_IO_CHOOSE == 2)
        led_status_set(ALL_OFF);
#endif
        break;

    case STATUS_PHONE_ACTIV:
        log_info("[STATUS_PHONE_ACTIV]\n");
        // pwm_led_mode_set(p_led->phone_activ);
#if (LED_IO_CHOOSE == 1)
        pwm_led_mode_set(PWM_LED_ALL_OFF);
#elif (LED_IO_CHOOSE == 2)
        led_status_set(ALL_OFF);
#endif
        break;

    case STATUS_POWERON_LOWPOWER:
        log_info("[STATUS_POWERON_LOWPOWER]\n");
        // pwm_led_mode_set(p_led->lowpower);
        break;

    case STATUS_BT_TWS_CONN:
        log_info("[STATUS_BT_TWS_CONN]\n");
        // G_log(" bt_tws_get_local_channel() = %d  ",bt_tws_get_local_channel()  );
          #if TCFG_USER_TWS_ENABLE
          if (bt_tws_get_local_channel()=='L'){
            
            // G_log("11111111111111111111");

#if (LED_IO_CHOOSE == 1)
            pwm_led_mode_set(PWM_LED0_LED1_FAST_FLASH);
#elif (LED_IO_CHOOSE == 2)
            led_status_set(LED1_LED2_FLASH);
#endif
     	}else{

            // G_log("22222222222222222222");

#if (LED_IO_CHOOSE == 1)
            pwm_led_mode_set(PWM_LED0_SLOW_FLASH);
#elif (LED_IO_CHOOSE == 2)
            led_status_set(LED1_FLASH_5S1);
#endif
		}
        #endif
        break;
    case STATUS_BT_TWS_DISCONN:
        log_info("[STATUS_BT_TWS_DISCONN]\n");
#if (LED_IO_CHOOSE == 1)
        pwm_led_mode_set(PWM_LED0_LED1_FAST_FLASH);  
#elif (LED_IO_CHOOSE == 2)
        led_status_set(LED1_LED2_FLASH);
#endif
        break;
    case STATUS_FACTORY_RESET:
                log_info("[STATUS_FACTORY_RESET]\n");
        // G_log("  ----------  ui  STATUS_FACTORY_RESET     ");
#if (LED_IO_CHOOSE == 1)
        pwm_led_mode_set(PWM_LED1_ON);
#elif (LED_IO_CHOOSE == 2)
        
#endif
        // G_log("  ----------  ui  STATUS_FACTORY_RESET     start    ");
        // led_status_set(ALL_ON);
        factory_reset_led = sys_timer_add(NULL, factory_reset_deal, 200);
        break;
      
    case STATUS_A2DP_START:
        log_info("[STATUS_A2DP_START]\n");
        pwm_led_mode_set(PWM_LED_ALL_OFF);
        break;
    case STATUS_A2DP_STOP:
        log_info("[STATUS_A2DP_STOP]\n");
        pwm_led_mode_set(PWM_LED_ALL_OFF);
        break;
    case STATUS_DUT_MODE:
#if (LED_IO_CHOOSE == 1)
        pwm_led_mode_set(PWM_LED_ALL_ON);
#elif (LED_IO_CHOOSE == 2)
        led_status_set(ALL_ON);
#endif
        break;
    }
}

int  ui_manage_init(void)
{
    if (!sys_ui_var.ui_init_flag) {
        cbuf_init(&(sys_ui_var.ui_cbuf), &(sys_ui_var.ui_buf), UI_MANAGE_BUF);
        sys_ui_var.ui_init_flag = 1;
    }
    return 0;
}

void ui_update_status(u8 status)
{
    STATUS *p_led = get_led_config();
    if (!sys_ui_var.ui_init_flag) {    //更新UI状态之前需先初始化ui_cbuf
        ui_manage_init();
    }
    log_info("update ui status :%d", status);
    if ((status == STATUS_POWERON && p_led->power_on == PWM_LED0_FLASH_THREE) || (status == STATUS_POWEROFF && p_led->power_off == PWM_LED1_FLASH_THREE)) {
        log_info(">>>set ui_flash_cnt");
        sys_ui_var.ui_flash_cnt = 7;
    }
    cbuf_write(&(sys_ui_var.ui_cbuf), &status, 1);
    /* if (!sys_ui_var.sys_ui_timer) { */
    /*     sys_ui_var.sys_ui_timer = usr_timeout_add(NULL, ui_manage_scan, 10); */
    /* } */
    if (sys_ui_var.ui_flash_cnt >= 1 && sys_ui_var.ui_flash_cnt <= 6) {
        return;
    }

    ui_manage_scan(NULL);
}

u8 pdown_keep_pw_gate()
{
#if(LED_VDDIO_KEEP == 1)        //led 进入powerdown时模块的电源是否降低，降低会导致灯抖动，不降的话功耗会高100ua
    return true;
#else
    if ((sys_ui_var.current_status == STATUS_BT_DISCONN) ||
        (sys_ui_var.current_status == STATUS_BT_INIT_OK) ||
        (sys_ui_var.current_status == STATUS_BT_TWS_DISCONN)) {
        return true;
    } else {
        return false;
    }
#endif
}

#if (TCFG_USER_TWS_ENABLE == 1)
__attribute__((weak))
bool is_led_module_on()
{
    return 0;
}

__attribute__((weak))
void led_module_on()
{

}

__attribute__((weak))
void led_module_off()
{

}

__attribute__((weak))
void led_timer_del()
{

}

__attribute__((weak))
void led_module_enter_sniff_mode()
{

}

__attribute__((weak))
void led_module_exit_sniff_mode()
{

}

__attribute__((weak))
void led_sniff_mode_double_display(u32 index)
{

}

extern int get_bt_tws_connect_status();
//对耳连接成功之后利用SNIFF的节奏来进行闪灯同步,只是通过开关LED模块来实现同步
void sniff_achor_point_hook(u32 slot)
{
    u8 led_mode = pwm_led_display_mode_get();
    if (led_mode != PWM_LED0_ONE_FLASH_5S && led_mode != PWM_LED1_ONE_FLASH_5S) {
        if (get_bt_tws_connect_status()) {
            //交替闪进sniff同步
            if (led_mode == PWM_LED0_LED1_FAST_FLASH) {
                if ((slot % ((CFG_DOUBLE_FAST_FLASH_FREQ * 1000) / 625)) < 800) {
                    led_sniff_mode_double_display((slot / ((CFG_DOUBLE_FAST_FLASH_FREQ * 1000) / 625)) & BIT(0));
                }
            } else if (led_mode == PWM_LED0_LED1_SLOW_FLASH) {
                if ((slot % ((CFG_DOUBLE_SLOW_FLASH_FREQ * 1000) / 625)) < 800) {
                    led_sniff_mode_double_display((slot / ((CFG_DOUBLE_SLOW_FLASH_FREQ * 1000) / 625)) & BIT(0));
                }
            }
        }
        return;
    }
    /* if (get_bt_tws_connect_status()) { */
#if defined CFG_LED_MODE && (CFG_LED_MODE == CFG_LED_SOFT_MODE)
    led_timer_del();
#endif
    if (is_led_module_on()) {
        led_module_off();
    }
    if ((slot % (1600 * 5)) < 800) {    //一个slot = 0.625ms 1600*0.625 = 1s
        led_module_on();
    }
    /* } else { */

    /* } */
}
#else

__attribute__((weak))
int get_bt_tws_connect_status()
{
    return 0;
}

#endif

