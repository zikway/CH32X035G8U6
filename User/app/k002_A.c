#include "hw_config.h"
#if K002_A
#include "hw_debug.h"
#include "app_led.h"
#include "app_key.h"
#include "app_imu.h"
#include "app_gamepad.h"
#include "gamepad_setting.h"
#include "gamepad_sensor.h"
#include "hw_adc.h"
#include "hw_gpio.h"
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
#include "PD_Process.h"
#define CUSTOM_CMD_CHGEAR    0XF3  
extern bool request_idx;
extern uint8_t idx;
bool charger_in(void)
{
    bool ret = false;
    // if(charger_evt){
    //     ret = true;
    // }
    return ret;
}
bool zkm_vendor_device_decode(tTrp_handle* cmd_tr,uint8_t *pDat,uint16_t len)
{ 
    bool ret = false;
    logd("zkm_decode_h:");dumpd(pDat,len);
    switch (pDat[2]){

    case CUSTOM_CMD_CHGEAR:
        switch(pDat[3]){
            case 1:
            
            break;
            case 2:
            PDO_Request(2);
            break;
            case 3:
            PDO_Request(3);
            break;
            case 4:
            PDO_Request(4);
            break;
        }

    }
	return ret;

}
void user_vender_init(void)//weak      2
{
    
}


void hw_user_vender_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);               /* Open PD I/O clock, AFIO clock and PD clock */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_USBPD, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOC, &GPIO_InitStructure); 
    AFIO->CTLR |= USBPD_IN_HVT | USBPD_PHY_V33;
    USBPD->CONFIG = PD_DMA_EN;
    USBPD->STATUS = BUF_ERR | IF_RX_BIT | IF_RX_BYTE | IF_RX_ACT | IF_RX_RESET | IF_TX_END;
}
void user_vender_deinit(void)
{
}


void user_task_handle(void)
{
    static timer_t ss;
    static bool cap = false;
    if(request_idx){
       app_send_command(&mTrp_uart, CUSTOM_CMD_CHGEAR, idx, 1);
       logd_r("send scc\r\n");
       request_idx = false;
    }
    // if(!cap && mSysTick >5000){
    //     cap = true;
    //     PDO_Request(3);
    //     logd_y("11111222\r\n");
    // }
    // if(mSysTick - ss>5000){
    // ss = mSysTick;
    // mTrp_uart.index = 1<<8;
    // app_send_command(&mTrp_uart, 0x11, NULL, 0);
    // }
}


#endif
