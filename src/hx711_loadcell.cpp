#include <hx711_loadcell.h>

hx711_loadcell::hx711_loadcell(int dout_pin, int sck_pin, int threshold)
{
    _scale = new HX711;
    _scale->begin(dout_pin, sck_pin);
    _cur_state, _prev_state = false;
    _threshold_val = threshold;
}

bool hx711_loadcell::isPressed()
{
    if (_scale != nullptr)
    {
        // Arbitrary value due to lack of calibration
        return _scale->read() > _threshold_val;
    }
    return false;
}

int hx711_loadcell::val()
{
    if (_scale != nullptr)
    {
        return _scale->read();
    }
    return 0;
}

void hx711_loadcell::setThreshold(int newVal)
{
    _threshold_val = newVal;
}
