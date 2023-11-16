/********************************** (C) COPYRIGHT *******************************
 * File Name          : main.c
 * Author             : WCH
 * Version            : V1.0.0
 * Date               : 2023/04/06
 * Description        : Main program body.
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/

/*
 *@Note
 *GPIO routine:
 *PA0 push-pull output.
 *
 ***Only PA0--PA15 and PC16--PC17 support input pull-down.
 */
#include "main.h"
#include "debug.h"
#include "hw_debug.h"
#include "gpad.h"
/* Global define */

/* Global Variable */

/*********************************************************************
 * @fn      GPIO_Toggle_INIT
 *
 * @brief   Initializes GPIOA.0
 *
 * @return  none
 */
#define pack_head1      0x05
#define type1           0x02
volatile uint32_t mSysTick;   //1ms
key_t_pack key_pack;
key_t_pack *p;
gamepad_key_t gpad_key;
uint16_t adc_date[6]={0};
extern u16 TxBuf[1024];


/*********************************************************************
 * @fn      main
 *
 * @brief   Main program.
 *
 * @return  none
 */
int main(void)
{
    uint32_t count = 0;
    uint32_t value = 0;
    key_pack.pack_head = pack_head1;
    key_pack.type = type1;
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    APP_Board_Init();
    gpad_board_init();
    printf("K001_V01\r\n");

    while(1)
    {
        gpad_handle();
    }
}

void TIM1_UP_IRQHandler(void)
{

    if((TIM_GetFlagStatus(TIM1,TIM_FLAG_Update))){
        TIM_ClearFlag(TIM1,TIM_FLAG_Update);
        ADC_SoftwareStartConvCmd(ADC1,ENABLE);
        gpad_key.rx=adc_date[0];
        gpad_key.lx=adc_date[1];
        gpad_key.ly=adc_date[2];
        gpad_key.ry=adc_date[3];
        gpad_key.l2=adc_date[4];
        gpad_key.r2=adc_date[5];
       // printf("adc_date[%d]=%d\r\n",adc_date[0]);
        mSysTick++;
    }
}

//void ADC1_IRQHandler(void)
//{
//    uint8_t adc_flag=0;
//    if(ADC_GetFlagStatus(ADC1,ADC_FLAG_EOC)){
//        ADC_ClearFlag(ADC1,ADC_FLAG_EOC);
//        adc_flag++;
//        if(adc_flag==1){
//            adc_date[0]=ADC_GetConversionValue(ADC1);
//        }
//        if(adc_flag==2){
//            adc_date[1]=ADC_GetConversionValue(ADC1);
//                }
//        if(adc_flag==3){
//            adc_date[2]=ADC_GetConversionValue(ADC1);
//                }
//        if(adc_flag==4){
//            adc_date[3]=ADC_GetConversionValue(ADC1);
//                }
//        if(adc_flag==5){
//            adc_date[4]=ADC_GetConversionValue(ADC1);
//                }
//        if(adc_flag==6){
//            adc_date[5]=ADC_GetConversionValue(ADC1);
//            adc_flag=0;
//                }
//
//
//    }






//}
