#ifndef HX711_LOADCELL
#define HX711_LOADCELL

#include <HX711.h>

class hx711_loadcell
{
public:
    hx711_loadcell(int dout_pin, int sck_pin, int threshold);

    /// @brief Check if the measured weight on `_scale` is greater than `threshold_val`
    /// @return `bool` If the weight is greater than `_threshold_val`, `true`
    bool isPressed();

    /// @brief Get the current measured weight from the scale
    /// @return `int` Current output of the scale
    int val();

    /// @brief Set the value for which the scale tips
    /// @param newVal New arbitrary value for the scale to tip
    void setThreshold(int newVal);

    /// @brief Calibrate the load cell by reading over 10 seconds and setting threshold
    /// Reads the load cell continuously for 10 seconds, calculates the average,
    /// and sets the threshold to 95% of the average (5% less)
    void calibrate();

    inline int getThreshold() { return _threshold_val; }

private:
    HX711 *_scale = nullptr;
    bool _cur_state;
    bool _prev_state;
    int _threshold_val;
    long _ambient;
    long _weighted;
};

#endif
