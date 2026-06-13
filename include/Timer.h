#ifndef TIMER_H
#define TIMER_H

#include <SDL2/SDL.h>

class Timer {
public:
    Timer();

    void tick();
    void reset();

    float getDeltaTime() const { return m_deltaTime; }
    float getTotalTime() const { return m_totalTime; }
    int   getFPS() const { return m_fps; }

    // Frame rate limiting
    void capFrameRate(float targetDelay);

private:
    Uint64 m_lastTick;
    Uint64 m_frequency;
    float  m_deltaTime;
    float  m_totalTime;

    // FPS calculation
    int    m_frameCount;
    float  m_fpsTimer;
    int    m_fps;
};

#endif // TIMER_H
