/********************************** (C) COPYRIGHT *******************************
* File Name          : PD_process.c
* Author             : WCH
* Version            : V1.0.0
* Date               : 2023/04/06
* Description        : This file provides all the PD firmware functions.
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/


#include "ch32x035_qc.h"

  _QC_CONTROL  QC_Ctrl = {0};











void Set_dp_0v6(void)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);               //On PD I/O clock, AFIO clock and PD clock
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_17;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
    GPIO_Init(GPIOC,&GPIO_InitStructure);
    AFIO->CTLR |= UDP_BC_VSRC;
}

void Set_dm_0v6(void)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);               //On PD I/O clock, AFIO clock and PD clock
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_16;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
    GPIO_Init(GPIOC,&GPIO_InitStructure);
    AFIO->CTLR |= UDM_BC_VSRC;
}
void Set_dm_3v3(void)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);               //On PD I/O clock, AFIO clock and PD clock
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_16;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOC,&GPIO_InitStructure);
    AFIO->CTLR &= ~UDM_BC_VSRC;
    AFIO->CTLR |= USB_IOEN | UDM_PUE1 |UDM_PUE0;
}
void Set_dp_3v3(void)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);               //On PD I/O clock, AFIO clock and PD clock
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_17;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOC,&GPIO_InitStructure);
    AFIO->CTLR &= ~UDP_BC_VSRC;
    AFIO->CTLR |= USB_IOEN | UDP_PUE1 |UDP_PUE0;
}

void Set_dp_0v0(void)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);               //On PD I/O clock, AFIO clock and PD clock
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_17;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
    GPIO_Init(GPIOC,&GPIO_InitStructure);
    AFIO->CTLR &= ~UDP_BC_VSRC;

}



void Set_dm_0v0(void)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);               //On PD I/O clock, AFIO clock and PD clock
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_16;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
    GPIO_Init(GPIOC,&GPIO_InitStructure);
    AFIO->CTLR &= ~UDM_BC_VSRC;

}




void QC_Request_5v(void)
{
    Set_dm_0v0();
    Set_dp_0v6();


}


void QC_Request_9v(void)
{
    Set_dm_0v6();
    Set_dp_3v3();
}


void QC_Request_12v(void)
{
    Set_dm_0v6();
    Set_dp_0v6();


}


void QC_Request_20v(void)
{
    Set_dm_3v3();
    Set_dp_3v3();

}

/*******************************************************************************
* Description    : BC1.2���
*******************************************************************************/
uint8_t BC1_2Check(void)
{
    printf( " BC1.2_Det  \r\n" );
    Set_dp_0v6();
    QC_Ctrl.BC_Det_Cnt = 40;
    while(QC_Ctrl.BC_Det_Cnt--)
    {
       if(AFIO->CTLR & UDM_BC_CMPO)
       {
           QC_Ctrl.BC_Det_Flg = ENABLE;
       }else{
           QC_Ctrl.BC_Det_Flg = DISABLE;
       }
       Delay_Ms(1);
    //    printf( " QC_Ctrl.BC_Det_Cnt = %d  \r\n",QC_Ctrl.BC_Det_Cnt );
    }//DCP,CDP
    if(QC_Ctrl.BC_Det_Flg)
    {
        Set_dp_0v0();
        Set_dm_0v6();
        QC_Ctrl.BC_Det_Cnt = 40;
        while(QC_Ctrl.BC_Det_Cnt--)
        {
            if(AFIO->CTLR & UDP_BC_CMPO)
            {
               QC_Ctrl.BC_Det_Flg = ENABLE;
            }else{
               QC_Ctrl.BC_Det_Flg = DISABLE;
            }
            Delay_Ms(1);
        }
        if(QC_Ctrl.BC_Det_Flg)
        {
            Set_dm_0v0();
            Set_dp_0v6();
            QC_Ctrl.BC_Det_Cnt = 135;
            while(QC_Ctrl.BC_Det_Cnt--)
            {
               if(!(AFIO->CTLR & UDM_BC_CMPO))
               {
                   printf( " BC1.2_OK  \r\n" );
                  return BC1_2;
               }
               Delay_Ms(10);
            }
        }else{
            return CDP;
        }
    }else{
        return SDP;
    }
    return DCP;
}


