#include "hw_config.h"
#if X108
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
#include "sev_seg.h"
#include "math.h"

#include "firfilter.h"
#include "hw_pwm.h"
#include "hw_powermanager.h"
/*
    手机散热器功能
    1. //电源管理（PD,QC 取电) //1  检查PD,无则检查QC，检测充电器是否输出12V(vol_match)
    1. 温控管理 （供需适配）//1
    1. //LED状态指示
    1. //板子温度NTC
    1. //按键管理
    1. //氛围灯管理
    1. 手机充电管理
    1. 休眠唤醒管理
*/
#define   REQ_VOL   mstorep->res[0] 
#define   PERIOD_MIN    600
#define   PERIOD_MAX    3000
#define KP 50.0
#define KI 0.1
#define KD 0.1
typedef enum{
    LEVEL_0 = 0,    //关闭
    LEVEL_1,
    LEVEL_2,
    LEVEL_3,
}xmode_t;

typedef enum{
    LIGHT_RAINBOW = 0,     //流水灯
    LIGHT_GRADIENT,        //炫彩
    LIGHT_BREATH,          //呼吸
    LIGHT_OFF,             //关灯
    LIGHT_END
}xlight_t;

#define RGB_USER_SETTING 0XF0
xmode_t mode = LEVEL_3;
bool enter;

xlight_t light = LIGHT_RAINBOW;
timer_t sleep_time = 0;    //休眠计数
uint32_t t100_ms_cnt = 0;
uint8_t temperature;     //实时温度
bool pd_flag = false;
bool otuo_crl =false;
bool violent_evt = false;
uint16_t m_period = PERIOD_MIN;
time_t start_time;
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
    KEY_A_GPIO,
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
}

#if APP_RGB_ENABLED

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
uint32_t rgb_get_color(app_rgb_id_t id)
{
    return rgb_ctb[id].r<<16 | rgb_ctb[id].g<<8 | rgb_ctb[id].b;
}

#endif

FIRFilter  filter; //左扳机，右扳机

void hw_user_vendor_adc_scan(uint16_t *adc_date)
{
    adc_date[ADC_NTC_ID] = FIRFilter_Update(&filter, adc_date[ADC_NTC_ID]);
}

//100K B值4250 NTC热敏电阻的温度
const float R0 = 100000.0; // 25℃下 电阻值
const float T0 = 273.15 + 25.0;
const float B = 4250.0;   // B 常数
//计算公式. RT = R0*exp(B*(1/T-1/T0)) //http://www.thermistors.cn/news/656.html
// T = 1 / (1/T0 + 1/B * ln(R/R0))
int8_t get_board_temperature()
{
    //NTC SENSOR
    // 100K B值4250 NTC热敏电阻的温度, adc to temperature, adc的单位是4096 对应3.3V
   
      //获得R的值
    //5v - 100K - R - GND
    // R/(R+100000)*5v = ADC*5v/4096;
    // 2380- 2000
    // 0: 368000 -> adc 32201
    // 50: 33000 -> adc 1016
    // K = 50/(1016-3220) = -0.0227
    // B = 50 - 1016 * -0.0227 = 73
    logd_r("m_adc_data=%d\r\n",m_adc_data[0]);
    return m_adc_data[0]*-0.0181 + 58;

    // uint32_t R = 100000*m_adc_data[ADC_NTC_ID]/(4096-m_adc_data[ADC_NTC_ID]);
    // // printf("adc R:%d %d\n", m_adc_data[ADC_NTC_ID], R);
    // int8_t temp = 1/(1/T0 + 1/B * (custom_log((float)R/R0))) - 237.15;
    // // int8_t temp = 1/(1/(273.15 + 25.0) + 1/4250 * log(R/100000))) - 237.15;
    // printf("temp: %d\n", temp);
    // return temp;  
}
void light_ctrl(xlight_t lt, uint16_t period)
{
   // logi("light_ctrl:%d, %d\n", lt, period);
    for(int i=0; i<APP_RGB_NUMS; i++){
        app_rgb_set_brightness(i, 50);
    }
    switch (lt)
    {
    case LIGHT_RAINBOW:
        for(int i= 0; i<APP_RGB_NUMS; i++){
            app_rgb_set_rainbow_mode(i, 0, period, RGB_FOREVER);
        }
        for(int i= 0; i<APP_RGB_NUMS; i++){
            app_rgb_set_rainbow_start_end(i, (65536/APP_RGB_NUMS)*i, 0);
        }
        break;
    case LIGHT_GRADIENT:
        for(int i= 0; i<APP_RGB_NUMS; i++){
            app_rgb_set_gradient_mode(i, period, RGB_FOREVER, RED);
            app_rgb_set_gradient_color(i, BLUE);
        }
        break;
    case LIGHT_BREATH:
        for(int i= 0; i<APP_RGB_NUMS; i++){
            app_rgb_breath(i, period, RGB_FOREVER, ORANGE);
        }
        break;
    case LIGHT_OFF:
        for(int i= 0; i<APP_RGB_NUMS; i++){
            app_rgb_static(i, BLACK);
        }
        break;
    }
}



/**
 * @brief 电压9V，电流0.2A（Max）
 * @param speed: 0~255
 */
void fan_ctrl(uint8_t speed)
{
    //logi("fan_ctrl:%d\n", speed);
    hw_pwm_set_duty(FAN_ID, speed);
    //todo 同时修改灯效周期
    // m_period = remap(speed, 50, 255, PERIOD_MAX, PERIOD_MIN);
    switch(pd_req_vol){
        case 5:
            m_period =  1500;
        break;
        case 9:
            m_period = 1000;
        break;
        case 12:
            m_period = 600;
        break;
    }
   // printf("m_period:%d\n", m_period);
    light_ctrl(light, m_period);
}

/**
 * @brief 电压9V，电流0.2A（Max）
 * @param speed: 0~255
*/
void temp_ctrl(uint8_t speed, uint8_t max_speed)
{
    if(speed > max_speed)
        speed = max_speed;
    
    // hw_pwm_set_freq
    hw_pwm_set_duty(COOLER_ID, speed);
}

bool vbus_req_vol_match(void)
{
    //adc to voltage
    // VBUS - 3.3K - 1K - GND
    // VBUS*1/4.3 = ADC/4096*5v
    for(int i=0; i<1000; i++){
        hw_adc_scan();
    }
    logi("adc:%d\n", m_adc_data[ADC_CHARGER]);
    // logi("vol:%d\n", (uint32_t)m_adc_data[ADC_CHARGER]*5*4.3/4096);
    logi("vol:%d\n", m_adc_data[ADC_CHARGER]/190);
    return (m_adc_data[ADC_CHARGER]/190 >= (9-1));
}

void en_5v_output(bool en)
{
    hw_gpio_output(EN_5V_OUTPUT, en);
}

void temp_ctr_outo(void)
{
    static temp_set = 5;
    static float error = 0;
    static float last_error = 0;
    static float integral = 0;
    static float derivative = 0;
    static float output = 0;
    static uint8_t fan_duty;
    static timer_t fan_timer=0;

        if(mSysTick - fan_timer > 500){//0.5S
            fan_timer = mSysTick;
          //  logd_r("temperature11=%d\r\n",temperature);
            if(temperature >= 15){
                output = 255;
            }else{
                // pd_req_vol = 5;
                // logd_r("11111");
                // if(pd_ready){
                //     PDO_Request(1);
                // }else{
                //     PD_Init();
                // }
                error = -(temp_set - temperature);
                integral += error;
                derivative = error - last_error;
                output = (KP * error) + (KI * integral) + (KD * derivative);
                last_error  = error;
               // logd_r("output=%d\r\n",output);
                if(output > 255){
                    output = 255;
                }else if(output < 0){
                    output = 0;
                }
              //  logi_r("error=%d,%d,%d,%d,%d\r\n",(temp_set - temperature),(int16_t)error,(int16_t)integral,(int16_t)derivative,(int16_t)output);
            }
        }
        fan_duty = (uint8_t)(output);                                             
        hw_pwm_set_duty(FAN_ID, fan_duty);
        temp_ctrl(fan_duty, 255);

}

void dev_ctrl(xmode_t mode)
{
    logi("dev_ctrl:%d\n", mode);
    switch (mode)
    {
    case LEVEL_0:
        otuo_crl = false;
        pd_flag = true;
        user_vender_deinit();
        Sleep_Wakeup_Cfg();
        MCU_Sleep_Wakeup_Operate();
        delay_ms(5000);
        user_vender_init();
        break;
    case LEVEL_2:
        // 9V
        pd_req_vol = 9;
        if(pd_ready){
            PDO_Request(2);
        }else{
            PD_Init();
        }
        otuo_crl = false;
        fan_ctrl(255);
        temp_ctrl(255, 255);
        display_Lx(1);
        t100_ms_cnt = 20;
        break;
    case LEVEL_3:
        fan_ctrl(255);
        temp_ctrl(255, 255);
        break;
    case LEVEL_1:
        pd_req_vol = 9;
        if(pd_ready){
            PDO_Request(2);
        }else{
            PD_Init();
        }
        display_Lx(2);
        otuo_crl = true;
        t100_ms_cnt = 20;
        break;
    }
}

void key_analyse(void)
{
    if(start_time <10)
    return;
    if(key_pressed_b & HW_KEY_B)
    {
        logi("key b pressed\n");
        if(++light >= LIGHT_END){
            light = LIGHT_RAINBOW;
        }
        light_ctrl(light, m_period);
        mstorep->light = light;
        storage_sync();
    }
    
    /* 开机默认狂暴模式，短按切智能温控模式, 再按关机。 */
    if(key_pressed_b & HW_KEY_A)
    {
        logi("key a pressed\n");
        mode--;
        dev_ctrl(mode);
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
    bool ret = false;
    uint8_t buff = 0;
    logd("zkm_decode_h:");dumpd(pDat,len);
        switch (pDat[2])
        {
            case CMD_READ_FAN_MODE:
                if (len > 4){                //set
                    mode = pDat[3];
                    dev_ctrl(mode);
                }else{
                    app_send_command(&mTrp_uart,CMD_READ_FAN_MODE,&mode,1);
                }
                ret = true;
                break;
            case RGB_USER_SETTING:
                if (len > 4){                //set
                    light = pDat[3];
                    mstorep->light = light;
                    storage_sync();
                    light_ctrl(light, m_period);
                }else{
                    app_send_command(&mTrp_uart,RGB_USER_SETTING,&light,1);
                }
                ret = true;
                break;
        }
    return ret;
}

void user_vender_init(void)
{
     hw_pwm_set_freq(FAN_ID, 100);
    logd_r("mstorep->light = %d",mstorep->light);
    if(mstorep->light == 0xff){
       mstorep->light = 0; 
    }
    mode = LEVEL_3;
    pd_req_vol = 12;
    if(pd_ready && !pd_flag){
        PDO_Request(3);
    }else{
        PD_Init();
    }
    enter = false;
    start_time = 0;
    t100_ms_cnt = 0;
    
    light = mstorep->light;
    storage_sync();
    dev_ctrl(mode);
}


void hw_user_vender_init(void)
{
    sev_seg_init();
    hw_gpio_cfg_output(EN_5V_OUTPUT);
    hw_gpio_output(EN_5V_OUTPUT, 1); //给手机供电
    
    hw_gpio_cfg_input(KEY_B_GPIO,HW_GPIO_PIN_PULLUP);
    hw_gpio_cfg_input(KEY_A_GPIO,HW_GPIO_PIN_PULLUP);
}

void user_vender_deinit(void)
{
    //关闭给手机供电
    hw_gpio_output(EN_5V_OUTPUT, 0);
    //关闭风扇
    fan_ctrl(0);
    //关闭散热器
    temp_ctrl(0, 0);
    //关闭氛围灯
    app_rgb_deinit();
    //关闭数码管
    sev_seg_deinit();
    
}

void user_task_handle(void)
{
    static time_t t;
    static timer_t temp_time = 0;
    static uint8_t s_temperature;

    if(start_time > 200 && !enter){
        enter = true;
        if(mode == LEVEL_3){
            
            mode = LEVEL_2;
            dev_ctrl(mode);
        }
    }else if(start_time > 1 && !enter && pd_ready && pd_flag){
        mode = LEVEL_3;
        logd_r("pdtongxin");
        pd_req_vol = 12;
        if(pd_ready){
            PDO_Request(3);
        }else{
            PD_Init();
        }
    }
    
    if(mSysTick - t > 100){
        t = mSysTick;
        start_time ++;
        if(t100_ms_cnt == 0){
            if(otuo_crl){
                temp_ctr_outo(); 
            }
        temperature = get_board_temperature();
        if(temperature != s_temperature){
            s_temperature = (temperature+s_temperature)/2;
            s_temperature = s_temperature;
           // app_send_command(&mTrp_uart, 0XF1, &temperature, 1);
        }
        if((temperature < 60)  && (temperature > 3)){
            display_dec(temperature);
        }
        if(temperature > 10){
            sleep_time = mSysTick; 
        }
        }else{
            t100_ms_cnt --;     
        }   
    }
    sev_disp_handle();
    //自动休眠
    if(mSysTick - sleep_time >= 1800000){
        user_vender_deinit();
        Sleep_Wakeup_Cfg();
        MCU_Sleep_Wakeup_Operate();
        user_vender_init();
    }

}

#endif
