#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
#include <TimerEvent.h>

Adafruit_MPU6050 mpu;
sensors_event_t a, g, temp;
bool newIMUDataAvailable = false;

TimerEvent imuTimer;
unsigned int imuTimerPeriod = 250;

void imu_setup();