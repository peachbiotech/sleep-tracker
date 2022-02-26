#define BAT_PIN 34

const float battery_raw_to_voltage_factor = 3.935 / 2960.0;

float read_battery_raw();
std::string read_battery_voltage();