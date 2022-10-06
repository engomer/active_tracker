#include <Wire.h>
#include "DFRobot_SHT20.h"
#include "SHTL.h"

DFRobot_SHT20 sht20;
float t = 0;
float h = 0;

void initSHT()
{
    sht20.initSHT20();
    delay(200);
    sht20.checkSHT20();
}

float getTemp()
{
  return t;
}
float getHum()
{
  return h;
}

void updateSHT()
{
  t = sht20.readTemperature();
  h = sht20.readHumidity();
}
