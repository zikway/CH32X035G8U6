#include "hw_config.h"
#if K002
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
#include "app_rgb.h"
#include "RGB1W.h"
#include "debug.h"
#include "app_joystick.h"
#include "gpad.h"
#include "app_rgb.h"
#include "app_command.h"
#include "hw_adc.h"

#include "debug.h"
#include "RGB1W.h"
#include "app_command.h"

#include "firfilter.h"

#define PHONE       0 //手機電源VBUS
#define CHARGER     1 //充電器電源VIN

#define CC1_5_1K_PULLDOWN_PIN       PA_00   //输出 高电平 -> 拉低5.1K，此表示从手机端取电
#define VBUS_EN_PIN                 PB_01   //输出 高电平 -> 手机充电，低电平 -> 停止手机充电
#define DISCHARGE_PIN               PB_04   //输出 高电平 -> 设备放电，达到快速关机
#define DISCHARGE(en)               hw_gpio_output(DISCHARGE_PIN, en)

#define CUSTOM_CMD_CHGEAR    0XF3
//extern bool charger_evt;
FIRFilter  filter[1]; //左扳机，右扳机


void vbus_on(void)
{
    hw_gpio_output(VBUS_EN_PIN, 1);
}

void vbus_off(void)
{
    hw_gpio_output(VBUS_EN_PIN, 0);
}

void cc1_5_1k_pulldown(void)
{
   hw_gpio_output(CC1_5_1K_PULLDOWN_PIN, 1);
}

void cc1_5_1k_pulldown_remove(void)
{
   hw_gpio_output(CC1_5_1K_PULLDOWN_PIN, 0);
}

bool phone_plugin(void){
    return (m_adc_data[PHONE] > 1000);
}

bool charger_in(void){
    static uint32_t t;
    static charger_flag = false;
    bool ret = false;
    if(m_adc_data[CHARGER] < 1000){
        //小米充电头，插入后，会有一个短时间的低电平，此时不认为是充电器插入
        t = mSysTick;
    }
    if((mSysTick -t > 1000 || mSysTick < 500) && m_adc_data[CHARGER] > 1010)
    {
         logi("m_adc_data[CHARGER] = %d\n",m_adc_data[CHARGER]);
          ret = true;
        // if(!charger_flag){
        //     logd_r("send 035111\r\n");
        //     charger_flag = true;
        //     mTrp_uart.index = 1<<8;
        //     app_send_command(&mTrp_uart, CUSTOM_CMD_CHGEAR, NULL, 0);
            //charger_evt = true;
       // }
    }
    // if(charger_evt){
    //     logd_y("chongg dian\r\n");
    //     ret = true;
    // }
    return ret;
}




void hw_user_vendor_adc_scan(uint16_t *adc_date)
{
    adc_date[CHARGER] = FIRFilter_Update(&filter[0], adc_date[CHARGER]);
}



void pd_hw_init(void)
{
    hw_gpio_cfg_output(CC1_5_1K_PULLDOWN_PIN);
    hw_gpio_cfg_output(VBUS_EN_PIN);
    cc1_5_1k_pulldown();
    vbus_off();
}

void power_manager_init(void)
{
    hw_gpio_cfg_output(DISCHARGE_PIN);
    DISCHARGE(0);
}

void power_manager_handle(void)
{
    static uint32_t t;
    static bool det_cc_disconnected = false;
    //检查充电的插入与拔出
    if(mSysTick -t > 10)
    {
        t = mSysTick;
        //手机拔出后，再把充电器的情况，快速放电,防止PD工作
        if(pd_mode == SRC){
            if(!det_cc_disconnected){
                if(cc_connected){
                    det_cc_disconnected = true;
                }
            }else{ //detecting cc disconnected
                if(!cc_connected){
                    if(!charger_in()){
                        logi("phone disconnected\n");
                        DISCHARGE(1);
                    }else{
                        det_cc_disconnected = false;
                    }
                }
            }
        }
    }
}

bool zkm_vendor_host_decode(tTrp_handle* cmd_tr,uint8_t *buf,uint16_t len)
{
    logd("zkm_decode_h:");dumpd(buf,len);
    return false;
}
bool zkm_vendor_device_decode(tTrp_handle* cmd_tr,uint8_t *pDat,uint16_t len)
{ 
    bool ret = false;
    logd("zkm_decode_h111:");dumpd(pDat,len);
    switch (pDat[2]){

    case CUSTOM_CMD_CHGEAR:
        //cfg wakeup pin
     //   charger_evt = true;
        ret = true;
        break;
    }
	return ret;

}

void user_vender_init(void)//weak      2
{
    
    for(int i=0; i<countof(filter); i++){
        FIRFilter_Init(&filter[i]);
    }
}


void hw_user_vender_init(void)
{
    power_manager_init();
}
void user_vender_deinit(void)
{
}


void user_task_handle(void)
{
    static uint32_t tx_timer;
    // static uint32_t t = 0;
    // if(mSysTick -t > 1000)
    // {
    //     t = mSysTick;
    //      logi("key:%x, lx:%d, ly:%d, rx:%d, ry:%d, lt:%d, rt:%d, %d, %d, %d, %d, %d, %d\n" \
    //      ,m_gpad_key.key,m_gpad_key.lx,m_gpad_key.ly,m_gpad_key.rx,m_gpad_key.ry,        \
    //      m_gpad_key.l2,m_gpad_key.r2, m_gpad_key.acc.x, m_gpad_key.acc.y, m_gpad_key.acc.z, m_gpad_key.gyro.x, m_gpad_key.gyro.y, m_gpad_key.gyro.z);
    // }
    
    
   power_manager_handle();
}


#endif
