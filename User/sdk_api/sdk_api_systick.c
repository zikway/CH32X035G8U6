#include "sdk_api_systick.h"

#include <stdint.h>
#include "debug.h"
void sdk_systick_init(void)
{
    
}
void sdk_delay_ns(unsigned int ns )
{
    unused(ns);
}
void sdk_delay_us(unsigned int us )
{
    Delay_Us(us);
}
void sdk_delay_ms(unsigned int ms)
{
    Delay_Ms(ms);
}

