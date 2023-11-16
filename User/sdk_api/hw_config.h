/************************************************************************************************************
**	FileName:	hw_config.h
**	Description:
**	Major Function:
**
--------------------------------------------------------------------------------------------------------
**	Software:
**	Hareware:
**
**	Company:	NGDS
--------------------------------------------------------------------------------------------------------
**	Modified:
**	Author 			Date 			 Comment
________________________________________________________________________________________________________
**  Cai			2020-03-26		Original
**
**
************************************************************************************************************/


#ifndef	HW_CONFIG_H
#define	HW_CONFIG_H



/*******************************************************************************************************************
**	Hardware  Defined
********************************************************************************************************************/





/*******************************************************************************************************************
**	Parameters
********************************************************************************************************************/
#define DEBUGEN                      1


#define PROJECT_CJ                  0           //吃鸡王座
#define PROJECT_CJ_GAMEPAD          1          //吃鸡手柄
#define PROJECT_CGP                 0           //主机手柄,主机转换器
#define PROJECT_CKM                 0          //主机键鼠转换器
#define PROJECT_KEYBOARD            0           //键盘设备

#if PROJECT_CGP
    #define C2SL				    1			//星闪手柄
    #define ZKM_SLAVE_ENABLED                   1
    
    //-------------------------------------------------------
    #define GPAD_MODE_DEFAULT	    USB_CGP_MODE    //默认模式
    #define DEVICE_MODE_BIT_DEFAULT BIT(MODE_CGP)
    #define SUB_MODE_DEFAULT        0xff            //默认未定义            
    //-------------------------------------------------------
    #if C2SL
        #define APP_RGB_ENABLED                     1
        #define APP_KEY_ENABLED                     1
        #define APP_RGB_NUMS                        1

        #define UART_CMD_MAX                        72
        #define UART_LONG_CMD_MAX                   0x220
        #define UART_RX_BUFF_LEN                    UART_LONG_CMD_MAX

        #define DEFAULT_MODEL						"ZJ-C2SL"
        #define DEFAULT_NAME			            "C2SL"
        #define SW_VERSION                          0x01
    #endif
#endif

#if PROJECT_CJ_GAMEPAD
    #define K001				                1			//CH32X035+JL
    #define ZKM_SLAVE_ENABLED                   1
    
    //-------------------------------------------------------
    #define GPAD_MODE_DEFAULT	    USB_CGP_MODE    //默认模式
    #define DEVICE_MODE_BIT_DEFAULT BIT(MODE_CGP)
    #define SUB_MODE_DEFAULT        0xff            //默认未定义            
    //-------------------------------------------------------
    #if K001
        #define APP_RGB_ENABLED                     1
        #define APP_KEY_ENABLED                     1
        #define APP_RGB_NUMS                        1

        #define UART_CMD_MAX                        72
        #define UART_LONG_CMD_MAX                   0x220
        #define UART_RX_BUFF_LEN                    UART_LONG_CMD_MAX

        #define DEFAULT_MODEL						"ZJ-K001"
        #define DEFAULT_NAME			            "K001"
        #define SW_VERSION                          0x01
    #endif
#endif
/*******************************************************************************************************************
**	default define
********************************************************************************************************************/
#ifndef BATTERY_ENABLED
#define BATTERY_ENABLED			0			//battery default close
#endif


#ifndef POWERMANAGER_ENABLED
#define POWERMANAGER_ENABLED	0
#endif

#ifndef STORAGE_ENABLED
#define STORAGE_ENABLED			1
#endif

#ifndef APP_KEY_ENABLED
#define APP_KEY_ENABLED			0
#endif

#ifndef APP_LED_ENABLED
#define APP_LED_ENABLED			0
#endif

#ifndef HW_UUID_ENABLE         
#define HW_UUID_ENABLE          0
#endif


#include "zkm_config.h"


#endif

