#ifndef KEY_H
#define KEY_H

#include "ch32x035.h"
#include "hw_borad.h"
#include <string.h>

typedef struct {
    uint32_t key;  //hw_xx_key
    int16_t lx;
    int16_t ly;
    int16_t rx;
    int16_t ry;
    uint8_t l2;         //仅模拟量有效
    uint8_t r2;         //14
}gamepad_key_t;         //28

typedef struct {
    uint8_t pack_head;
    uint8_t len;
    uint8_t type;
    gamepad_key_t gamepad_key;
    uint8_t sum;
}key_t_pack;         //28




#define KEY_FILTER_LONG_LONG    (500)           //单位10ms
#define KEY_FILTER_LONG         (200)
#define KEY_FILTER_SHORT        (100)



extern volatile uint32_t mSysTick;

#define HW_KEY_A_POS            (0)
#define HW_KEY_B_POS            (1)
#define HW_KEY_C_POS            (2)
#define HW_KEY_X_POS            (3)
#define HW_KEY_Y_POS            (4)
#define HW_KEY_Z_POS            (5)
#define HW_KEY_L1_POS           (6)
#define HW_KEY_R1_POS           (7)
#define HW_KEY_L2_POS           (8)
#define HW_KEY_R2_POS           (9)
#define HW_KEY_SELECT_POS       (10)
#define HW_KEY_START_POS        (11)
#define HW_KEY_HOME_POS         (12)
#define HW_KEY_L3_POS           (13)
#define HW_KEY_R3_POS           (14)
#define HW_KEY_RES_POS          (15)

#define HW_KEY_UP_POS           (16)
#define HW_KEY_DOWN_POS         (17)
#define HW_KEY_LEFT_POS         (18)
#define HW_KEY_RIGHT_POS        (19)
#define HW_KEY_BACK_POS         (20)
#define HW_KEY_MODE_POS         (21)
#define HW_KEY_MENU_POS         (22)
#define HW_KEY_M1_POS           (23)
#define HW_KEY_M2_POS           (24)
#define HW_KEY_M3_POS           (25)
#define HW_KEY_M4_POS           (26)
#define HW_KEY_M5_POS           (27)
#define HW_KEY_M6_POS           (28)
#define HW_KEY_M7_POS           (29)
#define HW_KEY_M8_POS           (30)
#define HW_KEY_POWER_POS        (31)        //POWER
#define HW_KEY_MAX_POS          (32)

#define HW_KEY_NULL         ( 0)
#define HW_KEY_A            ( 1UL<<0)
#define HW_KEY_B            ( 1UL<<1)
#define HW_KEY_C            ( 1UL<<2)
#define HW_KEY_X            ( 1UL<<3)
#define HW_KEY_Y            ( 1UL<<4)
#define HW_KEY_Z            ( 1UL<<5)
#define HW_KEY_L1           ( 1UL<<6)
#define HW_KEY_R1           ( 1UL<<7)
#define HW_KEY_L2           ( 1UL<<8)
#define HW_KEY_R2           ( 1UL<<9)
#define HW_KEY_SELECT       ( 1UL<<10)
#define HW_KEY_START        ( 1UL<<11)
#define HW_KEY_HOME         ( 1UL<<12)
#define HW_KEY_L3           ( 1UL<<13)
#define HW_KEY_R3           ( 1UL<<14)
#define HW_KEY_CAPTURE      ( 1UL<<15)      //截图/返回/touchkey

#define HW_KEY_UP           ( 1UL<<16)
#define HW_KEY_DOWN         ( 1UL<<17)
#define HW_KEY_LEFT         ( 1UL<<18)
#define HW_KEY_RIGHT        ( 1UL<<19)
#define HW_KEY_BACK         ( 1UL<<20)      //功能按键
#define HW_KEY_MODE         ( 1UL<<21)      //功能按键
#define HW_KEY_MENU         ( 1UL<<22)      //功能按键
#define HW_KEY_M1           ( 1UL<<23)
#define HW_KEY_M2           ( 1UL<<24)
#define HW_KEY_M3           ( 1UL<<25)
#define HW_KEY_M4           ( 1UL<<26)
#define HW_KEY_M5           ( 1UL<<27)
#define HW_KEY_M6           ( 1UL<<28)
#define HW_KEY_M7           ( 1UL<<29)
#define HW_KEY_M8           ( 1UL<<30)
#define HW_KEY_POWER        ( 1UL<<31)      //POWER




#define KEY_A                             (GPIO_ReadInputDataBit(GPIOB,KEY_A_GPIO))
#define KEY_B                             (GPIO_ReadInputDataBit(GPIOA,KEY_B_GPIO))
#define KEY_X                             (GPIO_ReadInputDataBit(GPIOB,KEY_X_GPIO))
#define KEY_Y                             (GPIO_ReadInputDataBit(GPIOB,KEY_Y_GPIO))

#define KEY_UP                            (GPIO_ReadInputDataBit(GPIOA,KEY_UP_GPIO))
#define KEY_DOWN                          (GPIO_ReadInputDataBit(GPIOA,KEY_DOWN_GPIO))
#define KEY_LEFT                          (GPIO_ReadInputDataBit(GPIOA,KEY_LEFT_GPIO))
#define KEY_RIGHT                         (GPIO_ReadInputDataBit(GPIOC,KEY_RIGHT_GPIO))

#define KEY_L1                            (GPIO_ReadInputDataBit(GPIOA,KEY_L1_GPIO))
#define KEY_R1                            (GPIO_ReadInputDataBit(GPIOB,KEY_R1_GPIO))
#define KEY_R2                            (Scan_R2_trigger())
#define KEY_L2                            (Scan_L2_trigger())
#define KEY_L3                            (GPIO_ReadInputDataBit(GPIOA,KEY_L3_GPIO))
#define KEY_R3                            (GPIO_ReadInputDataBit(GPIOA,KEY_R3_GPIO))

#define KEY_M1                            (GPIO_ReadInputDataBit(GPIOA,KEY_M1_GPIO))
#define KEY_M2                            (GPIO_ReadInputDataBit(GPIOA,KEY_M2_GPIO))
#define KEY_START                         (GPIO_ReadInputDataBit(GPIOB,KEY_START_GPIO)) //MEUM
#define KEY_M4                            (GPIO_ReadInputDataBit(GPIOB,KEY_M4_GPIO)) //TURBO

#define KEY_SELECT                        (GPIO_ReadInputDataBit(GPIOA,KEY_SELECT_GPIO)) //视窗
#define KEY_CAPTURE                       (GPIO_ReadInputDataBit(GPIOA,KEY_CAPTURE_GPIO)) //截图



#define KEY_A_TURN                          (GPIOA_InverseBits(KEY_A_GPIO))
#define KEY_B_TURN                          (GPIOA_InverseBits(KEY_B_GPIO))
#define KEY_X_TURN                          (GPIOA_InverseBits(KEY_X_GPIO))
#define KEY_Y_TURN                          (GPIOA_InverseBits(KEY_Y_GPIO))

#define KEY_UP_TURN                         (GPIOB_InverseBits(KEY_UP_GPIO))
#define KEY_DOWN_TURN                       (GPIOA_InverseBits(KEY_DOWN_GPIO))
#define KEY_LEFT_TURN                       (GPIOB_InverseBits(KEY_LEFT_GPIO))
#define KEY_RIGHT_TURN                      (GPIOA_InverseBits(KEY_RIGHT_GPIO))

#define KEY_L1_TURN                         (GPIOA_InverseBits(KEY_L1_GPIO))
#define KEY_R1_TURN                         (GPIOA_InverseBits(KEY_R1_GPIO))
#define KEY_R2_TURN
#define KEY_L2_TURN
#define KEY_L3_TURN                         (GPIOA_InverseBits(KEY_L3_GPIO))
#define KEY_R3_TURN                         (GPIOA_InverseBits(KEY_R3_GPIO))





extern gamepad_key_t gpad_key;



uint32_t iokey_scan(void);
uint8_t hw_key_scan(uint8_t *pkey,uint8_t len);
uint8_t Scan_R2_trigger(void);
uint8_t Scan_L2_trigger(void);
uint8_t check_sum(void * buf,uint16_t len);


#endif
