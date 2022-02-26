float read_battery_raw() {
    int reads = 15;
    double battery_avg_read = 0;

    for (int i = 0; i < reads; i++) {
        battery_avg_read += analogRead(BAT_PIN);
    }

    return battery_avg_read / reads;
}

std::string read_battery_voltage() {
    return std::to_string(read_battery_raw() * battery_raw_to_voltage_factor);
}