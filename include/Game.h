#ifndef GAME_H
#define GAME_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <string>
#include <vector>
#include <memory>

#include "Timer.h"
#include "Camera.h"
#include "Player.h"
#include "TileMap.h"
#include "TimeSystem.h"
#include "GameObject.h"
#include "Enemy.h"

enum class GameState { MENU, PLAYING, PAUSED, GAME_OVER };

struct YDrawable {
    enum Type { TYPE_PLAYER, TYPE_WORLD_OBJ, TYPE_ENEMY };
    Type type;
    float depthY;
    int index; // array index of WorldObject or Enemy if applicable

    bool operator<(const YDrawable& other) const {
        return depthY < other.depthY;
    }
};

struct EnergyGem {
    Vector2D position;
    Vector2D velocity;
    bool active;
    float energyValue;
    float bobTimer;
    GameConstants::ItemType itemType;
};

class Game {
public:
    static Game& getInstance();

    bool init(const std::string& title, int width, int height, bool fullscreen = false);
    void run();
    void clean();

    void handleEvents();
    void update(float deltaTime);
    void render();

    // Set Game State
    void setState(GameState state) { m_state = state; }
    GameState getState() const { return m_state; }

    // Getters
    SDL_Renderer* getRenderer() const { return m_renderer; }
    bool isRunning() const { return m_running; }
    Camera& getCamera() { return m_camera; }
    TimeSystem& getTime() { return m_timeSys; }
    Player* getPlayer() const { return m_player.get(); }

    void setDebugMode(bool d) { m_debugMode = d; }
    bool isDebugMode() const { return m_debugMode; }

    void triggerDialogue(const std::string& text);
    void resetGame();

private:
    Game();
    ~Game();
    Game(const Game&) = delete;
    Game& operator=(const Game&) = delete;

    void renderMenu();
    void renderHUD();
    void renderDialogueBox();
    void renderPauseOverlay();
    void renderGameOver();
    void renderDebugInfo();
    void checkCollisions();
    void spawnEnemy();
    void spawnEnergyGem(const Vector2D& pos, GameConstants::ItemType type = GameConstants::ITEM_NONE);
    void updateEnergyGems(float deltaTime);
    void renderEnergyGems(const Vector2D& cameraOffset);

    // Render helper for WorldObjects
    void drawWorldObject(SDL_Renderer* renderer, const WorldObject& obj, const Vector2D& camOff);

    SDL_Window*   m_window;
    SDL_Renderer* m_renderer;
    bool      m_running;
    bool      m_debugMode;
    GameState m_state;

    Timer      m_timer;
    Camera     m_camera;
    TimeSystem m_timeSys;

    std::unique_ptr<Player> m_player;
    std::vector<std::unique_ptr<Enemy>> m_enemies;
    float m_enemySpawnTimer;

    std::vector<EnergyGem> m_energyGems;
    std::vector<Enemy*> m_enemiesHitThisSwing;
    int m_lastComboState;

    TileMap m_tileMap;

    // Exploration Sandbox dialogue box state
    std::string m_dialogueText;
    float m_dialogueTimer;
};

#endif
