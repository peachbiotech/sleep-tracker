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

void ble_setup(void) {
    // Init BLE device
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
}