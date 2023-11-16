#ifndef SDK_API_POWERMANAGER
#define SDK_API_POWERMANAGER
#include "sdk_typedef.h"


void sdk_api_weakup_init(void);
void sdk_api_get_reset_reson(void);
void sdk_api_set_reset_reson(uint8_t reson);
void  sdk_api_boot(void);
void sdk_api_stop(void);

#endif //SDK_API_POWERMANAGER
