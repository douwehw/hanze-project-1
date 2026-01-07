#ifndef TM1367_DISPLAY
#define TM1367_DISPLAY

#include <TM1637Display.h>

class TM1367_Display
{
public:
    TM1367_Display(int CLK_Pin, int DIO_pin);
    void showTime(int secs);
private:
    int _prevTime;
    TM1637Display* _display;
};

#endif
