#include <stdint.h>
#include "Get_Time.h"


/* 单纯移植函数不一定要以下部分 */
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"
#include "queue.h"
struct Get_Time MY_Time;
extern QueueHandle_t g_RealTimeHandle;
uint32_t UnixTime=1782635570;
uint32_t Real_Time;
/* 判断是否闰年 */
static uint8_t IsLeapYear(uint16_t year)
{
    return ((year % 400 == 0) || ((year % 4 == 0) && (year % 100 != 0)));
}

/* 时间戳转换函数 */
void TimestampToTime(uint32_t timestamp)
{
    /* 北京时间 */
     timestamp += 8 * 3600;

    uint32_t days;
    uint32_t remain;
    uint16_t year = 1970;
    uint8_t month;

    const uint8_t MonthDay[12] =
    {
        31,28,31,30,31,30,
        31,31,30,31,30,31
    };

    /* 时分秒 */
    days = timestamp / 86400;
    remain = timestamp % 86400;

    MY_Time.Real_Hour = remain / 3600;
    remain %= 3600;

    MY_Time.Real_Minute = remain / 60;
    MY_Time.Real_Second = remain % 60;

    /* 年 */
    while (1)
    {
        uint16_t YearDay = IsLeapYear(year) ? 366 : 365;

        if (days >= YearDay)
        {
            days -= YearDay;
            year++;
        }
        else
        {
            break;
        }
    }

    MY_Time.Real_Year = year;

    /* 月 */
    for (month = 0; month < 12; month++)
    {
        uint8_t mdays = MonthDay[month];

        if ((month == 1) && IsLeapYear(year))
        {
            mdays = 29;
        }

        if (days >= mdays)
        {
            days -= mdays;
        }
        else
        {
            break;
        }
    }

    MY_Time.Real_Month = month + 1;

    /* 日 */
    MY_Time.Real_Day = days + 1;
}


/* 让时间戳自加1的函数,通过提高该任务优先级来完成模拟每秒+1的过程(单纯移植不用该函数) */
void RTime_MakeUnixUP(void *params){
	while(1){
	Real_Time=UnixTime++;
	vTaskDelay(1000);
		
	}
}


