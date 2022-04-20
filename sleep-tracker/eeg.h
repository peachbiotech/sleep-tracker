#define EEG_PIN_1 39
#define EEG_PIN_2 32
#define EEG_REF 38
#include <Filters.h>
#include <list>

FilterTwoPole filterLowpass;
FilterOnePole filterHighpass( HIGHPASS, 62 ); 

float eeg_1 = 0.0;
bool newEEGDataAvailable = false;

TimerEvent eegTimer;
unsigned int eegTimerPeriod = 5;

float read_eeg();
void eeg_setup();