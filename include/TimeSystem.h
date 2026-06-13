#ifndef TIME_SYSTEM_H
#define TIME_SYSTEM_H

#include "Constants.h"
#include <SDL2/SDL.h>
#include <string>

class TimeSystem {
public:
    TimeSystem();

    void update(float deltaTime);
    void advanceDay();

    // Getters
    int getHour() const { return m_hour; }
    int getMinute() const { return m_minute; }
    int getDay() const { return m_day; }
    float getDayProgress() const; // 0.0 = 6AM, 1.0 = 2AM next day
    bool isNight() const;
    bool isDayTransition() const { return m_dayTransition; }
    void clearDayTransition() { m_dayTransition = false; }

    std::string getTimeString() const;
    std::string getDateString() const;

    // Overlay darkness rendering
    void renderDayNightOverlay(SDL_Renderer* renderer);

    void setPaused(bool paused) { m_paused = paused; }

private:
    int m_hour, m_minute;
    int m_day;
    float m_timer;
    bool m_paused;
    bool m_dayTransition;
};

#endif
