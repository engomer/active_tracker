#include <STM32RTC.h>
#include "RTCS.h"


STM32RTC& rtc = STM32RTC::getInstance();

void initRTCS()
{
    rtc.begin();
}

void setRTC(String dateTime){
    String formattedDate = dateTime.substring(0,11) + "20" + formattedDate.substring(11);

    rtc.setTime(int)formattedDate.substring(0,2).toInt(),(int)formattedDate.substring(3,5).toInt(),(int)formattedDate.substring(6,8).toInt());
    rtc.setDate(1,(int)formattedDate.substring(17).toInt(),(int)formattedDate.substring(14,17).toInt(),(int)formattedDate.substring(9,13).toInt());
    Serial.printf("DateTime set: %s",formattedDate);
}
String getDate(){
    String date = "";
    date = rtc.getYear() + "/" + rtc.getMonth() + "/" + rtc.getDay();
    
    return date;
}

String getTime(){
    String tim = "";
    tim = rtc.getHours() + ":" + rtc.getMinutes() + ":" + rtc.getSeconds();

    return tim;
}