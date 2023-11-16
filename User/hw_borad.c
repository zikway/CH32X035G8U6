#include "hw_borad.h"



u16 TxBuf[1024];


void KEY_Init(void)
{
//        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
//        GPIO_InitTypeDef  GPIO_InitStructure;
//        GPIO_InitStructure.GPIO_Pin = KEY_A_GPIO | KEY_Y_GPIO | KEY_X_GPIO | KEY_M4_GPIO | KEY_R1_GPIO | KEY_START_GPIO | ADC_R2_GPIO;
//        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
//        GPIO_Init(GPIOB, &GPIO_InitStructure);
//
//        GPIO_InitStructure.GPIO_Pin = KEY_B_GPIO | KEY_UP_GPIO | KEY_DOWN_GPIO | KEY_LEFT_GPIO | KEY_M1_GPIO | KEY_M2_GPIO | KEY_L1_GPIO | KEY_L3_GPIO | KEY_R3_GPIO | KEY_SELECT_GPIO | KEY_CAPTURE_GPIO;
//        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
//        GPIO_Init(GPIOA, &GPIO_InitStructure);


}


void ADC1_Init(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
    GPIO_InitTypeDef  GPIO_InitStructure;
//    GPIO_InitStructure.GPIO_Pin = ADC_R2_GPIO;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    //GPIO_InitStructure.GPIO_Pin = ADC_LX_GPIO | ADC_RX_GPIO | ADC_LY_GPIO | ADC_L2_GPIO | ADC_RY_GPIO;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    ADC_InitTypeDef ADC_InitStructure;
    ADC_DeInit(ADC1);

    ADC_CLKConfig(ADC1, ADC_CLK_Div6);

    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
    ADC_InitStructure.ADC_ScanConvMode = ENABLE;
    ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
    ADC_InitStructure.ADC_NbrOfChannel = 6;
    ADC_Init(ADC1, &ADC_InitStructure);

    ADC_DMACmd(ADC1, ENABLE);
    ADC_Cmd(ADC1, ENABLE);

    ADC_Init(ADC1,&ADC_InitStructure);
    ADC_RegularChannelConfig(ADC1,ADC_Channel_7,1,ADC_SampleTime_11Cycles);//LX
    ADC_RegularChannelConfig(ADC1,ADC_Channel_6,2,ADC_SampleTime_11Cycles);//RX
    ADC_RegularChannelConfig(ADC1,ADC_Channel_1,3,ADC_SampleTime_11Cycles);//LY
    ADC_RegularChannelConfig(ADC1,ADC_Channel_8,4,ADC_SampleTime_11Cycles);//RY
    ADC_RegularChannelConfig(ADC1,ADC_Channel_4,5,ADC_SampleTime_11Cycles);//L2
    ADC_RegularChannelConfig(ADC1,ADC_Channel_5,6,ADC_SampleTime_11Cycles);//R2
   ADC_SoftwareStartConvCmd(ADC1, ENABLE);

    ADC_Cmd(ADC1,ENABLE);
}


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
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
    DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    DMA_Init(DMA_CHx, &DMA_InitStructure);
    DMA_Cmd(DMA1_Channel1, ENABLE);
}



void APP_Board_Init(void)
{
    SystemCoreClockUpdate();
    // KEY_Init();
    // ADC1_Init();
    TIM1_Init();
    USART_Printf_Init(1000000);
    // USART2_Printf_Init(115200);
    // USART2_Printf_Init(115200);
   // DMA_Tx_Init(DMA1_Channel1, (u32)&ADC1->RDATAR, (u32)adc_date, 6);
    printf("board Init\n");


}

