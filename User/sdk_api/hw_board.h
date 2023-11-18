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
