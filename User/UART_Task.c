#include "UART_Task.h"
#include "stdio.h"
#include "usart.h"
#include "FreeRTOS.h"
#include "task.h"
#include "string.h"
#include "MY_PWM.h"
#include "OLED.h"
/* 小车轮胎半径(单位毫米) */
#define Car_Tire_Redis  34  
/* 电机每秒最大转速 */
#define ElectricMachine_Speed 3.5
static uint8_t g_uart_Rx_Buf[128];
static uint16_t UART_Num=0;
static uint8_t Data=0;
 uint16_t All_Distance;
static float Distance;
uint8_t RPWM_Speed;
extern uint32_t UnixTime;
extern uint32_t Real_Time;


/* 重定向 */
int fputc(int ch,FILE *F){
	HAL_UART_Transmit(&huart1,(uint8_t *)&ch,1,300);
	return ch;
}
/* 回调函数 */
 void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)	 
{
	if(huart -> Instance==USART1){
	  /* 给全局缓冲区存一份,Cnt++表示记的数 */
	g_uart_Rx_Buf[UART_Num++]=Data;
	//printf("收到一字节:%02x\r\n",Data);
//		if(UART_Num>=128){
//		memset(g_uart_Rx_Buf,0,sizeof(g_uart_Rx_Buf));
//		}
		/* 使能下一个数据接收 */
	HAL_UART_Receive_IT(&huart1,&Data,1);
   }
}

/* 得到每秒的距离(一转) */
float Get_Distance_Second(void){
	Distance=Car_Tire_Redis*ElectricMachine_Speed;
	return Distance;
}

/* 等待接收完成 */
HAL_StatusTypeDef UART_Wait_Receive(void){
	uint8_t Wait_Num=0;
	Wait_Num=UART_Num;
	if(Wait_Num==0){
	return HAL_ERROR;
	}
    vTaskDelay(50);
	if(Wait_Num==UART_Num){
	return HAL_OK;
	}
	return HAL_ERROR;
}


HAL_StatusTypeDef UART_ReceiveData(void){
	while(UART_Wait_Receive());
	return HAL_OK;
}

/*每接收一次数据要清除原有数据便于下一次接收*/
void UART_Clean(void){
Data=0;
UART_Num=0;
memset(g_uart_Rx_Buf,0,sizeof(g_uart_Rx_Buf));
}

/* 发送数据 */
HAL_StatusTypeDef UART_Send_Command(char *Command){
	HAL_UART_Transmit(&huart1,(uint8_t *)Command,strlen(Command),300);
	return HAL_OK;
}


/* 将得到的速度发送到触摸屏 */
HAL_StatusTypeDef UART_SendSpeed(uint8_t RPWM_Speed){
char cmd[128];
	sprintf(cmd,"EveryData.n0.val=%d\r\n",RPWM_Speed);
    UART_Send_Command(cmd);
return HAL_OK;
}

/* 得到的距离发送到触摸屏 */
uint16_t UART_Send_Distant(void){
	char cmd[128];
	All_Distance=(float)RPWM_Speed/100.0f*ElectricMachine_Speed*Car_Tire_Redis*3.14159f*2*0.001*(Real_Time-1782635570);
	
	sprintf(cmd,"EveryData.n1.val=%d\r\n",All_Distance);
	UART_Send_Command(cmd);
	return All_Distance;
}

/* 得到的运行时间发送到触摸屏 */
HAL_StatusTypeDef UART_SendRunTime(uint16_t RunTime){
	char cmd[128];
	sprintf(cmd,"EveryData.n2.val=%d\r\n",RunTime);
	UART_Send_Command(cmd);
return HAL_OK;
}

/* 串口屏发送数据,驱动小车匀速转动 */
HAL_StatusTypeDef UART_PWM_AveRun(void){
	MY_PWM_Ave_Run(20);
	UART_Clean();
	return HAL_OK;
}

/* 串口屏发送数据,驱动小车匀加速转动 */
HAL_StatusTypeDef UART_PWM_Ave_Acc_Run(void){
	MY_PWM_Ave_Acc_Run(10);
	UART_Clean();
	return HAL_OK;
}

/* 串口屏发送数据,驱动小车匀减速转动 */
HAL_StatusTypeDef UART_PWM_Ave_Down_Run(void){
	MY_PWM_Ave_Down_Run(10);
	UART_Clean();
	return HAL_OK;
}

/* 串口屏发送数据,驱动小车阶梯加速转动 */
HAL_StatusTypeDef UART_PWM_Ladder_Acc_Run(void){
	MY_PWM_Ladder_Acc_Speed_Run(10);
	UART_Clean();
	return HAL_OK;
}

/* 串口屏发送数据,驱动小车阶梯反转转动 */
HAL_StatusTypeDef UART_PWM_back_Run(void){
	MY_PWM_Back_Run(50);
	UART_Clean();
	return HAL_OK;
}

/* 串口屏发送数据,驱动小车停止转动 */
HAL_StatusTypeDef UART_PWM_STOP_Run(void){
	MY_PWM_Stop();
	UART_Clean();
	return HAL_OK;
}



void UART_Task(void *params){
	printf("12\r\n");
	/* 使能接收 */
	HAL_UART_Receive_IT(&huart1,&Data,1);
	UART_Send_Command("ABC\r\n");
	
	while(1){
    //printf("我已准备就绪，可以发送数据\r\n");
	if(strcmp((char *)g_uart_Rx_Buf,"01 02 AA")==0){
	//匀速代码
	UART_Clean();
	}
	if(strcmp((char *)g_uart_Rx_Buf,"03 04 AA")==0){
	//匀加速代码
	UART_Clean();
	}
	if(strcmp((char *)g_uart_Rx_Buf,"05 06 AA")==0){
	//匀减速代码
	UART_Clean();
	}
	if(strcmp((char *)g_uart_Rx_Buf,"07 08 AA")==0){
	//阶梯加速代码
	UART_Clean();
	}
	if(strcmp((char *)g_uart_Rx_Buf,"09 10 AA")==0){
	//反转代码
	UART_Clean();
	}
	if(strcmp((char *)g_uart_Rx_Buf,"00 00 FF")==0){
	//停止代码
	UART_Clean();
	}
    vTaskDelay(1000);
	}
}


