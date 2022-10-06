#include <SPI.h>
#include <SD.h>
#include <Wire.h>
#include <ArduinoJson.h>
#include "DFRobot_SHT20.h"
#include <MPU6050_tockn.h>
#include <MyRealTimeClock.h>

#define DEFAULT_ARRAY_SIZE 12
#define SD_PIN 11
#define SIM_PWR 12
#define CCLK 9
#define CDAT 8
#define CRST 7

MyRealTimeClock mrtc(CCLK,CDAT,CRST); 
File myFile;
DFRobot_SHT20 sht20;
MPU6050 mpu6050(Wire);


StaticJsonDocument<2300> doc;
JsonObject object = doc.to<JsonObject>();

String returnedHttpResponse = "";

long timer = 0;

String SERIAL_NUMBER = "10512515";

String GPS_date = "";
String GPS_time = "";
String datee = "";
String timee = "";
float temp = 0;
float hum = 0;
float gyrox = 0;
float gyroy = 0;
float gyroz = 0;
float accx = 0;
float accy = 0;
float accz = 0;
float latt = 0;
float longg = 0;
byte gnssnum = 0;
byte glonassnum = 0;

byte counter = 0;
byte tempr = 0;

byte array_size = DEFAULT_ARRAY_SIZE;
String user = "1";
int dtime = 500;

void setRTC(){
  Serial.println(F("Time is setting up.."));
  String formattedDate;
  String dayStamp;
  String timeStamp;

  String day = "";
  String month = "";
  String year = "";

  String hour = "";
  String minute = "";
  String second = "";

  formattedDate = getGSMDate();


    year.concat("2");
    year.concat("0");
    year.concat(formattedDate[9]);
    year.concat(formattedDate[10]);
    month.concat(formattedDate[12]);
    month.concat(formattedDate[13]);
    day.concat(formattedDate[15]);
    day.concat(formattedDate[16]);
    hour.concat(formattedDate[0]);
    hour.concat(formattedDate[1]);
    minute.concat(formattedDate[3]);
    minute.concat(formattedDate[4]);
    second.concat(formattedDate[6]);
    second.concat(formattedDate[7]);

    mrtc.setDS1302Time((int)second.toInt(),(int)minute.toInt(),(int)hour.toInt(),1,(int)day.toInt(),(int)month.toInt(),(int)year.toInt());
}

String getDate(){
    String date = "";
    mrtc.updateTime();
    date = (String)mrtc.year; + "/" + getMonth() + "/" + (String)mrtc.dayofmonth;
    
    return date;
}
String getTime(){
    String tim = "";
    mrtc.updateTime();
    tim = getHour() + ":" + getMinute() + ":" + getSecond();
    return tim;
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

String getMonth(){
    mrtc.updateTime();
    if(mrtc.month < 10)
    {
      return "0" + (String)mrtc.month;
    }
    return (String)mrtc.month;
}

String getSplittedValue(String data, char separator, int index)
{
  int found = 0;
  int strIndex[] = {0, -1};
  int maxIndex = data.length() - 1;

  for (int i = 0; i <= maxIndex && found <= index; i++) {
    if (data.charAt(i) == separator || i == maxIndex) {
      found++;
      strIndex[0] = strIndex[1] + 1;
      strIndex[1] = (i == maxIndex) ? i + 1 : i;
    }
  }

  return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}

void initParts()
{
  Serial.print(F("Initializing SD card..."));
  if (!SD.begin(SD_PIN)) {
    Serial.println(F("initialization failed!"));
    while (1);
  }
  Serial.println(F("initialization done."));
  sht20.initSHT20();
  sht20.checkSHT20();
  Wire.begin();
  mpu6050.begin();
  mpu6050.calcGyroOffsets(true);
}

void update()
{
  hum = sht20.readHumidity();
  temp = sht20.readTemperature();
  Serial.print(F("Moisture Updated: "));
  Serial.print(F("Temperature: "));
  Serial.print(temp);
  Serial.print(F(" Humidity: "));
  Serial.print(hum);
  Serial.println();

  mpu6050.update();

  if(millis() - timer > 1000){
    
    
    Serial.print(F("accX : "));Serial.print(mpu6050.getAccX()); accx = mpu6050.getAccX();
    Serial.print(F("\taccY : "));Serial.print(mpu6050.getAccY());	accy = mpu6050.getAccY();
    Serial.print(F("\taccZ : "));Serial.println(mpu6050.getAccZ()); accz = mpu6050.getAccZ();
  
    Serial.print(F("gyroX : "));Serial.print(mpu6050.getGyroX()); gyrox = mpu6050.getGyroX();
    Serial.print(F("\tgyroY : "));Serial.print(mpu6050.getGyroY());	gyroy = mpu6050.getGyroY();
    Serial.print(F("\tgyroZ : "));Serial.println(mpu6050.getGyroZ()); gyroz = mpu6050.getGyroZ();

    timer = millis();


    
  }

  Serial.flush();
  Serial3.flush();
  Serial3.flush();

    datee = getDate();
    timee = getTime();

}

String getGSMDate()
{
    Serial3.println("AT+CCLK?"); //500
  delay(50);
  Serial.flush();
  Serial3.flush();
  //delay(500);
  while (Serial3.available())
  {
    Serial3.flush();
    Serial.flush();
    String tempAns = Serial3.readString();
    //Serial.println(tempAns);
    GPS_date = getSplittedValue(tempAns, ',', 0);
    GPS_date = GPS_date.substring(GPS_date.length() - 8, GPS_date.length());
    GPS_time = getSplittedValue(tempAns, ',', 1);
    GPS_time = GPS_time.substring(0, 8);

    //Serial.print(F("Date time updated: "));
    //Serial.print(GPS_time);
    //Serial.print(F(","));
    //Serial.print(GPS_date);
    //Serial.println();
  }
  String a = GPS_time;
  a+= ",";
  a+=GPS_date;

  return a;
}
void writeSD()
{
  int delaytime = 30;
  myFile = SD.open("TEST.TXT", FILE_WRITE);

  myFile.print(temp);
  delay(delaytime);
  myFile.print(",");
  delay(delaytime);
  myFile.print(hum);
  delay(delaytime);
  myFile.print(",");
  delay(delaytime);
  myFile.print(gyrox);
  delay(delaytime);
  myFile.print(",");
  delay(delaytime);
  myFile.print(gyroy);
  delay(delaytime);
  myFile.print(",");
  delay(delaytime);
  myFile.print(gyroz);
  delay(delaytime);
  myFile.print(",");
  delay(delaytime);
  myFile.print(accx);
  delay(delaytime);
  myFile.print(",");
  delay(delaytime);
  myFile.print(accy);
  delay(delaytime);
  myFile.print(",");
  delay(delaytime);
  myFile.print(accz);
  delay(delaytime);
  myFile.print(",");
  delay(delaytime);
  myFile.print(latt);
  delay(delaytime);
  myFile.print(",");
  delay(delaytime);
  myFile.print(longg);
  delay(delaytime);
  myFile.print(",");
  delay(delaytime);
  myFile.print(gnssnum);
  delay(delaytime);
  myFile.print(",");
  delay(delaytime);
  myFile.print(glonassnum);
  delay(delaytime);
  myFile.print(",");
  delay(delaytime);
  myFile.print(datee);
  delay(delaytime);
  myFile.print(",");
  delay(delaytime);
  myFile.print(timee);
  delay(delaytime);
  myFile.print("\n");
  myFile.flush();
  myFile.close();

  Serial.println(F("A row printed in SD"));
}

void fixSD()
{
  Serial.println(F("File Fixing process"));
  String tempString = "";
  Serial.println(F("Getting rows..."));
  myFile = SD.open("TEST.TXT");
  String ar[20];
  int i = 0;
  int iter = 0;
  if (myFile)
  {
    while (myFile.available())
    {

      tempString = myFile.readStringUntil('\n');
      i = i + 1;
      if (i >= array_size + 1)
      {
        ar[iter] = tempString;
        iter = iter + 1;
      }
    }
  }
  myFile.close();


  Serial.println(F("Removing old file..."));
  if (SD.exists("TEST.TXT"))
  {
    SD.remove("TEST.TXT");
  }

  Serial.println(F("Making new File.."));


  myFile = SD.open("TEST.TXT", FILE_WRITE);

  for (int j = 0; j < iter ; j++)
  {

    //Serial.println(ar[j]);
    myFile.print(ar[j]);
    myFile.print("\n");
  }
  myFile.close();
  Serial.print(F("New file size: "));
  Serial.print(iter);
  Serial.println();
  Serial.flush();
  Serial3.flush();
  i = array_size;
  iter = 0;



  tempString = "";
}





int sendCommand(String cmd, String answer, int *delay_time)
{
  Serial3.println(cmd);
  Serial.flush();
  Serial3.flush();
  //delay(&delay_time);
  while (Serial3.available())
  {
    Serial3.flush();
    Serial.flush();
    String ans = Serial3.readString();

    if (ans.equals(answer))
    {
      return 1;
    }
    Serial3.flush();
    Serial.flush();
  }

  return 0;

}

void sendSingleCommand(String cmd, int *delay_time)
{
  Serial3.flush();
  Serial.flush();
  Serial3.println(cmd);
  //delay(&delay_time);
  Serial.flush();
  Serial3.flush();
}


void showMessage()
{
  Serial3.flush();
  Serial.flush();
  while (Serial3.available())
  {
    Serial.println(Serial3.readString());
  }
}

String showMessageS()
{
  String mes = "";
  Serial3.flush();
  Serial.flush();
  while (Serial3.available())
  {
    mes = Serial3.readString();
  }

  return mes;
}

void powerTrigger()
{
  //Serial.println(F("Device is enabling.."));
  digitalWrite(SIM_PWR, HIGH);
  delay(3000);
  digitalWrite(SIM_PWR, LOW);
}

void setGPRS()
{
  Serial3.flush();
  Serial.flush();
  delay(50);
  int r = sendCommand("AT+SAPBR=3,1,\"Contype\",\"GPRS\"", "OK", 150); //300
  delay(300);
  Serial3.flush();
  Serial.flush();
  r = sendCommand("AT+SAPBR=3,1,\"APN\",\"INTERNET\"", "OK", 150); //300
  delay(300);
  Serial3.flush();
  Serial.flush();
  r = sendCommand("AT+SAPBR=1,1", "OK", 150); //300
  delay(300);
  Serial3.flush();
  Serial.flush();

  Serial.println(F("GPRS Enabled"));
}

void closeGPRS()
{
  int r = sendCommand("AT+SAPBR=0,1", "OK", 10); //300
  Serial3.flush();
  Serial.flush();
}

void checkGPRS()
{
  int res = sendCommand("AT", "OK", 10); //400
  while (res == 0)
  {
    powerTrigger();
    res = sendCommand("AT", "OK", 10); //400
  }
}


int getGPS()
{
  Serial3.flush();
  Serial.flush();
  sendSingleCommand("AT+CGNSINF", dtime); //500
  delay(50);
  
  Serial3.flush();
  Serial.flush();
  //delay(500);

  while (Serial3.available())
  {
    Serial3.flush();
    //Serial.flush();
    String tempAns = Serial3.readString();
    //Serial.println(tempAns);

    if (getSplittedValue(tempAns, ',', 1).equals("1"))
    {
      String latS = getSplittedValue(tempAns, ',', 3);
      String longS = getSplittedValue(tempAns, ',', 4);
      String gnss = getSplittedValue(tempAns, ',', 14);
      String glonass = getSplittedValue(tempAns, ',', 15);

      latt = latS.toFloat();
      longg = longS.toFloat();
      gnssnum = gnss.toInt();
      glonassnum = glonass.toInt();

      Serial.print(F("GPS Updated: "));
      Serial.print(latt);
      Serial.print(",");
      Serial.print(longg);
      Serial.print(",");
      Serial.print(gnssnum);
      Serial.print(",");
      Serial.print(glonassnum);
      Serial.println();
      return 1;
    }
    else
    {
      Serial.println(F("Cannot take GPS Signal"));
      return 0;
    }
  }
}

void getConfig()
{
  Serial3.flush();
  Serial.flush();
  Serial.println(F("Getting configuration data.."));
  delay(100);
  int r = sendCommand("AT+HTTPINIT", "OK", 100); //1000
  delay(2000);
  Serial3.flush();
  Serial.flush();
  r = sendCommand("AT+HTTPPARA=\"CID\",1", "OK", 100); //1000
    delay(1000);
  Serial3.flush();
  Serial.flush();
  String configurl = "AT+HTTPPARA=\"URL\",\"http://activetrackerv1.herokuapp.com/api/config.php?serial=" + SERIAL_NUMBER + "\"";
  r = sendCommand(configurl, "OK", 1000);
    delay(1000);
  Serial3.flush();
  Serial.flush();
  sendSingleCommand("AT+HTTPACTION=0", 3000); showMessage();
    delay(3000);
  Serial3.flush();
  Serial.flush();
  sendSingleCommand("AT+HTTPREAD", 3000);
    delay(3000);
  String jsonS = showMessageS();

  user = getSplittedValue(jsonS,',',0);
  user = user.substring(user.length()-6,user.length());
  array_size = getSplittedValue(jsonS,',',2).toInt();
  dtime = getSplittedValue(jsonS,',',1).toInt();

  

  Serial.println(user);
  Serial.println(array_size);
  Serial.println(dtime);
  delay(400);
  sendSingleCommand("AT+HTTPTERM" , 3000); showMessage();

  Serial.println(F("Configuration done."));
}

void parseJSON()
{
  Serial.println(F("Parsing process has started"));
  int r = sendCommand("AT+HTTPINIT", "OK", 100); //1000
    delay(1000);
  Serial3.flush();
  Serial.flush();
  r = sendCommand("AT+HTTPPARA=\"CID\",1", "OK", 100); //1000
    delay(1000);
  Serial3.flush();
  Serial.flush();
  r = sendCommand("AT+HTTPPARA=\"URL\",\"http://activetrackerv1.herokuapp.com/api/add.php\"", "OK", 1000);
    delay(1000);
  Serial3.flush();
  Serial.flush();
  r = sendCommand("AT+HTTPPARA=\"CONTENT\",\"application/json\"", "OK", 1000);
    delay(1000);
  Serial3.flush();
  Serial.flush();

  sendSingleCommand("AT+HTTPDATA=" + String(measureJsonPretty(object)) + ",100000", 6000);
    delay(5000);
  Serial3.flush();
  Serial.flush();
  serializeJsonPretty(object, Serial);
  serializeJsonPretty(object, Serial3); showMessage();
  Serial3.flush();
  Serial.flush();
  delay(2000);
  sendSingleCommand("AT+HTTPACTION=1", 10000);
    delay(2000);
  Serial3.flush();
  Serial.flush();

  String p = showMessageS();
  Serial.println(p);
  returnedHttpResponse = "";
  returnedHttpResponse = getSplittedValue(p, ',', 1);

  sendSingleCommand("AT+HTTPREAD", 10000); showMessage();
  Serial3.flush();
  Serial.flush();
  sendSingleCommand("AT+HTTPTERM", 10000); showMessage();
  Serial3.flush();
  Serial.flush();
  Serial.println(F("Parsing Success"));

  showMessage();
  delay(300);
  showMessage();
  delay(300);
  showMessage();
  //object.clear();
  doc.clear();
  doc.garbageCollect();
  object = doc.to<JsonObject>();



}

///// burda kaldın
void makeJSON()
{
  String tempString;

  int counter = 0;
  int i = 0;

  JsonArray temperatureArray = object.createNestedArray("TEMPERATURE");
  JsonArray humidityArray = object.createNestedArray("HUMIDITY");
  JsonArray gyroxArray = object.createNestedArray("GYROX");
  JsonArray gyroyArray = object.createNestedArray("GYROY");
  JsonArray gyrozArray = object.createNestedArray("GYROZ");
  JsonArray accxArray = object.createNestedArray("ACCX");
  JsonArray accyArray = object.createNestedArray("ACCY");
  JsonArray acczArray = object.createNestedArray("ACCZ");
  JsonArray lattitudeArray = object.createNestedArray("LATTITUDE");
  JsonArray longtitudeArray = object.createNestedArray("LONGTITUDE");
  JsonArray gnssArray = object.createNestedArray("GNSS");
  JsonArray glonassArray = object.createNestedArray("GLONASS");
  JsonArray dateArray = object.createNestedArray("DATE");
  JsonArray timeArray = object.createNestedArray("TIME");

  myFile = SD.open("TEST.TXT");
  if (myFile)
  {
    while (myFile.available())
    {
      tempString = myFile.readStringUntil('\n');

      temperatureArray.add(getSplittedValue(tempString, ',', 0).toFloat());
      humidityArray.add(getSplittedValue(tempString, ',', 1).toFloat());
      gyroxArray.add(getSplittedValue(tempString, ',', 2).toFloat());
      gyroyArray.add(getSplittedValue(tempString, ',', 3).toFloat());
      gyrozArray.add(getSplittedValue(tempString, ',', 4).toFloat());
      accxArray.add(getSplittedValue(tempString, ',', 5).toFloat());
      accyArray.add(getSplittedValue(tempString, ',', 6).toFloat());
      acczArray.add(getSplittedValue(tempString, ',', 7).toFloat());
      lattitudeArray.add(getSplittedValue(tempString, ',', 8).toFloat());
      longtitudeArray.add(getSplittedValue(tempString, ',', 9).toFloat());
      gnssArray.add(getSplittedValue(tempString, ',', 10).toInt());
      glonassArray.add(getSplittedValue(tempString, ',', 11).toInt());
      dateArray.add(getSplittedValue(tempString, ',', 12));
      timeArray.add(getSplittedValue(tempString, ',', 13));
      object["USER"] = user;

      counter = counter + 1;

      if (counter == array_size)
      {
        break;
      }

    }
    myFile.close();
  }
}

void GPRSConfig()
{
  powerTrigger();
  delay(3000);
  sendSingleCommand("AT", 10);
  //delay(3000);
  showMessage();
  showMessage();
  //delay(2000);

  showMessage();
  //delay(1000);
  sendSingleCommand("AT+CLTS=1", 200);
  //delay(1000);
  showMessage();
  sendSingleCommand("AT&W", 200);
  //delay(1000);
  powerTrigger();
  delay(3000);
  powerTrigger();
  Serial.println();
  // delay(5000);
  showMessage();
  //delay(3000);
  sendSingleCommand("AT+CGNSPWR=1", 200);
  //delay(2000);
  showMessage();
  //delay(100);
  sendSingleCommand("AT+CGNSSEQ=\"GSV\"", 200);
  //delay(2000);
  //sendCommand("AT+CGNSINF", 10);
  showMessage();
  //sendCommand("AT+CGNSINF", 10);
  showMessage();
  //sendCommand("AT+CGNSINF", 10);
  showMessage();
  sendSingleCommand("AT+CGNSINF", 10);
  showMessage();
  //delay(5000);
  showMessage();
  //delay(10000);
}

void setup()
{
  Serial.begin(9600);
  Serial3.begin(9600);
  Serial.println(F("Device initializing..."));

  pinMode(SIM_PWR, OUTPUT);

  initParts();
  GPRSConfig();
  Serial.println(F("Device initialized"));
  Serial.flush();
  Serial3.flush();
  
  delay(2000);

  Serial.println(F("Ready, Running"));
  delay(1000);
  Serial.println(F("Configuring GPS"));
  int resp = getGPS();
  while (resp != 1)
  {
    resp = getGPS();
    delay(1000);
  }
  setGPRS();
  
 // getConfig();
  delay(1000);
  setRTC();
}

void loop()
{
  Serial.println();
  //setGPRS();
  Serial.println(counter);
  update();
  tempr = getGPS();
  writeSD();
  counter = counter + 1;

  if (counter == array_size)
  {
    counter = 0;
    setGPRS();
    makeJSON();
    parseJSON();
    closeGPRS();
    if (returnedHttpResponse.equals("200"))
    {
      fixSD();
      Serial3.flush();
      Serial.flush();
      sendSingleCommand("AT", 1000);
      showMessage();
      setGPRS();
      returnedHttpResponse = "";
      getConfig();
      delay(50);
      
      
    }

    Serial.println(F("Done"));
  }

}
