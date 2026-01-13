#ifndef HX711_LOADCELL
#define HX711_LOADCELL

#include <HX711.h>

class hx711_loadcell
{
public:
    hx711_loadcell(int dout_pin, int sck_pin, int threshold);
    bool isPressed();
    int val();
    void setThreshold(int newVal);
private:
    HX711 *_scale = nullptr;
    bool _curState;
    bool _prevState;
    int _thresholdVal;
};

#endif
