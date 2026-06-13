#ifndef ANIMATION_H
#define ANIMATION_H

#include <SDL2/SDL.h>
#include <string>
#include <unordered_map>

struct AnimationData {
    std::string textureId;
    int spriteRow;
    int frameCount;
    float frameTime;     // seconds per frame
    bool loop;

    AnimationData()
        : spriteRow(0), frameCount(1), frameTime(0.1f), loop(true) {}

    AnimationData(const std::string& texId, int row, int count, float time, bool doLoop = true)
        : textureId(texId), spriteRow(row), frameCount(count), frameTime(time), loop(doLoop) {}
};

class Animation {
public:
    Animation();

    void addAnimation(const std::string& name, const AnimationData& data);
    void setAnimation(const std::string& name);
    void update(float deltaTime);

    // Getters
    std::string getCurrentTextureId() const;
    int getCurrentRow() const;
    int getCurrentFrame() const;
    bool isFinished() const;
    const std::string& getCurrentAnimationName() const { return m_currentAnim; }

    void reset();

private:
    std::unordered_map<std::string, AnimationData> m_animations;
    std::string m_currentAnim;

    int   m_currentFrame;
    float m_timer;
    bool  m_finished;
};

#endif // ANIMATION_H
