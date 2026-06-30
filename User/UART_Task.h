#ifndef __MY_USART_h
#define __MY_USART_h
#include "stdio.h"
#include "main.h"
void UART_Task(void *params);
int fputc(int ch,FILE *F);
HAL_StatusTypeDef UART_Wait_Receive(void);
void UART_SendData(void);
void UART_Task(void *params);
void UART_Clean(void);
HAL_StatusTypeDef UART_Send_Command(char *Command);
HAL_StatusTypeDef UART_SendSpeed(uint8_t PWM_Speed);
HAL_StatusTypeDef UART_SendDistant(uint8_t Distant);
uint16_t UART_Send_Distant(void);
HAL_StatusTypeDef UART_SendRunTime(uint16_t RunTime);
HAL_StatusTypeDef UART_PWM_AveRun(void);
HAL_StatusTypeDef UART_PWM_Ave_Acc_Run(void);
HAL_StatusTypeDef UART_PWM_Ave_Down_Run(void);
HAL_StatusTypeDef UART_PWM_Ladder_Acc_Run(void);
HAL_StatusTypeDef UART_PWM_back_Run(void);
HAL_StatusTypeDef UART_PWM_STOP_Run(void);

#endif
