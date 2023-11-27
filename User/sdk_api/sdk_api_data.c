#include "sdk_api_data.h"
#include <stdbool.h>
#include <stdint.h>
#include "ch32x035_flash.h"
#include <string.h>
bool sdk_api_data_erase(uint16_t offset)
{
     uint8_t s;
     uint32_t Page_Address = HW_FLASH_PAGE_ADDR + offset; 
     s=FLASH_ROM_ERASE(Page_Address, HW_FLASH_PAGE_SIZE);
      if(s != FLASH_COMPLETE)
    {
        printf("check FLASH_ADR_RANGE_ERROR FLASH_ALIGN_ERROR or FLASH_OP_RANGE_ERROR\r\n");
        return 0;
    }
     return true;
}
uint16_t sdk_api_data_write(uint16_t offset,uint8_t *buf,uint16_t len)
{
     uint8_t s;
     uint32_t Page_Address = HW_FLASH_PAGE_ADDR + offset; 
     uint32_t sbuf[64];
     memset(sbuf,0xff,sizeof(sbuf));
     memcpy(sbuf,buf,len);
     s = FLASH_ROM_WRITE(Page_Address,sbuf, HW_FLASH_PAGE_SIZE);
      if(s != FLASH_COMPLETE)
    {
        printf("check FLASH_ADR_RANGE_ERROR FLASH_ALIGN_ERROR or FLASH_OP_RANGE_ERROR\r\n");
        return 0;
    }
    return len;
}
uint16_t sdk_api_data_read(uint16_t offset,uint8_t *buf,uint16_t len)
{
    //copy data from flash to buf, uint32 to uint8
    uint32_t data;

    for(int i=0;i<(len+3)/4;i++){
        data = *(uint32_t*)(HW_FLASH_PAGE_ADDR + offset +i*4);
        for(int j=0;j<4;j++){
            if(i*4 + j >= len){
                break;
            }
            buf[i*4+j] = data >> (j*8);
        }
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
