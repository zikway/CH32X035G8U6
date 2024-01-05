/********************************** (C) COPYRIGHT  *******************************
 * File Name          : iap.h
 * Author             : WCH
 * Version            : V1.0.0
 * Date               : 2023/04/06
 * Description        : IAP
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/
#ifndef __IAP_H
#define __IAP_H

#include "ch32x035.h"
#include "stdio.h"
#include "ch32x035_usbfs_device.h"
#include "stdbool.h"

#define USBD_DATA_SIZE               64

#define BUILD_UINT16(loByte, hiByte) ((UINT16)(((loByte) & 0x00FF) | (((hiByte) & 0x00FF) << 8)))
#define BUILD_UINT32(Byte0, Byte1, Byte2, Byte3) \
          ((UINT32)((UINT32)((Byte0) & 0x00FF) \
          + ((((UINT32)Byte1) & 0x00FF) << 8) \
          + ((((UINT32)Byte2) & 0x00FF) << 16) \
          + ((((UINT32)Byte3) & 0x00FF) << 24)))

#define Uart_Sync_Head1   0x57
#define Uart_Sync_Head2   0xab


#define CMD_IAP_START            0x80
#define CMD_IAP_PROM             0x81
#define CMD_IAP_ERASE            0x82
#define CMD_IAP_VERIFY           0x83
#define CMD_IAP_END              0x84
#define CMD_IAP_BACKUP_START     0X86
#define CMD_IAP_BACKUP_DATA      0X88


#define ERR_SCUESS        0x00
#define ERR_ERROR         0x01
#define ERR_End           0x02


#define IAP_RX_BUFF_SIZE    (64+7)  //64:数据包最大长度 7:包头校验等信息长度
#define HEADCMD             0xA5    //包头

/********************************************************************************/
/*
  命令格式说明:
1，CMD_IAP_END(IAP结束标志):
    此命令格式如下:
        CMD IAP END+1字节数据长度+2字节状态 (保留)
    返回数据:
        无
2，CMD_IAP_VERIFY(IAP校验命令):
    此命令格式如下:
        CMD IAP VERIFY+1字节数据长度+2字节数据地址+后续数据
    返回数据:
        正确:0x0000
        错误:0x0001
3，CMD_IAP_PROM(IAP编程命令):
    此命令格式如下:
        CMD_IAP_PROM+1字节数据长度+2字节数据地址+后续数据.注意此命令发下来的数据个数必须为4字节的整数倍
    返回数据:
        正确:0x0000
        错误:0x0000
4，CMD IAP ERASE(IAP擦除命令):
    此命令格式如下:
        CMD_IAP_ERASE+1字节数据长度+2字节开始地址 (一般为xeeee) ，此命令只需要发送一次
    返回数据:
        正确:0x0000
        错误:0x0001
*/



typedef union __attribute__ ((aligned(4)))
{
    struct
    {
        uint8_t     head;
        uint8_t     lenth;                 //所有数据长度
        uint8_t     cmd;                   /* 命令码 0x80 开始指令*/
        uint8_t     len;                   /* 后续数据长度 */
        uint8_t     addr[2];
        uint8_t     sum;
    } start;  
    
    struct
    {
        uint8_t     head;
        uint8_t     lenth;                 //所有数据长度
        uint8_t     cmd;                   /* 命令码 0x81 */
        uint8_t     len;                   /* 后续数据长度 */
        uint8_t     addr[2];               /* 擦除地址 */
        uint8_t     sum;
    } erase;                            /* 擦除命令 */
    
    struct
    {
        uint8_t     head;
        uint8_t     lenth;                 //所有数据长度
        uint8_t     cmd;                   /* 命令码 0x83 */
        uint8_t     len;                   /* 后续数据长度 */
        uint8_t     addr[2];              
        uint8_t     sum;
    } end;                              /* 结束命令 */
    
    struct
    {
        uint8_t     head;
        uint8_t     lenth;                 //所有数据长度
        uint8_t     cmd;                   /* 命令码 0x82 */
        uint8_t     len;                   /* 后续数据长度 */
        uint8_t     addr[2];               /* 校验地址 */
        uint8_t     buf[IAP_RX_BUFF_SIZE-7];        /* 校验数据 */
        uint8_t     sum;
    } verify;                           /* 校验命令 */
    
    struct
    {
        uint8_t     head;
        uint8_t     lenth;                 //所有数据长度
        uint8_t     cmd;                   /* 命令码 0x80 */
        uint8_t     len;                   /* 后续数据长度 */
        uint8_t     addr[2];               /* 地址 */
        uint8_t     buf[IAP_RX_BUFF_SIZE-7];/* 后续数据 */
        uint8_t     sum;
    } program;                          /* 编程命令 */

    uint8_t    rxbuf[IAP_RX_BUFF_SIZE]; /* 接收数据包*/
} t_iap_cmd;



typedef void (*iapfun)(void);
extern u32 Program_Verity_addr;
extern u32 User_APP_Addr_offset;
extern u8 EP2_IN_Flag;
extern u8 EP2_OUT_Flag;
extern u16 EP2_Rx_Cnt;
extern u8 EP2_Tx_Buffer[2];
extern u16 EP2_Tx_Cnt;
extern u8 EP2_Rx_Buffer[IAP_RX_BUFF_SIZE];
extern u8 End_Flag;

u8 RecData_Deal(void);
void GPIO_Cfg_init(void);
void GPIO_Cfg_Float(void);
u8 PA0_Check(void);
void USART2_CFG(u32 baudrate);

void UART_Rx_Deal(void);
bool check_crc(void);
bool check_first_boot(void);
void clear_crc(void);
void set_crc(void);
#endif


