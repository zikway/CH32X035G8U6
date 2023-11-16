#ifndef HW_BORAD_H
#define HW_BORAD_H


#include "ch32x035.h"
#include "debug.h"
#include "main.h"
extern uint16_t adc_date[6];

                                          /*       KEY≈‰÷√      PB                                         */
       //KEY
       #define KEY_A_GPIO                  GPIO_Pin_8
       #define KEY_Y_GPIO                  GPIO_Pin_6
       #define KEY_X_GPIO                  GPIO_Pin_2
       #define KEY_M4_GPIO                 GPIO_Pin_10     //TURBO
       #define KEY_R1_GPIO                 GPIO_Pin_11     //RB
       #define KEY_START_GPIO              GPIO_Pin_9    //≤Àµ•º¸
       #define ADC_R2_GPIO                 GPIO_Pin_1

               /*             PA                                            */

       #define KEY_B_GPIO                GPIO_Pin_11
       #define KEY_UP_GPIO                 GPIO_Pin_23
       #define KEY_DOWN_GPIO               GPIO_Pin_22
       #define KEY_LEFT_GPIO               GPIO_Pin_20

       #define KEY_M1_GPIO                 GPIO_Pin_12
       #define KEY_M2_GPIO                 GPIO_Pin_10
       #define KEY_L1_GPIO                 GPIO_Pin_18     //LB

       #define KEY_L3_GPIO                 GPIO_Pin_15     //L3
       #define KEY_R3_GPIO                 GPIO_Pin_9     //R3
       #define KEY_SELECT_GPIO             GPIO_Pin_17     // ”¥∞
       #define KEY_CAPTURE_GPIO            GPIO_Pin_19     //ΩÿÕº

       #define ADC_LX_GPIO                 GPIO_Pin_1
       #define ADC_RX_GPIO                 GPIO_Pin_5
       #define ADC_LY_GPIO                 GPIO_Pin_0
       #define ADC_RY_GPIO                 GPIO_Pin_6
       #define ADC_L2_GPIO                 GPIO_Pin_4


       #define LEDS                        GPIO_Pin_16
       #define RES                         GPIO_Pin_21
       #define IIC_SDA                     GPIO_Pin_14
       #define IIC_SCL                     GPIO_Pin_13
                  /*             PC                                            */
       #define KEY_RIGHT_GPIO              GPIO_Pin_6






void APP_Board_Init(void);
void TIM1_Init(void);
void ADC1_Init(void);
void KEY_Init(void);
void DMA_Tx_Init(DMA_Channel_TypeDef *DMA_CHx, u32 ppadr, u32 memadr, u16 bufsize);





#endif
