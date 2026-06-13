#include "TimeSystem.h"
#include "Constants.h"
#include <sstream>
#include <iomanip>
#include <cmath>
#include <algorithm>

TimeSystem::TimeSystem()
    : m_hour(GameConstants::DAY_START_HOUR), m_minute(0),
      m_day(1), m_timer(0), m_paused(false), m_dayTransition(false) {}

void TimeSystem::update(float deltaTime) {
    if (m_paused) return;

    m_timer += deltaTime;
    if (m_timer >= GameConstants::SECONDS_PER_GAME_MINUTE) {
        m_timer -= GameConstants::SECONDS_PER_GAME_MINUTE;
        m_minute += 10;
        if (m_minute >= 60) {
            m_minute = 0;
            m_hour++;
            if (m_hour >= 24) {
                m_hour = 0;
            }
            if (m_hour == GameConstants::DAY_START_HOUR && m_minute == 0) {
                advanceDay();
            }
        }
    }
}

void TimeSystem::advanceDay() {
    m_day++;
    m_dayTransition = true;
}

float TimeSystem::getDayProgress() const {
    // Progress of the day starting from 6 AM
    int hourFrom6 = m_hour - 6;
    if (hourFrom6 < 0) hourFrom6 += 24;
    float totalMinutes = hourFrom6 * 60.0f + m_minute;
    return totalMinutes / (24.0f * 60.0f);
}

bool TimeSystem::isNight() const {
    return m_hour >= GameConstants::NIGHT_START_HOUR || m_hour < GameConstants::DAY_START_HOUR;
}

std::string TimeSystem::getTimeString() const {
    std::ostringstream ss;
    int displayHour = m_hour % 24;
    bool pm = displayHour >= 12;
    if (displayHour > 12) displayHour -= 12;
    if (displayHour == 0) displayHour = 12;
    ss << displayHour << ":" << std::setw(2) << std::setfill('0') << m_minute
       << (pm ? " PM" : " AM");
    return ss.str();
}

std::string TimeSystem::getDateString() const {
    std::ostringstream ss;
    ss << "Day " << m_day;
    return ss.str();
}

void TimeSystem::renderDayNightOverlay(SDL_Renderer* renderer) {
    float progress = getDayProgress();
    Uint8 alpha = 0;

    // Smooth morning dawn fade (6AM - 8AM)
    if (progress < 0.083f) {
        alpha = (Uint8)(140 * (1.0f - progress / 0.083f));
    }
    // Smooth dusk transition (6PM - 8PM / 18:00 - 20:00)
    else if (m_hour >= 18 && m_hour < 20) {
        float minProg = (float)(m_minute + (m_hour - 18) * 60) / 120.0f;
        alpha = (Uint8)(140 * minProg);
    }
    // Full Night time tint (8PM - 6AM next day)
    else if (m_hour >= 20 || m_hour < 6) {
        alpha = 140;
    }

    if (alpha > 0) {
        // Deep blue atmospheric night shader
        SDL_SetRenderDrawColor(renderer, 15, 10, 45, alpha);
        SDL_Rect screen = {0, 0, GameConstants::SCREEN_WIDTH, GameConstants::SCREEN_HEIGHT};
        SDL_RenderFillRect(renderer, &screen);
    }
}
