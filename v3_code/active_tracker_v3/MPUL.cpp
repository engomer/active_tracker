#include <Wire.h>
#include <MPU6050_tockn.h>
#include "MPUL.h"

MPU6050 mpu6050(Wire);
float gx = 0;
float gy = 0;
float gz = 0;
float ax = 0;
float ay = 0;
float az = 0;
long timer = 0;
void initMPU()
{
    Wire.begin();
    mpu6050.begin();
    mpu6050.calcGyroOffsets(true);
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
