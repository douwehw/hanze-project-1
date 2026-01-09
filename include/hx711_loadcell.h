#ifndef HX711_LOADCELL
#define HX711_LOADCELL

#include <HX711.h>

class hx711_loadcell
{
public:
    hx711_loadcell(int dout_pin, int sck_pin);
    bool isPressed();
private:
    HX711 *_scale;
    bool _curState;
    bool _prevState;
};

#endif
