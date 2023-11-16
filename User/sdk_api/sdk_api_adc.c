#include "hw_config.h"
#include "hw_board.h"
#include "hw_adc.h"
#include "hw_debug.h"



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

bool sdk_api_get_adc_value(uint8_t id, const pin_map_t* p_map, uint16_t* vaulep)
{
    uint32_t channel = p_map->attribute & ADC_CHANNEL_MASK;
    //todo
// 	if(NULL != vaulep) *vaulep = adc;
    return true;
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
    uint8_t i;
	uint32_t channel;
	uint32_t pull;
	uint32_t pin;

    return true;
}

bool sdk_api_adc_deinit(const pin_map_t* p_map, uint8_t count)
{
    p_map = p_map;
    count = count;
    return true;
}
