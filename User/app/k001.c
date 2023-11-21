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
#include "PD_Process.h"

#define PHONE       6 //手機電源VBUS
#define CHARGER     7 //充電器電源VIN

#define CC1_5_1K_PULLDOWN_PIN       PC_07   //输出 高电平 -> 拉低5.1K，此表示从手机端取电
#define VBUS_EN_PIN                 PC_15   //输出 高电平 -> 手机充电，低电平 -> 停止手机充电
#define DISCHARGE_PIN               PA_08   //输出 高电平 -> 设备放电，达到快速关机
#define GET_POWER_FROM_PHONE(en)    hw_gpio_output(CC1_5_1K_PULLDOWN_PIN, en);
#define CHARG_PHONE(en)             hw_gpio_output(VBUS_EN_PIN, en)
#define DISCHARGE(en)               hw_gpio_output(DISCHARGE_PIN, en)
#define CHARGER_DET()               (m_adc_data[CHARGER] > 1000)

bool charger_in = false;
bool ps_ready = true;
bool phone_disconnected = false;




void power_manager_init(void)
{
    hw_gpio_cfg_output(CC1_5_1K_PULLDOWN_PIN);
    hw_gpio_cfg_output(VBUS_EN_PIN);
    hw_gpio_cfg_output(DISCHARGE_PIN);

    GET_POWER_FROM_PHONE(1);
    CHARG_PHONE(0); //默认关闭给手机充电
    DISCHARGE(0);
}

void power_manager_handle(void)
{
    static uint32_t t;

    //检查充电的插入与拔出
    if(mSysTick -t > 10)
    {
        bool det;
        t = mSysTick;
        det = CHARGER_DET();
        if(charger_in != det){
            charger_in = det;
            if(charger_in){
                GET_POWER_FROM_PHONE(false);
                //todo pullup cc1.5k

            }
            logi("charger_in: %d\n", det);
        }
    }
}

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
    power_manager_init();
}
void hw_user_vender_deinit(void)
{

}


void user_task_handle(void)
{
    static uint32_t t = 0;
    if(mSysTick -t > 1000)
    {
        t = mSysTick;
        // logi("input: %d\n", CHARGER_DET());
    }
    power_manager_handle();
}
#endif
