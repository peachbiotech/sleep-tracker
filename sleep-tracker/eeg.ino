void eeg_setup() {
    filterLowpass.setAsFilter(LOWPASS_BUTTERWORTH, 45);
    eegTimer.set(eegTimerPeriod, measureEEG);
}

float filter(float inputValue) {
  filterLowpass.input(inputValue);
  filterHighpass.input(filterLowpass.output());
  filterLowpass.input(filterHighpass.output());
  return filterLowpass.output();
}

float read_eeg_1() {
    return filter((float)analogRead(EEG_PIN_1));
}