#include <Arduino.h>
#include <tm1637_display.h>

#include <timer.h>

#define CLK 17
#define DIO 16

// Initialising sensor classes on the heap
TM1367_Display *display;
Timer *timer;

void setup()
{
    Serial.begin(115200);

    display = new TM1367_Display(CLK, DIO);

    timer = new Timer();
    timer->start();
}

void loop()
{

}
