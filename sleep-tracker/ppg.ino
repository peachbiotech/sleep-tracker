
void ppg_setup(void) {
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
    delay(5000);

    // Callback setup
    heartRateO2Timer.set(heartRateO2TimerPeriod, measureHeartRateO2);
}