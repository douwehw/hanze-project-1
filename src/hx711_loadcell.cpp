#include <hx711_loadcell.h>

hx711_loadcell::hx711_loadcell(int dout_pin, int sck_pin)
{
    _scale = new HX711;
    _scale->begin(dout_pin, sck_pin);
    _curState, _prevState = false;
}

bool hx711_loadcell::isPressed()
{
    // Arbitrary value due to lack of calibration
    return _scale->read() > 150000;
}
