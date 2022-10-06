#include "RTCL.h"
#include "MPUL.h"
#include "SHTL.h"
#include "SDL.h"
#include <avr/pgmspace.h>
#include <MemoryFree.h>

#define SIM_PWR 12
#define SERIAL_NUM 10512515

const char s1[] PROGMEM = "AT";
const char s2[] PROGMEM = "AT&W";
const char s3[] PROGMEM = "AT+CLTS=1";
const char s4[] PROGMEM = "AT+CCLK?";

const char s5[] PROGMEM = "AT+SAPBR=3,1,\"Contype\",\"GPRS\"";
const char s6[] PROGMEM = "AT+SAPBR=3,1,\"APN\",\"INTERNET\"";
const char s7[] PROGMEM = "AT+SAPBR=1,1";
const char s8[] PROGMEM = "AT+SAPBR=0,1";

const char s9[] PROGMEM = "AT+CGNSPWR=1";
const char s10[] PROGMEM = "AT+CGNSSEQ=\"GSV\"";
const char s11[] PROGMEM = "AT+CGNSINF";

const char s12[] PROGMEM = "AT+HTTPINIT";
const char s13[] PROGMEM = "AT+HTTPPARA=\"CID\",1";
const char s14[] PROGMEM = "AT+HTTPPARA=\"URL\",\"http://activetrackerv1.herokuapp.com/api/config.php?serial=10512515\"";
const char s15[] PROGMEM = "AT+HTTPPARA=\"URL\",\"http://activetrackerv1.herokuapp.com/api/add.php\"";
const char s16[] PROGMEM = "AT+HTTPPARA=\"CONTENT\",\"application/json\"";
const char s17[] PROGMEM = "AT+HTTPDATA=\"%d\",100000";
const char s18[] PROGMEM = "AT+HTTPACTION=1";
const char s19[] PROGMEM = "AT+HTTPACTION=0";
const char s20[] PROGMEM = "AT+HTTPREAD";
const char s21[] PROGMEM = "AT+HTTPTERM";

const char *const conf[] PROGMEM = {s1,s2,s3,s4};
const char *const gprs[] PROGMEM = {s5,s6,s7,s8};
const char *const gps[] PROGMEM = {s9,s10,s11};
const char *const http[] PROGMEM = {s12,s13,s14,s15,s16,s17,s18,s19,s20,s21};

char progmem_buffer[100];
float GPS[] = {0.0,0.0,0.0,0.0};
int flag = 0;
int arr_size = 8;
bool arrived = false;
int counter = 0;
String userr= "";
int dtime = 0;
int resp = 0;
float sh[] = {0.0,0.0};
float mpu[] = {0.0,0.0,0.0,0.0,0.0,0.0};
String tm = "";
String dt = "";

int GSM_SEND_RECEIVE_COMMANDd(String command, int delay_time, String answer = "")
{
  Serial3.flush();
  Serial3.println(command);
  Serial3.flush();

  String response;
  delay(delay_time);

  while(Serial3.available())
  {
    response = Serial3.readString();
    if(response.equals(answer) || answer.equals(""))
    {
      return 1;
    }
    else if(response.equals("ERROR"))
    {
      Serial.println(F("Got ERROR on transmission."));
      return 0;
    }
    else
    {
      Serial.println(F("Error on communication."));
      return 0;
    }
  }
}
int GSM_SEND_RECEIVE_COMMAND(String command, int delay_time, String answer = "")
{
  Serial3.flush();

  Serial3.println(command);
  Serial3.flush();
  String response;
  long timer = millis();
  while(timer < delay_time);
  while(Serial3.available())
  {
    response = Serial3.readString();
    if(response.equals(answer) || answer.equals(""))
    {
      return 1;
    }
    else if(response.equals("ERROR"))
    {
      Serial.println(F("Got ERROR on transmission."));
      return 0;
    }
    else
    {
      Serial.println(F("Error on communication."));
      return 0;
    }
  }
  timer = 0;
}
String GSM_SHOW_ANSWERs(int dela)
{ String temp= "";
  delay(dela);
  Serial3.flush();
  while(Serial3.available())
  {
    temp = Serial3.readString();
    Serial3.flush();
  }
  Serial3.flush();
  return temp;
}
void GSM_SHOW_ANSWER(int dela)
{
  delay(dela);

  Serial3.flush();
  while(Serial3.available())
  {
    Serial.println(Serial3.readString());
    Serial3.flush();
  }
  Serial3.flush();
}
void GSM_FIX_SERIAL(int times, int d)
{
  for(int i=0; i<times; i++)
  {
    GSM_SHOW_ANSWER(d);
  }
}
void GSM_POWER()
{
  digitalWrite(SIM_PWR, HIGH);
  delay(3000);
  digitalWrite(SIM_PWR,LOW);
  delay(3000);
  Serial.println(GSM_SEND_RECEIVE_COMMAND("AT",100,"OK"));
  Serial3.flush();
  Serial.flush();
  GSM_FIX_SERIAL(10,500);
  Serial.println(GSM_SEND_RECEIVE_COMMAND("AT",100,"OK"));
  GSM_FIX_SERIAL(10,500);
  Serial.flush();
  Serial3.flush();
  Serial.println(F("GSM Powered."));
  //Serial.println(GSM_SEND_RECEIVE_COMMAND("AT+IPR=57600", 400, "OK"));
  //GSM_FIX_SERIAL(10,500);
  //Serial.println(GSM_SEND_RECEIVE_COMMAND("AT&W_SAVE", 400, "OK"));


}
void GSM_ENABLE_GPRS()
{
  flag = GSM_SEND_RECEIVE_COMMAND(String(strcpy_P(progmem_buffer, (char *)pgm_read_word(&(gprs[0])))),50,"OK");
  flag = GSM_SEND_RECEIVE_COMMAND(String(strcpy_P(progmem_buffer, (char *)pgm_read_word(&(gprs[1])))),50,"OK");
  flag = GSM_SEND_RECEIVE_COMMAND(String(strcpy_P(progmem_buffer, (char *)pgm_read_word(&(gprs[2])))),50,"OK");
  
  Serial.println(F("GPRS Enabled."));
}
void GSM_ENABLE_GPS()
{
  flag = GSM_SEND_RECEIVE_COMMAND(String(strcpy_P(progmem_buffer, (char *)pgm_read_word(&(gps[0])))),1500,"OK");
  flag = GSM_SEND_RECEIVE_COMMAND(String(strcpy_P(progmem_buffer, (char *)pgm_read_word(&(gps[1])))),1500,"OK");
  flag = GSM_SEND_RECEIVE_COMMAND(String(strcpy_P(progmem_buffer, (char *)pgm_read_word(&(gps[2])))),1500);
  Serial.println(F("GPS Enabled."));
}
void GSM_CONFIGURATION()
{
  flag = GSM_SEND_RECEIVE_COMMAND(String(strcpy_P(progmem_buffer, (char *)pgm_read_word(&(conf[0])))),150,"OK");
  flag = GSM_SEND_RECEIVE_COMMAND(String(strcpy_P(progmem_buffer, (char *)pgm_read_word(&(conf[2])))),150,"OK");
  Serial.println(F("Configuration Done."));
}

String GSM_GET_DATE()
{
  Serial3.println("AT+CCLK?");
  Serial3.flush();
  String gdate = "";
  String gtime = "";
  delay(500);
  int i1 = 0;
  String a = "";
  while (Serial3.available())
  {
    Serial3.flush();
    String tempAns = Serial3.readString();
    i1 = tempAns.indexOf('"');

    gdate = tempAns.substring(i1+1,i1+9);
    gtime = tempAns.substring(i1+10, i1+18);
  }
  a = gtime + "," + gdate;
  return a;
}
void GSM_GET_DATEe(int dl)
{
  /*
   * flag = GSM_SEND_RECEIVE_COMMAND("AT+CCLK?,200,"OK");
   * String tmp = showMessageS(dl);
   */
  Serial3.println("AT+CCLK?");
  Serial.flush();
  Serial3.flush();
  String gdate = "";
  String gtime = "";
  delay(dl);
  int i1 = 0;
  String a = "";
  while (Serial3.available())
  {
    Serial3.flush();
    String tempAns = Serial3.readString();
    i1 = tempAns.indexOf('"');

    gdate = tempAns.substring(i1+1,i1+9);
    gtime = tempAns.substring(i1+10, i1+18);
  }
  tm = gtime;
  dt = gdate;
}
int GSM_GET_GPS(int d,int cond)
{
  String temp = "";
  static float gps_arr[4];
  Serial3.flush();
  strcpy_P(progmem_buffer, (char *)pgm_read_word(&(gps[2])));
  Serial3.println(progmem_buffer);
  //long timer = millis();
  //while(timer < d);
  delay(d);
  while(Serial3.available())
  {
    Serial3.flush();
    String tempAns = Serial3.readString();
    temp = tempAns.substring(25,26);
    if (temp.equals("1"))
    {
      String latS = tempAns.substring(46,55);
      String longS = tempAns.substring(56,65);
      String gnss = tempAns.substring(tempAns.length()-18,tempAns.length()-17);
      String glonass = tempAns.substring(tempAns.length()-16,tempAns.length()-15);
      if(!String(tempAns[tempAns.length()-17]).equals(",")) glonass = String(tempAns[tempAns.length()-17]) + String(tempAns[tempAns.length()-16]);
      if(!String(tempAns[tempAns.length()-19]).equals(",")) gnss = String(tempAns[tempAns.length()-19]) + String(tempAns[tempAns.length()-18]);

      GPS[0] = latS.toFloat();
      GPS[1] = longS.toFloat();
      GPS[2] = gnss.toInt();
      GPS[3] = glonass.toInt();
      if(cond==1)
      {
        Serial.print(F("GPS Updated: "));
        Serial.print(GPS[0]);
        Serial.print(",");
        Serial.print(GPS[1]);
        Serial.print(",");
        Serial.print(GPS[2]);
        Serial.print(",");
        Serial.print(GPS[3]);
        Serial.println();
      }

      return 1;
    }
    else
    {
      Serial.println(F("Cannot take GPS Signal"));
      return 0;
    }
  }

}

void GSM_GET_CONFIGURATION()
{
  
  flag = GSM_SEND_RECEIVE_COMMAND(String(strcpy_P(progmem_buffer, (char *)pgm_read_word(&(http[0])))),200,"OK");
  GSM_FIX_SERIAL(2,100);
  flag = GSM_SEND_RECEIVE_COMMAND(String(strcpy_P(progmem_buffer, (char *)pgm_read_word(&(http[1])))),200,"OK");
  GSM_FIX_SERIAL(2,100);
  flag = GSM_SEND_RECEIVE_COMMAND(String(strcpy_P(progmem_buffer, (char *)pgm_read_word(&(http[2])))),200,"OK");
  GSM_FIX_SERIAL(2,100);
  flag = GSM_SEND_RECEIVE_COMMAND(String(strcpy_P(progmem_buffer, (char *)pgm_read_word(&(http[7])))),800,"OK");
  GSM_FIX_SERIAL(4,100);
  flag = GSM_SEND_RECEIVE_COMMAND(String(strcpy_P(progmem_buffer, (char *)pgm_read_word(&(http[8])))),2000,"OK");
 // GSM_FIX_SERIAL(4,100);
  String tmp = GSM_SHOW_ANSWERs(350);
  flag = GSM_SEND_RECEIVE_COMMAND(String(strcpy_P(progmem_buffer, (char *)pgm_read_word(&(http[9])))),900,"OK");
  GSM_FIX_SERIAL(4,100);

  String tmp2 = getSplittedValuee(tmp,',',0);
  tmp2 = tmp2.substring(tmp2.length()-6,tmp2.length());
  arr_size = getSplittedValuee(tmp,',',1).toInt();
  dtime = getSplittedValuee(tmp,',',2).toInt();
  SD_setUser(tmp2);

}

void GSM_GET_CONF()
{
  String tempAns = "";
  Serial.flush();
  Serial3.flush();
  Serial3.println(strcpy_P(progmem_buffer, (char *)pgm_read_word(&(http[0]))));
  delay(150);
  while(Serial3.available())
  {
    Serial3.flush();
    tempAns = Serial3.readString();
    Serial.println(tempAns);
  }
  
  Serial3.println(strcpy_P(progmem_buffer, (char *)pgm_read_word(&(http[1]))));
  delay(150);
  while(Serial3.available())
  {
    Serial3.flush();
    tempAns = Serial3.readString();
    Serial.println(tempAns);
  }
  Serial3.println(strcpy_P(progmem_buffer, (char *)pgm_read_word(&(http[2]))));
  delay(150);
  while(Serial3.available())
  {
    Serial3.flush();
    tempAns = Serial3.readString();
    Serial.println(tempAns);
  }
  Serial3.println(strcpy_P(progmem_buffer, (char *)pgm_read_word(&(http[7]))));
  delay(4000);
  while(Serial3.available())
  {
    Serial3.flush();
    tempAns = Serial3.readString();
    Serial.println(tempAns);
  }

  Serial3.println(strcpy_P(progmem_buffer, (char *)pgm_read_word(&(http[8]))));
  delay(4000);
  while(Serial3.available())
  {
    Serial3.flush();
    tempAns = Serial3.readString();
    Serial.println(tempAns);
  }
  String tmp2 = getSplittedValuee(tempAns,',',0);
  tmp2 = tmp2.substring(tmp2.length()-6,tmp2.length());
  arr_size = getSplittedValuee(tempAns,',',1).toInt();
  SD_setUser(tmp2);
  Serial3.println(strcpy_P(progmem_buffer, (char *)pgm_read_word(&(http[9]))));
  delay(450);
  while(Serial3.available())
  {
    Serial3.flush();
    tempAns = Serial3.readString();
    Serial.println(tempAns);
  }
  
}
void GSM_SEND_DATA()
{
  flag = GSM_SEND_RECEIVE_COMMAND(String(strcpy_P(progmem_buffer, (char *)pgm_read_word(&(http[0])))),100,"OK");
  GSM_FIX_SERIAL(2,100);
  
  flag = GSM_SEND_RECEIVE_COMMAND(String(strcpy_P(progmem_buffer, (char *)pgm_read_word(&(http[1])))),100,"OK");
  GSM_FIX_SERIAL(2,100);
  
  flag = GSM_SEND_RECEIVE_COMMAND(String(strcpy_P(progmem_buffer, (char *)pgm_read_word(&(http[3])))),100,"OK");
  GSM_FIX_SERIAL(2,100);
  
  flag = GSM_SEND_RECEIVE_COMMAND(String(strcpy_P(progmem_buffer, (char *)pgm_read_word(&(http[4])))),100,"OK");
  GSM_FIX_SERIAL(2,100);
  
  String tmp = String(strcpy_P(progmem_buffer, (char *)pgm_read_word(&(http[5]))));
  tmp = tmp.substring(0,13) + String(SD_getJsonSize()) + tmp.substring(15);
  flag = GSM_SEND_RECEIVE_COMMAND(String(tmp),150,"OK");
  GSM_FIX_SERIAL(2,100);
 
  SD_serializeJSON(3);
  GSM_FIX_SERIAL(2,100);
  
  flag = GSM_SEND_RECEIVE_COMMAND(String(strcpy_P(progmem_buffer, (char *)pgm_read_word(&(http[6])))),1500,"OK");
  GSM_FIX_SERIAL(2,100);
  
  flag = GSM_SEND_RECEIVE_COMMAND(String(strcpy_P(progmem_buffer, (char *)pgm_read_word(&(http[8])))),4000,"OK");
  String tempBuffer = GSM_SHOW_ANSWERs(2050);
  //Serial.println(F("TempBuffer"));
  //Serial.println(tempBuffer);
  Serial.println(getSplittedValuee(tempBuffer,',',1));
  
  flag = GSM_SEND_RECEIVE_COMMAND(String(strcpy_P(progmem_buffer, (char *)pgm_read_word(&(http[9])))),4000,"OK");
  String tempBuffer2 = GSM_SHOW_ANSWERs(2050);
  String tempBuffer3 = GSM_SHOW_ANSWERs(2050);

  Serial.println(F("Parsing Done"));
  Serial.println(F("Free Memory = "));
  Serial.print(freeMemory());
  Serial.println(F(""));
  
  SD_freeJSON();
  
  if(getSplittedValuee(tempBuffer,',',1).equals("200") || getSplittedValuee(tempBuffer2,',',1).equals("200") || getSplittedValuee(tempBuffer3,',',1).equals("200")){ SD_fixSD(arr_size);}

}

void GSM_SEND_DATA2()
{
  String tempAns = "";
  Serial.flush();
  Serial3.flush();
  Serial3.println(strcpy_P(progmem_buffer, (char *)pgm_read_word(&(http[0]))));
  delay(150);
  while(Serial3.available())
  {
    Serial3.flush();
    tempAns = Serial3.readString();
    Serial.println(tempAns);
  }
  
  Serial3.println(strcpy_P(progmem_buffer, (char *)pgm_read_word(&(http[1]))));
  delay(150);
  while(Serial3.available())
  {
    Serial3.flush();
    tempAns = Serial3.readString();
    Serial.println(tempAns);
  }
  Serial3.println(strcpy_P(progmem_buffer, (char *)pgm_read_word(&(http[3]))));
  delay(150);
  while(Serial3.available())
  {
    Serial3.flush();
    tempAns = Serial3.readString();
    Serial.println(tempAns);
  }
  Serial3.println(strcpy_P(progmem_buffer, (char *)pgm_read_word(&(http[4]))));
  delay(150);
  while(Serial3.available())
  {
    Serial3.flush();
    tempAns = Serial3.readString();
    Serial.println(tempAns);
  }
  String tmp = String(strcpy_P(progmem_buffer, (char *)pgm_read_word(&(http[5]))));
  tmp = tmp.substring(0,13) + String(SD_getJsonSize()) + tmp.substring(15);
  Serial3.println(tmp);
  delay(150);
  while(Serial3.available())
  {
    Serial3.flush();
    tempAns = Serial3.readString();
    Serial.println(tempAns);
  }
  delay(100);
  SD_serializeJSON(3);
  delay(100);
  SD_serializeJSON(0);
  delay(190);
  Serial.println(freeMemory());
  Serial3.println(strcpy_P(progmem_buffer, (char *)pgm_read_word(&(http[6]))));
  delay(1500);
  while(Serial3.available())
  {
    Serial3.flush();
    tempAns = Serial3.readString();
    Serial.println(tempAns);
    Serial.println(getSplittedValuee(tempAns,',',1));
    
  }
  Serial3.println(strcpy_P(progmem_buffer, (char *)pgm_read_word(&(http[8]))));
  delay(1000);
  while(Serial3.available())
  {
    Serial3.flush();
    tempAns = Serial3.readString();
    Serial.println(tempAns);
    Serial.println(getSplittedValuee(tempAns,',',1));
    Serial.println("a");
  }
  Serial3.println(strcpy_P(progmem_buffer, (char *)pgm_read_word(&(http[9]))));
  delay(150);
  while(Serial3.available())
  {
    Serial3.flush();
    tempAns = Serial3.readString();
    Serial.println(tempAns);
  }
  
  SD_freeJSON();
  Serial.println(getSplittedValuee(tempAns,',',1));
  if(getSplittedValuee(tempAns,',',1).equals("200")) SD_fixSD(arr_size);
}


void updateSensors()
{
  resp = GSM_GET_GPS(90,0);
  updateSHT();
  sh[0] = getTemp();
  sh[1] = getHum();
  updateIMU();
  mpu[0] = getGx();
  mpu[1] = getGy();
  mpu[2] = getGz();
  mpu[3] = getAx();
  mpu[4] = getAy();
  mpu[5] = getAz();
  tm = getTime();
  dt = getDate();
  //GSM_GET_DATEe(30);
} 

void show_LOG()
{
  Serial.print(dt);
  Serial.print(F(","));
  Serial.print(tm);
  Serial.println(F(""));
  
  Serial.print(sh[0]);
  Serial.print(F(","));
  Serial.print(sh[1]);
  Serial.println(F(""));
  
  Serial.print(mpu[0]);
  Serial.print(F(","));
  Serial.print(mpu[1]);
  Serial.print(F(","));
  Serial.print(mpu[2]);
  Serial.print(F(","));
  Serial.print(mpu[3]);
  Serial.print(F(","));
  Serial.print(mpu[4]);
  Serial.print(F(","));
  Serial.print(mpu[5]);
  Serial.println(F(""));  

  Serial.print(GPS[0]);
  Serial.print(F(","));
  Serial.print(GPS[1]);
  Serial.print(F(","));
  Serial.print(GPS[2]);
  Serial.print(F(","));
  Serial.print(GPS[3]);
  Serial.println(F(""));  

}

void setup()
{
  pinMode(SIM_PWR,OUTPUT);
  Serial.begin(57600);
  Serial3.begin(57600);

  GSM_POWER();
  GSM_FIX_SERIAL(5,500);
  GSM_ENABLE_GPRS();
  GSM_FIX_SERIAL(5,500);
  GSM_ENABLE_GPS();
  GSM_FIX_SERIAL(5,500);
  GSM_FIX_SERIAL(3,300);
  GSM_FIX_SERIAL(3,300);
  setRTC(GSM_GET_DATE());
  delay(100);
  initMPU();
  delay(100);
  initSHT();
  delay(100);
  SD_initSDL();
  delay(100);
  Serial.println(F("Removing old file..."));
  removeOld();

  GSM_GET_CONF();

  resp = GSM_GET_GPS(220,0);
  while(resp != 1)
  {
    resp = GSM_GET_GPS(220,1);
    if(resp == 1) break;
  }

  resp = GSM_GET_GPS(220,1);
  GSM_FIX_SERIAL(3,300);

  resp = GSM_GET_GPS(220,1);
  GSM_FIX_SERIAL(3,300);
}
void loop()
{
  
  updateSensors();
  show_LOG();
  SD_printSD(sh[0],sh[1],mpu[0],mpu[1],mpu[2],mpu[3],mpu[4],mpu[5],GPS[0],GPS[1],GPS[2],GPS[3],dt,tm);
  counter ++;
  if(counter == arr_size)
  {
    SD_makeJSON(arr_size);
    GSM_SEND_DATA();
    counter = 0;
  }
}
