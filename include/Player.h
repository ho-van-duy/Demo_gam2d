#ifndef PLAYER_H
#define PLAYER_H

#include "GameObject.h"
#include "InputManager.h"
#include "Inventory.h"
#include "Constants.h"

class TileMap;

class Player : public GameObject {
public:
    Player();
    ~Player() override;

    void init(const std::string& textureId, Vector2D position,
              int width, int height) override;
    void update(float deltaTime) override;
    void render(SDL_Renderer* renderer, const Vector2D& cameraOffset) override;

    // 3/4 collision adjustments
    SDL_Rect getCollider() const;

    // Direction & facing
    GameConstants::Direction getFacing() const { return m_facing; }
    Vector2D getFacingTilePos() const; // get tile in front of player

    // Interaction trigger
    bool interact(TileMap& tileMap, std::string& dialogueOut);

    // Inventory
    Inventory& getInventory() { return m_inventory; }

    // Stamina/Energy
    float getEnergy() const { return m_energy; }
    float getMaxEnergy() const { return m_maxEnergy; }
    void useEnergy(float amount);
    void restoreEnergy(float amount);
    void fullRestore();

    // Health / HP
    float getHP() const { return m_hp; }
    float getMaxHP() const { return m_maxHp; }
    void takeDamage(float amount);
    void heal(float amount);
    void applySpeedBoost(float duration);
    bool isDead() const { return m_hp <= 0.0f; }
    void resetHP();

    // Invincibility frames
    bool isInvincible() const { return m_invincibleTimer > 0.0f; }

    // Attack state
    bool isAttacking() const { return m_isAttacking; }
    int getComboState() const { return m_comboState; }
    SDL_Rect getAttackHitbox() const;

    // Movement & Stagger states
    bool isSprinting() const { return m_isSprinting; }
    bool isDodgeRolling() const { return m_isDodgeRolling; }
    bool isStaggered() const { return m_isStaggered; }

private:
    void handleInput(float deltaTime);
    void updateAnimation();

    GameConstants::Direction m_facing;
    float m_speed;
    bool  m_moving;

    float m_energy;
    float m_maxEnergy;

    float m_hp;
    float m_maxHp;
    float m_invincibleTimer;

    bool  m_isAttacking;
    float m_attackTimer;
    int   m_comboState;

    bool     m_isSprinting;
    bool     m_isDodgeRolling;
    float    m_dodgeTimer;
    Vector2D m_dodgeDir;

    bool     m_isStaggered;
    float    m_staggerTimer;

    float    m_speedBoostTimer;

    Inventory m_inventory;
};

#endif
