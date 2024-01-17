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

#ifndef __CH32X035_QC_H
#define __CH32X035_QC_H

#ifdef __cplusplus
 extern "C" {
#endif

#include "ch32x035.h"


#define  UDM_BC_CMPO      (1<<19)
#define  UDP_BC_CMPO      (1<<18)

#define  UDM_BC_VSRC      (1<<17)
#define  UDP_BC_VSRC      (1<<16)

#define  USB_IOEN         (1<<7)
#define  USB_PHY_V33      (1<<6)

#define  UDP_PUE1         (1<<3)
#define  UDP_PUE0         (1<<2)

#define  UDM_PUE1         (1<<1)
#define  UDM_PUE0         (1<<0)

#define  SDP         0x01
#define  CDP         0x02
#define  DCP         0x03
#define  BC1_2       0x04





 typedef struct
 {
     uint16_t BC_Det_Flg:1;
     uint16_t BC_Det_Cnt:8;
     uint16_t BC_Det_Ret:3;
 }_QC_CONTROL;


 /******************************************************************************/
 /* Variable extents */
 extern _QC_CONTROL  QC_Ctrl;



 void Set_dp_0v6(void);
 void Set_dm_0v6(void);
 void Set_dp_0v0(void);
 void Set_dm_0v0(void);
 void Set_dp_3v3(void);
 void Set_dm_3v3(void);

 void QC_Request_5v(void);
 void QC_Request_9v(void);
 void QC_Request_12v(void);
 void QC_Request_20v(void);



 uint8_t BC1_2Check(void);

#ifdef __cplusplus
}
#endif

#endif /* USER_CH32X035_QC2_0_H_ */
