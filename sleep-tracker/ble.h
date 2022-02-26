// BLE includes
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <BLE2902.h>

// Read services and characteristics

// ppg
#define HEARTRATEO2_SERVICE_UUID "7ab13626-ddc3-4fa0-b724-06460cc40223"
#define HEARTRATE_MEASUREMENT_CHARACTERISTIC_UUID "179499e2-a77f-44e8-893a-3e05a904d0e2"
#define O2_MEASUREMENT_CHARACTERISTIC_UUID "3b040b83-161b-4a11-8ef8-6a869b06e1e9"

// battery
#define BATTERY_SERVICE_UUID "d650e494-beac-45e5-ab00-d27ae4f9c603"
#define BATTERY_LEVEL_CHARACTERISTIC_UUID "64cb58e5-c7c6-4976-bfb2-8b0cb7527c22"
#define BATTERY_VOLTAGE_CHARACTERISTIC_UUID "8a1c6667-e658-4c43-9894-f6b119abb755"

// imu
#define IMU_SERVICE_UUID "3b7a1639-8d9a-49e7-8077-67f398d95c2b"
#define ACCELEROMETER_CHARACTERISTIC_UUID "c89c6c9f-ef3e-4e78-84c6-8834566269b8"
#define GYROSCOPE_CHARACTERISTIC_UUID "4f6aaa29-8919-44bd-b21d-2950bb9e6922"
#define TEMPERATURE_CHARACTERISTIC_UUID "9d175382-ac47-4be4-8c60-24ec6024fc8d"

// Write services and characteristics
#define HEARTRATEO2_READ_RATE_SERVICE_UUID "71b55e6a-a938-432c-9304-b119b4f626d2"
#define HEARTRATEO2_READ_RATE_CHARACTERISTIC_UUID "aba135ba-32a2-4190-a3d6-b26bdc8f123d"

BLEServer *pServer = NULL;

BLEService *pHeartRateO2Service = NULL;
BLECharacteristic *pHeartRateMeasurementCharacteristic = NULL;
BLECharacteristic *pOxygenMeasurementCharacteristic = NULL;

BLEService *pBatteryService = NULL;
BLECharacteristic *pBatteryLevelCharacteristic = NULL;
BLECharacteristic *pBatteryVoltageCharacteristic = NULL;

BLEService *pIMUService = NULL;
BLECharacteristic *pAccelerometerCharacteristic = NULL;
BLECharacteristic *pGyroscopeCharacteristic = NULL;
BLECharacteristic *pTemperatureCharacteristic = NULL;

BLEService *pHeartRateO2ReadRateService = NULL;
BLECharacteristic *pHeartRateO2ReadRateCharacteristic = NULL;

bool deviceConnected = false;
bool oldDeviceConnected = false;

void ble_setup();