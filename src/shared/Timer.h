/*
 * This file is part of the OregonCore Project. See AUTHORS file for Copyright information
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef OREGON_TIMER_H
#define OREGON_TIMER_H

#include "Platform/CompilerDefs.h"
#include "Common.h"

#if PLATFORM == PLATFORM_WINDOWS
#   include <mmsystem.h>
#else
#   include <sys/time.h>
#   include <sys/timeb.h>
#endif

#if PLATFORM == PLATFORM_WINDOWS
inline uint32 getMSTime()
{
    return GetTickCount();
}

inline uint64 getMSTime64()
{
    #if _WIN32_WINNT >= 0x0600 // Vista and higher
    return GetTickCount64();
    #else // Backwards compatibility for XP and lower
    static uint32 previous = 0;
    static uint64 total = 0;
    uint32 now = GetTickCount();
    total += (previous > now) ? (0xFFFFFFFF - previous) + now : (now - previous);
    previous = now;
    return total;
    #endif
}
#else
inline uint32 getMSTime()
{
    #if defined(_POSIX_C_SOURCE) && _POSIX_C_SOURCE >= 199309L
    struct timespec tp;
    clock_gettime(CLOCK_MONOTONIC, &tp);
    return (tp.tv_sec * 1000) + (tp.tv_nsec / 1000 / 1000);
    #else // Backwards compatibility
    struct timeval tv;
    gettimeofday( &tv, NULL );
    return (tv.tv_sec * 1000) + (tv.tv_usec / 1000);
    #endif
}

inline uint64 getMSTime64()
{
    #if defined(_POSIX_C_SOURCE) && _POSIX_C_SOURCE >= 199309L
    struct timespec tp;
    clock_gettime(CLOCK_MONOTONIC, &tp);
    return (tp.tv_sec * 1000) + (tp.tv_nsec / 1000 / 1000);
    #else // Backwards compatibility
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (tv.tv_sec * 1000) + (tv.tv_usec / 1000);
    #endif
}
#endif

inline uint32 getMSTimeDiff(uint32 oldMSTime, uint32 newMSTime)
{
    // getMSTime() have limited data range and this is case when it overflow in this tick
    if (oldMSTime > newMSTime)
        return (0xFFFFFFFF - oldMSTime) + newMSTime;
    else
        return newMSTime - oldMSTime;
}

inline uint64 getMSTimeDiff64(uint64 t1, uint64 t2)
{
    if (t1 > t2)
        return t1 - t2;
    return t2 - t1;
}

inline uint32 GetMSTimeDiffToNow(uint32 oldMSTime)
{
    return getMSTimeDiff(oldMSTime, getMSTime());
}

class IntervalTimer
{
    public:
        IntervalTimer() : _interval(0), _current(0) {}

        void Update(time_t diff)
        {
            _current += diff;
            if (_current < 0) _current = 0;
        }
        bool Passed()
        {
            return _current >= _interval;
        }
        void Reset()
        {
            if (_current >= _interval) _current -= _interval;
        }

        void SetCurrent(time_t current)
        {
            _current = current;
        }
        void SetInterval(time_t interval)
        {
            _interval = interval;
        }
        time_t GetInterval() const
        {
            return _interval;
        }
        time_t GetCurrent() const
        {
            return _current;
        }

    private:

        time_t _interval;
        time_t _current;
};

struct TimeTracker
{
    TimeTracker(time_t expiry) : i_expiryTime(expiry) {}
    void Update(time_t diff)
    {
        i_expiryTime -= diff;
    }
    bool Passed(void) const
    {
        return (i_expiryTime <= 0);
    }
    void Reset(time_t interval)
    {
        i_expiryTime = interval;
    }
    time_t GetExpiry(void) const
    {
        return i_expiryTime;
    }
    time_t i_expiryTime;
};

struct TimeTrackerSmall
{
    TimeTrackerSmall(int32 expiry) : i_expiryTime(expiry) {}
    void Update(int32 diff)
    {
        i_expiryTime -= diff;
    }
    bool Passed(void) const
    {
        return (i_expiryTime <= 0);
    }
    void Reset(int32 interval)
    {
        i_expiryTime = interval;
    }
    int32 GetExpiry(void) const
    {
        return i_expiryTime;
    }
    int32 i_expiryTime;
};

struct PeriodicTimer
{
    public:

        PeriodicTimer(int32 period, int32 start_time)
            : i_expireTime(start_time), i_period(period)
        {
        }

        bool Update(const uint32& diff)
        {
            if ((i_expireTime -= diff) > 0)
                return false;

            i_expireTime += i_period > int32(diff) ? i_period : diff;
            return true;
        }

        void SetPeriodic(int32 period, int32 start_time)
        {
            i_expireTime = start_time;
            i_period = period;
        }

        // Tracker interface
        void TUpdate(int32 diff)
        {
            i_expireTime -= diff;
        }
        bool TPassed() const
        {
            return i_expireTime <= 0;
        }
        void TReset(int32 diff, int32 period)
        {
            i_expireTime += period > diff ? period : diff;
        }

    private:

        int32 i_expireTime;
        int32 i_period;
};

#endif

