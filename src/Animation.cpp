#include "Animation.h"

Animation::Animation() : m_currentFrame(0), m_timer(0.0f), m_finished(false) {}

void Animation::addAnimation(const std::string &name,
                             const AnimationData &data) {
  m_animations[name] = data;
  if (m_currentAnim.empty()) {
    m_currentAnim = name;
  }
}

void Animation::setAnimation(const std::string &name) {
  if (m_currentAnim == name)
    return;

  auto it = m_animations.find(name);
  if (it == m_animations.end())
    return;

  m_currentAnim = name;
  m_currentFrame = 0;
  m_timer = 0.0f;
  m_finished = false;
}

void Animation::update(float deltaTime) {
  if (m_currentAnim.empty())
    return;

  auto it = m_animations.find(m_currentAnim);
  if (it == m_animations.end())
    return;

  const AnimationData &data = it->second;

  if (m_finished && !data.loop)
    return;

  m_timer += deltaTime;
  if (m_timer >= data.frameTime) {
    m_timer -= data.frameTime;
    m_currentFrame++;

    if (m_currentFrame >= data.frameCount) {
      if (data.loop) {
        m_currentFrame = 0;
      } else {
        m_currentFrame = data.frameCount - 1;
        m_finished = true;
      }
    }
  }
}

std::string Animation::getCurrentTextureId() const {
  auto it = m_animations.find(m_currentAnim);
  if (it != m_animations.end()) {
    return it->second.textureId;
  }
  return "";
}

int Animation::getCurrentRow() const {
  auto it = m_animations.find(m_currentAnim);
  if (it != m_animations.end()) {
    return it->second.spriteRow;
  }
  return 0;
}

int Animation::getCurrentFrame() const { return m_currentFrame; }

bool Animation::isFinished() const { return m_finished; }

void Animation::reset() {
  m_currentFrame = 0;
  m_timer = 0.0f;
  m_finished = false;
}
