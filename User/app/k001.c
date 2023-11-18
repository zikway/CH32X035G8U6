#include "hw_config.h"
#if K001
#include "hw_debug.h"
#include "app_led.h"
#include "app_key.h"
#include "app_imu.h"
#include "app_gamepad.h"
#include "gamepad_setting.h"
#include "gamepad_sensor.h"
#include "hw_adc.h"
#include "hw_gpio.h"

#define CC1_5_1K_PULLDOWN   PC_07   //高电平 -> 拉低5.1K，此表示取电
#define PHONE       6 //手機電源VBUS
#define CHARGER     7 //充電器電源VIN

void key_analyse(void)
{
    if(key_pressed & HW_KEY_A)
    {
        logi("key a pressed\n");
    }
    if(key_pressed & HW_KEY_B)
    {
        logi("key b pressed\n");
    }
    if(key_pressed & HW_KEY_R1)
    {
        logi("key R1 pressed\n");
        hw_gpio_output(CC1_5_1K_PULLDOWN, !hw_gpio_input(CC1_5_1K_PULLDOWN));
        logi("input: %d", hw_gpio_input(CC1_5_1K_PULLDOWN));
    }
    if(key_pressed & 0X100)
    {
        logi("key R1 pressed\n");
        hw_gpio_output(CC1_5_1K_PULLDOWN, !hw_gpio_input(CC1_5_1K_PULLDOWN));
        logi("input: %d", hw_gpio_input(CC1_5_1K_PULLDOWN));
    }
}



bool zkm_vendor_host_decode(tTrp_handle* cmd_tr,uint8_t *buf,uint16_t len)
{
    return false;
}
bool zkm_vendor_device_decode(tTrp_handle* cmd_tr,uint8_t *pDat,uint16_t len)
{ 
    return false;
}


void user_vender_init(void)//weak      2
{
    logi("%s\n",__func__);
}


void hw_user_vender_init(void)
{
    //默认从手机端获取电量
    hw_gpio_cfg_output(CC1_5_1K_PULLDOWN);
    hw_gpio_output(CC1_5_1K_PULLDOWN, 1);
}
void hw_user_vender_deinit(void)
{

}
/**
 * \brief 这个任务本身就是4ms执行一次
 *
 */
void user_task_handle(void)
{
    static uint32_t t = 0;
    if(mSysTick -t > 1000)
    {
        t = mSysTick;
        logi("input: %d\n", hw_gpio_input(CC1_5_1K_PULLDOWN));
    }
}


void power_manager_init(void)
{
    logi("%s\n",__func__);
    //check 
}

void power_manager_handle(void)
{
    logi("%s\n",__func__);
    static uint32_t t = 0;
    if(mSysTick -t > 20)
    {
        // m_adc_data[CHARGER] = hw_adc_value(ADC_BATTERY_ID);
        // logi("input: %d\n", hw_gpio_input(CC1_5_1K_PULLDOWN));
    }
}


#endif
