#ifndef CAMERA_H
#define CAMERA_H

#include "Vector2D.h"
#include <SDL2/SDL.h>

class Camera {
public:
    Camera();

    void init(int levelWidth, int levelHeight);
    void update(const Vector2D& target, float deltaTime);

    // Getters
    Vector2D getPosition() const { return m_position; }
    SDL_Rect getViewport() const { return m_viewport; }
    float getZoom() const { return m_zoom; }

    // Setters
    void setPosition(const Vector2D& pos) { m_position = pos; }
    void setZoom(float zoom) { m_zoom = zoom; }
    void setLerpSpeed(float speed) { m_lerpSpeed = speed; }
    void setDeadzone(float x, float y) { m_deadzone = Vector2D(x, y); }

    // Convert world coordinates to screen coordinates
    Vector2D worldToScreen(const Vector2D& worldPos) const;
    Vector2D screenToWorld(const Vector2D& screenPos) const;

    // Shake effect
    void shake(float intensity, float duration);

private:
    Vector2D m_position;
    Vector2D m_target;
    Vector2D m_deadzone;
    SDL_Rect m_viewport;

    float m_zoom;
    float m_lerpSpeed;

    int m_levelWidth;
    int m_levelHeight;

    // Shake
    float m_shakeIntensity;
    float m_shakeDuration;
    float m_shakeTimer;
    Vector2D m_shakeOffset;
};

#endif // CAMERA_H
