// SPDX-License-Identifier: MIT

#include "sdk_api_uart.h"
#include "hw_debug.h"
#include "hw_uart.h"
#include "app_fifo.h"
#include "zkm.h"
#define UART_BAUDRATE                      460800
#define UART_DATA_BITS                     UART_DATA_BIT_8
#define UART_STOP_BITS                     UART_STOP_BIT_1
#define UART_PARITY_BIT                    UART_PARITY_NONE


bool sdk_uart_init(void)
{


    return true;
}

void sdk_uart_close(void)
{

}

void sdk_uart_send(uint8_t index, uint8_t * buffer, uint16_t len)
{

}
