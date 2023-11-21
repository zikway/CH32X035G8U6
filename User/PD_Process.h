/********************************** (C) COPYRIGHT *******************************
* File Name          : PD_Process.h
* Author             : WCH
* Version            : V1.0.0
* Date               : 2023/04/06
* Description        : This file contains all the functions prototypes for the
*                      PD library.
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/

#ifndef USER_PD_PROCESS_H_
#define USER_PD_PROCESS_H_
#include "debug.h"
#include "stdbool.h"
#ifdef __cplusplus
 extern "C" {
#endif

/******************************************************************************/

enum {
	SNK,
	SRC
};

/* Variable extents */
extern uint8_t  Tmr_Ms_Cnt_Last;
extern uint8_t  Tmr_Ms_Dlt;
extern uint8_t  Tim_Ms_Cnt;

extern uint8_t  PDO_Len;
extern PD_CONTROL PD_Ctl;

extern uint8_t send_data[ ];
extern uint8_t PD_Ack_Buf[ ];

extern __attribute__ ((aligned(4))) uint8_t PD_Rx_Buf[ 34 ];
extern __attribute__ ((aligned(4))) uint8_t PD_Tx_Buf[ 34 ];

extern bool pd_mode;

/***********************************************************************************************************************/
/* Function extensibility */
extern void PD_Rx_Mode( void );
extern void PD_SRC_Init( void );
extern void PD_SINK_Init( void );
extern void PD_PHY_Reset( void );
extern void PD_Init( void );
extern uint8_t PD_SRC_Detect( void );
extern uint8_t PD_Detect( void );
extern void PD_Det_Proc( void );
extern void PD_SRC_Det_Proc( void );
extern void PD_Load_Header( uint8_t ex, uint8_t msg_type );
extern uint8_t PD_Send_Handle( uint8_t *pbuf, uint8_t len );
extern void PD_Phy_SendPack( uint8_t mode, uint8_t *pbuf, uint8_t len, uint8_t sop );
extern void PD_Main_Proc( void );
extern void PD_SRC_Main_Proc( void );
extern void PD_Request_Analyse( uint8_t pdo_idx, uint8_t *srccap, uint16_t *current );
extern void PD_PDO_Analyse( uint8_t pdo_idx, uint8_t *srccap, uint16_t *current, uint16_t *voltage );

#ifdef __cplusplus
}
#endif

#endif /* USER_PD_PROCESS_H_ */
