/************************************************************************************************************
**	FileName:	hw_board.c
**	Description:
**	Major Function:
**
--------------------------------------------------------------------------------------------------------
**	Software:
**	Hareware:
**
**	Company:	zikway
--------------------------------------------------------------------------------------------------------
**	Modified:

**	Author 			Date 			 Comment
_________________________________________________________________________________________________________
**	Cai			2020-03-26		Original
**
**
************************************************************************************************************/


#ifndef HW_BOARD_H
#define HW_BOARD_H

#include "hw_config.h"


#define STORAGE_MAP_MAX	0x200



#define INVALID_GPIO	(-1)			//		gpio 31 are not used

#if PROJECT_CJ_GAMEPAD
    #if K001
        #define USER_DEBUG_PIN              PB_03
        #define UART_BOARDATE               1000000

        #define TR_UART2_TX_PIN              PA_02
        #define TR_UART2_RX_PIN              PA_03

        #define KEY_A_GPIO                  PB_08
        #define KEY_B_GPIO                  PA_11
        #define KEY_Y_GPIO                  PB_06
        #define KEY_X_GPIO                  PB_02

        #define EN_PIN                       PA_16
    //    #define KEY_UP_GPIO                 PA_23
    //     #define KEY_DOWN_GPIO               PA_22
    //     #define KEY_LEFT_GPIO               PA_20
    //     #define KEY_RIGHT_GPIO              PC_06

       // #define KEY_M1_GPIO                 PA_12
        #define KEY_M2_GPIO                 PA_10
        //#define KEY_L1_GPIO                 PA_18     //LB
        #define KEY_M4_GPIO                 PB_10     //TURBO
        #define KEY_L3_GPIO                 PA_15     //L3
        #define KEY_R3_GPIO                 PA_09     //R3
        #define KEY_R1_GPIO                 PB_11     //RB
        #define KEY_START_GPIO               PB_09     //菜单键
        // #define KEY_CAPTURE_GPIO             PA_17    //截图
        // #define KEY_SELECT_GPIO             PA_19     //视窗

        #define MATRIX_KEY_IN_GPIO          {PA_17,PA_18}
        #define MATRIX_KEY_OUT_GPIO         {PA_19,PA_20,PB_07,PC_06}
         #define KEY_LOOKUP                  {\
            HW_KEY_SELECT, HW_KEY_CAPTURE,   HW_KEY_L1,      HW_KEY_M1,      \
             HW_KEY_UP, HW_KEY_DOWN,   HW_KEY_LEFT,      HW_KEY_RIGHT,      \
             }

        #define HW_ADC_MAP_NUM  8
        //lx,
        //ly,
        //rx,
        //ry,
        //lt,
        //rt,
        //手機電源VBUS
        //充電器電源VIN,
        #define HW_ADC_MAP      {\
            {PA_01, 0UL, 1},\
            {PA_00, 0UL, 0},\
            {PA_05, 0UL, 5},\
            {PA_06, 0UL, 6},\
            {PA_04, 0UL, 4},\
            {PB_01, 0UL, 9},\
            {PB_00, 0UL, 8},\
            {PA_07, 0UL, 7},\
            }

        #define ADC_LX_ID		0
        #define ADC_LY_ID		1
        #define ADC_RX_ID		2
        #define ADC_RY_ID		3
        #define ADC_L2_ID		4
        #define ADC_R2_ID		5



        #define IIC_GPIO_SDA_PIN_PORT       PA_14
        #define IIC_GPIO_SCK_PIN_PORT       PA_13


        #define SET_IIC_SDA_DIR_IN          GPIOA->CFGHR = GPIOA->CFGHR &(~0XF000000) | 0b1000<<((14-8)*4); GPIOA->BSHR |= 1<<14;
        #define SET_IIC_SDA_DIR_OUT         GPIOA->CFGHR = GPIOA->CFGHR &(~0XF000000) | 0b0010<<((14-8)*4);

        #define SET_IIC_SDA_HIGH            GPIOA->BSHR |= 1<<14;
        #define SET_IIC_SDA_LOW             GPIOA->BCR |= 1<<14;
        #define SET_IIC_SCL_HIGH            GPIOA->BSHR |= 1<<13;
        #define SET_IIC_SCL_LOW             GPIOA->BCR |= 1<<13;
        #define GET_IIC_SDA_DATA            (GPIOA->INDR & (1<<14))
        
        #define HW_PWM_MAP {\
            {PA_22, pwm_timer2,pwm_ch2 },\
            {PA_23, pwm_timer2,pwm_ch3 },\
        }
    #endif
    #if X108
        #define USER_DEBUG_PIN              PB_03
        #define UART_BOARDATE               1000000

        #define HW_PWM_MAP {\
            {PB_11, pwm_timer2,pwm_ch1 },\
            {PB_12, pwm_timer2,pwm_ch2 },\
        }
        #define FAN_ID                      0
        #define COOLER_ID                   1
        
        #define HW_ADC_MAP_NUM  3
        #define HW_ADC_MAP      {\
            {PA_01, 0UL, 1},\
            {PA_00, 0UL, 0},\
            {PC_00, 0UL, 10},\
            }

        #define ADC_NTC_ID		1   //外设探测NTC
        #define ADC_LY_ID		0   //半导体冷面NTC
        #define ADC_CHARGER		2   //检测充电器电压
 
        #define SEG_ROW_0_PIN PA_04
        #define SEG_ROW_1_PIN PA_05
        #define SEG_ROW_2_PIN PA_06
        #define SEG_ROW_3_PIN PA_07


        #define COL_0_PIN PB_06
        #define COL_1_PIN PB_07
        #define COL_2_PIN PB_08
        #define COL_3_PIN PB_09
        #define COL_4_PIN PB_10
        
        #define EN_5V_OUTPUT                 -1
        
        #define KEY_A_GPIO                   PB_04
        #define KEY_B_GPIO                   PB_00
        #define TR_UART2_TX_PIN              PA_02
        #define TR_UART2_RX_PIN              PA_03

    #endif

    #ifndef STORE_STRUCT_DEF
    #define     STORE_STRUCT_DEF    {\
        uint8_t flash_head;\
        uint8_t flash_res[5];\
        uint8_t gpad_mode;\
        uint8_t sub_mode;\
        uint8_t res[7];\
        int16_t rocker_cal[18];\
        uint8_t app_imu_data[13];\
    }
    #endif
#endif
#endif // HW_BOARD_H
