#ifndef TIMER_H
#define TIMER_H

#include <Arduino.h>

class Timer
{
public:
    Timer();

    /// @brief Starts the timer, also works as a reset.
    void start();

    /// @brief Stop the timer from counting.
    void stop();

    /// @brief Stops the timer while keeping track of the time when it was paused.
    void pause();

    /// @brief Continue a paused timer from the point it paused.
    void unpause();

    /// @brief (Un)pause the timer.
    /// @param b `Bool` timer's new pause state.
    void paused(bool b);

    /// @brief Shorthand for `start()`. Purely for code readability.
    inline void restart() { start(); };

    /// @brief Get the seconds a started timer has been running.
    /// @return `double` amount of seconds the timer has been running.
    double seconds();

    /// @brief Get the amount of milliseconds the timer has been running. This method utilises the `millis()` function from Arduino.h, using this is the same as just using `millis()` in your code by comparing the current to the start value
    /// @return `long` amount of milliseconds the timer has been running.
    long milliseconds();

    /// @brief Check if the timer has started.
    /// @return `bool` If the timer has started or not.
    inline bool started() { return _started; }

private:
    long _start_ticks;
    long _paused_ticks;
    bool _paused;
    bool _started;
};

#endif
