#include <timer.h>

Timer::Timer()
{
    _startTicks = millis();
    _pausedTicks = 0;
    _paused = false;
    _started = false;
}

void Timer::start()
{
    _started = true;
    _paused = false;

    _startTicks = millis();
}

void Timer::stop()
{
    _started = false;
    _paused = false;
}

void Timer::pause()
{
    if (_started && !_paused)
    {
        _paused = true;
        _pausedTicks = millis() - _startTicks;
    }
}

void Timer::unpause()
{
    if (_paused)
    {
        _paused = false;
        _startTicks = millis() - _pausedTicks;
        _pausedTicks = 0;
    }
}

void Timer::paused(bool b)
{
    if (b)
    {
        pause();
    }
    else
    {
        unpause();
    }
}

double Timer::seconds()
{
    if (_started)
    {
        if (_paused)
        {
            return (static_cast<double>(_pausedTicks) / 1000.0);
        }
        else
        {
            return ((static_cast<double>(millis()) - static_cast<double>(_startTicks)) / 1000.0);
        }
    }

    return 0;
}

long Timer::milliseconds()
{
	if (_started)
	{
		if (_paused)
		{
			return _pausedTicks;
		}
        else
        {
            return (millis() - _startTicks);
        }
	}

    return 0;
}
