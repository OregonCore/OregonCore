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

#ifndef __WEATHER_H
#define __WEATHER_H

#include "Common.h"
#include "SharedDefines.h"
#include "Timer.h"

class Player;

enum WeatherState
{
    WEATHER_STATE_FINE              = 0,
    WEATHER_STATE_LIGHT_RAIN        = 3,
    WEATHER_STATE_MEDIUM_RAIN       = 4,
    WEATHER_STATE_HEAVY_RAIN        = 5,
    WEATHER_STATE_LIGHT_SNOW        = 6,
    WEATHER_STATE_MEDIUM_SNOW       = 7,
    WEATHER_STATE_HEAVY_SNOW        = 8,
    WEATHER_STATE_LIGHT_SANDSTORM   = 22,
    WEATHER_STATE_MEDIUM_SANDSTORM  = 41,
    WEATHER_STATE_HEAVY_SANDSTORM   = 42,
    WEATHER_STATE_THUNDERS          = 86,
    WEATHER_STATE_BLACKRAIN         = 90
};

struct WeatherZoneChances;

// Weather for one zone
class Weather
{
    public:
        Weather(uint32 zone, WeatherZoneChances const* weatherChances);
        ~Weather() { };
        bool ReGenerate();
        bool UpdateWeather();
        void SendWeatherUpdateToPlayer(Player* player);
        static void SendFineWeatherUpdateToPlayer(Player* player);
        void SetWeather(WeatherType type, float grade);
        // For which zone is this weather?
        uint32 GetZone()
        {
            return m_zone;
        };
        bool Update(time_t diff);
    private:
        WeatherState GetWeatherState() const;
        uint32 m_zone;
        WeatherType m_type;
        float m_grade;
        IntervalTimer m_timer;
        WeatherZoneChances const* m_weatherChances;
};
#endif

