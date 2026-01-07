#include <tm1637_display.h>

TM1367_Display::TM1367_Display(int CLK_pin, int DIO_pin)
{
    _prevTime = 0;
    _display = new TM1637Display(CLK_pin, DIO_pin);
}

void TM1367_Display::showTime(int sec)
{
    if (sec != _prevTime)
    {
        int minutes = sec / 60;
        int seconds = sec % 60;
        int timeValue = minutes * 100 + seconds;

        bool showLeadingZero = (minutes >= 10);

        _display->showNumberDecEx(timeValue, 0b01000000, showLeadingZero);
    }
    _prevTime = sec;
}
