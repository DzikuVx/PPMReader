/*
PPMReader library based on https://github.com/Hasi123/read-any-ppm
Works with Servo library

Paweł (DzikuVx) Spychalski 2017
https://quadmeup.com

License: GNU GPL v3
*/

#include "PPMReader.h"
#include "Arduino.h"
#include <PinChangeInterrupt.h>

volatile int PPMReader::ppm[PMM_CHANNEL_COUNT];

PPMReader::PPMReader(int pin, int interrupt, int mode)
{
    _pin = pin;
    _interrupt = interrupt;
    _mode = mode;

    for (uint8_t i = 0; i < PMM_CHANNEL_COUNT; i++) {
        ppm[i] = 0;
    }

    if (mode == MODE_INTERRUPT) {
        pinMode(_pin, INPUT);
        attachInterrupt(_interrupt, PPMReader::handler, CHANGE);
    } else if (mode == MODE_PIN_CHANGE_INTERRUPT) {
        attachPinChangeInterrupt(pin, PPMReader::handler, CHANGE);
    }
}

int PPMReader::get(uint8_t channel)
{
    return ppm[channel];
}

void PPMReader::start(void) {
    attachInterrupt(_interrupt, PPMReader::handler, CHANGE);
}

void PPMReader::stop(void) {
    detachInterrupt(_interrupt);
}

static void PPMReader::handler()
{
    static unsigned int pulse;
    static unsigned long counter;
    static byte channel;
    static unsigned long previousCounter = 0;
    static unsigned long currentMicros = 0;

    currentMicros = micros();
    counter = (currentMicros - previousCounter) * 2;
    previousCounter = currentMicros;

    if (counter < 1020)
    { //must be a pulse
        pulse = counter;
    }
    else if (counter > 3820)
    { //sync
        channel = 0;
    }
    else
    { //servo values between 810 and 2210 will end up here
        ppm[channel] = (counter + pulse) / 2;
        channel++;
    }
}