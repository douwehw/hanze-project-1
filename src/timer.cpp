#include <timer.h>

Timer::Timer()
{
    _start_ticks = millis();
    _paused_ticks = 0;
    _paused = false;
    _started = false;
}

void Timer::start()
{
    _started = true;
    _paused = false;

    _start_ticks = millis();
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
        _paused_ticks = millis() - _start_ticks;
    }
}

void Timer::unpause()
{
    if (_paused)
    {
        _paused = false;
        _start_ticks = millis() - _paused_ticks;
        _paused_ticks = 0;
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
            return (static_cast<double>(_paused_ticks) / 1000.0);
        }
        else
        {
            return ((static_cast<double>(millis()) - static_cast<double>(_start_ticks)) / 1000.0);
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
			return _paused_ticks;
		}
        else
        {
            return (millis() - _start_ticks);
        }
	}

    return 0;
}
