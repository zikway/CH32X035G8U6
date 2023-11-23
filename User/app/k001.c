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
#include "app_rgb.h"
#include "RGB1W.h"
#include "debug.h"
#include "app_joystick.h"

#define PHONE       6 //手機電源VBUS
#define CHARGER     7 //充電器電源VIN

#define CC1_5_1K_PULLDOWN_PIN       PC_07   //输出 高电平 -> 拉低5.1K，此表示从手机端取电
#define VBUS_EN_PIN                 PC_15   //输出 高电平 -> 手机充电，低电平 -> 停止手机充电
#define DISCHARGE_PIN               PA_08   //输出 高电平 -> 设备放电，达到快速关机
#define GET_POWER_FROM_PHONE(en)    hw_gpio_output(CC1_5_1K_PULLDOWN_PIN, en);
#define CHARG_PHONE(en)             hw_gpio_output(VBUS_EN_PIN, en)
#define DISCHARGE(en)               hw_gpio_output(DISCHARGE_PIN, en)
#define CHARGER_DET()               (m_adc_data[CHARGER] > 1000)

extern bool charger_in;



void power_manager_init(void)
{
    hw_gpio_cfg_output(CC1_5_1K_PULLDOWN_PIN);
    // hw_gpio_cfg_output(VBUS_EN_PIN);
    hw_gpio_cfg_output(DISCHARGE_PIN);

    GET_POWER_FROM_PHONE(1);
    // CHARG_PHONE(0); //默认关闭给手机充电
    DISCHARGE(0);
}

void power_manager_handle(void)
{
    static uint32_t t;
    static bool det_cc_disconnected = false;
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
        //手机拔出后，再把充电器的情况，快速放电,防止PD工作
        if(pd_mode == SRC){
            if(!det_cc_disconnected){
                if(cc_connected){
                    det_cc_disconnected = true;
                }
            }else{ //detecting cc disconnected
                if(!cc_connected){
                    if(!charger_in){
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

#if APP_RGB_ENABLED

#define F_CPU       HSI_VALUE
void ex_app_rgb_init(void)
{
    // GPIO_InitTypeDef GPIO_InitStructure = {0};
    // RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO | RCC_APB2Periph_GPIOC, ENABLE);
    // RCC_AHBPeriphClockCmd(RCC_AHBPeriph_IO2W, ENABLE);

    // GPIO_PinRemapConfig(GPIO_Remap_SWJ_Disable, ENABLE);
    // GPIO_SetBits(GPIOC, GPIO_Pin_18);

    // GPIO_InitStructure.GPIO_Pin = GPIO_Pin_18;
    // GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    // GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    // GPIO_Init(GPIOC, &GPIO_InitStructure);

	// PIOC->D8_SYS_CFG = RB_MST_RESET | RB_MST_IO_EN0;	// reset PIOC & enable IO0
	// memcpy( (uint8_t *)(PIOC_SRAM_BASE), PIOC_1W_CODE, sizeof( PIOC_1W_CODE )/ sizeof(PIOC_1W_CODE[0]));	// load code for PIOC
}

bool ex_app_rgb_show(uint8_t *frame)
{
	static uint8_t s_frame[APP_RGB_NUMS*3];
	uint8_t grb[APP_RGB_NUMS*3];
    uint32_t i;
    uint8_t brightness;
    return true;// todo
	if(0 == memcmp(s_frame,frame,APP_RGB_NUMS*3)) return true;
	memcpy(s_frame,frame,APP_RGB_NUMS*3);
	memcpy(grb,frame,APP_RGB_NUMS*3);

	for (i = 0; i < APP_RGB_NUMS*3; i+=3){
		brightness = grb[i+1];
		grb[i+1] = grb[i];
		grb[i] = brightness;
	}

    if(APP_RGB_NUMS*3 > 32){
        RGB1W_SendRAM_Wait(APP_RGB_NUMS*3, grb);
    }else{
        RGB1W_SendSFR_Wait(APP_RGB_NUMS*3, grb);
    }
    return true;
}

void ex_app_rgb_deinit(void)
{
}
#endif

void key_analyse(void)
{
    if(key_pressed & HW_KEY_A)
    {
        logi("key a pressed\n");
    }

    if (JOYSTICK_CAl_END_KEY == (JOYSTICK_CAl_END_KEY & key_press_long)){
        key_single_event_send(KEY_EVT_CALIBRATION, true);
    }else{
        key_single_event_send(KEY_EVT_CALIBRATION, false);
    }
    if ((JOYSTICK_CAl_END_KEY & key_pressed_b)){
        app_joystick_calibration_end();
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
      bool ret = false;
//  logd("zkm_decode_h:");dumpd(pDat,len);
    switch (pDat[2]){
    case CMD_ROCKER_EVE:
      // logd_r("11111");
        ret = true;
        break; 
    }
	return ret;

}


void user_vender_init(void)//weak      2
{
    logi("%s\n",__func__);
    app_rgb_blink(0, 1000, RGB_FOREVER, WHITE);
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
    uint8_t w_len=5;
    gamepad_key_t gpad_key;
    //app_fifo_write(&m_uart_rx_fifo,test_buf,&w_len);
    //logd("222=%x\n",(uint8_t)USART_ReceiveData(USART2));
    // if(usart_flag){
    //     usart_flag =false;
    //  app_fifo_write(&m_uart_rx_fifo,test_buf,&w_len);
    // }
    if(mSysTick -t > 1000)
    {
        t = mSysTick;
        logi("key:%x, lx:%d, ly:%d, rx:%d, ry:%d, lt:%d, rt:%d, %d, %d, %d, %d, %d, %d\n" \
        ,m_gpad_key.key,m_gpad_key.lx,m_gpad_key.ly,m_gpad_key.rx,m_gpad_key.ry,        \
        m_gpad_key.l2,m_gpad_key.r2, m_gpad_key.acc.x, m_gpad_key.acc.y, m_gpad_key.acc.z, m_gpad_key.gyro.x, m_gpad_key.gyro.y, m_gpad_key.gyro.z);
    }
    power_manager_handle();
}
#endif
