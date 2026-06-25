#include "MY_PWM.h"
#include "tim.h"
#include "stdio.h"
#include "usart.h"
#include "FreeRTOS.h"
#include "task.h"
uint8_t *Speed;
void My_PWM_Init(void){
HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_3);
}

HAL_StatusTypeDef Motor_Set(int8_t Speed,uint8_t dir){
	/* 设置占空比 */
__HAL_TIM_SET_COMPARE(&htim3,TIM_CHANNEL_3,Speed);
	
	/* 正转 */
	if(dir==1){
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_1,GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_2,GPIO_PIN_RESET);
	}
	/* 反转 */
	else if(dir==0){
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_2,GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_1,GPIO_PIN_RESET);
	}
	return HAL_OK;
}

/*匀速*/
HAL_StatusTypeDef MY_PWM_Ave_Run(void){
	Motor_Set(50,1);
	return HAL_OK;

}
/* 匀加速 */
HAL_StatusTypeDef MY_PWM_Ave_Acc_Run(void){
	int8_t Speed=20;
	for(Speed=20;Speed<100;Speed++){
	Motor_Set(Speed,1);
	if(Speed>=100){
		Speed=100;
	}
	vTaskDelay(100);
  }
	return HAL_OK;
}

/*匀减速*/
HAL_StatusTypeDef MY_PWM_Ave_Down_Run(void){
	int8_t Speed=60;
	Motor_Set(Speed--,1);
	if(Speed<=20){
	Speed=20;
	}
	return HAL_OK;
}

/* 阶梯加速 */
HAL_StatusTypeDef MY_PWM_Ladder_Acc_Speed_Run(void){
	int8_t Speed=0;
	Motor_Set(Speed,1);
	Speed+=10;
	if(Speed>=100){
	Speed=100;
	}
	return HAL_OK;
}

/* 反转 */
HAL_StatusTypeDef MY_PWM_Back_Run(void){
	Motor_Set(50,0);
	return HAL_OK;
}

/* 停止 */
HAL_StatusTypeDef MY_PWM_Stop(void){
	Motor_Set(0,1);
	return HAL_OK;
}














