#include "Player.h"
#include "TextureManager.h"
#include "TileMap.h"
#include <algorithm>
#include <cmath>

Player::Player()
    : m_facing(GameConstants::DIR_DOWN), m_speed(GameConstants::PLAYER_SPEED),
      m_moving(false), m_energy(100.0f), m_maxEnergy(100.0f),
      m_hp(100.0f), m_maxHp(100.0f), m_invincibleTimer(0.0f),
      m_isAttacking(false), m_attackTimer(0.0f), m_comboState(0),
      m_isSprinting(false), m_isDodgeRolling(false), m_dodgeTimer(0.0f),
      m_dodgeDir(Vector2D::Zero()), m_isStaggered(false), m_staggerTimer(0.0f), m_speedBoostTimer(0.0f) {}

Player::~Player() {}

void Player::init(const std::string& textureId, Vector2D position,
                   int width, int height) {
    GameObject::init(textureId, position, width, height);
    m_animation.addAnimation("idle",          AnimationData("player_idle", 0, 10, 0.08f, true));
    m_animation.addAnimation("walk",          AnimationData("player_walk", 0, 10, 0.06f, true));
    m_animation.addAnimation("run",           AnimationData("player_run", 0, 10, 0.06f, true));
    m_animation.addAnimation("attack",        AnimationData("player_attack", 0, 3, 0.08f, false));
    m_animation.addAnimation("attack_finish", AnimationData("player_attack_finish", 0, 4, 0.06f, false));
    m_animation.addAnimation("jump",          AnimationData("player_jump", 0, 6, 0.06f, false));
    m_animation.addAnimation("fall",          AnimationData("player_fall", 0, 4, 0.10f, false));
    m_animation.setAnimation("idle");
}

void Player::update(float deltaTime) {
    if (!m_active) return;

    // 1. Invincibility Tick
    if (m_invincibleTimer > 0.0f) {
        m_invincibleTimer -= deltaTime;
    }
    if (m_speedBoostTimer > 0.0f) {
        m_speedBoostTimer -= deltaTime;
    }

    // 2. State Ticks
    if (m_isStaggered) {
        m_staggerTimer -= deltaTime;
        m_velocity = Vector2D::Zero(); // Freeze movement on stagger
        if (m_staggerTimer <= 0.0f) {
            m_isStaggered = false;
        }
    } else if (m_isDodgeRolling) {
        m_dodgeTimer -= deltaTime;
        m_velocity = m_dodgeDir * (GameConstants::PLAYER_SPEED * 2.2f);
        if (m_dodgeTimer <= 0.0f) {
            m_isDodgeRolling = false;
        }
    } else if (m_isAttacking) {
        m_attackTimer -= deltaTime;
        m_velocity = Vector2D::Zero(); // Freeze movement while attacking

        // Check for Combo 2 chain request during the tail end of first combo swing
        InputManager& input = InputManager::getInstance();
        if (m_comboState == 1 && m_attackTimer <= 0.12f) {
            if (input.isMouseButtonPressed(1) || input.isKeyPressed(SDL_SCANCODE_SPACE)) {
                m_comboState = 2;
                m_attackTimer = 0.24f; // 4 frames * 0.06s = 0.24 seconds
                m_animation.setAnimation("attack_finish");
            }
        }

        if (m_attackTimer <= 0.0f) {
            m_isAttacking = false;
            m_comboState = 0;
        }
    } else {
        handleInput(deltaTime);
    }

    // 3. Movement
    m_position += m_velocity * deltaTime;
    updateAnimation();
    m_animation.update(deltaTime);
}

void Player::handleInput(float deltaTime) {
    InputManager& input = InputManager::getInstance();
    m_velocity = Vector2D::Zero();
    m_moving = false;
    m_isSprinting = false;

    // A. Trigger Dodge Roll (Right Click - Mouse Button 3)
    if (input.isMouseButtonPressed(3)) {
        if (m_energy >= 15.0f) {
            useEnergy(15.0f);
            m_isDodgeRolling = true;
            m_dodgeTimer = 0.36f; // 6 frames * 0.06s = 0.36s
            m_invincibleTimer = 0.36f;

            // Get move direction
            Vector2D dDir;
            if (input.isKeyDown(SDL_SCANCODE_W) || input.isKeyDown(SDL_SCANCODE_UP))    dDir.y -= 1;
            if (input.isKeyDown(SDL_SCANCODE_S) || input.isKeyDown(SDL_SCANCODE_DOWN))  dDir.y += 1;
            if (input.isKeyDown(SDL_SCANCODE_A) || input.isKeyDown(SDL_SCANCODE_LEFT))  dDir.x -= 1;
            if (input.isKeyDown(SDL_SCANCODE_D) || input.isKeyDown(SDL_SCANCODE_RIGHT)) dDir.x += 1;

            if (dDir.magnitudeSquared() > 0.0f) {
                dDir.normalize();
            } else {
                // Dodge facing direction if standing
                if (m_facing == GameConstants::DIR_UP) dDir = Vector2D(0, -1);
                else if (m_facing == GameConstants::DIR_DOWN) dDir = Vector2D(0, 1);
                else if (m_facing == GameConstants::DIR_LEFT) dDir = Vector2D(-1, 0);
                else dDir = Vector2D(1, 0);
            }
            m_dodgeDir = dDir;
            m_animation.setAnimation("jump");
            return;
        }
    }

    // B. Trigger Attack Combo 1 (Left Mouse click or Spacebar)
    if (input.isMouseButtonPressed(1) || input.isKeyPressed(SDL_SCANCODE_SPACE)) {
        m_isAttacking = true;
        m_attackTimer = 0.24f; // 3 frames * 0.08s = 0.24s
        m_comboState = 1;
        m_moving = false;
        m_velocity = Vector2D::Zero();
        m_animation.setAnimation("attack");
        return;
    }

    // C. Regular movement or sprinting
    Vector2D dir;
    if (input.isKeyDown(SDL_SCANCODE_W) || input.isKeyDown(SDL_SCANCODE_UP))    { dir.y -= 1; m_facing = GameConstants::DIR_UP; }
    if (input.isKeyDown(SDL_SCANCODE_S) || input.isKeyDown(SDL_SCANCODE_DOWN))  { dir.y += 1; m_facing = GameConstants::DIR_DOWN; }
    if (input.isKeyDown(SDL_SCANCODE_A) || input.isKeyDown(SDL_SCANCODE_LEFT))  { dir.x -= 1; m_facing = GameConstants::DIR_LEFT;  m_flip = SDL_FLIP_HORIZONTAL; }
    if (input.isKeyDown(SDL_SCANCODE_D) || input.isKeyDown(SDL_SCANCODE_RIGHT)) { dir.x += 1; m_facing = GameConstants::DIR_RIGHT; m_flip = SDL_FLIP_NONE; }

    if (dir.magnitudeSquared() > 0) {
        dir.normalize();

        float activeSpeed = m_speed;
        if (m_speedBoostTimer > 0.0f) {
            activeSpeed *= 1.5f; // 50% speed boost
        }

        // Check Sprinting (Left Shift)
        if (input.isKeyDown(SDL_SCANCODE_LSHIFT) && m_energy > 0.0f) {
            m_isSprinting = true;
            useEnergy(15.0f * deltaTime); // drain 15 stamina/sec
            m_velocity = dir * (activeSpeed * 1.5f);
        } else {
            m_velocity = dir * activeSpeed;
        }
        m_moving = true;
    }

    // Hotbar / Inventory inputs
    int wheel = input.getMouseWheelDelta();
    if (wheel != 0) m_inventory.scrollHotbar(-wheel);

    for (int i = 0; i < GameConstants::HOTBAR_SLOTS; i++) {
        if (input.isKeyPressed(static_cast<SDL_Scancode>(SDL_SCANCODE_1 + i)))
            m_inventory.setSelectedSlot(i);
    }

    if (input.isKeyPressed(SDL_SCANCODE_F) || input.isKeyPressed(SDL_SCANCODE_Q)) {
        m_inventory.useSelected(*this);
    }

    if (input.isKeyPressed(SDL_SCANCODE_E) || input.isKeyPressed(SDL_SCANCODE_TAB))
        m_inventory.toggleOpen();
}

void Player::updateAnimation() {
    if (m_isStaggered) {
        m_animation.setAnimation("fall");
    } else if (m_isDodgeRolling) {
        m_animation.setAnimation("jump");
    } else if (m_isAttacking) {
        if (m_comboState == 2) {
            m_animation.setAnimation("attack_finish");
        } else {
            m_animation.setAnimation("attack");
        }
    } else if (m_moving) {
        if (m_isSprinting) {
            m_animation.setAnimation("run");
        } else {
            m_animation.setAnimation("walk");
        }
    } else {
        m_animation.setAnimation("idle");
    }
}

SDL_Rect Player::getAttackHitbox() const {
    int w = m_width;
    if (m_comboState == 2) {
        // Combo Finish: 1.5x wider sweep range!
        if (m_flip == SDL_FLIP_HORIZONTAL) {
            return { static_cast<int>(m_position.x - 76), static_cast<int>(m_position.y - 15), 86, 76 };
        } else {
            return { static_cast<int>(m_position.x + w - 10), static_cast<int>(m_position.y - 15), 86, 76 };
        }
    } else {
        if (m_flip == SDL_FLIP_HORIZONTAL) {
            return { static_cast<int>(m_position.x - 54), static_cast<int>(m_position.y - 10), 64, 64 };
        } else {
            return { static_cast<int>(m_position.x + w - 10), static_cast<int>(m_position.y - 10), 64, 64 };
        }
    }
}

SDL_Rect Player::getCollider() const {
    // Axonometric feet collider
    int feetH = 12;
    return {
        static_cast<int>(m_position.x + 4),
        static_cast<int>(m_position.y + m_height - feetH),
        m_width - 8,
        feetH
    };
}

Vector2D Player::getFacingTilePos() const {
    Vector2D center(m_position.x + m_width / 2.0f, m_position.y + m_height / 2.0f);
    int ts = GameConstants::TILE_SIZE;
    int tileX = (int)center.x / ts;
    int tileY = (int)center.y / ts;

    switch (m_facing) {
        case GameConstants::DIR_UP:    tileY--; break;
        case GameConstants::DIR_DOWN:  tileY++; break;
        case GameConstants::DIR_LEFT:  tileX--; break;
        case GameConstants::DIR_RIGHT: tileX++; break;
    }
    return Vector2D((float)tileX, (float)tileY);
}

bool Player::interact(TileMap& tileMap, std::string& dialogueOut) {
    Vector2D ft = getFacingTilePos();
    int tRow = (int)ft.y;
    int tCol = (int)ft.x;

    if (tileMap.isInBounds(tRow, tCol)) {
        return tileMap.interactWithObject(tRow, tCol, *this, dialogueOut);
    }
    return false;
}

void Player::render(SDL_Renderer* renderer, const Vector2D& cameraOffset) {
    if (!m_active) return;
    int drawX = (int)(m_position.x - cameraOffset.x);
    int drawY = (int)(m_position.y - cameraOffset.y);

    int w = m_width;
    int h = m_height;

    // 1. Beautiful slanted diagonal shadow casting bottom-left (isometric pop)
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 45);
    for (int i = 0; i < 6; i++) {
        SDL_Rect sh = {
            drawX - 4 - i * 3,
            drawY + h - 8 + i * 2,
            w - 4,
            8
        };
        SDL_RenderFillRect(renderer, &sh);
    }

    // 2. Damage Flash / Dodge Roll trail effect
    std::string currentTexId = m_animation.getCurrentTextureId();
    SDL_Texture* tex = TextureManager::getInstance().getTexture(currentTexId);
    bool flash = (m_invincibleTimer > 0.0f) && (static_cast<int>(m_invincibleTimer * 15.0f) % 2 == 0);
    if (tex) {
        if (flash) {
            SDL_SetTextureColorMod(tex, 255, 60, 60);
            SDL_SetTextureAlphaMod(tex, 255);
        } else if (m_isDodgeRolling) {
            // Semi-transparent Cyan-Blue dash color mod!
            SDL_SetTextureColorMod(tex, 100, 180, 255);
            SDL_SetTextureAlphaMod(tex, 180);
        } else {
            SDL_SetTextureColorMod(tex, 255, 255, 255);
            SDL_SetTextureAlphaMod(tex, 255);
        }
    }

    // 3. Render animated sprite (128x128 frame scaled to 166x166 centered on collider)
    int spriteW = 166;
    int spriteH = 166;
    int spriteX = drawX - (spriteW - w) / 2;
    int spriteY = drawY + h - 103;

    TextureManager::getInstance().drawFrameScaled(
        currentTexId,
        spriteX,
        spriteY,
        spriteW,
        spriteH,
        128, // srcWidth
        128, // srcHeight
        m_animation.getCurrentRow(),
        m_animation.getCurrentFrame(),
        renderer,
        m_flip
    );
}

void Player::useEnergy(float amount) {
    m_energy = std::max(0.0f, m_energy - amount);
}

void Player::restoreEnergy(float amount) {
    m_energy = std::min(m_maxEnergy, m_energy + amount);
}

void Player::fullRestore() {
    m_energy = m_maxEnergy;
}

void Player::takeDamage(float amount) {
    if (m_invincibleTimer > 0.0f) return;
    m_hp = std::max(0.0f, m_hp - amount);
    m_invincibleTimer = 1.0f; // 1 second of invincibility frame

    // Trigger heavy knockdown stagger when hit at low health (HP < 30.0f)
    if (m_hp > 0.0f && m_hp < 30.0f && !m_isStaggered) {
        m_isStaggered = true;
        m_staggerTimer = 0.40f; // 4 frames * 0.10s = 0.40s
        m_velocity = Vector2D::Zero();
        m_isAttacking = false;
        m_comboState = 0;
        m_isDodgeRolling = false;
        m_animation.setAnimation("fall");
    }
}

void Player::heal(float amount) {
    m_hp = std::min(m_maxHp, m_hp + amount);
}

void Player::applySpeedBoost(float duration) {
    m_speedBoostTimer = std::max(m_speedBoostTimer, duration);
}

void Player::resetHP() {
    m_hp = m_maxHp;
    m_invincibleTimer = 0.0f;
    m_speedBoostTimer = 0.0f;
}
