
// Project includes
#include "imu.h"
#include "ble.h"
#include "ppg.h"
#include "audio.h"

void setup() {

    Serial.begin(115200);

    imu_setup();

    ble_setup();
    
    ppg_setup();

}

void loop() {

    int audio = read_audio();
    int gate = read_gate();
    Serial.print(audio);
    Serial.print('\t');
    Serial.println(gate);
    
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
    //Serial.println("Measurements:");
    //Serial.print("    Heartrate:                        ");
    //Serial.println(body.heartRate);
    //Serial.print("    Oxygen:                             ");
    //Serial.println(body.oxygen);
    //Serial.print("    HeartRate Confidence: ");
    //Serial.println(body.confidence);
    //Serial.print("    Status:                             ");
    //Serial.println(body.status);

    newSensorDataAvailable = true;
}
