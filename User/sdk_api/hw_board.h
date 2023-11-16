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
    #if C2SL
    #define KEY_HOME_GPIO           6//S_MGPIO6
    #define HW_PWM_MAP 	{\
        {31,   3,  4},\
    }

    
    #define HW_ADC_MAP_NUM  2

    //pin, null, channel
   
//    #define HW_ADC_MAP      {\
//        {22         , 0UL, SINGLE_END_AINN4},\
//        {23         , 0UL, SINGLE_END_AINP4},\
//    }
//
    
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
