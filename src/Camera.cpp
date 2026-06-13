#include "Camera.h"
#include "Constants.h"
#include <cstdlib>
#include <cmath>
#include <algorithm>

Camera::Camera()
    : m_zoom(1.0f), m_lerpSpeed(GameConstants::CAMERA_LERP_SPEED),
      m_levelWidth(0), m_levelHeight(0),
      m_shakeIntensity(0.0f), m_shakeDuration(0.0f), m_shakeTimer(0.0f)
{
    m_viewport = {0, 0, GameConstants::SCREEN_WIDTH, GameConstants::SCREEN_HEIGHT};
    m_deadzone = Vector2D(50.0f, 50.0f);
}

void Camera::init(int levelWidth, int levelHeight) {
    m_levelWidth = levelWidth;
    m_levelHeight = levelHeight;
}

void Camera::update(const Vector2D& target, float deltaTime) {
    // Target position (center the camera on target)
    float targetX = target.x - (m_viewport.w / (2.0f * m_zoom));
    float targetY = target.y - (m_viewport.h / (2.0f * m_zoom));

    // Smooth interpolation (lerp)
    float lerpFactor = 1.0f - std::exp(-m_lerpSpeed * deltaTime);
    m_position.x += (targetX - m_position.x) * lerpFactor;
    m_position.y += (targetY - m_position.y) * lerpFactor;

    // Clamp to level boundaries
    float maxX = static_cast<float>(m_levelWidth) - m_viewport.w / m_zoom;
    float maxY = static_cast<float>(m_levelHeight) - m_viewport.h / m_zoom;

    m_position.x = std::max(0.0f, std::min(m_position.x, maxX));
    m_position.y = std::max(0.0f, std::min(m_position.y, maxY));

    // Update shake
    if (m_shakeTimer > 0.0f) {
        m_shakeTimer -= deltaTime;
        float progress = m_shakeTimer / m_shakeDuration;
        float currentIntensity = m_shakeIntensity * progress;

        m_shakeOffset.x = (static_cast<float>(rand() % 200) / 100.0f - 1.0f) * currentIntensity;
        m_shakeOffset.y = (static_cast<float>(rand() % 200) / 100.0f - 1.0f) * currentIntensity;
    } else {
        m_shakeOffset = Vector2D::Zero();
    }

    // Update viewport
    m_viewport.x = static_cast<int>(m_position.x + m_shakeOffset.x);
    m_viewport.y = static_cast<int>(m_position.y + m_shakeOffset.y);
}

Vector2D Camera::worldToScreen(const Vector2D& worldPos) const {
    return Vector2D(
        (worldPos.x - m_position.x - m_shakeOffset.x) * m_zoom,
        (worldPos.y - m_position.y - m_shakeOffset.y) * m_zoom
    );
}

Vector2D Camera::screenToWorld(const Vector2D& screenPos) const {
    return Vector2D(
        screenPos.x / m_zoom + m_position.x + m_shakeOffset.x,
        screenPos.y / m_zoom + m_position.y + m_shakeOffset.y
    );
}

void Camera::shake(float intensity, float duration) {
    m_shakeIntensity = intensity;
    m_shakeDuration = duration;
    m_shakeTimer = duration;
}
