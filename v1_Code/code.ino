#include <SPI.h>
#include <SD.h>
#include <Wire.h>
#include <ArduinoJson.h>
#include "DFRobot_SHT20.h"
#include <MPU6050_tockn.h>

#define DEFAULT_ARRAY_SIZE 12

File myFile;
#define SD_PIN 11
#define SIM_PWR 12


StaticJsonDocument<2100> doc;
JsonObject object = doc.to<JsonObject>();
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
String returnedHttpResponse = "";

DFRobot_SHT20 sht20;
MPU6050 mpu6050(Wire);

long timer = 0;

String SERIAL_NUMBER = "10512515";

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



String getSplittedValue(String data, char *separator, int *index)
{
  int found = 0;
  int strIndex[] = {0, -1};
  int maxIndex = data.length() - 1;

  for (int i = 0; i <= maxIndex && found <= &index; i++) {
    if (data.charAt(i) == &separator || i == maxIndex) {
      found++;
      strIndex[0] = strIndex[1] + 1;
      strIndex[1] = (i == maxIndex) ? i + 1 : i;
    }
  }

  return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}

void initParts()
{

  if (!SD.begin(SD_PIN)) {

    while (1);
  }
  Serial.println("SD");

  sht20.initSHT20();
  sht20.checkSHT20();
  Wire.begin();
  mpu6050.begin();
  mpu6050.calcGyroOffsets(true);
}


void updatePeripherals()
{
  hum = sht20.readHumidity();
  temp = sht20.readTemperature();


  mpu6050.update();

  if(millis() - timer > 1000){
    
    
    accx = mpu6050.getAccX();
    accy = mpu6050.getAccY();
    accz = mpu6050.getAccZ();
  
    gyrox = mpu6050.getGyroX();
    gyroy = mpu6050.getGyroY();
    gyroz = mpu6050.getGyroZ();

    timer = millis();
    
  }


  sendSingleCommand("AT+CCLK?", dtime); //500

  //delay(500);
  while (Serial3.available())
  {
    String tempAns = Serial3.readString();

    datee = getSplittedValue(tempAns, ',', 0);
    datee = datee.substring(datee.length() - 8, datee.length());
    timee = getSplittedValue(tempAns, ',', 1);
    timee = timee.substring(0, 8);
    Serial.println(timee);

  }
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

  myFile.close();

}

void fixSD()
{
  String tempString = "";
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


  if (SD.exists("TEST.TXT"))
  {
    SD.remove("TEST.TXT");
  }


  myFile = SD.open("TEST.TXT", FILE_WRITE);

  for (int j = 0; j < iter ; j++)
  {

    //Serial.println(ar[j]);
    myFile.print(ar[j]);
    myFile.print("\n");
  }
  myFile.close();

  i = array_size;
  iter = 0;



  tempString = "";
}





int sendCommand(String cmd, String answer, int *delay_time)
{
  Serial3.println(cmd);

  delay(&delay_time);
  while (Serial3.available())
  {
    String ans = Serial3.readString();

    if (ans.equals(answer))
    {
      return 1;
    }
  }

  return 0;

}

void sendSingleCommand(String cmd, int *delay_time)
{
  Serial3.println(cmd);
  delay(&delay_time);
}


void showMessage()
{

  while (Serial3.available())
  {
        Serial.println(Serial3.readString());
  }
}

String showMessageS()
{
  String mes = "";

  while (Serial3.available())
  {
    mes = Serial3.readString();
  }

  return mes;
}

void powerTrigger()
{
  digitalWrite(SIM_PWR, HIGH);
  delay(3000);
  digitalWrite(SIM_PWR, LOW);
}

void setGPRS()
{
  int r = sendCommand("AT+SAPBR=3,1,\"Contype\",\"GPRS\"", "OK", 150); //300
  Serial3.flush();
  Serial.flush();
  r = sendCommand("AT+SAPBR=3,1,\"APN\",\"INTERNET\"", "OK", 150); //300
  r = sendCommand("AT+SAPBR=1,1", "OK", 150); //300

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

  sendSingleCommand("AT+CGNSINF", dtime); //500

  //delay(500);

  while (Serial3.available())
  {
    String tempAns = Serial3.readString();
    // Serial.println(tempAns);

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
      Serial.println(1);
      return 1;
    }
    else
    {
      return 0;
    }
  }
}



void getConfig()
{
  int r = sendCommand("AT+HTTPINIT", "OK", 100); //1000

  r = sendCommand("AT+HTTPPARA=\"CID\",1", "OK", 100); //1000

  String configurl = "AT+HTTPPARA=\"URL\",\"http://activetrackerv1.herokuapp.com/api/config.php?serial=" + SERIAL_NUMBER + "\"";
  r = sendCommand(configurl, "OK", 1000);

  sendSingleCommand("AT+HTTPACTION=0", 3000); showMessage();

  sendSingleCommand("AT+HTTPREAD", 3000);
  String jsonS = showMessageS();

  user = getSplittedValue(jsonS,',',0);
  user = user.substring(user.length()-6,user.length());
  array_size = getSplittedValue(jsonS,',',1).toInt();
  dtime = getSplittedValue(jsonS,',',2).toInt();

  


  delay(400);
  sendSingleCommand("AT+HTTPTERM" , 3000); showMessage();


}

void parseJSON()
{

  int r = sendCommand("AT+HTTPINIT", "OK", 100); //1000

  r = sendCommand("AT+HTTPPARA=\"CID\",1", "OK", 100); //1000

  r = sendCommand("AT+HTTPPARA=\"URL\",\"http://activetrackerv1.herokuapp.com/api/add.php\"", "OK", 1000);

  r = sendCommand("AT+HTTPPARA=\"CONTENT\",\"application/json\"", "OK", 1000);


  sendSingleCommand("AT+HTTPDATA=" + String(measureJsonPretty(object)) + ",100000", 6000);


  serializeJsonPretty(object, Serial3); 

  delay(4000);
  sendSingleCommand("AT+HTTPACTION=1", 10000);


  String p = showMessageS();
  Serial.println(p);
  returnedHttpResponse = "";
  returnedHttpResponse = getSplittedValue(p, ',', 1);

  sendSingleCommand("AT+HTTPREAD", 10000); showMessage();

  sendSingleCommand("AT+HTTPTERM", 10000); showMessage();



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
  delay(2000);

  pinMode(SIM_PWR, OUTPUT);

  initParts();
  GPRSConfig();

 // delay(30000);


  delay(1000);

  int resp = getGPS();
  while (resp != 1)
  {
    resp = getGPS();
    delay(1000);
  }
  setGPRS();
  delay(1000);
  getConfig();
}

void loop()
{
  //setGPRS();

  updatePeripherals();
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

    
  }

}
