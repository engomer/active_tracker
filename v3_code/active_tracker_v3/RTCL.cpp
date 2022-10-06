#include <MyRealTimeClock.h>
#include "RTCL.h"

#define CCLK 9
#define CDAT 8
#define CRST 7

MyRealTimeClock mrtc(CCLK,CDAT,CRST);


void setRTC(String dateTime){
    String formattedDate = dateTime;  
    formattedDate = formattedDate.substring(0,11) + "20" + formattedDate.substring(11);
    Serial.println(formattedDate);

    mrtc.setDS1302Time((int)formattedDate.substring(6,8).toInt(),(int)formattedDate.substring(3,5).toInt(),(int)formattedDate.substring(0,2).toInt(),1,(int)formattedDate.substring(17).toInt(),(int)formattedDate.substring(14,17).toInt(),(int)formattedDate.substring(9,13).toInt());
}

String getDate(){
    String date = "";
    mrtc.updateTime();
    date = getYear() + "/" + getMonth() + "/" + getDay();
    
    return date;
}

String getTime(){
    String tim = "";
    mrtc.updateTime();
    tim = getHour() + ":" + getMinute() + ":" + getSecond();

    return tim;
}

String getDay(){
    mrtc.updateTime();
    if(mrtc.dayofmonth < 10)
    {
      return "0" + (String)mrtc.dayofmonth;
    }
    return (String)mrtc.dayofmonth;
}

String getMonth(){
    mrtc.updateTime();
    if(mrtc.month < 10)
    {
      return "0" + (String)mrtc.month;
    }
    return (String)mrtc.month;
}

String getYear(){
    mrtc.updateTime();
    return (String)mrtc.year;
}

String getHour(){
    mrtc.updateTime();
    if(mrtc.hours < 10)
    {
      return "0" + (String)mrtc.hours;
    }
    return (String)mrtc.hours;
}

String getMinute(){
    mrtc.updateTime();
    if(mrtc.minutes < 10)
    {
      return "0" + (String)mrtc.minutes;
    }
    return (String)mrtc.minutes;
}

String getSecond(){
    mrtc.updateTime();
    if(mrtc.seconds < 10)
    {
      return "0" + (String)mrtc.seconds;
    }
    return (String)mrtc.seconds;
}
