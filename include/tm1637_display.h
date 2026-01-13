#ifndef TM1367_DISPLAY
#define TM1367_DISPLAY

#include <TM1637Display.h>

class TM1367_Display
{
public:
    TM1367_Display(int CLK_Pin, int DIO_pin);

    /// @brief Formats and displays and an amount of seconds.
    /// @param secs `int` time in seconds to display on the display.
    void showTime(int secs);
private:
    int _prev_time;
    TM1637Display* _display;
};

#endif
