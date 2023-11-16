#ifndef SDK_API_PWM_H
#define SDK_API_PWM_H

#include "common_type.h"
#include "hw_gpio.h"


#define SDI_API_PWM_USING_LOW_HIGH_TIME

//pwm attribute

/**
 * bit00: PWM_CHANNEL
 * bit01: PWM_CHANNEL
 * bit02: PWM_CHANNEL
 * bit03: PWM_CHANNEL
 * bit04: PWM_CHANNEL
 * bit05: PWM_CHANNEL
 * bit06: PWM_CHANNEL
 * bit07: PWM_CHANNEL
 * bit08: PWM_INVERT
 */
#define PWM_CHANNEL_MSK         0Xff
#define PWM_INVERT_ENABLED		0X0100
/*
#define HW_PWM_MAP \
{\
{GPIO_NUM,   PIN_MODE,     Channel+invert},\
{31,   3,     4|PWM_INVERT_ENABLED},\
*/

bool sdk_api_pwm_set_low_high_time(const pin_map_t* p_map, uint32_t freq, uint8_t duty);
bool sdk_api_pwm_init(const pin_map_t* p_map,uint32_t freq, uint8_t duty);
bool sdk_api_pwm_deinit(const pin_map_t* p_map);
uint16_t sdk_api_pwm_set_pwm_duty(const pin_map_t* p_map, uint8_t duty);
bool sdk_api_pwm_set_pwm_freq(const pin_map_t* p_map, uint32_t hz);

#endif //USER_DRIVER_HW_MC_PWM_H
