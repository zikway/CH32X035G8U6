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
#include "hw_debug.h"
#include "debug.h"
GPIO_TypeDef * get_gpio_port(pin_t io)
{
    GPIO_TypeDef * base;
    switch (io>>16)
    {
    case 0:
        base = GPIOA;
        break;
    case 1:
        base = GPIOB;
        break;
    case 2:
        base = GPIOC;
        break;
    default:
        break;
    }
    return base;
}

uint32_t get_gpio_rcc(pin_t io)
{
    uint32_t rcc;
    switch (io>>16)
    {
    case 0:
        rcc = RCC_APB2Periph_GPIOA;
        break;
    case 1:
        rcc = RCC_APB2Periph_GPIOB;
        break;
    case 2:
        rcc = RCC_APB2Periph_GPIOC;
        break;
    default:
        break;
    }
    return rcc;
}

uint32_t get_gpio_pin(pin_t io)
{
    return 1<<(io&0xffff);
}
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
    if((uint32_t)-1 == io) return;
    GPIO_TypeDef * gpiox = get_gpio_port((pin_t)io);

    //config io as gpio input
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    RCC_APB2PeriphClockCmd(get_gpio_rcc((pin_t)io), ENABLE);

    GPIO_InitStructure.GPIO_Pin = get_gpio_pin((pin_t)io);
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    
    if(pull_config == HW_GPIO_PIN_PULLDOWN){
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
    }else if(pull_config == HW_GPIO_PIN_PULLUP){
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    }else{
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    }
    GPIO_Init(gpiox, &GPIO_InitStructure);
}


uint32_t sdk_api_gpio_input(uint32_t io)
{
    return GPIO_ReadInputDataBit(get_gpio_port((pin_t)io), get_gpio_pin((pin_t)io));
}


void sdk_api_gpio_cfg_output(uint32_t io)
{
    if((uint32_t)-1 == io) return;
    if((uint32_t)-1 == io) return;
    GPIO_TypeDef * gpiox = get_gpio_port(io);

    //config io as gpio input
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    RCC_APB2PeriphClockCmd(get_gpio_rcc(io), ENABLE);

    GPIO_InitStructure.GPIO_Pin = get_gpio_pin(io);
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(gpiox, &GPIO_InitStructure);
}

void sdk_api_gpio_output(uint32_t io,uint8_t val)
{
    GPIO_WriteBit(get_gpio_port(io), get_gpio_pin(io), val);
}

void sdk_api_gpio_cfg_mode(uint32_t io,uint8_t mode)
{
    //NULL
}
