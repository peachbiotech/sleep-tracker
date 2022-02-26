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

BLEServer *pServer = NULL;

BLEService *pHeartRateO2Service = NULL;
BLECharacteristic *pHeartRateMeasurementCharacteristic = NULL;
BLECharacteristic *pOxygenMeasurementCharacteristic = NULL;

BLEService *pHeartRateO2ReadRateService = NULL;
BLECharacteristic *pHeartRateO2ReadRateCharacteristic = NULL;

bool deviceConnected = false;
bool oldDeviceConnected = false;

void ble_setup();