
#include "hw_pwm.h"
#include "sdk_api_pwm.h"



bool sdk_api_pwm_set_low_high_time(const pin_map_t* p_map, uint32_t freq, uint8_t duty)
{
    //low_time, high time 的unit= 1/32M (second)
    //计算low_time
    uint32_t low_time, high_time;
    return true;
}

bool sdk_api_pwm_init(const pin_map_t* p_map,uint32_t freq, uint8_t duty)
{

    return 1;
}

bool sdk_api_pwm_deinit(const pin_map_t* p_map)
{
    return true;
}

