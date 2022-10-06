/*
 * Worked on SCL = PB6 SDA = PB7
 */

#include <Wire.h>
#include "DFRobot_SHT20.h"
#include <MPU6050_tockn.h>

MPU6050 mpu6050(Wire);
float gx = 0;
float gy = 0;
float gz = 0;
float ax = 0;
float ay = 0;
float az = 0;
long timer = 0;

DFRobot_SHT20 sht20;
float t = 0;
float h = 0;




void setup() {
  // put your setup code here, to run once:

    Serial.begin(9600);
    
    Wire.begin();
    mpu6050.begin();
    mpu6050.calcGyroOffsets(true);

    sht20.initSHT20();
    delay(200);
    sht20.checkSHT20();
}

void loop() {
  // put your main code here, to run repeatedly:
  updateSHT();
  updateIMU();
  Serial.print("Temp: ");
  Serial.print(getTemp());
  Serial.print(",");
  Serial.print("Hum: ");
  Serial.print(getHum());
  Serial.println();
  Serial.print(getGx());
  Serial.print(",");
  Serial.print(getGy());
  Serial.print(",");
  Serial.print(getGz());
  Serial.print(",");
  Serial.print(getAx());
  Serial.print(",");
  Serial.print(getAy());
  Serial.print(",");
  Serial.print(getAz());
  Serial.println();

  delay(200);
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

void updateIMU()
{
    static float mpu_arr[6];

    mpu6050.update();

    if(millis() - timer > 1000){
    
    
        ax = mpu6050.getAccX();
        ay = mpu6050.getAccY();
        az = mpu6050.getAccZ();
        gx = mpu6050.getGyroX();
        gy = mpu6050.getGyroY();
        gz = mpu6050.getGyroZ();

        timer = millis();
    }


}
  float getGx(){ return gx;} 
  float getGy(){ return gy;}
  float getGz(){ return gz;}
  float getAx(){ return ax;}
  float getAy(){ return ay;}
  float getAz(){ return az;}
