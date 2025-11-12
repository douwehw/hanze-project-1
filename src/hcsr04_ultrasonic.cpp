#include <hcsr04_ultrasonic.h>

HCSR04_Ultrasonic::HCSR04_Ultrasonic(uint8_t echo, uint8_t trig)
{
    _echo = echo;
    _trig = trig;

    pinMode(_echo, INPUT);
    pinMode(_trig, OUTPUT);
}

unsigned long HCSR04_Ultrasonic::distance()
{
    // Clears the TRIG
    digitalWrite(_trig, LOW);
    delayMicroseconds(1);

    // Sets the _ on HIGH state for 10 micro seconds
    digitalWrite(_trig, HIGH);
    delayMicroseconds(10);
    digitalWrite(_trig, LOW);

    // Reads the ECHO, returns the sound wave travel time in microseconds
    unsigned long duration = pulseIn(_echo, HIGH);

    // Calculate the distance
    return (duration * SOUND_SPEED / 2.0);
}
