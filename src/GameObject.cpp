#include "GameObject.h"
#include "TextureManager.h"

GameObject::GameObject()
    : m_width(0), m_height(0), m_active(true), m_flip(SDL_FLIP_NONE) {}

GameObject::~GameObject() {
    clean();
}

void GameObject::init(const std::string& textureId, Vector2D position,
                       int width, int height) {
    m_textureId = textureId;
    m_position = position;
    m_width = width;
    m_height = height;
    m_velocity = Vector2D::Zero();
    m_acceleration = Vector2D::Zero();
    m_active = true;
}

void GameObject::update(float deltaTime) {
    if (!m_active) return;

    m_velocity += m_acceleration * deltaTime;
    m_position += m_velocity * deltaTime;

    m_animation.update(deltaTime);
}

void GameObject::render(SDL_Renderer* renderer, const Vector2D& cameraOffset) {
    if (!m_active) return;

    int drawX = static_cast<int>(m_position.x - cameraOffset.x);
    int drawY = static_cast<int>(m_position.y - cameraOffset.y);

    std::string texId = m_animation.getCurrentTextureId();
    if (texId.empty()) texId = m_textureId;

    TextureManager::getInstance().drawFrame(
        texId, drawX, drawY, m_width, m_height,
        m_animation.getCurrentRow(), m_animation.getCurrentFrame(),
        renderer, m_flip
    );
}

void GameObject::clean() {
    m_active = false;
}

SDL_Rect GameObject::getCollider() const {
    return {
        static_cast<int>(m_position.x),
        static_cast<int>(m_position.y),
        m_width,
        m_height
    };
}

bool GameObject::checkCollision(const GameObject& other) const {
    SDL_Rect a = getCollider();
    SDL_Rect b = other.getCollider();
    return SDL_HasIntersection(&a, &b);
}
