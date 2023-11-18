/************************************************************************************************************
**	FileName:	hw_board.c
**	Description:
**	Major Function:
**
--------------------------------------------------------------------------------------------------------
**	Software:
**	Hareware:
**
**	Company:	zikway
--------------------------------------------------------------------------------------------------------
**	Modified:

**	Author 			Date 			 Comment
_________________________________________________________________________________________________________
**	Cai			2020-03-26		Original
**
**
************************************************************************************************************/


#ifndef HW_BOARD_H
#define HW_BOARD_H

#include "hw_config.h"


#define STORAGE_MAP_MAX	0x200



#define INVALID_GPIO	(-1)			//		gpio 31 are not used

#if PROJECT_CJ_GAMEPAD
    #if K001
    #define KEY_A_GPIO           PB_08
    #define KEY_SELECT_GPIO           PB_09      //MENU
    #define KEY_START_GPIO           PB_10  //turbo
    #define KEY_R1_GPIO           PB_11     //RB
    #define KEY_M2_GPIO           PA_10      //M2
    #define KEY_B_GPIO           PA_11
    #define KEY_M1_GPIO           PA_12         //M1
    #define KEY_CAPTURE_GPIO           PA_15         //LS
    #define KEY_L2_GPIO           PA_18         //LB
    #define KEY_LEFT_GPIO           PA_20         //LEFT

    #define HW_ADC_MAP_NUM  8
    //PA_00 lx,
    //PA_01 ly,
    //PA_05 rx,
    //PA_06 ry,
    //PA_04 lt,
    //PA_09 rt,
    //PB_00 手機電源VBUS
    //PA_07 充電器電源VIN,
    #define HW_ADC_MAP      {\
        {PA_00, 0UL, 0},\
        {PA_01, 0UL, 1},\
        {PA_05, 0UL, 5},\
        {PA_06, 0UL, 6},\
        {PA_04, 0UL, 4},\
        {PA_09, 0UL, 9},\
        {PB_00, 0UL, 8},\
        {PA_07, 0UL, 7},\
        }  
    
    #endif

    #ifndef STORE_STRUCT_DEF
    #define     STORE_STRUCT_DEF    {\
        uint8_t flash_head;\
        uint8_t flash_res[5];\
        uint8_t gpad_mode;\
        uint8_t sub_mode;\
        uint8_t res[7];\
        int16_t rocker_cal[18];\
        uint8_t map [GAMEPAD_CONFIG_MAX*GAMEPAD_SETTING_LEN];\
    }
    #endif
#endif
#endif // HW_BOARD_H
