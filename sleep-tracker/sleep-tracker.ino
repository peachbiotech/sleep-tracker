/*
  This example sketch gives you exactly what the SparkFun Pulse Oximiter and
  Heart Rate Monitor is designed to do: read heart rate and blood oxygen levels.
  This board requires I-squared-C connections but also connections to the reset
  and mfio pins. When using the device keep LIGHT and CONSISTENT pressure on the
  sensor. Otherwise you may crush the capillaries in your finger which results
  in bad or no results. A summary of the hardware connections are as follows:
  SDA -> SDA
  SCL -> SCL
  RESET -> PIN 4
  MFIO -> PIN 5

  Author: Elias Santistevan
  Date: 8/2019
  SparkFun Electronics

  If you run into an error code check the following table to help diagnose your
  problem:
  1 = Unavailable Command
  2 = Unavailable Function
  3 = Data Format Error
  4 = Input Value Error
  5 = Try Again
  255 = Error Unknown
*/

// Heartrate and PulseOx includes
#include <SparkFun_Bio_Sensor_Hub_Library.h>
#include <Wire.h>
#include <TimerEvent.h>

// BLE includes
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <BLE2902.h>

// Read services and characteristics
#define HEARTRATEO2_SERVICE_UUID "7ab13626-ddc3-4fa0-b724-06460cc40223"
#define HEARTRATE_MEASUREMENT_CHARACTERISTIC_UUID "179499e2-a77f-44e8-893a-3e05a904d0e2"
#define O2_MEASUREMENT_CHARACTERISTIC_UUID "3b040b83-161b-4a11-8ef8-6a869b06e1e9"

// Write services and characteristics
#define HEARTRATEO2_READ_RATE_SERVICE_UUID "71b55e6a-a938-432c-9304-b119b4f626d2"
#define HEARTRATEO2_READ_RATE_CHARACTERISTIC_UUID "aba135ba-32a2-4190-a3d6-b26bdc8f123d"


// Reset pin, MFIO pin for Heartrate and PulseOx
int resPin = 12;
int mfioPin = 13;

// Timer callback for monitoring vitals
// Changes according to HeartRateO2ReadRateService
unsigned long startTime;
TimerEvent heartRateO2Timer;
unsigned int heartRateO2TimerPeriod = 250;//5*60000; // 5 Minute timer
bool newRecordingRate = false;

// Takes address, reset pin, and MFIO pin.
SparkFun_Bio_Sensor_Hub bioHub(resPin, mfioPin);

bioData body;
// body.heartrate  - Heartrate
// body.confidence - Confidence in the heartrate value
// body.oxygen     - Blood oxygen level
// body.status     - Has a finger been sensed?

bool newSensorDataAvailable = false;

BLEServer *pServer = NULL;

BLEService *pHeartRateO2Service = NULL;
BLECharacteristic *pHeartRateMeasurementCharacteristic = NULL;
BLECharacteristic *pOxygenMeasurementCharacteristic = NULL;

BLEService *pHeartRateO2ReadRateService = NULL;
BLECharacteristic *pHeartRateO2ReadRateCharacteristic = NULL;

bool deviceConnected = false;
bool oldDeviceConnected = false;


class ServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      Serial.println("Device connected");
      deviceConnected = true;
    };

    void onDisconnect(BLEServer* pServer) {
      deviceConnected = false;
      Serial.println("Device disconnected");
    }
};

class HeartRateO2ReadRateCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
      std::string value = pCharacteristic->getValue();

      if (value.length() > 0) {
        heartRateO2TimerPeriod = (unsigned int) std::stoi(value);
        Serial.println("*********");
        Serial.print("New rate: ");
        Serial.println(heartRateO2TimerPeriod);
        Serial.println("*********");
        newRecordingRate = true;
      }
    }
};

void setup() {

  Serial.begin(115200);

  // Init device
  BLEDevice::init("SleepTracker");  
  
  // Create BLE server
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new ServerCallbacks());

  // Create services and characteristics
  // pHeartRateO2Service
  pHeartRateO2Service = pServer->createService(HEARTRATEO2_SERVICE_UUID);
  pHeartRateMeasurementCharacteristic = pHeartRateO2Service->createCharacteristic(
                                         HEARTRATE_MEASUREMENT_CHARACTERISTIC_UUID,
                                         BLECharacteristic::PROPERTY_NOTIFY
                                        );
  pHeartRateMeasurementCharacteristic->addDescriptor(new BLE2902());
  pOxygenMeasurementCharacteristic = pHeartRateO2Service->createCharacteristic(
                                      O2_MEASUREMENT_CHARACTERISTIC_UUID,
                                      BLECharacteristic::PROPERTY_NOTIFY
                                     );
  pHeartRateO2Service->start();
  
  // pHeartRateO2ReadRateService
  pHeartRateO2ReadRateService = pServer->createService(HEARTRATEO2_READ_RATE_SERVICE_UUID);
  pHeartRateO2ReadRateCharacteristic = pHeartRateO2ReadRateService->createCharacteristic(
                                        HEARTRATEO2_READ_RATE_CHARACTERISTIC_UUID,
                                        BLECharacteristic::PROPERTY_READ |
                                        BLECharacteristic::PROPERTY_WRITE
                                       );
  pHeartRateO2ReadRateCharacteristic->setCallbacks(new HeartRateO2ReadRateCallbacks());
  pHeartRateO2ReadRateCharacteristic->setValue("None");
  pHeartRateO2ReadRateService->start();

  // Start advertising
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(HEARTRATEO2_SERVICE_UUID);
  pAdvertising->setScanResponse(false);
  pAdvertising->setMinPreferred(0x0);
  BLEDevice::startAdvertising();
  Serial.println("Waiting a client connection to notify...");
  randomSeed( (unsigned long)( micros()%millis() ) );
  startTime = millis();

  // Callback setup
  heartRateO2Timer.set(heartRateO2TimerPeriod, measureHeartRateO2);
  
  // PulseOx and Heart Rate initialization
  Wire.begin();
  int result = bioHub.begin();
  if (result == 0) // Zero errors!
    Serial.println("Sensor started!");
  else
    Serial.println("Could not communicate with the sensor!!!");

  Serial.println("Configuring Sensor....");
  int error = bioHub.configBpm(MODE_ONE); // Configuring just the BPM settings.
  if (error == 0) { // Zero errors!
    Serial.println("Sensor configured.");
  }
  else {
    Serial.println("Error configuring sensor.");
    Serial.print("Error: ");
    Serial.println(error);
  }

  // Data lags a bit behind the sensor, if you're finger is on the sensor when
  // it's being configured this delay will give some time for the data to catch
  // up.
  Serial.println("Loading up the buffer with data....");
  delay(4000);
}

void loop() {
  heartRateO2Timer.update();
  if (newRecordingRate) {
    heartRateO2Timer.set(heartRateO2TimerPeriod, measureHeartRateO2);
    newRecordingRate = false;
  }
  if (deviceConnected) {
    if (newSensorDataAvailable) {
      if (body.confidence >= 95 && body.oxygen >= 50) {
        Serial.println("Notifying central of new data");
        pHeartRateMeasurementCharacteristic->setValue(body.heartRate);
        pOxygenMeasurementCharacteristic->setValue(body.oxygen);
      }
      else {
        Serial.println("Notifying central of invalid data");
        uint16_t nil_val = 0;
        pHeartRateMeasurementCharacteristic->setValue(nil_val);
        pOxygenMeasurementCharacteristic->setValue(nil_val);
      }
      pHeartRateMeasurementCharacteristic->notify();
      pOxygenMeasurementCharacteristic->notify();
      newSensorDataAvailable = false;
    }
  }
  // disconnecting
  if (!deviceConnected && oldDeviceConnected) {
    Serial.println("Disconnected");
    delay(500); // give the bluetooth stack the chance to get things ready
    pServer->startAdvertising(); // restart advertising
    Serial.println("Restart advertising");
    oldDeviceConnected = deviceConnected; // false
  }
  // connecting
  if (deviceConnected && !oldDeviceConnected) {
    oldDeviceConnected = deviceConnected;
  }
}

void measureHeartRateO2() {
  body = bioHub.readBpm();
  Serial.println("Measurements:");
  Serial.print("  Heartrate:            ");
  Serial.println(body.heartRate);
  Serial.print("  Oxygen:               ");
  Serial.println(body.oxygen);
  Serial.print("  HeartRate Confidence: ");
  Serial.println(body.confidence);
  Serial.print("  Status:               ");
  Serial.println(body.status);

  newSensorDataAvailable = true;
}
