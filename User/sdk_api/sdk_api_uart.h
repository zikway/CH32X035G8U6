#ifndef SDK_API_UART_H
#define SDK_API_UART_H


#include "sdk_typedef.h"

bool sdk_uart_init(void);

void sdk_api_uart_deinit(void);
void USART2_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));

#endif //SDK_API_UART_H
