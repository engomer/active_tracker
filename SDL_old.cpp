#include <SPI.h>
#include <SD.h>
#include <ArduinoJson.h>
#include "SDL.h"

#define SDCS 11

String folderName = "TEST.TXT";
String user = "1";
File folder;

StaticJsonDocument<3100> doc;
JsonObject object = doc.to<JsonObject>();






String getSplittedValuee(String data, char separator, int index)
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

void SD_setUser(String usr)
{
    user = usr;
}

String SD_getUser()
{
    return user;
}

size_t getJsonObjectSize()
{
    return object.size();
}
/*
void showJsonArraySize()
{
    Serial.print(F("Temp = "));
    Serial.print(temperatureArray);
    Serial.println(F(""));
    Serial.print(F("Hum = "));
    Serial.print(humidityArray);
    Serial.println(F(""));
    Serial.print(F("Gyrox = "));
    Serial.print(gyroxArray);
    Serial.println(F(""));
    Serial.print(F("Gyroy = "));
    Serial.print(gyroyArray);
    Serial.println(F(""));
    Serial.print(F("Gyroz = "));
    Serial.print(gyrozArray);
    Serial.println(F(""));
    Serial.print(F("Accx = "));
    Serial.print(accxArray);
    Serial.println(F(""));
    Serial.print(F("Accy = "));
    Serial.print(accyArray);
    Serial.println(F(""));
    Serial.print(F("Accz = "));
    Serial.print(acczArray);
    Serial.println(F(""));
    Serial.print(F("Lattitude = "));
    Serial.print(lattitudeArray);
    Serial.println(F(""));
    Serial.print(F("Longtitude = "));
    Serial.print(longtitudeArray);
    Serial.println(F(""));
    Serial.print(F("GNSS = "));
    Serial.print(gnssArray);
    Serial.println(F(""));
    Serial.print(F("GLONASS = "));
    Serial.print(glonassArray);
    Serial.println(F(""));
    Serial.print(F("Date = "));
    Serial.print(dateArray);
    Serial.println(F(""));
    Serial.print(F("Time = "));
    Serial.print(timeArray);
    Serial.println(F(""));

}*/
void SD_initSDL()
{
    Serial.println(F("Initializing the SD card."));
    
    if(!SD.begin(SDCS))
    {
        Serial.print(F("Initialization Failed."));
        while(1);    
    }
    Serial.println(F("Initialization done."));
}


void SD_fixSD(int array_size)
{
    String tempString = "";
    folder = SD.open("TEST.TXT");
    int i = 0;
    int iter = 0;
    String ar[150];
    if (folder)
    {
        while (folder.available())
        {

            tempString = folder.readStringUntil('\n');
            i = i + 1;
            if (i >= array_size + 1)
            {
                ar[iter] = tempString;
                iter = iter + 1;
            }
        }
    }
    folder.close();


    Serial.println(F("Removing old file..."));
    if (SD.exists("TEST.TXT"))
    {
        SD.remove("TEST.TXT");
    }

    Serial.println(F("Making new File.."));


    folder = SD.open("TEST.TXT", FILE_WRITE);

    for (int j = 0; j < iter ; j++)
    {
        folder.print(ar[j]);
        folder.print("\n");
    }
    folder.close();
    Serial.print(F("New file size: "));
    Serial.print(iter);
    Serial.println();
}

void SD_printSD(float temp, float hum, float gyrox, float gyroy, float gyroz, float accx, float accy, float accz, float latt, float longg, float gnssnum, float glonassnum, String datee, String timee)
{   String tmp = "";
    char tmpBuffer[50];
    folder = SD.open("TEST.TXT", FILE_WRITE);

    folder.print(temp);
    folder.print(",");
    folder.print(hum);
    folder.print(",");
    dtostrf(gyrox,10, 5, tmpBuffer);
    tmp = String(tmpBuffer);
    tmp.trim();
    folder.print(tmp);
    folder.print(",");
    dtostrf(gyroy,10, 5, tmpBuffer);
    tmp = String(tmpBuffer);
    tmp.trim();
    folder.print(tmp);
    folder.print(",");
    dtostrf(gyroz,10, 5, tmpBuffer);
    tmp = String(tmpBuffer);
    tmp.trim();
    folder.print(tmp);
    folder.print(",");
    dtostrf(accx,10, 5, tmpBuffer);
    tmp = String(tmpBuffer);
    tmp.trim();
    folder.print(tmp);
    folder.print(",");
    dtostrf(accy,10, 5, tmpBuffer);
    tmp = String(tmpBuffer);
    tmp.trim();
    folder.print(tmp);
    folder.print(",");
    dtostrf(accz,10, 5, tmpBuffer);
    tmp = String(tmpBuffer);
    tmp.trim();
    folder.print(tmp);
    folder.print(",");
    dtostrf(latt,7, 6, tmpBuffer);
    tmp = String(tmpBuffer);
    tmp.trim();
    folder.print(tmp);
    folder.print(",");
    dtostrf(longg,7, 6, tmpBuffer);
    tmp = String(tmpBuffer);
    tmp.trim();
    folder.print(tmp);
    folder.print(",");
    folder.print(round(gnssnum));
    folder.print(",");
    folder.print(round(glonassnum));
    folder.print(",");
    folder.print(datee);
    folder.print(",");
    folder.print(timee);
    folder.print("\n");
    folder.flush();
    folder.close();

    Serial.println(F("Row printed.")); 
}

void SD_makeJSON(int array_size)
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

    folder = SD.open("TEST.TXT");
    if(folder)
    {
      while(1){
        tempString = folder.readStringUntil('\n');
        //Serial.println(tempString);
        temperatureArray.add(getSplittedValuee(tempString, ',', 0).toFloat());
        humidityArray.add(getSplittedValuee(tempString, ',', 1).toFloat());
        gyroxArray.add(getSplittedValuee(tempString, ',', 2).toFloat());
        gyroyArray.add(getSplittedValuee(tempString, ',', 3).toFloat());
        gyrozArray.add(getSplittedValuee(tempString, ',', 4).toFloat());
        accxArray.add(getSplittedValuee(tempString, ',', 5).toFloat());
        accyArray.add(getSplittedValuee(tempString, ',', 6).toFloat());
        acczArray.add(getSplittedValuee(tempString, ',', 7).toFloat());
        lattitudeArray.add(getSplittedValuee(tempString, ',', 8).toFloat());
        longtitudeArray.add(getSplittedValuee(tempString, ',', 9).toFloat());
        gnssArray.add(getSplittedValuee(tempString, ',', 10).toInt());
        glonassArray.add(getSplittedValuee(tempString, ',', 11).toInt());
        dateArray.add(getSplittedValuee(tempString, ',', 12));
        timeArray.add(getSplittedValuee(tempString, ',', 13));
        object["USER"] = user;

        counter = counter + 1;

        if(counter == array_size)
        {
            break;
        }

      }
        folder.close();
    }

}

int SD_getJsonSize()
{
    return measureJsonPretty(object);
}

void SD_serializeJSON(int type)
{
    if(type==0)
    {
        serializeJsonPretty(object,Serial);
    }
    if(type==3)
    {
        serializeJsonPretty(object,Serial3);
    }

}

void SD_freeJSON()
{
    doc.clear();
    doc.garbageCollect();
    object = doc.to<JsonObject>();
}
