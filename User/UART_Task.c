#include "UART_Task.h"
#include "stdio.h"
#include "usart.h"
#include "FreeRTOS.h"
#include "task.h"
#include "string.h"
#include "MY_PWM.h"
#include "OLED.h"
#include "Get_Time.h"
#include "queue.h"
/* 小车轮胎半径(单位毫米) */
#define Car_Tire_Redis  34  
/* 电机每秒最大转速 */
#define ElectricMachine_Speed  	    3.5
/* 控制电机匀速的速度 */
#define Motor_Ave_Start_Speed   	20
/* 控制电机匀加速的初速度 */
#define Motor_Ave_Acc_Start_Speed   10
/* 控制电机匀减速的初速度 */
#define Motor_Ave_Down_Speed        10
/* 控制电机阶梯加速的初速度 */
#define Motor_Ladder_Acc_Speed		10
/* 控制电机反转的初速度 */
#define Motor_Ave_Back_Speed   		20


/* 设置定时器时间状态的枚举类型,用于计时小车10S/20S */
typedef enum{
Timer_Idle=0,
Timer_10s_Run=1,
Timer_20s_Run=2
}Timer_State;

extern QueueHandle_t g_RealTimeHandle;
static uint8_t g_uart_Rx_Buf[128];
static uint16_t UART_Num=0;
static uint8_t Data=0;
static uint16_t All_Distance;
static Timer_State g_Timer_State=Timer_Idle;
/* 小车速度 */
uint8_t RPWM_Speed;
/* 临时变量,用于计时运行10s,此处暂时没想到要怎么只让临时变量只等于Real_Time一次,我就只能把这个变量和函数分开写了,虽然看起来不太专业,但其实都是一样的哈哈 */
uint32_t Temp_Time;
extern uint32_t UnixTime;
extern uint32_t Real_Time;
extern struct Get_Time MY_Time;


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
	float Distance=0;
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
	/* 单位CM */
	All_Distance=(float)RPWM_Speed/100.0f*ElectricMachine_Speed*Car_Tire_Redis*3.14159f*2*0.001*(Real_Time-UnixTime)*50;
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
	/* 由于我在该函数设置了给串口屏发送数据的函数,所以该函数不仅设置了速度而且将数据发送给了串口屏 */
	MY_PWM_Ave_Run(Motor_Ave_Start_Speed);
	UART_Clean();
	return HAL_OK;
}

/* 串口屏发送数据,驱动小车匀加速转动 */
HAL_StatusTypeDef UART_PWM_Ave_Acc_Run(void){
	/* 由于我在该函数设置了给串口屏发送数据的函数,所以该函数不仅设置了速度而且将数据发送给了串口屏 */
	MY_PWM_Ave_Acc_Run(Motor_Ave_Acc_Start_Speed);
	UART_Clean();
	return HAL_OK;
}

/* 串口屏发送数据,驱动小车匀减速转动 */
HAL_StatusTypeDef UART_PWM_Ave_Down_Run(void){
	/* 由于我在该函数设置了给串口屏发送数据的函数,所以该函数不仅设置了速度而且将数据发送给了串口屏 */
	MY_PWM_Ave_Down_Run(Motor_Ave_Down_Speed);
	UART_Clean();
	return HAL_OK;
}

/* 串口屏发送数据,驱动小车阶梯加速转动 */
HAL_StatusTypeDef UART_PWM_Ladder_Acc_Run(void){
	/* 由于我在该函数设置了给串口屏发送数据的函数,所以该函数不仅设置了速度而且将数据发送给了串口屏 */
	MY_PWM_Ladder_Acc_Speed_Run(Motor_Ladder_Acc_Speed);
	UART_Clean();
	return HAL_OK;
}

/* 串口屏发送数据,驱动小车阶梯反转转动 */
HAL_StatusTypeDef UART_PWM_back_Run(void){
	/* 由于我在该函数设置了给串口屏发送数据的函数,所以该函数不仅设置了速度而且将数据发送给了串口屏 */
	MY_PWM_Back_Run(Motor_Ave_Back_Speed);
	UART_Clean();
	return HAL_OK;
}

/* 串口屏发送数据,驱动小车停止转动 */
HAL_StatusTypeDef UART_PWM_STOP_Run(void){
	/* 由于我在该函数设置了给串口屏发送数据的函数,所以该函数不仅设置了速度而且将数据发送给了串口屏 */
	MY_PWM_Stop();
	UART_Clean();
	return HAL_OK;
}

/* 给串口屏发送时间 */
HAL_StatusTypeDef UART_Send_Time(uint32_t Real_Time_Now){
	/* 为何设置cmd1-6就不行嘞？ */
char cmd[128];
	TimestampToTime(Real_Time_Now);
	sprintf(cmd,"Time.n0.val=%d",MY_Time.Real_Year);
	UART_Send_Command(cmd);
	sprintf(cmd,"Time.n1.val=%d",MY_Time.Real_Month);
	UART_Send_Command(cmd);
	sprintf(cmd,"Time.n2.val=%d",MY_Time.Real_Day);
	UART_Send_Command(cmd);
	sprintf(cmd,"Time.n3.val=%d",MY_Time.Real_Hour);
	UART_Send_Command(cmd);
	sprintf(cmd,"Time.n4.val=%d",MY_Time.Real_Minute);
	UART_Send_Command(cmd);
	sprintf(cmd,"Time.n5.val=%d",MY_Time.Real_Second);
	UART_Send_Command(cmd);
return HAL_OK;
}

/* 通过触摸屏按键发送指令,计时匀速运行10s */
HAL_StatusTypeDef UART_Set_Time_10Second(void){
	if(Real_Time<=Temp_Time+10){
	UART_PWM_AveRun();
	}else
	{
	UART_PWM_STOP_Run();
	}
	return HAL_OK;
}
/* 通过触摸屏按键发送指令,计时匀速运行20s */
HAL_StatusTypeDef UART_Set_Time_20Second(void){
	if(Real_Time<=Temp_Time+20){
	UART_PWM_AveRun();
	}else
	{
	UART_PWM_STOP_Run();
	}
	return HAL_OK;
}

/* 通过枚举类型来使TempTime和RealTime等于一次的情况下还不影响系统的实时性,说白了就是程序判断设立一个状态再在这个得到的判断的值来进行相应操作 */
HAL_StatusTypeDef Timer_Enum_State_Judege(uint8_t State){
switch(State){
	
	case Timer_10s_Run:if(Real_Time<=Temp_Time+10){UART_PWM_AveRun();}else{UART_PWM_STOP_Run();g_Timer_State=Timer_Idle;}break;
	case Timer_20s_Run:if(Real_Time<=Temp_Time+20){UART_PWM_AveRun();}else{UART_PWM_STOP_Run();g_Timer_State=Timer_Idle;}break;
	default: break;
 }
return HAL_OK;
}

void UART_Task(void *params){
	printf("12\r\n");
	/* 使能接收 */
	HAL_UART_Receive_IT(&huart1,&Data,1);
	UART_Send_Command("ABC\r\n");
	//UART_PWM_AveRun();
	//UART_PWM_Ladder_Acc_Run();
	while(1){
    //printf("我已准备就绪，可以发送数据\r\n");
	if(strcmp((char *)g_uart_Rx_Buf,"01 02 A A")==0){
	//匀速代码
	UART_PWM_AveRun();

	UART_Clean();
	}
	if(strcmp((char *)g_uart_Rx_Buf,"03 04 AA")==0){
	//匀加速代码
	UART_PWM_Ave_Acc_Run();
	UART_Clean();
	}
	if(strcmp((char *)g_uart_Rx_Buf,"05 06 AA")==0){
	//匀减速代码
	UART_PWM_Ladder_Acc_Run();
	
	UART_Clean();
	}
	if(strcmp((char *)g_uart_Rx_Buf,"07 08 AA")==0){
	//阶梯加速代码
	UART_PWM_Ladder_Acc_Run();
	UART_Clean();
	}
	if(strcmp((char *)g_uart_Rx_Buf,"09 10 AA")==0){
	//反转代码
	UART_PWM_back_Run();
	UART_Clean();
	}
	if(strcmp((char *)g_uart_Rx_Buf,"00 00 FF")==0){
	//停止代码
	MY_PWM_Stop();
	UART_Clean();
	}
	
	if(strcmp((char *)g_uart_Rx_Buf,"10 10 FF")==0){
	/* 计时10s */
	Temp_Time=Real_Time;
	g_Timer_State=Timer_10s_Run;
	UART_Clean();
	}
	
	if(strcmp((char *)g_uart_Rx_Buf,"20 20 FF")==0){
	/* 计时20s */
	Temp_Time=Real_Time;
	g_Timer_State=Timer_20s_Run;
	UART_Clean();
	}
	/* 给串口屏发送距离 */
	UART_Send_Distant();
	xQueueReceive(g_RealTimeHandle,&Real_Time,portMAX_DELAY);
	/* 给串口屏发送时间(年月日时分秒6个参数) */
	UART_Send_Time(Real_Time);
	OLED_ShowNum(1,1,MY_Time.Real_Year,4);
	OLED_ShowNum(1,8,MY_Time.Real_Month,2);
	OLED_ShowNum(2,1,MY_Time.Real_Day,2);
	OLED_ShowNum(2,8,MY_Time.Real_Hour,2);
	OLED_ShowNum(3,1,MY_Time.Real_Minute,2);
	OLED_ShowNum(3,8,MY_Time.Real_Second,2);
	//OLED_ShowNum(4,1,UART_Send_Distant(),6);
	/* 测试定时函数 */
	Timer_Enum_State_Judege(g_Timer_State);
	//printf("RX[%d]",UART_Num);
	//	printf("%s",g_uart_Rx_Buf);
	//	OLED_ShowNum(2,13,2,1);

	//UART_Set_Time_20Second();
	OLED_ShowNum(1,13,RPWM_Speed,3);
    vTaskDelay(200);
	}
}




