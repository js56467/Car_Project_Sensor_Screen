#include "MY_PWM.h"
#include "tim.h"
#include "stdio.h"
#include "usart.h"
#include "FreeRTOS.h"
#include "task.h"
#include "OLED.h"
extern uint8_t RPWM_Speed;
void My_PWM_Init(void){
HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_3);
}

HAL_StatusTypeDef Motor_Set(int8_t Speed,uint8_t dir){
	/* 设置占空比 */
__HAL_TIM_SET_COMPARE(&htim3,TIM_CHANNEL_3,Speed);
	
	/* 正转 */
	if(dir==1){
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_12,GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_13,GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_14,GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_15,GPIO_PIN_RESET);
	}
	/* 反转 */
	else if(dir==0){
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_12,GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_13,GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_14,GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_15,GPIO_PIN_RESET);
	}
	return HAL_OK;
}

/*匀速*/
HAL_StatusTypeDef MY_PWM_Ave_Run(uint8_t Compare){
	Motor_Set(Compare,1);
	RPWM_Speed=Compare;
	return HAL_OK;

}
/* 匀加速 */
HAL_StatusTypeDef MY_PWM_Ave_Acc_Run(uint8_t Compare_Start){
	uint8_t i;
	for(i=Compare_Start;i<80;i++){
	Motor_Set(i,1);
	RPWM_Speed=i;
	OLED_ShowNum(4,1,RPWM_Speed,3);
	
	if(i>=80){
	i=80;
	}
	vTaskDelay(100);
  }
	return HAL_OK;
}

/*匀减速*/
HAL_StatusTypeDef MY_PWM_Ave_Down_Run(uint8_t Compare_Start){
	uint8_t i;
	for(i=Compare_Start;i>10;i--){
	Motor_Set(i,1);
	RPWM_Speed=i;
	OLED_ShowNum(4,1,RPWM_Speed,3);
	if(i<=10){
	i=10;
	}
	vTaskDelay(100);
  }
	return HAL_OK;
}

/* 阶梯加速 */
HAL_StatusTypeDef MY_PWM_Ladder_Acc_Speed_Run(uint8_t Compare_Start){
	uint8_t i;
	for(i=Compare_Start;i<80;i+=10){
	Motor_Set(i,1);
	RPWM_Speed=i;
	OLED_ShowNum(4,1,RPWM_Speed,3);
	if(i>=80){
	i=80;
	}
	vTaskDelay(1000);
}
	return HAL_OK;
}

/* 反转 */
HAL_StatusTypeDef MY_PWM_Back_Run(uint8_t Compare){
	Motor_Set(Compare,0);
	RPWM_Speed=Compare;
	return HAL_OK;
}

/* 停止 */
HAL_StatusTypeDef MY_PWM_Stop(void){
	Motor_Set(0,1);
	RPWM_Speed=0;
	return HAL_OK;
}














