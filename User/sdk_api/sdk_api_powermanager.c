#include <stdint.h>
#include "sdk_api_powermanager.h"
#include "debug.h"

void sdk_api_weakup_init(void)
{

}
void sdk_api_get_reset_reson(void)
{

}
void sdk_api_set_reset_reson(uint8_t reson)
{
}
void  sdk_api_boot(void)
{
	NVIC_SystemReset();
}
void sdk_api_reset(void)
{
	NVIC_SystemReset();
}
void sdk_api_stop(void)
{

}
void sdk_api_powermanager(void)
{

}
