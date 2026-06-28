#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"
#include "adc.h"
#include "LightSensor.h"
#include "OLED.h"
#include "string.h"
#include "Get_Time.h"
#include "queue.h"
#include "string.h"
#include "timers.h"
#include "Get_Time.h"
#define xQueueLen_RealTime 10
extern uint32_t UnixTime;
extern struct Get_Time MY_Time;

static uint16_t Sensor_DMA_Data[30];
static uint16_t Sensor_All_Data[6]={0},Sensor_Ave_Data[6]={0};
/* 6个通道参数存在结构体变量中 */
static struct Sensor RedSensor;
void LightSensor_ReadData_Init(void){
	/* 开启校验 */
	HAL_ADCEx_Calibration_Start(&hadc1);
	/* 开启DMA传输数据,传满30个进中断 */
	HAL_ADC_Start_DMA(&hadc1,(uint32_t *)Sensor_DMA_Data,30);
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc){
	if(hadc->Instance==ADC1){
	uint8_t i=0,j=0;
	memset(Sensor_All_Data,0,sizeof(Sensor_All_Data));
	/* 每个通道取平均值 */
		for(j=0;j<30;j+=6){
		Sensor_All_Data[0]+=Sensor_DMA_Data[j];	
		}	
		for(j=5;j<30;j+=6){
		Sensor_All_Data[1]+=Sensor_DMA_Data[j];	
		}	
		for(j=10;j<30;j+=6){
		Sensor_All_Data[2]+=Sensor_DMA_Data[j];	
		}	
		for(j=15;j<30;j+=6){
		Sensor_All_Data[3]+=Sensor_DMA_Data[j];	
		}	
		for(j=20;j<30;j+=6){
		Sensor_All_Data[4]+=Sensor_DMA_Data[j];	
		}	
		for(j=25;j<30;j+=6){
		Sensor_All_Data[5]+=Sensor_DMA_Data[j];	
		}	
		for(i=0;i<6;i++){
		Sensor_Ave_Data[i]=Sensor_All_Data[i]/5;
		}
	}
}

void LightSensor_Task(void *params){
	LightSensor_ReadData_Init();


while(1){
	RedSensor.Sensor1=Sensor_Ave_Data[0];
	RedSensor.Sensor2=Sensor_Ave_Data[1];
	RedSensor.Sensor3=Sensor_Ave_Data[2];
	RedSensor.Sensor4=Sensor_Ave_Data[3];
	RedSensor.Sensor5=Sensor_Ave_Data[4];
	RedSensor.Sensor6=Sensor_Ave_Data[5];
	OLED_ShowNum(1,1,RedSensor.Sensor1,4);
//	OLED_ShowNum(1,8,RedSensor.Sensor2,4);
//	OLED_ShowNum(2,1,RedSensor.Sensor3,4);
//	OLED_ShowNum(2,8,RedSensor.Sensor4,4);
//	OLED_ShowNum(3,1,RedSensor.Sensor5,4);
//	OLED_ShowNum(3,8,RedSensor.Sensor6,4);
	
//	xQueueReceive( g_RealTimeHandle,&UnixTime,portMAX_DELAY);
	TimestampToTime(UnixTime);
	OLED_ShowString(1,1,"    ");
	OLED_ShowString(1,8,"    ");
	OLED_ShowString(2,1,"    ");
	OLED_ShowString(2,8,"    ");
	OLED_ShowString(3,1,"    ");
	OLED_ShowString(3,8,"    ");

	OLED_ShowNum(1,1,MY_Time.Real_Year,4);
	OLED_ShowNum(1,8,MY_Time.Real_Month,2);
	OLED_ShowNum(2,1,MY_Time.Real_Day,2);
	OLED_ShowNum(2,8,MY_Time.Real_Hour,2);
	OLED_ShowNum(3,1,MY_Time.Real_Minute,2);
	OLED_ShowNum(3,8,MY_Time.Real_Second,2);
	vTaskDelay(200);
  }
}
