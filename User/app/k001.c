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
#include "gpad.h"
#include "app_rgb.h"
#include "app_command.h"
#include "hw_adc.h"

#include "debug.h"
#include "RGB1W.h"
#include "app_command.h"

#include "firfilter.h"

#define PHONE       6 //手機電源VBUS
#define CHARGER     7 //充電器電源VIN

#define CC1_5_1K_PULLDOWN_PIN       PB_04   //输出 高电平 -> 拉低5.1K，此表示从手机端取电
#define VBUS_EN_PIN                 PB_05   //输出 高电平 -> 手机充电，低电平 -> 停止手机充电
#define DISCHARGE_PIN               PA_08   //输出 高电平 -> 设备放电，达到快速关机
#define DISCHARGE(en)               hw_gpio_output(DISCHARGE_PIN, en)

FIRFilter  filter[3]; //左扳机，右扳机，插入充电器的电压


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

bool charger_in(void)
{
    static uint32_t t;
    bool ret = false;
    if(m_adc_data[CHARGER] < 1000){
        //小米充电头，插入后，会有一个短时间的低电平，此时不认为是充电器插入
        t = mSysTick;
    }
    
    if((mSysTick -t > 1000 || mSysTick < 500) && m_adc_data[CHARGER] > 1000)
    {
        logi("m_adc_data[CHARGER] = %d\n",m_adc_data[CHARGER]);
        ret = true;
    }
    return ret;
}




void hw_user_vendor_adc_scan(uint16_t *adc_date)
{
    //摇杆扳机加上fir滤波
    for(int i=0; i<2; i++){
        adc_date[4+i] = FIRFilter_Update(&filter[i], adc_date[4+i]);
    }
    adc_date[CHARGER] = FIRFilter_Update(&filter[2], adc_date[CHARGER]);  
}


#define CUSTOM_CMD_RGB_CB           0xF1
#define CUSTOM_CMD_SLEEP            0xF2

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

#if APP_RGB_ENABLED

#define F_CPU       HSI_VALUE
void ex_app_rgb_init(void)
{
    RGB1W_Init();
}

bool ex_app_rgb_show(uint8_t *frame)
{
	static uint8_t s_frame[APP_RGB_NUMS*3];
	uint8_t grb[APP_RGB_NUMS*3];
    uint32_t i;
    uint8_t brightness;

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
    
    memset(rgbs_frame, 0, APP_RGB_NUMS*3); //不断刷新，解决偶现亮着后自动熄灭的问题。
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

    
    if(key_pressed & HW_KEY_B)
    {
        logi("key b pressed\n");
    }
    if(key_pressed & HW_KEY_R1)
    {
        logi("key R1 pressed\n");
    }
}

uint8_t get_port_source(pin_t pin)
{
    uint8_t portsrc = GPIO_PortSourceGPIOA; 
    switch (pin & HW_PORT_MASK)
    {
    case HW_PORTA_BASE: //PA
        portsrc =  GPIO_PortSourceGPIOA;
        break;
    case HW_PORTB_BASE:
        portsrc =  GPIO_PortSourceGPIOB;
        break;
    case HW_PORTC_BASE:
        portsrc =  GPIO_PortSourceGPIOC;
        break;
    default:
        break;
    }
    return portsrc;
}

uint8_t get_pin_source(pin_t pin)
{
    uint8_t src = GPIO_PinSource0;
    return src + (pin & HW_PIN_MASK);
}

uint32_t get_pin_line(pin_t pin)
{
    uint8_t line = EXTI_Line0; 
    return line<< (pin & HW_PIN_MASK);
}

const pin_t wake_pin[] = {
    KEY_A_GPIO      ,
    KEY_B_GPIO      ,
    KEY_Y_GPIO      ,
    KEY_X_GPIO      ,

    PA_17           ,
    PA_18           ,

    KEY_M4_GPIO     ,
    KEY_M2_GPIO     ,
    KEY_L3_GPIO     ,
    KEY_R3_GPIO     ,
    KEY_START_GPIO  ,
    //KEY_CAPTURE_GPIO,
    //KEY_SELECT_GPIO ,
};

void Wakeup_Cfg(pin_t pin)
{
    EXTI_InitTypeDef EXTI_InitStructure = { 0 };

    /* Enable GPIOB clock */
    RCC_APB2PeriphClockCmd( RCC_APB2Periph_AFIO, ENABLE );

    GPIO_EXTILineConfig(get_port_source(pin), get_pin_source(pin));
    EXTI_InitStructure.EXTI_Line = get_pin_line(pin);
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Event;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init( &EXTI_InitStructure );

    EXTI->INTENR |= 1<<(pin & HW_PIN_MASK);
}

void clear_exti_flag(pin_t pin)
{
    EXTI_ClearFlag(1<<(pin & HW_PIN_MASK));
}

void clear_wakeup_flag()
{
    for(int i = 0; i < sizeof(wake_pin)/sizeof(pin_t); i++){
        clear_exti_flag(wake_pin[i]);
    }
}

void Sleep_Wakeup_Cfg()
{
    for(int i = 0; i < sizeof(wake_pin)/sizeof(pin_t); i++){
        Wakeup_Cfg(wake_pin[i]);
    }
}

void MCU_Sleep_Wakeup_Operate( void )
{
    printf( "Sleep\r\n" );
    __disable_irq();
    clear_wakeup_flag();
    
    PWR_EnterSTOPMode(PWR_STOPEntry_WFE);
    clear_wakeup_flag();
    __enable_irq();
    printf( "Wake\r\n" );
    hw_uart_close();
    //拉低TX电平>2ms
    hw_gpio_cfg_output(TR_UART2_TX_PIN);
    hw_gpio_output(TR_UART2_TX_PIN, 0);
    delay_ms(5);
    hw_uart_init();
}

bool zkm_vendor_host_decode(tTrp_handle* cmd_tr,uint8_t *buf,uint16_t len)
{
    return false;
}
bool zkm_vendor_device_decode(tTrp_handle* cmd_tr,uint8_t *pDat,uint16_t len)
{ 
    bool ret = false;
    logd("zkm_decode_h:");dumpd(pDat,len);
    switch (pDat[2]){

    case CUSTOM_CMD_SLEEP:
        //cfg wakeup pin
        hw_gpio_cfg_output(PA_19);
        hw_gpio_output(PA_19,0);
        hw_gpio_cfg_output(PA_20);
        hw_gpio_output(PA_20,0);
        hw_gpio_cfg_output(PB_07);
        hw_gpio_output(PB_07,0);
        hw_gpio_cfg_output(PC_06);
        hw_gpio_output(PC_06,0);
        app_rgb_deinit();
        Sleep_Wakeup_Cfg();
        MCU_Sleep_Wakeup_Operate();
        ret = true;
        break;
    }
	return ret;

}
void gamepade_moror_handle(void)
{
    bool ret = true;
    bool en = false; //所有类型的马达都未工作
	gamepad_motor_t s_gamepad_motor;
	static timer_t motor_timer;
	if(mSysTick - motor_timer >= 16){ //设定为16是因为swich主机下,有可能有50ms间隔的震动
		motor_timer = mSysTick;

		if(m_motor_sync){
			for(int i=0;i<APP_MOTOR_MAX;i++){
				s_gamepad_motor.strong[i] =m_gamepad_motor.strong[i];
			}
			#if HW_MOTOR_ENABLED			//实际马达震动调整曲线, TODO APP_MOTOR_ENABLED宏定义修改了,需要适配
			if(hw_motor_enable){
				uint8_t motor_l,motor_r;
                hw_set_motor(APP_MOTOR_BIG_LEFT, s_gamepad_motor.strong[0]);
                hw_set_motor(APP_MOTOR_BIG_RIGHT, s_gamepad_motor.strong[1]);
                logd_r("s_gamepad_motor.strong1[0]=%d s_gamepad_motor.strong1[1]=%d \r\n",m_gamepad_motor.strong[0],m_gamepad_motor.strong[1]);
                en = true;
                ret &= true;
			}
			#endif
			if(ret && en) m_motor_sync--;
		}
	}
}

void user_vender_init(void)//weak      2
{
    hw_gpio_output(EN_PIN,1);
    //  TIM2->CH2CVR=255;
    //  TIM2->CH3CVR=255;
    logd_r("mstorep->flash_head=%d\n",mstorep->flash_head);
    logd_r("mstorep->sub_mode=%d\n",mstorep->sub_mode);
    logi("%s\n",__func__);
    
    for(int i=0; i<countof(filter); i++){
        FIRFilter_Init(&filter[i]);
    }
}


void hw_user_vender_init(void)
{
    hw_gpio_cfg_output(EN_PIN);
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
    
    
    if(m_task_tick10us - tx_timer >= 200){
        tx_timer = m_task_tick10us;
        gamepad_key_t   gpad_key;
        gpad_key = m_gpad_key;
        gamepad_key_hl_swap(&gpad_key);
        mTrp_uart.index = TR_CMD_INDEX;
        app_send_command(&mTrp_uart,CMD_ROCKER_EVE, (uint8_t *)&gpad_key, 14);
    }
   power_manager_handle();
}

void app_rgb_finished_cb(app_rgb_id_t id){
    app_send_command(&mTrp_uart, CUSTOM_CMD_RGB_CB, NULL, 0);
}

void app_joystick_weak_cal_event(app_joystick_cal_t event)
{
    static uint8_t cal_event;
    uint8_t buff;
    if(cal_event != event){
        cal_event = event;
        switch (event)
        {
        case APP_JOYSTICK_CAL_MID:
            buff = APP_JOYSTICK_CAL_MID;
            app_send_command(&mTrp_uart,CMD_CALIBRATION, &buff, 1);
            break;
        case APP_JOYSTICK_CAL_SUCCEED:
            buff = APP_JOYSTICK_CAL_SUCCEED;
            app_send_command(&mTrp_uart,CMD_CALIBRATION, &buff,1);
            break;
        case APP_JOYSTICK_CAL_FAILED:
            buff = APP_JOYSTICK_CAL_FAILED;
            app_send_command(&mTrp_uart,CMD_CALIBRATION, &buff,1);
            break;
        default:
            break;
        }
    }
}

#endif
