#include "sdk_api_data.h"
#include <stdbool.h>
#include <stdint.h>
#include "ch32x035_flash.h"
bool sdk_api_data_erase(uint16_t offset)
{
     uint8_t s;
     uint8_t i;
     uint32_t Page_Address = HW_FLASH_PAGE_ADDR + offset; 
     FLASH_Unlock_Fast();
     s=FLASH_ROM_ERASE(Page_Address,Fsize*4);
      if(s != FLASH_COMPLETE)
    {
        printf("check FLASH_ADR_RANGE_ERROR FLASH_ALIGN_ERROR or FLASH_OP_RANGE_ERROR\r\n");
        FLASH_Lock_Fast();
        return 0;
    }
     FLASH_Lock_Fast();
     return true;
}
uint16_t sdk_api_data_write(uint16_t offset,uint8_t *buf,uint16_t len)
{
     uint16_t i;
     uint8_t s;
     uint32_t Page_Address = HW_FLASH_PAGE_ADDR + offset; 
     uint32_t sbuf[64];
     FLASH_Unlock_Fast();
     memset(sbuf,0xff,sizeof(sbuf));
   
      for(i=0;i<len;i++){
         sbuf[i]=(uint32_t)buf[i];
     }
     for(i=0;i<len;i++){
         printf("adr-%08x \r\n",sbuf[i]);
     }
    //Delay_Ms(2000);
     s = FLASH_ROM_WRITE(Page_Address,sbuf, Fsize*4);
      if(s != FLASH_COMPLETE)
    {
        printf("check FLASH_ADR_RANGE_ERROR FLASH_ALIGN_ERROR or FLASH_OP_RANGE_ERROR\r\n");
        FLASH_Lock_Fast();
        return 0;
    }
     FLASH_Lock_Fast();
    return len;
}
uint16_t sdk_api_data_read(uint16_t offset,uint8_t *buf,uint16_t len)
{
    uint16_t i;
    for(i=0;i<len;i++){
        *buf =*(uint32_t*)(HW_FLASH_PAGE_ADDR + offset +i*4);
        buf++;
    }
   return len;
}
uint8_t sdk_api_reset_reson_read(void)
{
    return 0;
}
void sdk_api_reset_reson_write(uint8_t val)
{

}
bool sdk_api_data_init(void)
{
    return true;
}
uint32_t sdk_api_get_flash_uuid( uint8_t *uuidbuf )
{
    return 0;
}
