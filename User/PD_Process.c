/********************************** (C) COPYRIGHT *******************************
* File Name          : PD_process.c
* Author             : WCH
* Version            : V1.0.0
* Date               : 2023/04/06
* Description        : This file provides all the PD firmware functions.
*********************************************************************************
* Copyright (c) 2023 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/

#include "debug.h"
#include <string.h>
#include "PD_Process.h"
#include <stdbool.h>
#include "hw_debug.h"
#include "hw_gpio.h"
#include "powermanager.h"
#include "hw_config.h"
void USBPD_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));

__attribute__ ((aligned(4))) uint8_t PD_Rx_Buf[ 34 ];                           /* PD receive buffer */
__attribute__ ((aligned(4))) uint8_t PD_Tx_Buf[ 34 ];                           /* PD send buffer */
#ifndef PD_REQ_VOLTAGE  
#define PD_REQ_VOLTAGE 5
#endif
/******************************************************************************/
UINT8 PD_Ack_Buf[ 2 ];                                                          /* PD-ACK buffer */

UINT8  Tmr_Ms_Cnt_Last;                                                         /* System timer millisecond timing final value */
UINT8  Tmr_Ms_Dlt;                                                              /* System timer millisecond timing this interval value */
UINT8  Tim_Ms_Cnt;                                                              /* System timer millisecond timing counter */

PD_CONTROL PD_Ctl;                                                              /* PD Control Related Structures */

UINT8  Adapter_SrcCap[ 30 ];                                                    /* SrcCap message from the adapter or Contents of the SrcCap message for the adapter */

UINT8  PDO_Len;

bool cc_connected = false; //仅用于SRC模式
bool pd_mode = SNK;
bool pd_vol_match = false;
uint8_t pd_req_vol = PD_REQ_VOLTAGE;
bool pd_ready = false;  //pd 模式否则是qc模式
typedef enum {
    STATE_START_SWAP,
    STATE_WAIT_PS_READY,
    STATE_PS_SWAP_END,
}prswap_t;


prswap_t prswap_sta = STATE_PS_SWAP_END;
/* SrcCap Table */
UINT8 SrcCap_5V3A_Tab[ 4 ]  = { 0X2C, 0X91, 0X01, 0X3E };
UINT8 SrcCap_5V1A5_Tab[ 4 ] = { 0X96, 0X90, 0X01, 0X3E };
UINT8 SrcCap_5V2A_Tab[ 4 ]  = { 0XC8, 0X90, 0X01, 0X3E };
UINT8 SinkCap_5V1A_Tab[ 4 ] = { 0X64, 0X90, 0X01, 0X36 };

/* PD3.0 */
UINT8 SrcCap_Ext_Tab[ 28 ] =
{
    0X18, 0X80, 0X63, 0X00,
    0X00, 0X00, 0X00, 0X00,
    0X00, 0X00, 0X01, 0X00,
    0X00, 0X00, 0X07, 0X03,
    0X00, 0X00, 0X00, 0X00,
    0X00, 0X00, 0X00, 0X03,
    0X00, 0X12, 0X00, 0X00,
};

UINT8 Status_Ext_Tab[ 8 ] =
{
    0X06, 0X80, 0X16, 0X00,
    0X00, 0X00, 0X00, 0X00,
};

__WEAK void vbus_on(void)
{
    
}

__WEAK void vbus_off(void)
{
    
}

__WEAK void cc1_5_1k_pulldown(void)
{
    
}

__WEAK void cc1_5_1k_pulldown_remove(void)
{
    
}

__WEAK bool phone_plugin(void)
{
    return false;
}

__WEAK bool charger_in(void)
{
    return false;
}

__WEAK void pd_hw_init(void)
{
    
}

// 检查最终申请的VBUs电压是否与申请充电器输出电压匹配
__WEAK bool vbus_req_vol_match(void)
{
    return false;
}

/*********************************************************************
 * @fn      USBPD_IRQHandler
 *
 * @brief   This function handles USBPD interrupt.
 *
 * @return  none
 */
void USBPD_IRQHandler(void)
{
    if(USBPD->STATUS & IF_RX_ACT)
    {
        USBPD->STATUS |= IF_RX_ACT;
        if( ( USBPD->STATUS & MASK_PD_STAT ) == PD_RX_SOP0 )
        {
            if( USBPD->BMC_BYTE_CNT >= 6 )
            {
                /* If GOODCRC, do not answer and ignore this reception */
                if( ( USBPD->BMC_BYTE_CNT != 6 ) || ( ( PD_Rx_Buf[ 0 ] & 0x1F ) != DEF_TYPE_GOODCRC ) )
                {
                    Delay_Us(30);                       /* Delay 30us, answer GoodCRC */
					if(pd_mode == SNK)
                    PD_Ack_Buf[ 0 ] = 0x41;
					else
					PD_Ack_Buf[ 0 ] = 0x61;
                    PD_Ack_Buf[ 1 ] = ( PD_Rx_Buf[ 1 ] & 0x0E ) | PD_Ctl.Flag.Bit.Auto_Ack_PRRole;
                    USBPD->CONFIG |= IE_TX_END ;
                    PD_Phy_SendPack( 0, PD_Ack_Buf, 2, UPD_SOP0 );
                }
            }
        }
    }
    if(USBPD->STATUS & IF_TX_END)
    {
        /* Packet send completion interrupt (GoodCRC send completion interrupt only) */
        USBPD->PORT_CC1 &= ~CC_LVE;
        USBPD->PORT_CC2 &= ~CC_LVE;

        /* Interrupts are turned off and can be turned on after the main function has finished processing the data */
        NVIC_DisableIRQ(USBPD_IRQn);
        PD_Ctl.Flag.Bit.Msg_Recvd = 1;                                          /* Packet received flag */
        USBPD->STATUS |= IF_TX_END;
    }
    if(USBPD->STATUS & IF_RX_RESET)
    {
        USBPD->STATUS |= IF_RX_RESET;
        PD_SINK_Init( );
        printf("IF_RX_RESET\r\n");
    }
}

/*********************************************************************
 * @fn      PD_Rx_Mode
 *
 * @brief   This function uses to enter reception mode.
 *
 * @return  none
 */
void PD_Rx_Mode( void )
{
    USBPD->CONFIG |= PD_ALL_CLR;
    USBPD->CONFIG &= ~PD_ALL_CLR;
    USBPD->CONFIG |= IE_RX_ACT | IE_RX_RESET|PD_DMA_EN;
    USBPD->DMA = (UINT32)(UINT8 *)PD_Rx_Buf;
    USBPD->CONTROL &= ~PD_TX_EN;
    USBPD->BMC_CLK_CNT = UPD_TMR_RX_48M;
    USBPD->CONTROL |= BMC_START;
    NVIC_EnableIRQ( USBPD_IRQn );
}

/*********************************************************************
 * @fn      PD_SRC_Init
 *
 * @brief   This function uses to initialize SRC mode.
 *
 * @return  none
 */
void PD_SRC_Init( )
{
    PD_Ctl.Flag.Bit.PR_Role = 1;                                          /* SRC mode */
    PD_Ctl.Flag.Bit.Auto_Ack_PRRole = 1;                                  /* Default auto-responder role is SRC */
    USBPD->PORT_CC1 = CC_CMP_66 | CC_PU_330;
    USBPD->PORT_CC2 = CC_CMP_66 | CC_PU_330;
}

/*********************************************************************
 * @fn      PD_SINK_Init
 *
 * @brief   This function uses to initialize SNK mode.
 *
 * @return  none
 */
void PD_SINK_Init( )
{
    PD_Ctl.Flag.Bit.PR_Role = 0;                                          /* SINK mode */
    PD_Ctl.Flag.Bit.Auto_Ack_PRRole = 0;                                  /* Default auto-responder role is SINK */
    USBPD->PORT_CC1 = CC_CMP_66 | CC_PD;
    USBPD->PORT_CC2 = CC_CMP_66 | CC_PD;
}

/*********************************************************************
 * @fn      PD_PHY_Reset
 *
 * @brief   This function uses to reset PD PHY.
 *
 * @return  none
 */
void PD_PHY_Reset( void )
{
    printf("PD_PHY_Reset\r\n");
    //set PD_Ctl to 0
    memset(&PD_Ctl, 0, sizeof(PD_CONTROL));
	if(pd_mode == SNK){
        cc1_5_1k_pulldown();
    	PD_SINK_Init( );
    	PD_Ctl.Flag.Bit.Stop_Det_Chk = 0;                                     /* PD disconnection detection is enabled by default */
    	PD_Ctl.PD_State = STA_IDLE;                                           /* Set idle state */
    	PD_Ctl.Flag.Bit.PD_Comm_Succ = 0;
        PD_Ctl.Err_Op_Cnt = 0;
	}else{
	    PD_Ctl.Flag.Bit.Msg_Recvd = 0;
	    PD_Ctl.Msg_ID = 0;
	    PD_Ctl.Flag.Bit.PD_Version = 0;
	    PD_Ctl.Det_Cnt = 0;
	    PD_Ctl.Flag.Bit.Connected = 0;
	    PD_Ctl.PD_Comm_Timer = 0;
	    PD_Ctl.PD_BusIdle_Timer = 0;
	    PD_Ctl.Mode_Try_Cnt = 0x80;
	    PD_Ctl.Flag.Bit.PD_Role = 1;
	    PD_Ctl.Flag.Bit.Stop_Det_Chk = 0;
	    PD_Ctl.PD_State = STA_IDLE;
	    PD_Ctl.Flag.Bit.PD_Comm_Succ = 0;
        cc1_5_1k_pulldown_remove();
	    PD_SRC_Init( );
	    PD_Rx_Mode( );
	}
}

/*********************************************************************
 * @fn      PD_Init
 *
 * @brief   This function uses to initialize PD registers and states.
 *
 * @return  none
 */
void PD_Init( void )
{
    pd_hw_init();
    
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);               /* Open PD I/O clock, AFIO clock and PD clock */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_USBPD, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    AFIO->CTLR |= USBPD_IN_HVT | USBPD_PHY_V33;
    USBPD->CONFIG = PD_DMA_EN;
    USBPD->STATUS = BUF_ERR | IF_RX_BIT | IF_RX_BYTE | IF_RX_ACT | IF_RX_RESET | IF_TX_END;
    /* Initialize all variables */
    memset( &PD_Ctl.PD_State, 0x00, sizeof( PD_CONTROL ) );
    Adapter_SrcCap[ 0 ] = 1;
    memcpy( &Adapter_SrcCap[ 1 ], SrcCap_5V3A_Tab, 4 );
    
    if(charger_in()){
        pd_mode = SRC;
    }else{
        pd_mode = SNK;
    }
    PD_PHY_Reset( );
    PD_Rx_Mode( );
}


/*********************************************************************
 * @fn      PD_Detect
 *
 * @brief   This function uses to detect CC connection.
 *
 * @return  0:No connection; 1:CC1 connection; 2:CC2 connection
 */
UINT8 PD_Detect( void )
{
    UINT8  ret = 0;
    UINT8  cmp_cc1 = 0;
    UINT8  cmp_cc2 = 0;

    if(PD_Ctl.Flag.Bit.Connected)                                       /* Detect disconnection */
    {
        /* According to the usage scenario of PD SNK, whether
         * it is removed or not should be determined by detecting
         * the Vbus voltage, this code only shows the detection
         * and the subsequent communication flow. */
    }
    else                                                                /* Detect insertion */
    {
        USBPD->PORT_CC1 &= ~( CC_CMP_Mask|PA_CC_AI );
        USBPD->PORT_CC1 |= CC_CMP_22;
        Delay_Us(2);
        if( USBPD->PORT_CC1 & PA_CC_AI )
        {
            cmp_cc1 |= bCC_CMP_22;
        }
        USBPD->PORT_CC1 &= ~( CC_CMP_Mask|PA_CC_AI );
        USBPD->PORT_CC1 |= CC_CMP_66;


        USBPD->PORT_CC2 &= ~( CC_CMP_Mask|PA_CC_AI );
        USBPD->PORT_CC2 |= CC_CMP_22;
        Delay_Us(2);
        if( USBPD->PORT_CC2 & PA_CC_AI )
        {
            cmp_cc2 |= bCC_CMP_22;
        }
        USBPD->PORT_CC2 &= ~( CC_CMP_Mask|PA_CC_AI );
        USBPD->PORT_CC2 |= CC_CMP_66;


        if (USBPD->PORT_CC1 & CC_PD)
        {
            if (cmp_cc1 & bCC_CMP_22 == bCC_CMP_22)
            {
                ret = 1;
            }
            if (cmp_cc2 & bCC_CMP_22 == bCC_CMP_22)
            {
                if( ret )
                {
                    ret = 1;   /* Huawei A to C cable has two pull-up resistors */
                }
                else
                {
                    ret = 2;
                }
            }
        }
        else
        {
            /* SRC mode insertion detection */
        }
    }
    return( ret );
}

UINT8 PD_SRC_Detect( void )
{
    UINT8  ret = 0;
    UINT8  cmp_cc1 = 0;
    UINT8  cmp_cc2 = 0;

    if(PD_Ctl.Flag.Bit.Connected)                                       /*Detect disconnection*/
    {
        USBPD->PORT_CC1 &= ~( CC_CMP_Mask | PA_CC_AI );
        USBPD->PORT_CC1 |= CC_CMP_22;
        Delay_Us(2);
        if( USBPD->PORT_CC1 & PA_CC_AI )
        {
            cmp_cc1 = bCC_CMP_22;
        }
        USBPD->PORT_CC1 &= ~( CC_CMP_Mask | PA_CC_AI );
        USBPD->PORT_CC1 |= CC_CMP_66;

        USBPD->PORT_CC2 &= ~( CC_CMP_Mask | PA_CC_AI );
        USBPD->PORT_CC2 |= CC_CMP_22;
        Delay_Us(2);
        if( USBPD->PORT_CC2 & PA_CC_AI )
        {
            cmp_cc2 = bCC_CMP_22;
        }
        USBPD->PORT_CC2 &= ~( CC_CMP_Mask | PA_CC_AI );
        USBPD->PORT_CC2 |= CC_CMP_66;

        if((GPIOC->INDR & PIN_CC1) != (uint32_t)Bit_RESET)
        {
            cmp_cc1 |= bCC_CMP_220;
        }
        if((GPIOC->INDR & PIN_CC2) != (uint32_t)Bit_RESET)
        {
            cmp_cc2 |= bCC_CMP_220;
        }

        if( USBPD->PORT_CC1 & CC_PD )
        {
            /* SRC sample code does not handle SNK */
        }
        else
        {
            if (USBPD->CONFIG & CC_SEL)
            {
                if ((cmp_cc2 & bCC_CMP_220) == bCC_CMP_220)
                {
                    ret=0;
                }
                else
                {
                    ret = 2;
                }
            }
            else
            {
                if ((cmp_cc1 & bCC_CMP_220) == bCC_CMP_220)
                {
                    ret=0;
                }
                else
                {
                    ret = 1;
                }
            }
        }
    }
    else                                                                /*Detect insertion*/
    {
        USBPD->PORT_CC1 &= ~( CC_CMP_Mask|PA_CC_AI );
        USBPD->PORT_CC1 |= CC_CMP_22;
        Delay_Us(2);
        if( USBPD->PORT_CC1 & PA_CC_AI )
        {
            cmp_cc1 |= bCC_CMP_22;
        }
        USBPD->PORT_CC1 &= ~( CC_CMP_Mask|PA_CC_AI );
        USBPD->PORT_CC1 |= CC_CMP_66;
        Delay_Us(2);
        if( USBPD->PORT_CC1 & PA_CC_AI )
        {
            cmp_cc1 |= bCC_CMP_66;
        }
        if((GPIOC->INDR & PIN_CC1) != (uint32_t)Bit_RESET)
        {
            cmp_cc1 |= bCC_CMP_220;
        }

        USBPD->PORT_CC2 &= ~( CC_CMP_Mask|PA_CC_AI );
        USBPD->PORT_CC2 |= CC_CMP_22;
        Delay_Us(2);
        if( USBPD->PORT_CC2 & PA_CC_AI )
        {
            cmp_cc2 |= bCC_CMP_22;
        }
        USBPD->PORT_CC2 &= ~( CC_CMP_Mask|PA_CC_AI );
        USBPD->PORT_CC2 |= CC_CMP_66;
        Delay_Us(2);
        if( USBPD->PORT_CC2 & PA_CC_AI )
        {
            cmp_cc2 |= bCC_CMP_66;
        }
        if((GPIOC->INDR & PIN_CC2) != (uint32_t)Bit_RESET)
        {
            cmp_cc2 |= bCC_CMP_220;
        }

        if( USBPD->PORT_CC1 & CC_PD )
        {
           /* SRC sample code does not handle SNK */
        }
        else
        {
            if ((((cmp_cc1 & bCC_CMP_66) == bCC_CMP_66) & ((cmp_cc1 & bCC_CMP_220) == 0x00)) == 1)
            {
                if ((((cmp_cc2 & bCC_CMP_22) == bCC_CMP_22) & ((cmp_cc2 & bCC_CMP_66) == 0x00)) == 1)
                {
                  ret = 1;
                }
                if ((cmp_cc2 & bCC_CMP_220) == bCC_CMP_220)
                {
                  ret = 1;
                }
            }
            if ((((cmp_cc2 & bCC_CMP_66) == bCC_CMP_66) & ((cmp_cc2 & bCC_CMP_220) == 0x00)) == 1)
            {
                if(ret)
                {
                    ret = 0;
                }
                else
                {
                    if ((((cmp_cc1 & bCC_CMP_22) == bCC_CMP_22) && ((cmp_cc1 & bCC_CMP_66) == 0x00)) == 1)
                    {
                      ret = 2;
                    }
                    if ((cmp_cc1 & bCC_CMP_220) == bCC_CMP_220)
                    {
                      ret = 2;
                    }
                }
            }
        }
    }
    if(ret == 0)
    {
        cc_connected = false;
    }else{
        cc_connected = true;
    }
    return( ret );
}
/*********************************************************************
 * @fn      PD_Det_Proc
 *
 * @brief   This function uses to process the return value of PD_Detect.
 *
 * @return  none
 */
void PD_Det_Proc( void )
{
    UINT8  status;

    if( PD_Ctl.Flag.Bit.Connected )
    {
        /* PD is connected, detect its disconnection */

        /* According to the usage scenario of PD SNK, whether
         * it is removed or not should be determined by detecting
         * the Vbus voltage, this code only shows the detection
         * and the subsequent communication flow. */

    }
    else
    {
        /* PD disconnected, check connection */
        status = PD_Detect( );

        /* Determine connection status */
        if( status == 0 )
        {
            PD_Ctl.Det_Cnt = 0;
        }
        else
        {
            PD_Ctl.Det_Cnt++;
        }
        if( PD_Ctl.Det_Cnt >= 5 )
        {
            PD_Ctl.Det_Cnt = 0;
            PD_Ctl.Flag.Bit.Connected = 1;
            if( PD_Ctl.Flag.Bit.Stop_Det_Chk == 0 )
            {
                if( (USBPD->PORT_CC1 & CC_PD) || (USBPD->PORT_CC2 & CC_PD) )
                {
                    /* Select the corresponding PD channel */
                    if( status == 1 )
                    {
                        USBPD->CONFIG &= ~CC_SEL;
                    }
                    else
                    {
                        USBPD->CONFIG |= CC_SEL;
                    }
                    PD_Ctl.PD_State = STA_SRC_CONNECT;
                    printf("CC%d SRC Connect\r\n",status);
                }

                PD_Ctl.PD_Comm_Timer = 0;
            }
        }
    }
}

void PD_SRC_Det_Proc( void )
{
    UINT8  status;
    status = PD_SRC_Detect( );
    if( PD_Ctl.Flag.Bit.Connected )
    {
        /* PD is connected, detect its disconnection */
        if( status )
        {
            PD_Ctl.Det_Cnt = 0;
        }
        else
        {
            PD_Ctl.Det_Cnt++;
            if( PD_Ctl.Det_Cnt >= 5 )
            {
                PD_Ctl.Det_Cnt = 0;
                PD_Ctl.Flag.Bit.Connected = 0;
                if( PD_Ctl.Flag.Bit.Stop_Det_Chk == 0 )
                {
                    PD_Ctl.PD_State = STA_DISCONNECT;
                }
            }
        }
    }
    else
    {
        /* PD is disconnected, check its connection */

        /* Determine connection status */
        if( status == 0 )
        {
            PD_Ctl.Det_Cnt = 0;
        }
        else
        {
            PD_Ctl.Det_Cnt++;
        }
        if( PD_Ctl.Det_Cnt >= 5 )
        {
            PD_Ctl.Det_Cnt = 0;
            PD_Ctl.Flag.Bit.Connected = 1;
            if( PD_Ctl.Flag.Bit.Stop_Det_Chk == 0 )
            {
                /* Select the corresponding PD channel */
                if( status == 1 )
                {
                    USBPD->CONFIG &= ~CC_SEL;
                }
                else
                {
                    USBPD->CONFIG |= CC_SEL;
                }
                if( (USBPD->PORT_CC1 & CC_PD) || (USBPD->PORT_CC2 & CC_PD) )
                {
                    PD_Ctl.PD_State = STA_SRC_CONNECT;
                    printf("SRC CC%d SRC Connect\r\n",status);
                }
                else
                {
                    PD_Ctl.PD_State = STA_SINK_CONNECT;
                    Delay_Ms(100);
                    printf("SRC CC%d SINK Connect\r\n",status);
                }

                PD_Ctl.PD_Comm_Timer = 0;
            }
        }
    }
}
/*********************************************************************
 * @fn      PD_Phy_SendPack
 *
 * @brief   This function uses to send PD data.
 *
 * @return  none
 */
void PD_Phy_SendPack( UINT8 mode, UINT8 *pbuf, UINT8 len, UINT8 sop )
{

    if ((USBPD->CONFIG & CC_SEL) == CC_SEL )
    {
        USBPD->PORT_CC2 |= CC_LVE;
    }
    else
    {
        USBPD->PORT_CC1 |= CC_LVE;
    }

    USBPD->BMC_CLK_CNT = UPD_TMR_TX_48M;

    USBPD->DMA = (UINT32)(UINT8 *)pbuf;

    USBPD->TX_SEL = sop;

    USBPD->BMC_TX_SZ = len;
    USBPD->CONTROL |= PD_TX_EN;
    USBPD->STATUS &= BMC_AUX_INVALID;
    USBPD->CONTROL |= BMC_START;

    /* Determine if you need to wait for the send to complete */
    if( mode )
    {
        /* Wait for the send to complete, this will definitely complete, no need to do a timeout */
        while( (USBPD->STATUS & IF_TX_END) == 0 );
        USBPD->STATUS |= IF_TX_END;
        if((USBPD->CONFIG & CC_SEL) == CC_SEL )
        {
            USBPD->PORT_CC2 &= ~CC_LVE;
        }
        else
        {
            USBPD->PORT_CC1 &= ~CC_LVE;
        }

        /* Switch to receive ready to receive GoodCRC */
        USBPD->CONFIG |=  PD_ALL_CLR ;
        USBPD->CONFIG &= ~( PD_ALL_CLR );
        USBPD->CONTROL &= ~ ( PD_TX_EN );
        USBPD->DMA = (UINT32)(UINT8 *)PD_Rx_Buf;
        USBPD->BMC_CLK_CNT = UPD_TMR_RX_48M;
        USBPD->CONTROL |= BMC_START;
    }
}

/*********************************************************************
 * @fn      PD_Load_Header
 *
 * @brief   This function uses to load pd header packets.
 *
 * @return  none
 */
void PD_Load_Header( UINT8 ex, UINT8 msg_type )
{
    /* Message Header
       BIT15 - Extended;
       BIT[14:12] - Number of Data Objects
       BIT[11:9] - Message ID
       BIT8 - PortPower Role/Cable Plug  0: SINK; 1: SOURCE
       BIT[7:6] - Revision, 00: V1.0; 01: V2.0; 10: V3.0;
       BIT5 - Port Data Role, 0: UFP; 1: DFP
       BIT[4:0] - Message Type
    */
    PD_Tx_Buf[ 0 ] = msg_type;
    if( PD_Ctl.Flag.Bit.PD_Role )
    {
        PD_Tx_Buf[ 0 ] |= 0x20;
    }
    if( PD_Ctl.Flag.Bit.PD_Version )
    {
        /* PD3.0 */
        PD_Tx_Buf[ 0 ] |= 0x80;
    }
    else
    {
        /* PD2.0 */
        PD_Tx_Buf[ 0 ] |= 0x40;
    }

    PD_Tx_Buf[ 1 ] = PD_Ctl.Msg_ID & 0x0E;
    if( PD_Ctl.Flag.Bit.PR_Role )
    {
        PD_Tx_Buf[ 1 ] |= 0x01;
    }
    if( ex )
    {
        PD_Tx_Buf[ 1 ] |= 0x80;
    }
}

/*********************************************************************
 * @fn      PD_Send_Handle
 *
 * @brief   This function uses to handle sending transactions.
 *
 * @return  0:success; 1:fail
 */
UINT8 PD_Send_Handle( UINT8 *pbuf, UINT8 len )
{
    UINT8  pd_tx_trycnt;
    UINT8  cnt;

    if( ( len % 4 ) != 0 )
    {
        /* Send failed */
        return( DEF_PD_TX_FAIL );
    }
    if( len > 28 )
    {
        /* Send failed */
        return( DEF_PD_TX_FAIL );
    }

    cnt = len >> 2;
    PD_Tx_Buf[ 1 ] |= ( cnt << 4 );
    for( cnt = 0; cnt != len; cnt++ )
    {
        PD_Tx_Buf[ 2 + cnt ] = pbuf[ cnt ];
    }

    pd_tx_trycnt = 4;
    while( --pd_tx_trycnt )                                                     /* Maximum 3 executions */
    {
        NVIC_DisableIRQ( USBPD_IRQn );
        PD_Phy_SendPack( 0x01, PD_Tx_Buf, ( len + 2 ), UPD_SOP0 );

        /* Set receive timeout 750US */
        cnt = 250;
        while( --cnt )
        {
            if( (USBPD->STATUS & IF_RX_ACT) == IF_RX_ACT)
            {
                USBPD->STATUS |= IF_RX_ACT;
                if( ( USBPD->BMC_BYTE_CNT == 6 ) && ( ( PD_Rx_Buf[ 0 ] & 0x1F ) == DEF_TYPE_GOODCRC ) )
                {
                    PD_Ctl.Msg_ID += 2;
                    break;
                }
            }
            Delay_Us( 3 );
        }
        if( cnt !=0 )
        {
            break;
        }
    }

    /* Switch to receive mode */
    PD_Rx_Mode( );
    if( pd_tx_trycnt )
    {
        /* Send successful */
        return( DEF_PD_TX_OK );
    }
    else
    {
        /* Send failed */
        return( DEF_PD_TX_FAIL );
    }
}

/*********************************************************************
 * @fn      PDO_Request
 *
 * @brief   This function uses to Send the specified PDO.
 *
 * @return  none
 */
bool PDO_Request( UINT8 pdo_index )
{
    UINT16 Current,Voltage;
    UINT8  status;
    bool ret = false;
    if ((pdo_index > PDO_Len) || (pdo_index == 0))
    {
        while(1)
        {
            printf("pdo_index error!\r\n");
            Delay_Ms(500);
        }
    }
    else
    {
        memcpy( &PD_Rx_Buf[ 2 ], &Adapter_SrcCap[ 4*(pdo_index-1) + 1 ], 4 );
        PD_PDO_Analyse( 1, &PD_Rx_Buf[ 2 ], &Current, &Voltage );
        printf("Request:\r\nCurrent:%d mA\r\nVoltage:%d mV\r\n",Current,Voltage);

        PD_Load_Header( 0x00, DEF_TYPE_REQUEST );
        PD_Rx_Buf[ 5 ] = 0x03;
        PD_Rx_Buf[ 5 ] |= pdo_index<<4;
        PD_Rx_Buf[ 3 ] = PD_Rx_Buf[ 3 ] & 0x03;
        PD_Rx_Buf[ 3 ] |= ( PD_Rx_Buf[ 2 ] << 2 );
        PD_Rx_Buf[ 4 ] = PD_Rx_Buf[ 3 ];
        PD_Rx_Buf[ 4 ] <<= 2;
        PD_Rx_Buf[ 4 ] = PD_Rx_Buf[ 4 ] & 0x0C;
        PD_Rx_Buf[ 4 ] |= ( PD_Rx_Buf[ 2 ] >> 6 );
    }
    status = PD_Send_Handle( &PD_Rx_Buf[ 2 ], 4 );

    if( status == DEF_PD_TX_OK )
    {
        ret = true;
        PD_Ctl.PD_State = STA_RX_ACCEPT_WAIT;
    }
    else
    {
        PD_Ctl.PD_State = STA_TX_SOFTRST;
    }
    PD_Ctl.PD_Comm_Timer = 0;
    PD_Ctl.Flag.Bit.PD_Comm_Succ = 1;
    return ret;
}

/*********************************************************************
 * @fn      PD_Save_Adapter_SrcCap
 *
 * @brief   This function uses to save the adapter SrcCap information.
 *
 * @return  none
 */
void PD_Save_Adapter_SrcCap( void )
{
    UINT8  i, len;

    /* Calculate the number of NDO's (Number of Data Objects) in the Message Header */
    len = ( ( PD_Rx_Buf[ 1 ] >> 4 ) & 0x07 );

    /* Remove the PPS section */
    for( i = 0; i < len; i++ )
    {
        if( ( PD_Rx_Buf[ 2 + ( i << 2 ) + 3 ] & 0xC0 ) == 0xC0 )
        {
            break;
        }
    }

    PDO_Len = i;

    /* Modify SrcCap information */
       /* BIT[31:30] - Fixed Supply */
       /* BIT29 - Dual-Role Power */
       /* BIT28 - USB Suspend Power */
       /* BIT27 - Unconstrained Power */
       /* BIT26 - USB Communications */
       /* BIT25 - Dual-Role Data */
       /* BIT24 - Unchunked Extended Message Supported */
       /* BIT23 - EPR Mode Capable */
       /* BIT22 - Reserved,shall be set to zero */
       /* BIT[21:20] - Peak Current */
       /* BIT[19:10] - Voltage in 50mV units */
       /* BIT[9:0] - Maximum Current in 10mA units */
    PD_Rx_Buf[ 5 ] = 0x3E;

    /* Save the adapter's SrcCap information */
    PD_Rx_Buf[ 1 ] &= 0x8F;
    PD_Rx_Buf[ 1 ] |= i << 4;
    Adapter_SrcCap[ 0 ] = i;
    memcpy( &Adapter_SrcCap[ 1 ], &PD_Rx_Buf[ 2 ], ( i << 2 ) );
}

/*********************************************************************
 * @fn      PD_PDO_Analyse
 *
 * @brief   This function uses to analyse PDO's voltage and current.
 *
 * @return  none
 */
void PD_PDO_Analyse( UINT8 pdo_idx, UINT8 *srccap, UINT16 *current, UINT16 *voltage )
{
    UINT32 temp32;

    temp32 = srccap[ (  ( pdo_idx - 1 ) << 2 ) + 0 ] +
                        ( (UINT32)srccap[ ( ( pdo_idx - 1 ) << 2 ) + 1 ] << 8 ) +
                        ( (UINT32)srccap[ ( ( pdo_idx - 1 ) << 2 ) + 2 ] << 16 );

    /* Calculation of current values */
    if( current != NULL )
    {
        *current = ( temp32 & 0x000003FF ) * 10;
    }

    /* Calculation of voltage values */
    if( voltage != NULL )
    {
        temp32 = temp32 >> 10;
        *voltage = ( temp32 & 0x000003FF ) * 50;
    }
}

void PD_Request_Analyse( UINT8 pdo_idx, UINT8 *srccap, UINT16 *current )
{
    UINT32 temp32;

    temp32 = srccap[ (  ( pdo_idx - 1 ) << 2 ) + 0 ] +
                        ( (UINT32)srccap[ ( ( pdo_idx - 1 ) << 2 ) + 1 ] << 8 );

    /* Calculation of current values */
    if( current != NULL )
    {
        *current = ( temp32 & 0x000003FF ) * 10;
    }

}

/*********************************************************************
 * @fn      PD_Main_Proc
 *
 * @brief   This function uses to process PD status.
 *
 * @return  none
 */
void PD_Main_Proc( )
{
    UINT8  status;
    UINT8  pd_header;
    UINT8 var;
    UINT16 Current,Voltage;

    /* Receive idle timer count */
    PD_Ctl.PD_BusIdle_Timer += Tmr_Ms_Dlt;

    /* Status analysis processing */
    switch( PD_Ctl.PD_State )
    {
        case STA_DET_CHARGING:
            /* Status: idle */
            if(charger_in()){
                PD_Ctl.PD_State = STA_TX_PR_SWAP;
            }
            break;
        case STA_DISCONNECT:
            /* Status: Disconnected */
            printf("Disconnect\r\n");
            PD_PHY_Reset( );
            break;

        case STA_SRC_CONNECT:
            /* Status: SRC access */
            /* If SRC_CAP is received within 1S, reset operation is performed */
            PD_Ctl.PD_Comm_Timer += Tmr_Ms_Dlt;
            if( PD_Ctl.PD_Comm_Timer > 999 )
            {
                /* Retry on exception (abort after 5 attempts) */
                PD_Ctl.Err_Op_Cnt++;
                if( PD_Ctl.Err_Op_Cnt > 5 )
                {
                    PD_Ctl.Err_Op_Cnt = 0;
                    PD_Ctl.PD_State = STA_IDLE;
                }
                else
                {
                    #if defined QC_SUPPROT
                    switch(BC1_2Check())
                    {
                        case SDP:
                            printf( " SDP \r\n" );
                            PD_Ctl.PD_State = STA_IDLE;
                            
                                break;
                        case CDP:
                            printf( " CDP \r\n" );
                            PD_Ctl.PD_State = STA_IDLE;
                            
                                break;
                        case DCP:
                            printf( " DCP \r\n" );
                            PD_Ctl.PD_State = STA_IDLE;
                            
                                break;
                        case BC1_2:
                            printf( " BC1_2 \r\n" );
                            Delay_Ms(50);
                            PD_Ctl.PD_State = STA_IDLE;
                            switch (pd_req_vol)
                            {
                            case 5:
                                QC_Request_5v();
                                /* code */
                                break;
                            case 9:
                                QC_Request_9v();
                                break;
                            case 12:
                                QC_Request_12v();
                                break;
                            default:
                                break;
                            }
                            Delay_Ms(50);
                            if(vbus_req_vol_match()){
                                pd_vol_match = true;
                            }else{
                                pd_vol_match = false;
                            }
                            break;
                    }
                    #else
                    PD_PHY_Reset( );
                    #endif
                }
            }
            break;

        case STA_RX_ACCEPT_WAIT:
            /* Status: waiting to receive ACCEPT */
        case STA_RX_PS_RDY_WAIT:
            /* Status: waiting to receive PS_RDY */
            PD_Ctl.PD_Comm_Timer += Tmr_Ms_Dlt;
            if( PD_Ctl.PD_Comm_Timer > 1000 )
            {
                printf("Timeout,STA_TX_SOFTRST\r\n");
                PD_Ctl.Flag.Bit.Stop_Det_Chk = 0;                         /* Enable connection detection*/
                PD_Ctl.PD_State = STA_TX_SOFTRST;
                PD_Ctl.PD_Comm_Timer = 0;
            }
            break;

        case STA_RX_PS_RDY:
            /* Status: PS_RDY received */
            if(vbus_req_vol_match()){
                pd_vol_match = true;
            }else{
                pd_vol_match = false;
            }
            PD_Ctl.PD_Comm_Timer = 0;
            if(prswap_sta == STATE_START_SWAP){
                printf("go tx ps rdy\r\n");
                pd_mode = SRC;
                PD_PHY_Reset();
	            PD_Ctl.Flag.Bit.PD_Role = 0;
                vbus_on();
                PD_Ctl.PD_State = STA_TX_PS_RDY;
            }else{
                PD_Ctl.PD_State = STA_DET_CHARGING;
            }    
            break;

        case STA_TX_SOFTRST:
            /* Status: send software reset */
            /* Send soft reset, if sent successfully, mode unchanged, count +1 for retry */
            PD_Load_Header( 0x00, DEF_TYPE_SOFT_RESET );
            status = PD_Send_Handle( NULL, 0 );
            if( status == DEF_PD_TX_OK )
            {
                /* current mode unchanged, jump to initial state of current mode, mode retry count, switch mode if exceeded */
                PD_Ctl.PD_State = STA_IDLE;
            }
            else
            {
                PD_Ctl.PD_State = STA_TX_HRST;
            }
            PD_Ctl.PD_Comm_Timer = 0;
            break;

        case STA_TX_HRST:
            /* Status: Sending a hardware reset */
            /* Sending a hard reset */
            PD_Ctl.Flag.Bit.Stop_Det_Chk = 1;
            PD_Phy_SendPack( 0x01, NULL, 0, UPD_HARD_RESET );                   /* send HRST */
            PD_Rx_Mode( );                                                      /* switch to rx mode */
            PD_Ctl.PD_State = STA_IDLE;
            PD_Ctl.PD_Comm_Timer = 0;
            break;
        case STA_TX_PR_SWAP:
            PD_Ctl.PD_Comm_Timer += Tmr_Ms_Dlt;
            if( PD_Ctl.PD_Comm_Timer > 19 )
            {                
                PD_Load_Header( 0x00, DEF_TYPE_PR_SWAP );
                status = PD_Send_Handle( NULL, 0 );
                if( status == DEF_PD_TX_OK )
                {
                    printf("Tx PR Swap\r\n");
                    PD_Ctl.PD_State = STA_RX_PR_SWAP_ACCEPT;
                    PD_Ctl.PD_Comm_Timer = 0;
                    prswap_sta = STATE_START_SWAP;
                }
                else
                {
                    PD_Ctl.PD_State = STA_TX_SOFTRST;
                    PD_Ctl.PD_Comm_Timer = 0;
                }
            }
            break;
            case STA_RX_PR_SWAP_ACCEPT:
                if( PD_Ctl.Flag.Bit.Msg_Recvd )
                {
                    pd_header = PD_Rx_Buf[ 0 ] & 0x1F;
                    if( pd_header == DEF_TYPE_ACCEPT )
                    {
                        PD_Rx_Mode( );
                        PD_Ctl.Flag.Bit.Msg_Recvd = 0;
                        PD_Ctl.PD_BusIdle_Timer = 0;                                      /* Idle time cleared */
                        printf("Rx PR Swap Accept\r\n");
                        PD_Ctl.PD_State = STA_RX_PS_RDY_WAIT;
                    }
                }
                PD_Ctl.PD_Comm_Timer = 0;
                break;
        default:
            break;
    }

    /* Receive message processing */
    if( PD_Ctl.Flag.Bit.Msg_Recvd )
    {
        uint8_t idx = PDO_INDEX_1;
        /* Adapter communication idle timing */
        PD_Ctl.Adapter_Idle_Cnt = 0x00;
        pd_header = PD_Rx_Buf[ 0 ] & 0x1F;
        switch( pd_header )
        {
            case DEF_TYPE_SRC_CAP:
                Delay_Ms( 5 );
                PD_Ctl.Flag.Bit.Stop_Det_Chk = 0;                         /* Enable PD disconnection detection */

                PD_Save_Adapter_SrcCap( );

                /* Analysis of the voltage and current of each PDO group */
                for (var = 1; var <= PDO_Len; ++var)
                {
                    PD_PDO_Analyse( var, &PD_Rx_Buf[ 2 ], &Current, &Voltage );
                    switch (pd_req_vol)
                    {
                    case 9:
                        if(Voltage == 9000){
                            idx = var;
                        }
                        break;
                    case 12:
                        if(Voltage == 12000){
                            idx = var;
                        }
                        break;
                    default:
                        break;
                    }
                    printf("PDO:%d\r\nCurrent:%d mA\r\nVoltage:%d mV\r\n",var,Current,Voltage);
                }
                printf("\r\n");
                /* Different PDO's for different voltages and currents */
                /* Default application for the first group of PDO, 5V */
                PDO_Request( idx );
                break;

            case DEF_TYPE_ACCEPT:
                /* ACCEPT received */
                PD_Ctl.PD_State = STA_RX_PS_RDY_WAIT;
                PD_Ctl.PD_Comm_Timer = 0;
                break;

            case DEF_TYPE_PS_RDY:
                /* PS_RDY is received */
                printf("Success\r\n");
                pd_ready = true;
                PD_Ctl.PD_State = STA_RX_PS_RDY;
                break;

            case DEF_TYPE_WAIT:
                /* WAIT received, many requests may receive WAIT, need specific analysis */
                break;

            case DEF_TYPE_GET_SNK_CAP:
                Delay_Ms( 1 );
                PD_Load_Header( 0x00, DEF_TYPE_SNK_CAP );
                PD_Send_Handle( SinkCap_5V1A_Tab, sizeof( SinkCap_5V1A_Tab ) );
                break;

            case DEF_TYPE_SOFT_RESET:
                Delay_Ms( 1 );
                PD_Load_Header( 0x00, DEF_TYPE_ACCEPT );
                PD_Send_Handle( NULL, 0 );
                break;

            case DEF_TYPE_GET_SRC_CAP_EX:
                Delay_Ms( 1 );
                PD_Load_Header( 0x01, DEF_TYPE_SRC_CAP );
                PD_Send_Handle( SrcCap_Ext_Tab, sizeof( SrcCap_Ext_Tab ) );
                break;

            case DEF_TYPE_GET_STATUS:
                Delay_Ms( 1 );
                PD_Load_Header( 0x01, DEF_TYPE_GET_STATUS_R );
                PD_Send_Handle( Status_Ext_Tab, sizeof( Status_Ext_Tab ) );
                break;

            case DEF_TYPE_VCONN_SWAP:
                Delay_Ms( 1 );
                PD_Load_Header( 0x00, DEF_TYPE_REJECT );
                PD_Send_Handle( NULL, 0 );
                break;

            case DEF_TYPE_VENDOR_DEFINED:
                /* VDM message handling */
                if( ( PD_Rx_Buf[ 2 ] & 0xC0 ) == 0 )
                {
                    /* REQ */
                    Delay_Ms( 1 );

                    /* Data to be sent is cached to PD_Tx_Buf */
                    PD_Load_Header( 0x00, DEF_TYPE_VENDOR_DEFINED );

                    /* Return to NAK */
                    if( ( PD_Rx_Buf[ 3 ] & 0x60 ) == 0 )
                    {
                        PD_Ctl.Flag.Bit.VDM_Version = 0;
                    }
                    else
                    {
                        PD_Ctl.Flag.Bit.VDM_Version = 1;
                    }
                    PD_Rx_Buf[ 2 ] |= 0x80;
                    PD_Send_Handle( &PD_Rx_Buf[ 2 ], 4 );
                }
                break;

            default:
                printf("Unsupported Command :0X%X\r\n", pd_header);
                break;
        }

        /* Message has been processed, interrupt reception is turned on again */
        PD_Rx_Mode( );
        PD_Ctl.Flag.Bit.Msg_Recvd = 0;                                    /* Clear the received flag */
        PD_Ctl.PD_BusIdle_Timer = 0;                                      /* Idle time cleared */
    }
}

void PD_SRC_Main_Proc( )
{
    UINT8  status;
    UINT8  pd_header;
    UINT16 Current;

    /* Receive idle timer count */
    PD_Ctl.PD_BusIdle_Timer += Tmr_Ms_Dlt;

    /* Status analysis processing */
    switch( PD_Ctl.PD_State )
    {
        case STA_DISCONNECT:
            printf("Disconnect\r\n");
            PD_PHY_Reset( );
            vbus_off();
            prswap_sta = STATE_PS_SWAP_END;
            break;

        case STA_SINK_CONNECT:
            PD_Ctl.PD_Comm_Timer += Tmr_Ms_Dlt;
            vbus_on();
            if( PD_Ctl.PD_Comm_Timer > 159 )
            {
              PD_Ctl.Flag.Bit.Stop_Det_Chk = 0;
              PD_Ctl.PD_Comm_Timer = 0;
              PD_Ctl.PD_State = STA_TX_SRC_CAP;
            }
            break;
        case STA_RX_PS_RDY:
            /* Status: PS_RDY received */
            //send ps_rdy
            PD_Ctl.PD_Comm_Timer += Tmr_Ms_Dlt;
            if( PD_Ctl.PD_Comm_Timer > 159 )
            {
              PD_Ctl.Flag.Bit.Stop_Det_Chk = 0;
              PD_Ctl.PD_Comm_Timer = 0;
              PD_Ctl.PD_State = STA_TX_PS_RDY;
            }
            break;

        case STA_TX_SRC_CAP:
            PD_Ctl.PD_Comm_Timer += Tmr_Ms_Dlt;

            if( PD_Ctl.PD_Comm_Timer > 159 )
            {
                PD_Load_Header( 0x00, DEF_TYPE_SRC_CAP );
                status = PD_Send_Handle(SinkCap_5V1A_Tab, 4 );
                if( status == DEF_PD_TX_OK )
                {
                    PD_Ctl.PD_State = STA_RX_REQ_WAIT;
                    printf("Send Source Cap Successfully\r\n");
                }else{
                    if( ++PD_Ctl.Err_Op_Cnt > 2 ){
                    //手机连上了，1秒内连不上PD,说明PD协议不通的。这个情况切换到SINK模式
                        logi("Unspurrort PD Phone plugin, switch to SINK MODE \r\n");
                        pd_mode = SNK;
                        vbus_off();
                        cc1_5_1k_pulldown();
                        PD_PHY_Reset( );
                        PD_Ctl.Err_Op_Cnt = 0;
                    }
                }
                PD_Ctl.PD_Comm_Timer = 0;
            }
            break;

        case STA_RX_REQ_WAIT:
            PD_Ctl.PD_Comm_Timer += Tmr_Ms_Dlt;
            if( PD_Ctl.PD_Comm_Timer > 29 )
            {
                PD_Ctl.PD_State = STA_TX_HRST;
            }
            break;

        case STA_TX_ACCEPT:
            PD_Ctl.PD_Comm_Timer += Tmr_Ms_Dlt;
            if( PD_Ctl.PD_Comm_Timer > 2 )
            {
                PD_Load_Header( 0x00, DEF_TYPE_ACCEPT );
                status = PD_Send_Handle( NULL, 0 );
                if( status == DEF_PD_TX_OK )
                {
                    printf("Accept\r\n");
                    PD_Ctl.PD_State = STA_TX_PS_RDY;
                    PD_Ctl.PD_Comm_Timer = 0;
                }
                else
                {
                    PD_Ctl.PD_State = STA_TX_SOFTRST;
                    PD_Ctl.PD_Comm_Timer = 0;
                }
            }
            break;

        case STA_TX_PS_RDY:
            PD_Ctl.PD_Comm_Timer += Tmr_Ms_Dlt;
            if( PD_Ctl.PD_Comm_Timer > 19 )
            {
                PD_Load_Header( 0x00, DEF_TYPE_PS_RDY );
                status = PD_Send_Handle( NULL, 0 );
                if( status == DEF_PD_TX_OK )
                {
                    printf("PS ready\r\n");
                    if(prswap_sta < STATE_PS_SWAP_END){
                        prswap_sta++;
                        if(prswap_sta == STATE_WAIT_PS_READY){
                            PD_Ctl.PD_State = STA_TX_SRC_CAP;
                        }else{
                            PD_Ctl.PD_State = STA_IDLE;
                        }
                    }else{
                        PD_Ctl.PD_State = STA_TX_DR_SWAP; 
                    }
                    PD_Ctl.PD_Comm_Timer = 0;
                }
                else
                {
                    PD_Ctl.PD_State = STA_TX_SOFTRST;
                    PD_Ctl.PD_Comm_Timer = 0;
                }
            }
            break;

        case STA_TX_SOFTRST:
            /* Send soft reset, if sent successfully, mode unchanged, count +1 for retry */
            PD_Load_Header( 0x00, DEF_TYPE_SOFT_RESET );
            status = PD_Send_Handle( NULL, 0 );
            if( status == DEF_PD_TX_OK )
            {
                /* current mode unchanged, jump to initial state of current mode, mode retry count, switch mode if exceeded */
                PD_Ctl.PD_State = STA_IDLE;
            }
            else
            {
                PD_Ctl.PD_State = STA_TX_HRST;
            }
            PD_Ctl.PD_Comm_Timer = 0;
            break;

        case STA_TX_HRST:
            /* Sending a hard reset */
            PD_Ctl.Flag.Bit.Stop_Det_Chk = 1;
            PD_Phy_SendPack( 0x01, NULL, 0, UPD_HARD_RESET );                   /* send HRST */
            PD_Rx_Mode( );                                                      /* switch to rx mode */
            PD_Ctl.PD_State = STA_IDLE;
            PD_Ctl.PD_Comm_Timer = 0;
            break;
        
        case STA_TX_DR_SWAP:
            PD_Ctl.PD_Comm_Timer += Tmr_Ms_Dlt;
            if( PD_Ctl.PD_Comm_Timer > 19 )
            {
                PD_Load_Header( 0x00, DEF_TYPE_DR_SWAP );
                status = PD_Send_Handle( NULL, 0 );
                if( status == DEF_PD_TX_OK )
                {
                    printf("Tx DR Swap\r\n"); //充电时，CC线5.1K下拉取消了，USB会工作不正常。需要执行 DR Swap
                    PD_Ctl.PD_State = STA_RX_DR_SWAP_ACCEPT;
                    PD_Ctl.PD_Comm_Timer = 0;
                }
                else
                {
                    PD_Ctl.PD_State = STA_TX_SOFTRST;
                    PD_Ctl.PD_Comm_Timer = 0;
                }
            }
            break;
            
        case STA_RX_DR_SWAP_ACCEPT:
            PD_Ctl.PD_Comm_Timer += Tmr_Ms_Dlt;
            if( PD_Ctl.PD_Comm_Timer > 25 )
            {
                PD_Ctl.PD_State = STA_IDLE;
                PD_Ctl.PD_Comm_Timer = 0;
            }
            break;

        default:
            break;
    }

    /* Receive message processing */
    if( PD_Ctl.Flag.Bit.Msg_Recvd )
    {
        /* Adapter communication idle timing */
        PD_Ctl.Adapter_Idle_Cnt = 0x00;
        pd_header = PD_Rx_Buf[ 0 ] & 0x1F;
        switch( pd_header )
        {
            case DEF_TYPE_ACCEPT:
                PD_Ctl.PD_Comm_Timer = 0;
                if( PD_Ctl.PD_State == STA_RX_ACCEPT_WAIT )
                {
                    PD_Ctl.PD_State = STA_RX_PS_RDY_WAIT;
                }
                break;
            case DEF_TYPE_PS_RDY:
                /* PS_RDY is received */
                printf("PS_RDY is received\r\n");
                PD_Ctl.PD_State = STA_RX_PS_RDY;
                break;
            
            case DEF_TYPE_REQUEST:
                /* Request is received */
                printf("Handle Request\r\n");
                Delay_Ms( 2 );
                PD_Ctl.ReqPDO_Idx =  ( PD_Rx_Buf[ 5 ] & 0x70 ) >> 4;
                printf("  Request:\r\n  PDO_Idx:%d\r\n",PD_Ctl.ReqPDO_Idx);
                if( ( PD_Ctl.ReqPDO_Idx == 0 ) || ( PD_Ctl.ReqPDO_Idx > 7 ) )
                {
                    PD_Ctl.PD_State = STA_TX_HRST;
                }
                else
                {
                    PD_Request_Analyse( 1, &PD_Rx_Buf[ 2 ], &Current );
                    printf("  Current:%d mA\r\n",Current);
                    if( ( PD_Rx_Buf[ 0 ] & 0xC0 ) == 0x80 )
                    {
                        /* PD3.0 */
                        PD_Ctl.Flag.Bit.PD_Version = 1;
                    }
                    else
                    {
                        PD_Ctl.Flag.Bit.PD_Version = 0;
                    }

                    PD_Ctl.PD_State = STA_TX_ACCEPT;
                    PD_Ctl.PD_Comm_Timer = 0;
                }
                break;

            case DEF_TYPE_WAIT:
                /* WAIT received, many requests may receive WAIT, need specific analysis */
                break;

            case DEF_TYPE_SOFT_RESET:
                Delay_Ms( 1 );
                PD_Load_Header( 0x00, DEF_TYPE_ACCEPT );
                PD_Send_Handle( NULL, 0 );
                break;
            case DEF_TYPE_VENDOR_DEFINED:
                /* VDM message handling */
                if( ( PD_Rx_Buf[ 2 ] & 0xC0 ) == 0 )
                {
                    /* REQ */
                    Delay_Ms( 1 );

                    /* Data to be sent is cached to PD_Tx_Buf */
                    PD_Load_Header( 0x00, DEF_TYPE_VENDOR_DEFINED );

                    /* Return to NAK */
                    if( ( PD_Rx_Buf[ 3 ] & 0x60 ) == 0 )
                    {
                        PD_Ctl.Flag.Bit.VDM_Version = 0;
                    }
                    else
                    {
                        PD_Ctl.Flag.Bit.VDM_Version = 1;
                    }
                    PD_Rx_Buf[ 2 ] |= 0x80;
                    PD_Send_Handle( &PD_Rx_Buf[ 2 ], 4 );
                }
                break;

            default:
                printf("Unsupported Command :0X%X\r\n", pd_header);
                break;
        }

        /* Message has been processed, interrupt reception is turned on again */
        PD_Rx_Mode( );
        PD_Ctl.Flag.Bit.Msg_Recvd = 0;                                    /* Clear the received flag */
        PD_Ctl.PD_BusIdle_Timer = 0;                                      /* Idle time cleared */
    }
}
