#include <ky012_buzzer.h>

KY012_Buzzer::KY012_Buzzer(uint8_t pin)
{
    _pin = pin;

    pinMode(_pin, OUTPUT);
}

void KY012_Buzzer::buzz(bool enable)
{
    digitalWrite(_pin, enable);
}
