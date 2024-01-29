/********************************** (C) COPYRIGHT  *******************************
 * File Name          : iap.c
 * Author             : WCH
 * Version            : V1.0.0
 * Date               : 2023/04/06
 * Description        : IAP
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/
#include "iap.h"
#include "string.h"
#include "core_riscv.h"
#include "stdbool.h"
/******************************************************************************/
#define CRC 0xA5A5A5A5  //CRC校验值  前4个字节为first boot标志位，后4个字节为 OTA CRC校验值
#define FLASH_Base   0x08003000   
#define CRC_BASE     (FLASH_Base - 0X100)

#define USBD_DATA_SIZE   64
iapfun jump2app;
u32 Program_addr = FLASH_Base;
u32 Verity_addr = FLASH_Base;
u32 User_APP_Addr_offset = 0x5000;
u8 Verity_Star_flag = 0;
u8 Fast_Program_Buf[390];
u32 CodeLen = 0;
u8 End_Flag = 0;
u8 EP2_Rx_Buffer[IAP_RX_BUFF_SIZE];
#define  isp_cmd_t   ((t_iap_cmd  *)EP2_Rx_Buffer)
extern void UART2_SendMultiyData(u8* pbuf, u8 num);
extern uint32_t IapRxTimeout;
uint16_t IapReceLen;
uint16_t m_BackupAppBinLenth;
uint16_t m_BackupAppBinSum;
uint16_t m_send_len;
extern uint32_t end_time;



//calculate check sum from FLASH BASE to len
uint16_t iap_flash_calsum(uint16_t len)
{
    uint16_t i;
    uint8_t *p = (uint8_t *)FLASH_Base;
    uint16_t sum = 0;

    for(i=0; i<len; i++){
        sum += *(p+i);
    }
    return sum;
}
/*********************************************************************
 * @fn      CH32_IAP_Program
 *
 * @brief   adr - the date address
 *          buf - the date buffer
 *
 * @return  none
 */
void CH32_IAP_Program(u32 adr, u32* buf)
{
    u8 i;

    FLASH_BufReset();
    for(i=0; i<64; i++){
        FLASH_BufLoad(adr+4*i, buf[i]);
    }
    FLASH_ProgramPage_Fast(adr);
}

void clear_crc(void)
{
    Fast_Program_Buf[0] = 0xA5;
    Fast_Program_Buf[1] = 0xA5;
    Fast_Program_Buf[2] = 0x00;
    Fast_Program_Buf[3] = 0x00;
    FLASH_Unlock_Fast();
    FLASH_ErasePage_Fast(CRC_BASE);
    CH32_IAP_Program(CRC_BASE, (u32*)Fast_Program_Buf);
}

void set_crc(void)
{
    Fast_Program_Buf[0] = 0xA5;
    Fast_Program_Buf[1] = 0xA5;
    Fast_Program_Buf[2] = 0xA5;
    Fast_Program_Buf[3] = 0xA5;
    FLASH_Unlock_Fast();
    FLASH_ErasePage_Fast(CRC_BASE);
    CH32_IAP_Program(CRC_BASE, (u32*)Fast_Program_Buf);
}

bool check_first_boot(void)
{
    uint16_t crc = 0;
    crc = *(uint16_t *)CRC_BASE;
    if(crc == 0xA5A5)
        return false;
    else
        return true;
}

bool check_crc(void)
{
    uint32_t crc = 0;
    crc = *(uint32_t *)CRC_BASE;
    if(crc == CRC)
        return true;
    else
        return false;
}
/*********************************************************************
 * @fn      RecData_Deal
 *
 * @brief   UART-USB deal date
 *
 * @return  ERR_ERROR - ERROR
 *          ERR_SCUESS - SCUESS
 *          ERR_End - End
 */

u8 RecData_Deal(void)
{
     u8 i, s;
     u8 buf[20];
     u16 IapRecesum;
     switch ( isp_cmd_t->start.cmd) {
     case CMD_IAP_START:
         s = ERR_SCUESS;
         m_send_len = 4;
         buf[0] = 0xA5;
         buf[1] = m_send_len;
         buf[2] = isp_cmd_t->start.cmd;
         buf[3] = buf[0]+buf[1]+buf[2];
         UART2_SendMultiyData(buf, m_send_len);
         printf("S:\n");
        break;
     case CMD_IAP_ERASE:
         printf("E:\n");
         FLASH_Unlock_Fast();
         s = ERR_SCUESS;
         m_send_len = 4;
         buf[0] = 0xA5;
         buf[1] = m_send_len;
         buf[2] = isp_cmd_t->start.cmd;
         buf[3] = buf[0]+buf[1]+buf[2];
         UART2_SendMultiyData(buf, m_send_len);
         clear_crc();
         break;

     case CMD_IAP_PROM:
         printf("P:\n");
         for (i = 0; i < isp_cmd_t->program.len; i++) {
             Fast_Program_Buf[CodeLen + i] = isp_cmd_t->program.buf[i];
         }
         CodeLen += isp_cmd_t->program.len;
         IapReceLen += isp_cmd_t->program.len;;
         if (CodeLen >= 256) {
             FLASH_Unlock_Fast();
             FLASH_ErasePage_Fast(Program_addr);
             CH32_IAP_Program(Program_addr, (u32*) Fast_Program_Buf);
             CodeLen -= 256;
             for (i = 0; i < CodeLen; i++) {
                 Fast_Program_Buf[i] = Fast_Program_Buf[256 + i];
             }
             Program_addr += 0x100;
         }
         
         if(isp_cmd_t->program.len < 64){
            for (i = 0; i < (256 - CodeLen); i++) {
                 Fast_Program_Buf[CodeLen + i] = 0xff;
             }
             FLASH_Unlock_Fast();
             FLASH_ErasePage_Fast(Program_addr);
             CH32_IAP_Program(Program_addr, (u32*) Fast_Program_Buf);
             CodeLen = 0;
         }
         s = ERR_SCUESS;
        printf(".");
         break;

     case CMD_IAP_VERIFY:

         s = ERR_SCUESS;
         for (i = 0; i < isp_cmd_t->verify.len; i++) {
             if (isp_cmd_t->verify.buf[i] != *(u8*) (Verity_addr + i)) {
                 s = ERR_ERROR;
                 break;
             }
         }
         m_send_len = 4;
         buf[0] = 0xA5;
         buf[1] = m_send_len;
         buf[2] = isp_cmd_t->start.cmd;
         buf[3] = buf[0]+buf[1]+buf[2];
         UART2_SendMultiyData(buf, m_send_len);
         Verity_addr += isp_cmd_t->verify.len;

         break;

     case CMD_IAP_END:
         Verity_Star_flag = 0;
         End_Flag = 1;
         Program_addr = FLASH_Base;
         Verity_addr = FLASH_Base;

        m_BackupAppBinLenth = isp_cmd_t->program.buf[0] | (isp_cmd_t->program.buf[1] << 8);
        m_BackupAppBinSum 	= isp_cmd_t->program.buf[2] | (isp_cmd_t->program.buf[3] << 8);
        //m_Version 			= isp_cmd_t->program.buf[4];	
  
        IapRecesum = iap_flash_calsum(IapReceLen);
        m_send_len = 8;
         buf[0] = 0xA5;
         buf[1] = m_send_len;
         buf[2] = isp_cmd_t->start.cmd;
        if((IapRecesum != m_BackupAppBinSum) || (IapReceLen != m_BackupAppBinLenth))
        {
            //print unequal information
            printf("IapRecesum:%x\n", IapRecesum);
            printf("m_BackupAppBinSum:%x\n", m_BackupAppBinSum);
            printf("IapReceLen:%x\n", IapReceLen);
            printf("m_BackupAppBinLenth:%x\n", m_BackupAppBinLenth);
         buf[3] = 1;
            //m_iap_stu = IAP_NONE;
            printf("fail\n");
        }else{
            set_crc();
            printf("success\n");
         buf[3] = 0;
        }
        
        buf[4] = (IapReceLen & 0xff);
        buf[5] = (IapReceLen >>8);
        buf[6] = (IapRecesum & 0xff);	
        buf[7] = (IapRecesum >>8);	
        UART2_SendMultiyData(buf, m_send_len);
        s = ERR_End;
         break;

     default:
        printf("cmd err:%x\n", isp_cmd_t->start.cmd);
         s = ERR_ERROR;
         break;
     }

     return s;
}

/*********************************************************************
 * @fn      GPIO_Cfg_init
 *
 * @brief   GPIO init
 *
 * @return  none
 */
void GPIO_Cfg_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

}

/*********************************************************************
 * @fn      GPIO_Cfg_Float
 *
 * @brief   GPIO float
 *
 * @return  none
 */
void GPIO_Cfg_Float(void)
{
    GPIO_DeInit(GPIOA);
    GPIO_DeInit(GPIOC);
    GPIO_AFIODeInit();
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC, DISABLE);
}

/*********************************************************************
 * @fn      PA0_Check
 *
 * @brief   Check PA0 state
 *
 * @return  1 - IAP
 *          0 - APP
 */
u8 PA0_Check(void)
{
    u8 i, cnt=0;

    GPIO_Cfg_init();

    for(i=0; i<10; i++){
        if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0)==0) cnt++;
        Delay_Ms(5);
    }

    if(cnt>6) return 0;
    else return 1;
}

/*********************************************************************
 * @fn      USART2_CFG
 *
 * @brief   baudrate:UART2 baudrate
 *
 * @return  none
 */
void USART2_CFG(u32 baudrate)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    USART_InitTypeDef USART_InitStructure = {0};

    RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    USART_InitStructure.USART_BaudRate = baudrate;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl =
    USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;

    USART_Init(USART2, &USART_InitStructure);
    USART_Cmd(USART2, ENABLE);
}
/*********************************************************************
 * @fn      UART2_SendMultiyData
 *
 * @brief   Deal device Endpoint 3 OUT.
 *
 * @param   l: Data length.
 *
 * @return  none
 */
void UART2_SendMultiyData(u8* pbuf, u8 num)
{
    u8 i = 0;

    while(i<num)
    {
        while(USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET);
        USART_SendData(USART2, pbuf[i]);
        i++;
    }
}
/*********************************************************************
 * @fn      UART2_SendMultiyData
 *
 * @brief   USART2 send date
 *
 * @param   pbuf - Packet to be sent
 *          num - the number of date
 *
 * @return  none
 */
void UART2_SendData(u8 data)
{
    while(USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET);
    USART_SendData(USART2, data);
}

/*********************************************************************
 * @fn      Uart2_Rx
 *
 * @brief   Uart2 receive date
 *
 * @return  none
 */
u8 Uart2_Rx(void)
{
    while( USART_GetFlagStatus(USART2, USART_FLAG_RXNE) == RESET);
    return USART_ReceiveData( USART2);
}

uint16_t calculate_check_sum(uint8_t *buf, uint16_t len)
{
	uint16_t i;
	uint16_t sum = 0;

	for(i=0; i<len; i++){
		sum += *(buf+i);
	}
	return sum;
}

/*********************************************************************************
** Function:			
** Parameters:		
** Returns:			
*********************************************************************************/
bool  get_uart_command(uint8_t c)
{
	static uint8_t* s_buf = EP2_Rx_Buffer;
	static uint8_t s_len=0;

	uint8_t ret = false;

	if(0 != s_len) {
		s_buf[s_len] = c;
		s_len++;
		if (s_len == 2){
			if(HEADCMD == (c)){
				s_buf[0] = c;
				s_len = 1;
			}else if(s_buf[1] > IAP_RX_BUFF_SIZE){		
				printf("len er");
				s_len = 0;
			}
		}else if(s_len >= s_buf[1]){
			uint8_t sum;        //????��???
			sum = calculate_check_sum(s_buf, (uint16_t)(s_len-1));
			if(s_buf[s_len-1] == sum){
				ret = true;
			}else{
			    printf("sum er\n");
			}
			s_len = 0;
		}
	}else{
		if(HEADCMD == c){
			s_buf[0] = c;
			s_len = 1;
		}
	}
	return ret;
}

/*********************************************************************
 * @fn      UART_Rx_Deal
 *
 * @brief   UART Rx data deal
 *
 * @return  none
 */
void UART_Rx_Deal(void)
{
    u8 s;

    if(get_uart_command(Uart2_Rx()))
    {

        s = RecData_Deal();
        if(s== ERR_SCUESS){
            IapRxTimeout = 0;
            end_time = 800000;
        }else{
        }
    }
}

void SW_Handler(void) __attribute__((interrupt("WCH-Interrupt-fast")));

/*********************************************************************
 * @fn      SW_Handler
 *
 * @brief   This function handles Software exception.
 *
 * @return  none
 */
void SW_Handler(void) {
    __asm("li  a6, 0x3000");
    __asm("jr  a6");

    while(1);
}


