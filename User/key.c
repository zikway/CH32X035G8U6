#include "key.h"


uint32_t key_pressed;
uint32_t key_pressed_b;     //后发
uint32_t key_pressed_double_b;      //后发
uint32_t key_pre_pressed_b;         //用于处理准备后发的按键

uint32_t key_press_short;
uint32_t key_press_long;
uint32_t key_press_long_long;







uint16_t    l2_value;
uint16_t    r2_value;
uint16_t    ly_value;
uint16_t    ry_value;
uint16_t    rx_value;
uint16_t    lx_value;






uint32_t iokey_scan(void)
{
    uint32_t value=0;


        #ifdef KEY_A_GPIO
            value |= (KEY_A? HW_KEY_A : HW_KEY_NULL);
        #endif
        #ifdef KEY_B_GPIO
            value |= (KEY_B? HW_KEY_B : HW_KEY_NULL);
        #endif
        #ifdef KEY_X_GPIO
            value |= (KEY_X? HW_KEY_X : HW_KEY_NULL);
        #endif
        #ifdef KEY_Y_GPIO
            value |= (KEY_Y? HW_KEY_Y : HW_KEY_NULL);
        #endif
        #ifdef KEY_L1_GPIO
            value |= (KEY_L1? HW_KEY_L1 : HW_KEY_NULL);
        #endif
        #ifdef KEY_R1_GPIO
            value |= (KEY_R1? HW_KEY_R1 : HW_KEY_NULL);
        #endif
        #ifdef ADC_L2_GPIO
            value |= (KEY_L2? HW_KEY_L2 : HW_KEY_NULL);
        #endif
        #ifdef ADC_R2_GPIO
            value |= (KEY_R2? HW_KEY_R2 : HW_KEY_NULL);
        #endif
        #ifdef KEY_L3_GPIO
            value |= (KEY_L3? HW_KEY_L3 : HW_KEY_NULL);
        #endif
        #ifdef KEY_R3_GPIO
            value |= (KEY_R3? HW_KEY_R3 : HW_KEY_NULL);
        #endif
        #ifdef KEY_UP_GPIO
            value |= (KEY_UP? HW_KEY_UP : HW_KEY_NULL);
        #endif
        #ifdef KEY_DOWN_GPIO
            value |= (KEY_DOWN? HW_KEY_DOWN : HW_KEY_NULL);
        #endif
        #ifdef KEY_LEFT_GPIO
            value |= (KEY_LEFT? HW_KEY_LEFT : HW_KEY_NULL);
        #endif
        #ifdef KEY_RIGHT_GPIO
            value |= (KEY_RIGHT? HW_KEY_RIGHT : HW_KEY_NULL);
        #endif

        #ifdef KEY_M1_GPIO
            value |= (KEY_M1? HW_KEY_M1 : HW_KEY_NULL);
        #endif
        #ifdef KEY_M2_GPIO
            value |= (KEY_M2? HW_KEY_M2 : HW_KEY_NULL);
        #endif
        #ifdef KEY_START_GPIO
            value |= (KEY_START? HW_KEY_START : HW_KEY_NULL);
        #endif
        #ifdef KEY_M4_GPIO
            value |= (KEY_M4? HW_KEY_M4 : HW_KEY_NULL);
        #endif
        #ifdef KEY_SELECT_GPIO
            value |= (KEY_SELECT? HW_KEY_SELECT : HW_KEY_NULL);
        #endif
        #ifdef KEY_CAPTURE_GPIO
            value |= (KEY_CAPTURE? HW_KEY_CAPTURE : HW_KEY_NULL);
        #endif


   return value;
}

/**
 * \return
 *      0: 没有新的按键值
 *      1: 有新的按键值
 */
uint8_t hw_key_scan(uint8_t *pkey,uint8_t len)
{
    static uint32_t key_old=0;
    static uint8_t scan_num = 0;
    static uint32_t vaild_value=0;
    static timer_t scankey_timer;

    uint32_t value=0;
    uint8_t ret = 0;

    if((mSysTick - scankey_timer) >= 10){
        scankey_timer = mSysTick;

        if(len > 4) len = 4;


        value |= iokey_scan();

        if(key_old == value){
            if(++scan_num == 1){
                if(vaild_value != value)
                {
                    vaild_value = value;
                    if(vaild_value){
                        ret = 1;
                        // printf("key=0x%x\r\n ",vaild_value);
                    }
                }
            }
        }else{
            key_old = value;
            scan_num = 0;
        }
    }
        memcpy(pkey,&vaild_value,len);

        return ret;
    }


uint8_t Scan_R2_trigger(void)
{
    if(gpad_key.r2 < 0.6){
        return 1;
    }
    return 0;
}


uint8_t Scan_L2_trigger(void)
{
    if(gpad_key.l2 < 0.6){
        return 1;
    }
    return 0;

}

uint8_t check_sum(void * buf,uint16_t len)
{
    uint16_t i;
    uint8_t sum = 0;
    uint8_t *p;
    p = (uint8_t *)buf;
    for(i=0;i<len;i++){
        sum += *(p+i);
    }
    return sum;
}
