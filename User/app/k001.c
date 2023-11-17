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

}




#endif
