#ifndef MAIN_H
#define MAIN_H



#include "ch32x035.h"
#include "hw_borad.h"
#include "key.h"

extern uint16_t    l2_value;
extern uint16_t    r2_value;
extern uint16_t    ly_value;
extern uint16_t    ry_value;
extern uint16_t    rx_value;
extern uint16_t    lx_value;


void TIM1_UP_IRQHandler(void)__attribute__((interrupt()));

void ADC1_IRQHandler(void)__attribute__((interrupt()));


















#endif
