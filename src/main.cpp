#include <Arduino.h>

#include <hcsr04_ultrasonic.h>
#include <ky012_buzzer.h>
#include <timer.h>

// Initialising pins inside struct
const HCSR04_PINS LEFT_HCSR04_PINS = {0, 0};
const HCSR04_PINS RIGHT_HCSR04_PINS = {0, 0};

// Initialising sensor classes on the heap
HCSR04_Ultrasonic *left_hcsr04;
HCSR04_Ultrasonic *right_hcsr04;
Timer *timer;

void setup()
{
    Serial.begin(115200);

    left_hcsr04 = new HCSR04_Ultrasonic(LEFT_HCSR04_PINS.echo, LEFT_HCSR04_PINS.trig);
    right_hcsr04 = new HCSR04_Ultrasonic(RIGHT_HCSR04_PINS.echo, RIGHT_HCSR04_PINS.trig);

    timer = new Timer();
}

void loop()
{

}
