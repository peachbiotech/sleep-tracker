// Heartrate and PulseOx includes
#include <SparkFun_Bio_Sensor_Hub_Library.h>
#include <Wire.h>
#include <TimerEvent.h>

// Reset pin, MFIO pin for Heartrate and PulseOx
#define RES_PIN 12
#define MFIO_PIN 13

// Timer callback for monitoring vitals
// Changes according to HeartRateO2ReadRateService
unsigned long startTime;
TimerEvent heartRateO2Timer;
unsigned int heartRateO2TimerPeriod = 250;//5*60000; // 5 Minute timer
bool newRecordingRate = false;

// Takes address, reset pin, and MFIO pin.
SparkFun_Bio_Sensor_Hub bioHub(RES_PIN, MFIO_PIN);

bioData body;
// body.heartrate  - Heartrate
// body.confidence - Confidence in the heartrate value
// body.oxygen     - Blood oxygen level
// body.status     - Has a finger been sensed?

bool newSensorDataAvailable = false;

void ppg_setup();