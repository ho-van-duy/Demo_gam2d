#include "Timer.h"

Timer::Timer()
    : m_lastTick(0), m_frequency(0), m_deltaTime(0.0f), m_totalTime(0.0f),
      m_frameCount(0), m_fpsTimer(0.0f), m_fps(0)
{
    m_frequency = SDL_GetPerformanceFrequency();
    m_lastTick = SDL_GetPerformanceCounter();
}

void Timer::tick() {
    Uint64 currentTick = SDL_GetPerformanceCounter();
    m_deltaTime = static_cast<float>(currentTick - m_lastTick) / static_cast<float>(m_frequency);
    m_lastTick = currentTick;

    // Clamp delta time to prevent spiral of death
    if (m_deltaTime > 0.05f) {
        m_deltaTime = 0.05f;
    }

    m_totalTime += m_deltaTime;

    // FPS calculation
    m_frameCount++;
    m_fpsTimer += m_deltaTime;
    if (m_fpsTimer >= 1.0f) {
        m_fps = m_frameCount;
        m_frameCount = 0;
        m_fpsTimer -= 1.0f;
    }
}

void Timer::reset() {
    m_lastTick = SDL_GetPerformanceCounter();
    m_deltaTime = 0.0f;
    m_totalTime = 0.0f;
    m_frameCount = 0;
    m_fpsTimer = 0.0f;
    m_fps = 0;
}

void Timer::capFrameRate(float targetDelay) {
    Uint64 currentTick = SDL_GetPerformanceCounter();
    float elapsed = static_cast<float>(currentTick - m_lastTick) / static_cast<float>(m_frequency) * 1000.0f;
    if (elapsed < targetDelay) {
        SDL_Delay(static_cast<Uint32>(targetDelay - elapsed));
    }
}
