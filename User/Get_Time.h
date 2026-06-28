#ifndef __Get_Time_h
#define __Get_Time_h

struct Get_Time{
uint16_t Real_Year;
uint8_t  Real_Month;
uint8_t  Real_Day;
uint8_t  Real_Hour;
uint8_t  Real_Minute;
uint8_t  Real_Second;
};
static uint8_t IsLeapYear(uint16_t year);
void TimestampToTime(uint32_t timestamp);
void RTime_MakeUnixUP(void *);
#endif
