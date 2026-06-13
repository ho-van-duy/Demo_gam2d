#ifndef GAME_OBJECT_H
#define GAME_OBJECT_H

#include "Vector2D.h"
#include "Animation.h"
#include <SDL2/SDL.h>
#include <string>

class GameObject {
public:
    GameObject();
    virtual ~GameObject();

    virtual void init(const std::string& textureId, Vector2D position,
                      int width, int height);
    virtual void update(float deltaTime);
    virtual void render(SDL_Renderer* renderer, const Vector2D& cameraOffset);
    virtual void clean();

    // Collision
    SDL_Rect getCollider() const;
    bool checkCollision(const GameObject& other) const;

    // Getters
    Vector2D getPosition() const { return m_position; }
    Vector2D getVelocity() const { return m_velocity; }
    int getWidth() const { return m_width; }
    int getHeight() const { return m_height; }
    bool isActive() const { return m_active; }
    const std::string& getTextureId() const { return m_textureId; }

    // Setters
    void setPosition(const Vector2D& pos) { m_position = pos; }
    void setVelocity(const Vector2D& vel) { m_velocity = vel; }
    void setActive(bool active) { m_active = active; }
    void setFlip(SDL_RendererFlip flip) { m_flip = flip; }

protected:
    std::string m_textureId;
    Vector2D m_position;
    Vector2D m_velocity;
    Vector2D m_acceleration;

    int m_width;
    int m_height;

    bool m_active;
    SDL_RendererFlip m_flip;
    Animation m_animation;
};

#endif // GAME_OBJECT_H
