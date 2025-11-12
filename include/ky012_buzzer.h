#ifndef KY012_BUZZER
#define KY012_BUZZER

#include <Arduino.h>

class KY012_Buzzer
{
public:
    KY012_Buzzer(uint8_t pin);

    void buzz(bool enable);
private:
    uint8_t _pin;
};

#endif
