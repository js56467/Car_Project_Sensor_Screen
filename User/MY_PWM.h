#ifndef __MY_PWM_H
#define __MY_PWM_H
#include "main.h"
void My_PWM_Init(void);
HAL_StatusTypeDef Motor_Set(int8_t Speed,uint8_t dir);
HAL_StatusTypeDef MY_PWM_Ave_Run(uint8_t Compare);
HAL_StatusTypeDef MY_PWM_Ave_Acc_Run(uint8_t Compare_Start);
HAL_StatusTypeDef MY_PWM_Ave_Down_Run(uint8_t Compare_Start);
HAL_StatusTypeDef MY_PWM_Ladder_Acc_Speed_Run(uint8_t Compare_Start);
HAL_StatusTypeDef MY_PWM_Back_Run(uint8_t Compare);
HAL_StatusTypeDef MY_PWM_Stop(void);
#endif
