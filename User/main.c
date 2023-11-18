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


void TIM1_Init(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitTypeDef;
    TIM_TimeBaseInitTypeDef.TIM_Prescaler=48-1;   //Ԥ����
    TIM_TimeBaseInitTypeDef.TIM_CounterMode=TIM_CounterMode_Up;  // ���ϼ���
    TIM_TimeBaseInitTypeDef.TIM_Period=1000-1; //��װ��ֵ
    TIM_TimeBaseInitTypeDef.TIM_ClockDivision=TIM_CKD_DIV1; //������
    TIM_TimeBaseStructInit(&TIM_TimeBaseInitTypeDef);

    TIM_UpdateRequestConfig(TIM1,TIM_UpdateSource_Regular);//��������ж�Դ
    TIM_GenerateEvent(TIM1,TIM_EventSource_Update);   //��Ϊ���������¼�
    TIM_ITConfig(TIM1,TIM_IT_Update,ENABLE);              //�����ж�ʹ��

    NVIC_InitTypeDef NVIC_InitTypeDef;
    NVIC_InitTypeDef.NVIC_IRQChannel=TIM1_UP_IRQn;
    NVIC_InitTypeDef.NVIC_IRQChannelCmd=ENABLE;
    NVIC_InitTypeDef.NVIC_IRQChannelPreemptionPriority=1;
    NVIC_InitTypeDef.NVIC_IRQChannelSubPriority=1;
    NVIC_Init(&NVIC_InitTypeDef);



    TIM_Cmd(TIM1,ENABLE);                       //ʹ��TIM1

}


void DMA_Tx_Init(DMA_Channel_TypeDef *DMA_CHx, u32 ppadr, u32 memadr, u16 bufsize)
{
    DMA_InitTypeDef DMA_InitStructure = {0};

    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

    DMA_DeInit(DMA_CHx);
    DMA_InitStructure.DMA_PeripheralBaseAddr = ppadr;
    DMA_InitStructure.DMA_MemoryBaseAddr = memadr;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
    DMA_InitStructure.DMA_BufferSize = bufsize;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
    DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    DMA_Init(DMA_CHx, &DMA_InitStructure);
    DMA_Cmd(DMA1_Channel1, ENABLE);
}

/*********************************************************************
 * @fn      main
 *
 * @brief   Main program.
 *
 * @return  none
 */
int main(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    SystemCoreClockUpdate();
    TIM1_Init();
    USART_Printf_Init(1000000);

    logd_b("MODEL, VERSION:%s,V%x\n",DEFAULT_MODEL,SW_VERSION);
    gpad_board_init();
    gpad_init();
    while(1)
    {
        static uint32_t s_time = 0;
        if(mSysTick - s_time > 4){
            s_time = mSysTick;
            gpad_handle();
        }
    }
}

void TIM1_UP_IRQHandler(void)
{

    if((TIM_GetFlagStatus(TIM1,TIM_FLAG_Update))){
        TIM_ClearFlag(TIM1,TIM_FLAG_Update);
        mSysTick++;
    }
}
