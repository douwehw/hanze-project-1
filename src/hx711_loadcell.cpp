#include <hx711_loadcell.h>
#include <Arduino.h>
#include <timer.h>

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
        long current_reading = abs(_scale->read());

        // Check direction based on calibration
        if (_weighted > _ambient)
        {
            return current_reading > _threshold_val;
        }
        else
        {
            return current_reading < _threshold_val;
        }
    }
    return false;
}

int hx711_loadcell::val()
{
    if (_scale != nullptr)
    {
        return abs(_scale->read());
    }
    return 0;
}

void hx711_loadcell::setThreshold(int newVal)
{
    _threshold_val = newVal;
}

void hx711_loadcell::calibrate()
{
    if (_scale == nullptr)
    {
        return;
    }

    // PHASE 1: Measure ambient/baseline value (10 seconds)
    Serial.println("Phase 1: Measuring ambient value...");
    const unsigned long phase1_duration = 10000; // 10 seconds
    Timer timer;
    timer.start();
    long sum = 0;
    int count = 0;

    while (timer.milliseconds() < phase1_duration)
    {
        if (_scale->is_ready())
        {
            sum += abs(_scale->read());
            count++;
        }
        delay(10);
    }

    if (count == 0)
    {
        Serial.println("Calibration failed: No readings");
        return;
    }

    long ambient_average = sum / count;
    Serial.print("Ambient value: ");
    Serial.println(ambient_average);

    // Wait for weight to be placed (>5% difference)
    Serial.println("Place weight on load cell...");
    bool weight_detected = false;
    while (!weight_detected)
    {
        if (_scale->is_ready())
        {
            long current_reading = abs(_scale->read());
            long difference = abs(current_reading - ambient_average);
            long threshold_diff = ambient_average * 0.05;

            if (difference > threshold_diff)
            {
                weight_detected = true;
                Serial.println("Weight detected, measuring...");
            }
        }
        delay(50);
    }

    // PHASE 2: Measure weighted value (5 seconds)
    const unsigned long phase2_duration = 5000; // 5 seconds
    timer.start();
    sum = 0;
    count = 0;

    while (timer.milliseconds() < phase2_duration)
    {
        if (_scale->is_ready())
        {
            sum += abs(_scale->read());
            count++;
        }
        delay(10);
    }

    if (count > 0)
    {
        long weighted_average = sum / count;
        Serial.print("Weighted value: ");
        Serial.println(weighted_average);

        // Store calibration values
        _ambient = ambient_average;
        _weighted = weighted_average;

        // Calculate 10% of the difference between ambient and weighted
        long difference = abs(_weighted - _ambient);
        long margin = difference * 0.10;

        // Set threshold based on direction
        if (_weighted > _ambient)
        {
            _threshold_val = _weighted - margin; // Move 10% closer to ambient
        }
        else
        {
            _threshold_val = _weighted + margin; // Move 10% closer to ambient
        }

        Serial.print("Threshold set to: ");
        Serial.println(_threshold_val);
    }
}
