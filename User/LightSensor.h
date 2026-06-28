#ifndef __LightSensor_h
#define __LightSensor_h
struct Sensor{
	uint16_t Sensor1;
	uint16_t Sensor2;
	uint16_t Sensor3;
	uint16_t Sensor4;
	uint16_t Sensor5;
	uint16_t Sensor6;
};
void LightSensor_Task(void *params);
void LightSensor_ReadData_Init(void);

#endif
