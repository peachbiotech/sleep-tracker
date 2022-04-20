
// Project includes
#include "imu.h"
#include "ble.h"
#include "ppg.h"
#include "audio.h"
#include "battery_manager.h"
#include "eeg.h"


void setup() {
    Serial.begin(115200);

    ppg_setup();

    imu_setup();

    ble_setup();

    eeg_setup();

}

void loop() {

    // audio
    //int audio = read_audio();
    //int gate = read_gate();
    //Serial.print(audio);
    //Serial.print('\t');
    //Serial.println(gate);

    // battery
    float battery_raw = read_battery_raw();
    std::string battery_voltage = read_battery_voltage();
    pBatteryLevelCharacteristic->setValue(std::to_string(battery_raw));
    pBatteryVoltageCharacteristic->setValue(battery_voltage);
    
    // notifications (ppg, imu, eeg)
    heartRateO2Timer.update();
    imuTimer.update();
    eegTimer.update();
    if (newRecordingRate) {
        heartRateO2Timer.set(heartRateO2TimerPeriod, measureHeartRateO2);
        newRecordingRate = false;
    }
    if (deviceConnected) {
        if (newHeartRateO2DataAvailable) {
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
            newHeartRateO2DataAvailable = false;
        }

        if (newIMUDataAvailable) {
            float a_x = a.acceleration.x;
            float a_y = a.acceleration.y;
            float a_z = a.acceleration.z;

            float g_x = g.gyro.x;
            float g_y = g.gyro.y;
            float g_z = g.gyro.z;
            std::string accel_str = std::to_string(a_x) + '\t' + std::to_string(a_y) + '\t' + std::to_string(a_z);
            std::string gyro_str = std::to_string(g_x) + '\t' + std::to_string(g_y) + '\t' + std::to_string(g_z);
            
            pAccelerometerCharacteristic->setValue(accel_str);
            pGyroscopeCharacteristic->setValue(gyro_str);
            pTemperatureCharacteristic->setValue(std::to_string(temp.temperature));
            pAccelerometerCharacteristic->notify();
            pGyroscopeCharacteristic->notify();
            pTemperatureCharacteristic->notify();
            newIMUDataAvailable = false;
        }

        if (newEEGDataAvailable) {
            pEEGMeasurementCharacteristic->setValue(std::to_string(eeg_1));
            pEEGMeasurementCharacteristic->notify();
            newEEGDataAvailable = false;
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

void measureEEG() {
  Serial.println("eeg measure");
  eeg_1= read_eeg_1();
  Serial.println(eeg_1);  
  newEEGDataAvailable = true;
}

void measureIMU() {
    // imu
    Serial.println("imu measure");
    mpu.getEvent(&a, &g, &temp);
    newIMUDataAvailable = true;
}

void measureHeartRateO2() {

    body = bioHub.readBpm();
    //Serial.println("ppg measure");
    //Serial.println("Measurements:");
    //Serial.print("    Heartrate:                        ");
    //Serial.println(body.heartRate);
    //Serial.print("    Oxygen:                             ");
    //Serial.println(body.oxygen);
    //Serial.print("    HeartRate Confidence: ");
    //Serial.println(body.confidence);
    //Serial.print("    Status:                             ");
    //Serial.println(body.status);

    newHeartRateO2DataAvailable = true;
}
