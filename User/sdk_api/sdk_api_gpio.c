/************************************************************************************************************
**	FileName:	sdk_api_gpio.c
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
#include "sdk_api_gpio.h"
#include "hw_gpio.h"

/**
 * \brief Function for initializing the FIFO.
 *
 * \param[in]  io
 *             \arg \ref  IO_PORTA_00
 *             \arg \ref  IO_PORTA_01
 *             \arg \ref  IO_PORTB_00
 *             \arg \ref  IO_PORTB_01
 *             \arg \ref  IO_PORTC_00
 *             \arg \ref  IO_PORTC_01
 *             \arg \ref  IO_PORTD_00
 *             \arg \ref  IO_PORTD_01
 * \param[in]  pull_config
 *             This parameter can be one of the following values:
 *             \arg \ref  HW_GPIO_PIN_NOPULL
 *             \arg \ref  HW_GPIO_PIN_PULLUP
 *             \arg \ref  HW_GPIO_PIN_PULLDOWN
 * \retval     void
 */
void sdk_api_gpio_cfg_input(uint32_t io, uint8_t pull_config)
{
    uint8_t pull;
    if((uint32_t)-1 == io) return;

    // uapi_pin_set_mode(io, HAL_PIO_FUNC_GPIO);

    // gpio_select_core(io, CORES_APPS_CORE);

    // uapi_gpio_set_dir(io, GPIO_DIRECTION_INPUT);

    // if(pull_config == HW_GPIO_PIN_PULLDOWN){
    //     pull = PIN_PULL_DOWN;
    // }else if(pull_config == HW_GPIO_PIN_PULLUP){
    //     pull = PIN_PULL_UP;
    // }else{
    //     pull = PIN_PULL_NONE;
    // }
    // uapi_pin_set_pull(io, pull);
}


uint32_t sdk_api_gpio_input(uint32_t io)
{
    return 1;
}


void sdk_api_gpio_cfg_output(uint32_t io)
{
    if((uint32_t)-1 == io) return;
    // uapi_pin_set_mode(io, HAL_PIO_FUNC_GPIO);
    // uapi_gpio_set_dir(io, GPIO_DIRECTION_OUTPUT);
    // uapi_gpio_set_val(io, GPIO_LEVEL_LOW);
}

void sdk_api_gpio_output(uint32_t io,uint8_t val)
{
    // if(val)
    // uapi_gpio_set_val(io, GPIO_LEVEL_HIGH);
    // else
    // uapi_gpio_set_val(io, GPIO_LEVEL_LOW);
}

void sdk_api_gpio_cfg_mode(uint32_t io,uint8_t mode)
{
    //NULL
}
