#ifndef HCSR04_ULTRASONIC
#define HCSR04_ULTRASONIC

#include <Arduino.h>

#define SOUND_SPEED 0.034F

class HCSR04_Ultrasonic
{
public:
    HCSR04_Ultrasonic(uint8_t echo, uint8_t trig);

    unsigned long distance();
private:
    uint8_t _echo, _trig;
    long _dur;
    float _dist;

};

typedef struct HCSR04_PINS
{
    const uint8_t echo, trig;
};

#endif
