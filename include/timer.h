#ifndef TIMER_H
#define TIMER_H

#include <Arduino.h>

class Timer
{
public:
    Timer();

    void start();
    void stop();
    void pause();
    void unpause();
    void paused(bool b);
    inline void restart() { start(); };

    double seconds();
    long milliseconds();
    inline bool started() { return _started; }

private:
    long _startTicks;
    long _pausedTicks;
    bool _paused;
    bool _started;
};

#endif
