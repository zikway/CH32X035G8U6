#include "hw_config.h"
#include "hw_board.h"
#include "hw_adc.h"
#include "hw_debug.h"

#include "debug.h"
#include "sdk_api_gpio.h"
/*
应用例程, 根据硬件定义map 和ID
#define HW_ADC_MAP_NUM  2
#define HW_ADC_MAP      {\
                        IO_PORTB_04,0UL,AD_CH_PB4,\
                        IO_PORTB_08,0UL,AD_CH_PB8,\
                        }


#define ADC_BATTERY_ID	0
#define ADC_LX_ID		1
#define ADC_LY_ID		2
#define ADC_RX_ID		3
#define ADC_RY_ID		4
#define ADC_L2_ID		5
#define ADC_R2_ID		6
*/


/*****************************************************************************************************
**  Function
******************************************************************************************************/
#define ADC_REFERENCE_VOLTAGE_MV          2600
#define ADC_REF_VOL_DIFFERENCE_MULT       2
#define ADC_TICK2VOL_REF_VOLTAGE_MV       (ADC_REFERENCE_VOLTAGE_MV * ADC_REF_VOL_DIFFERENCE_MULT)

extern void DMA_Tx_Init(DMA_Channel_TypeDef *DMA_CHx, u32 ppadr, u32 memadr, u16 bufsize);

uint8_t get_channel(const pin_map_t *p)
{
    return p->attribute;
}

uint8_t get_pin(pin_map_t *p)
{
    return p->pin;
}

bool sdk_api_get_adc_value(uint8_t id, const pin_map_t* p_map, uint16_t* vaulep)
{
    
    return false;
}


//单位mv
uint16_t sdk_api_adc_to_voltage(uint16_t adc)
{
    //todo

    return 1;
}


bool sdk_api_adc_isr_start(void)
{
	return true;
}

bool sdk_api_adc_init(const pin_map_t* p_map,uint8_t count)
{
    ADC_InitTypeDef  ADC_InitStructure = {0};
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    printf("sdk_api_adc_init\n");
    
    for(int i=0; i<count; i++)
    {
        RCC_APB2PeriphClockCmd(get_gpio_rcc(p_map[i].pin), ENABLE);
        GPIO_InitStructure.GPIO_Pin = get_gpio_pin(p_map[i].pin);
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
        GPIO_Init(GPIOA, &GPIO_InitStructure);
    }
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
    ADC_DeInit(ADC1);
    ADC_CLKConfig(ADC1, ADC_CLK_Div6);

    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
    ADC_InitStructure.ADC_ScanConvMode = ENABLE;
    ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
    ADC_InitStructure.ADC_NbrOfChannel = HW_ADC_MAP_NUM;
    ADC_Init(ADC1, &ADC_InitStructure);
    for(int i=0; i<= HW_ADC_MAP_NUM; i++)
    {
        ADC_RegularChannelConfig(ADC1, get_channel(&p_map[i]), i+1, ADC_SampleTime_11Cycles);
    }
    DMA_Tx_Init(DMA1_Channel1, (u32)&ADC1->RDATAR, (u32)m_adc_data, HW_ADC_MAP_NUM);
    DMA_Cmd(DMA1_Channel1, ENABLE);

    ADC_DMACmd(ADC1, ENABLE);
    ADC_Cmd(ADC1, ENABLE);
    ADC_SoftwareStartConvCmd(ADC1, ENABLE);

    return true;
}

bool sdk_api_adc_deinit(const pin_map_t* p_map, uint8_t count)
{
    p_map = p_map;
    count = count;
    return true;
}
