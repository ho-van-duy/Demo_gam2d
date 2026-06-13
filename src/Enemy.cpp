#include "Enemy.h"
#include "TextureManager.h"
#include "Game.h"
#include <cmath>
#include <algorithm>

Enemy::Enemy()
    : m_speed(GameConstants::ENEMY_SPEED),
      m_hp(30.0f),
      m_maxHp(30.0f),
      m_damage(GameConstants::ENEMY_DAMAGE),
      m_bobTimer((float)(rand() % 100) * 0.1f),
      m_hurtTimer(0.0f),
      m_knockback(Vector2D::Zero()),
      m_state(EnemyState::CHASE),
      m_attackTimer(0.0f),
      m_attackCooldown(0.5f + (rand() % 50) * 0.01f), // Small initial warm-up cooldown
      m_attackType(1),
      m_hasDealtDamage(false),
      m_deathTimer(0.0f) {
    m_active = true;
}

Enemy::~Enemy() {}

void Enemy::init(const std::string& textureId, Vector2D position,
                 int width, int height) {
    GameObject::init(textureId, position, width, height);
    m_animation.addAnimation("idle",    AnimationData("enemy_idle", 0, 6, 0.10f, true));
    m_animation.addAnimation("walk",    AnimationData("enemy_walk", 0, 8, 0.08f, true));
    m_animation.addAnimation("attack1", AnimationData("enemy_attack1", 0, 6, 0.06f, false));
    m_animation.addAnimation("attack2", AnimationData("enemy_attack2", 0, 6, 0.08f, false));
    m_animation.addAnimation("hurt",    AnimationData("enemy_hurt", 0, 4, 0.06f, false));
    m_animation.addAnimation("death",   AnimationData("enemy_death", 0, 4, 0.10f, false));
    m_animation.setAnimation("idle");
}

void Enemy::update(float deltaTime) {
    // Standard compatibility update
    m_position += (m_velocity + m_knockback) * deltaTime;
    m_knockback -= m_knockback * 10.0f * deltaTime;
}

void Enemy::update(float deltaTime, const Vector2D& playerPos) {
    if (!m_active) return;

    m_bobTimer += deltaTime;
    if (m_attackCooldown > 0.0f) {
        m_attackCooldown -= deltaTime;
    }

    // STATE MACHINE
    if (m_state == EnemyState::DEATH) {
        m_velocity = Vector2D::Zero();
        m_deathTimer -= deltaTime;
        if (m_deathTimer <= 0.0f) {
            m_active = false; // officially dead and cleaned up by Game
        }
    } else if (m_state == EnemyState::HURT) {
        m_velocity = Vector2D::Zero();
        m_hurtTimer -= deltaTime;
        if (m_hurtTimer <= 0.0f) {
            m_state = EnemyState::CHASE;
        }
    } else if (m_state == EnemyState::ATTACK) {
        m_velocity = Vector2D::Zero();
        m_attackTimer -= deltaTime;

        // Damage calculation on designated swing impact frame
        bool isDamageFrame = false;
        if (m_attackType == 1) {
            // attack1: 6 frames, hits at frame 3 or 4 (timer in [0.12, 0.24])
            isDamageFrame = (m_attackTimer >= 0.12f && m_attackTimer <= 0.24f);
        } else {
            // attack2: 6 frames, hits at frame 4 or 5 (timer in [0.08, 0.24])
            isDamageFrame = (m_attackTimer >= 0.08f && m_attackTimer <= 0.24f);
        }

        if (isDamageFrame && !m_hasDealtDamage) {
            Player* player = Game::getInstance().getPlayer();
            if (player && player->isActive() && !player->isInvincible()) {
                SDL_Rect aBox = getAttackHitbox();
                SDL_Rect pBox = player->getCollider();
                if (SDL_HasIntersection(&aBox, &pBox)) {
                    float dmg = (m_attackType == 1) ? 10.0f : 20.0f;
                    player->takeDamage(dmg);
                    m_hasDealtDamage = true;

                    if (m_attackType == 2) {
                        Game::getInstance().getCamera().shake(6.0f, 0.15f); // Screen shake on heavy axe slam!
                    }
                }
            }
        }

        if (m_attackTimer <= 0.0f) {
            m_state = EnemyState::CHASE;
            m_attackCooldown = 1.0f + (rand() % 100) * 0.01f; // Cooldown 1.0 - 2.0s
        }
    } else { // CHASE STATE
        // Direct path tracking to player's center
        Vector2D targetPos = playerPos;
        targetPos.x += GameConstants::PLAYER_WIDTH / 2.0f - m_width / 2.0f;
        targetPos.y += GameConstants::PLAYER_HEIGHT / 2.0f - m_height / 2.0f;

        Vector2D direction = targetPos - m_position;
        float distance = direction.magnitude();

        // 1. Trigger Attack check
        if (distance < 45.0f && m_attackCooldown <= 0.0f) {
            m_state = EnemyState::ATTACK;
            m_attackType = (rand() % 2) + 1; // Randomly choose Attack 1 or 2
            m_hasDealtDamage = false;

            if (m_attackType == 1) {
                m_attackTimer = 0.36f; // 6 frames * 0.06s = 0.36s
                m_animation.setAnimation("attack1");
            } else {
                m_attackTimer = 0.48f; // 6 frames * 0.08s = 0.48s
                m_animation.setAnimation("attack2");
            }
            m_velocity = Vector2D::Zero();
        } else {
            // 2. Standard Chase Movement
            if (distance > 1.0f) {
                direction.normalize();
                m_velocity = direction * m_speed;
            } else {
                m_velocity = Vector2D::Zero();
            }

            // Face player horizontally
            if (playerPos.x < m_position.x) {
                m_flip = SDL_FLIP_HORIZONTAL;
            } else {
                m_flip = SDL_FLIP_NONE;
            }

            // Set animations
            if (m_velocity.magnitudeSquared() > 0.0f) {
                m_animation.setAnimation("walk");
            } else {
                m_animation.setAnimation("idle");
            }
        }
    }

    // Apply movement & Decaying Knockback
    m_position += (m_velocity + m_knockback) * deltaTime;
    m_knockback -= m_knockback * 10.0f * deltaTime;
    if (m_knockback.magnitudeSquared() < 1.0f) {
        m_knockback = Vector2D::Zero();
    }

    m_animation.update(deltaTime);
}

void Enemy::takeDamage(float amount) {
    if (m_state == EnemyState::DEATH) return; // Already dying!

    m_hp = std::max(0.0f, m_hp - amount);

    if (m_hp <= 0.0f) {
        m_state = EnemyState::DEATH;
        m_deathTimer = 0.40f; // 4 frames * 0.10s = 0.40s
        m_velocity = Vector2D::Zero();
        m_animation.setAnimation("death");
    } else {
        m_state = EnemyState::HURT;
        m_hurtTimer = 0.24f; // 4 frames * 0.06s = 0.24s
        m_velocity = Vector2D::Zero();
        m_animation.setAnimation("hurt");
    }
}

void Enemy::applyKnockback(const Vector2D& force) {
    m_knockback = force;
}

SDL_Rect Enemy::getCollider() const {
    int feetH = 12;
    return {
        static_cast<int>(m_position.x + 4),
        static_cast<int>(m_position.y + m_height - feetH),
        m_width - 8,
        feetH
    };
}

SDL_Rect Enemy::getAttackHitbox() const {
    int w = m_width;
    // Sweep box in front of the Orc
    if (m_flip == SDL_FLIP_HORIZONTAL) {
        return { static_cast<int>(m_position.x - 45), static_cast<int>(m_position.y), 50, m_height };
    } else {
        return { static_cast<int>(m_position.x + w - 5), static_cast<int>(m_position.y), 50, m_height };
    }
}

void Enemy::render(SDL_Renderer* renderer, const Vector2D& cameraOffset) {
    if (!m_active) return;

    int drawX = (int)(m_position.x - cameraOffset.x);
    int drawY = (int)(m_position.y - cameraOffset.y);

    int w = m_width;
    int h = m_height;

    // Organic bobbing only when NOT attacking or dying
    float bob = 0.0f;
    if (m_state != EnemyState::ATTACK && m_state != EnemyState::DEATH) {
        bob = std::sin(m_bobTimer * 12.0f) * 4.0f;
    }
    int bobY = static_cast<int>(bob);

    // 1. Shadow casting (diminishes when dead)
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 45);
    int shadowShrink = std::max(0, 3 + bobY / 2);
    if (m_state == EnemyState::DEATH) {
        shadowShrink += static_cast<int>((1.0f - (m_deathTimer / 0.40f)) * 15.0f);
    }
    for (int i = 0; i < 6; i++) {
        SDL_Rect sh = {
            drawX - 4 - i * 3 + shadowShrink/2,
            drawY + h - 8 + i * 2,
            w - 4 - shadowShrink,
            8
        };
        SDL_RenderFillRect(renderer, &sh);
    }

    // 2. Scaling parameters (Orc is drawn at 250x250)
    int spriteW = 250;
    int spriteH = 250;
    int spriteX = drawX - (spriteW - w) / 2;
    int spriteY = drawY + h - 148 + bobY;

    std::string currentTexId = m_animation.getCurrentTextureId();
    SDL_Texture* tex = TextureManager::getInstance().getTexture(currentTexId);
    if (tex) {
        // Apply alpha dissolve / fade-out when dead
        if (m_state == EnemyState::DEATH) {
            SDL_SetTextureAlphaMod(tex, static_cast<Uint8>((m_deathTimer / 0.40f) * 255.0f));
            SDL_SetTextureColorMod(tex, 255, 255, 255);
        } else if (m_state == EnemyState::HURT) {
            SDL_SetTextureColorMod(tex, 255, 100, 100); // Tint red-hurt
            SDL_SetTextureAlphaMod(tex, 255);
        } else {
            SDL_SetTextureColorMod(tex, 255, 255, 255); // Normal
            SDL_SetTextureAlphaMod(tex, 255);
        }
    }

    TextureManager::getInstance().drawFrameScaled(
        currentTexId,
        spriteX,
        spriteY,
        spriteW,
        spriteH,
        100, // srcWidth
        100, // srcHeight
        m_animation.getCurrentRow(),
        m_animation.getCurrentFrame(),
        renderer,
        m_flip
    );
}
