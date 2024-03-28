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
 u8  test_buf[255]={0};
 bool usart_flag=false;
bool sdk_uart_init(void)
{
//    GPIO_InitTypeDef  GPIO_InitStructure;
//    USART_InitTypeDef USART_InitStructure;

  #if (INVALID_GPIO != TR_UART_TX_PIN) || (INVALID_GPIO != TR_UART_RX_PIN)
           GPIO_InitTypeDef  GPIO_InitStructure = {0};
           USART_InitTypeDef USART_InitStructure = {0};
           NVIC_InitTypeDef  NVIC_InitStructure = {0};

           RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
		   GPIO_PinRemapConfig(GPIO_PartialRemap1_USART3,ENABLE);
           GPIO_PinRemapConfig(GPIO_Remap_SWJ_Disable,ENABLE);
           RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
           RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
           /* USART2 TX-->A.2   RX-->A.3 */
           GPIO_InitStructure.GPIO_Pin = GPIO_Pin_18;
           GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
           GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
           GPIO_Init(GPIOC, &GPIO_InitStructure);
           GPIO_InitStructure.GPIO_Pin = GPIO_Pin_19;
           GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
           GPIO_Init(GPIOC, &GPIO_InitStructure);

           USART_InitStructure.USART_BaudRate = UART_BOARDATE;
           USART_InitStructure.USART_WordLength = USART_WordLength_8b;
           USART_InitStructure.USART_StopBits = USART_StopBits_1;
           USART_InitStructure.USART_Parity = USART_Parity_No;
           USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
           USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;

           USART_Init(USART3, &USART_InitStructure);
           USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);


           NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
           NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
           NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
           NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
           NVIC_Init(&NVIC_InitStructure);

           USART_Cmd(USART3, ENABLE);
  #endif
#if (INVALID_GPIO != TR_UART2_TX_PIN) || (INVALID_GPIO != TR_UART2_RX_PIN)
           GPIO_InitTypeDef  gpio_InitStructure = {0};
           USART_InitTypeDef usart_InitStructure = {0};
           NVIC_InitTypeDef  nvic_InitStructure = {0};

           RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
           RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

           /* USART2 TX-->A.2   RX-->A.3 */
           gpio_InitStructure.GPIO_Pin = GPIO_Pin_2;
           gpio_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
           gpio_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
           GPIO_Init(GPIOA, &gpio_InitStructure);
           gpio_InitStructure.GPIO_Pin = GPIO_Pin_3;
           gpio_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
           GPIO_Init(GPIOA, &gpio_InitStructure);

           usart_InitStructure.USART_BaudRate = UART_BOARDATE;
           usart_InitStructure.USART_WordLength = USART_WordLength_8b;
           usart_InitStructure.USART_StopBits = USART_StopBits_1;
           usart_InitStructure.USART_Parity = USART_Parity_No;
           usart_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
           usart_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;

           USART_Init(USART2, &usart_InitStructure);
           USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);


           nvic_InitStructure.NVIC_IRQChannel = USART2_IRQn;
           nvic_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
           nvic_InitStructure.NVIC_IRQChannelSubPriority = 1;
           nvic_InitStructure.NVIC_IRQChannelCmd = ENABLE;
           NVIC_Init(&nvic_InitStructure);

           USART_Cmd(USART2, ENABLE);
     #endif
   logd("UART init\r\n");
    return true;
}

void sdk_uart_close(void)
{

}


void sdk_uart_send(uint8_t index, uint8_t * buffer, uint16_t len)
{
    uint8_t i;
    if(0 == index){
        for(i=0;i<len;i++){
            while(USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET);
            USART_SendData(USART2, *buffer);
            buffer++;
       }     
    }else{
        printf("tx:\r\n");dump(buffer,len);
        for(i=0;i<len;i++){
            while(USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET);
            USART_SendData(USART3, *buffer);

            buffer++;
       }        
    }
}

 void USART2_IRQHandler(void)
{
     uint16_t w_len=9;
     static uint32_t i;
     volatile uint8_t val;
     if(USART_GetFlagStatus(USART2, USART_FLAG_ORE) == SET)
    {
        //logd("hard ov \r\n");  //串口硬件缓冲buf溢出 上一个字节还未处理  又收到第二个字节
        USART_ClearFlag(USART2, USART_FLAG_ORE); //清除溢出中断
       // USART_ReceiveData(USART2);
    }
//    if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
       {
//        USART_ClearITPendingBit(USART2, USART_IT_RXNE);
//        test_buf[i] =USART_ReceiveData(USART2);
//        i++;
//        if(i==9){
//            i=0;
//           app_fifo_write(&m_uart_rx_fifo,test_buf,&w_len);
//        }
        val = USART_ReceiveData(USART2);
               if(ERROR_SUCCESS != app_fifo_put(&m_uart_rx_fifo, val))
               {
                   #if DEBUGEN
                   #endif
               }
       }
}
void USART3_IRQHandler(void)
{
     uint16_t w_len=9;
     static uint32_t i;
     volatile uint8_t val;
     if(USART_GetFlagStatus(USART3, USART_FLAG_ORE) == SET)
    {
        //logd("hard ov \r\n");  //串口硬件缓冲buf溢出 上一个字节还未处理  又收到第二个字节
        USART_ClearFlag(USART3, USART_FLAG_ORE); //清除溢出中断
       // USART_ReceiveData(USART2);
    }
//    if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
       {
//        USART_ClearITPendingBit(USART2, USART_IT_RXNE);
//        test_buf[i] =USART_ReceiveData(USART2);
//        i++;
//        if(i==9){
//            i=0;
//           app_fifo_write(&m_uart_rx_fifo,test_buf,&w_len);
//        }

        val = USART_ReceiveData(USART3);
               if(ERROR_SUCCESS != app_fifo_put(&m_uart_rx_fifo, val))
               {
                   #if DEBUGEN
                   logd("uv\n\r");
                   #endif
               }
       }
}
