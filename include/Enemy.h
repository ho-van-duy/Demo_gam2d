#ifndef ENEMY_H
#define ENEMY_H

#include "GameObject.h"
#include "Constants.h"

enum class EnemyState {
    CHASE,
    ATTACK,
    HURT,
    DEATH
};

class Enemy : public GameObject {
public:
    Enemy();
    ~Enemy() override;

    void init(const std::string& textureId, Vector2D position,
              int width, int height) override;
    void update(float deltaTime) override; // Keep override compatible
    void update(float deltaTime, const Vector2D& playerPos); // Custom chase update

    void render(SDL_Renderer* renderer, const Vector2D& cameraOffset) override;

    SDL_Rect getCollider() const;
    SDL_Rect getAttackHitbox() const;

    float getDamage() const { return m_damage; }
    void takeDamage(float amount);
    void applyKnockback(const Vector2D& force);
    bool isDead() const { return m_hp <= 0.0f; }
    bool isHurt() const { return m_state == EnemyState::HURT; }
    bool isDying() const { return m_state == EnemyState::DEATH; }

private:
    float m_speed;
    float m_hp;
    float m_maxHp;
    float m_damage;
    float m_bobTimer;
    float m_hurtTimer;
    Vector2D m_knockback;

    EnemyState m_state;
    float m_attackTimer;
    float m_attackCooldown;
    int   m_attackType;
    bool  m_hasDealtDamage;
    float m_deathTimer;
};

#endif // ENEMY_H
