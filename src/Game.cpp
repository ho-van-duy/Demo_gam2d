#include "Game.h"
#include "TextureManager.h"
#include "InputManager.h"
#include "Constants.h"
#include <iostream>
#include <algorithm>
#include <cmath>

Game& Game::getInstance() { static Game inst; return inst; }
Game::Game() : m_window(nullptr), m_renderer(nullptr), m_running(false),
    m_debugMode(false), m_state(GameState::MENU), m_dialogueTimer(0), m_enemySpawnTimer(0.0f), m_lastComboState(0) {}
Game::~Game() { clean(); }

bool Game::init(const std::string& title, int width, int height, bool fullscreen) {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) < 0) return false;
    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) return false;

    Uint32 flags = SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE;
    if (fullscreen) flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
    m_window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED, width, height, flags);
    if (!m_window) return false;
    m_renderer = SDL_CreateRenderer(m_window, -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!m_renderer) return false;
    SDL_SetRenderDrawBlendMode(m_renderer, SDL_BLENDMODE_BLEND);

    // Solve fullscreen stretching and scaling bugs using logical size scaling
    SDL_RenderSetLogicalSize(m_renderer, GameConstants::SCREEN_WIDTH, GameConstants::SCREEN_HEIGHT);

    // Load asset textures for sprite animations
    auto& texMgr = TextureManager::getInstance();
    texMgr.loadTexture("player_idle", "assets/textures/player_idle.png", m_renderer);
    texMgr.loadTexture("player_walk", "assets/textures/player_walk.png", m_renderer);
    texMgr.loadTexture("player_run",  "assets/textures/player_run.png",  m_renderer);
    texMgr.loadTexture("player_attack", "assets/textures/player_attack.png", m_renderer);
    texMgr.loadTexture("player_attack_finish", "assets/textures/player_attack_finish.png", m_renderer);
    texMgr.loadTexture("player_jump", "assets/textures/player_jump.png", m_renderer);
    texMgr.loadTexture("player_fall", "assets/textures/player_fall.png", m_renderer);
    texMgr.loadTexture("enemy_idle",  "assets/textures/enemy_idle.png",  m_renderer);
    texMgr.loadTexture("enemy_walk",  "assets/textures/enemy_walk.png",  m_renderer);
    texMgr.loadTexture("enemy_attack1", "assets/textures/enemy_attack1.png", m_renderer);
    texMgr.loadTexture("enemy_attack2", "assets/textures/enemy_attack2.png", m_renderer);
    texMgr.loadTexture("enemy_hurt",  "assets/textures/enemy_hurt.png",  m_renderer);
    texMgr.loadTexture("enemy_death", "assets/textures/enemy_death.png", m_renderer);

    // Initial spawn at Cổng Chính (Main Gate boulevard entry)
    m_player = std::make_unique<Player>();
    m_player->init("player", Vector2D(80.0f * 32.0f, 103.0f * 32.0f),
                   GameConstants::PLAYER_WIDTH, GameConstants::PLAYER_HEIGHT);

    // Setup DUT sandbox campus world
    m_tileMap.generateSandboxWorld(GameConstants::MAP_ROWS, GameConstants::MAP_COLS);
    m_camera.init(m_tileMap.getPixelWidth(), m_tileMap.getPixelHeight());

    m_running = true;
    m_state = GameState::MENU;
    std::cout << "[Game] DUT Campus Explorer Sandbox Initialized." << std::endl;
    return true;
}

void Game::run() {
    while (m_running) {
        m_timer.tick();
        handleEvents();
        update(m_timer.getDeltaTime());
        render();
        m_timer.capFrameRate(GameConstants::FRAME_DELAY);
    }
}

void Game::handleEvents() {
    auto& input = InputManager::getInstance();
    input.update();
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        input.handleEvent(e);
        if (e.type == SDL_KEYDOWN) {
            switch (e.key.keysym.scancode) {
                case SDL_SCANCODE_ESCAPE:
                    if (m_state == GameState::PLAYING) m_state = GameState::PAUSED;
                    else if (m_state == GameState::PAUSED) m_state = GameState::PLAYING;
                    else if (m_state == GameState::MENU) m_running = false;
                    else if (m_state == GameState::GAME_OVER) m_state = GameState::MENU;
                    break;
                case SDL_SCANCODE_RETURN:
                    if (m_state == GameState::MENU) {
                        m_state = GameState::PLAYING;
                    } else if (m_state == GameState::GAME_OVER) {
                        resetGame();
                        m_state = GameState::PLAYING;
                    }
                    break;
                case SDL_SCANCODE_F1: m_debugMode = !m_debugMode; break;
                case SDL_SCANCODE_F11: {
                    Uint32 winFlags = SDL_GetWindowFlags(m_window);
                    if (winFlags & SDL_WINDOW_FULLSCREEN_DESKTOP) {
                        SDL_SetWindowFullscreen(m_window, 0);
                    } else {
                        SDL_SetWindowFullscreen(m_window, SDL_WINDOW_FULLSCREEN_DESKTOP);
                    }
                    break;
                }
                default: break;
            }
        }
    }
    if (input.isQuitRequested()) m_running = false;
}

void Game::update(float dt) {
    if (m_state != GameState::PLAYING) return;

    m_timeSys.update(dt);

    if (m_dialogueTimer > 0.0f) {
        m_dialogueTimer -= dt;
    }

    m_player->update(dt);

    // Update active enemies
    for (auto& enemy : m_enemies) {
        if (enemy->isActive()) {
            enemy->update(dt, m_player->getPosition());
        }
    }
    // Clean dead enemies
    m_enemies.erase(std::remove_if(m_enemies.begin(), m_enemies.end(),
        [](const std::unique_ptr<Enemy>& e) { return !e->isActive(); }), m_enemies.end());

    // Spawn enemies
    m_enemySpawnTimer += dt;
    float currentSpawnInterval = m_timeSys.isNight() ? GameConstants::ENEMY_SPAWN_INTERVAL * 0.15f : GameConstants::ENEMY_SPAWN_INTERVAL * 2.0f;
    if (m_enemySpawnTimer >= currentSpawnInterval) {
        m_enemySpawnTimer = 0.0f;
        spawnEnemy();
    }

    checkCollisions();

    // Reset hit list or handle combo transitions dynamically
    if (m_player->isAttacking()) {
        if (m_player->getComboState() != m_lastComboState) {
            m_enemiesHitThisSwing.clear(); // Reset hit list so enemies can be hit by the new combo step
            m_lastComboState = m_player->getComboState();
        }
    } else {
        m_enemiesHitThisSwing.clear();
        m_lastComboState = 0;
    }

    // Check Player Sword Attack Hits
    if (m_player->isAttacking()) {
        SDL_Rect attackBox = m_player->getAttackHitbox();
        for (auto& enemy : m_enemies) {
            if (!enemy->isActive() || enemy->isDying()) continue;
            SDL_Rect enemyBox = enemy->getCollider();
            if (SDL_HasIntersection(&attackBox, &enemyBox)) {
                // Ensure we hit this enemy only once per combo step
                if (std::find(m_enemiesHitThisSwing.begin(), m_enemiesHitThisSwing.end(), enemy.get()) == m_enemiesHitThisSwing.end()) {
                    m_enemiesHitThisSwing.push_back(enemy.get());
                    
                    // Scale impact based on combo state (Combo 2 finish is a heavy finisher!)
                    float damage = 15.0f;
                    float knockForce = 350.0f;
                    float shakeForce = 4.0f;
                    float shakeTime = 0.12f;

                    if (m_player->getComboState() == 2) {
                        damage = 30.0f;        // Double Damage!
                        knockForce = 700.0f;   // Double Knockback Force!
                        shakeForce = 8.0f;     // Heavier Screen Shake!
                        shakeTime = 0.18f;
                    }

                    enemy->takeDamage(damage);

                    // Knockback direction away from player
                    Vector2D knockDir = enemy->getPosition() - m_player->getPosition();
                    if (knockDir.magnitudeSquared() > 0.0f) {
                        knockDir.normalize();
                    } else {
                        knockDir = Vector2D(1, 0);
                    }
                    enemy->applyKnockback(knockDir * knockForce);

                    m_camera.shake(shakeForce, shakeTime);

                    // Spawn energy gem or item if enemy was killed
                    if (enemy->isDead()) {
                        int r = rand() % 100;
                        if (r < 15) {
                            spawnEnergyGem(enemy->getPosition(), GameConstants::ITEM_HEALTH_POTION);
                        } else if (r < 30) {
                            spawnEnergyGem(enemy->getPosition(), GameConstants::ITEM_SPEED_POTION);
                        } else {
                            spawnEnergyGem(enemy->getPosition());
                        }
                    }
                }
            }
        }
    }

    // Update Energy Gems (Magnet attraction & Pick up recovery)
    updateEnergyGems(dt);

    // Trigger dialogue interaction on Space/Click
    auto& input = InputManager::getInstance();
    if (input.isKeyPressed(SDL_SCANCODE_SPACE) || input.isMouseButtonPressed(SDL_BUTTON_LEFT)) {
        if (!m_player->getInventory().isOpen()) {
            std::string textOut;
            if (m_player->interact(m_tileMap, textOut)) {
                triggerDialogue(textOut);
            }
        }
    }

    Vector2D center(m_player->getPosition().x + m_player->getWidth() / 2.0f,
                    m_player->getPosition().y + m_player->getHeight() / 2.0f);
    m_camera.update(center, dt);
}

void Game::checkCollisions() {
    SDL_Rect pCol = m_player->getCollider();
    auto obstacles = m_tileMap.getSolidCollidersNear(pCol);

    for (const auto& rect : obstacles) {
        SDL_Rect pr = m_player->getCollider();
        if (!SDL_HasIntersection(&pr, &rect)) continue;

        int overlapL = (pr.x + pr.w) - rect.x;
        int overlapR = (rect.x + rect.w) - pr.x;
        int overlapT = (pr.y + pr.h) - rect.y;
        int overlapB = (rect.y + rect.h) - pr.y;
        int minOverlap = std::min({overlapL, overlapR, overlapT, overlapB});

        Vector2D pos = m_player->getPosition();
        if (minOverlap == overlapL)      pos.x = (float)(rect.x - pr.w - 4);
        else if (minOverlap == overlapR) pos.x = (float)(rect.x + rect.w - 4);
        else if (minOverlap == overlapT) pos.y = (float)(rect.y - m_player->getHeight() + (m_player->getHeight() - 12));
        else                             pos.y = (float)(rect.y + rect.h - m_player->getHeight() + 12);

        m_player->setPosition(pos);
    }

    // Check collision between Player and Enemies
    for (auto& enemy : m_enemies) {
        if (!enemy->isActive() || enemy->isDying() || enemy->isHurt()) continue;

        SDL_Rect pr = m_player->getCollider();
        SDL_Rect er = enemy->getCollider();

        if (SDL_HasIntersection(&pr, &er)) {
            if (!m_player->isInvincible()) {
                m_player->takeDamage(enemy->getDamage());
                m_camera.shake(8.0f, 0.25f); // Shake camera when hit!

                // Knockback player away from enemy
                Vector2D knockDir = m_player->getPosition() - enemy->getPosition();
                if (knockDir.magnitudeSquared() > 0.0f) {
                    knockDir.normalize();
                } else {
                    knockDir = Vector2D(0, 1);
                }
                m_player->setPosition(m_player->getPosition() + knockDir * 25.0f);

                if (m_player->isDead()) {
                    m_state = GameState::GAME_OVER;
                }
            }
        }
    }
}

void Game::triggerDialogue(const std::string& text) {
    m_dialogueText = text;
    m_dialogueTimer = 4.0f;
}

void Game::render() {
    SDL_SetRenderDrawColor(m_renderer, 15, 15, 20, 255);
    SDL_RenderClear(m_renderer);

    if (m_state == GameState::MENU) {
        renderMenu();
        SDL_RenderPresent(m_renderer);
        return;
    }

    Vector2D camPos = m_camera.getPosition();

    // 1. Draw grass lawns & pavement networks
    m_tileMap.renderGround(m_renderer, camPos);

    // Render Ground Energy Gems (before sorted entities)
    renderEnergyGems(camPos);

    // 2. Y-Sorting render lists
    std::vector<YDrawable> drawList;

    // Add Player
    float playerFeetY = m_player->getPosition().y + m_player->getHeight();
    drawList.push_back({YDrawable::TYPE_PLAYER, playerFeetY, 0});

    // Add Active Buildings, Fences, Trees, and NPCs
    const auto& objs = m_tileMap.getObjects();
    for (size_t i = 0; i < objs.size(); i++) {
        if (!objs[i].active) continue;
        float objFeetY = objs[i].position.y + objs[i].height;
        drawList.push_back({YDrawable::TYPE_WORLD_OBJ, objFeetY, static_cast<int>(i)});
    }

    // Add Active Enemies
    for (size_t i = 0; i < m_enemies.size(); i++) {
        if (!m_enemies[i]->isActive()) continue;
        float enemyFeetY = m_enemies[i]->getPosition().y + m_enemies[i]->getHeight();
        drawList.push_back({YDrawable::TYPE_ENEMY, enemyFeetY, static_cast<int>(i)});
    }

    // Sort list dynamically by bottom-Y
    std::sort(drawList.begin(), drawList.end());

    // Loop and draw Y-sorted elements
    for (const auto& drawable : drawList) {
        if (drawable.type == YDrawable::TYPE_PLAYER) {
            m_player->render(m_renderer, camPos);
        } else if (drawable.type == YDrawable::TYPE_WORLD_OBJ) {
            drawWorldObject(m_renderer, objs[drawable.index], camPos);
        } else if (drawable.type == YDrawable::TYPE_ENEMY) {
            m_enemies[drawable.index]->render(m_renderer, camPos);
        }
    }

    // 3. Environment lighting system
    m_timeSys.renderDayNightOverlay(m_renderer);

    // 4. UI Layouts
    renderHUD();

    if (m_dialogueTimer > 0.0f) {
        renderDialogueBox();
    }

    if (m_state == GameState::PAUSED) {
        renderPauseOverlay();
    }

    if (m_state == GameState::GAME_OVER) {
        renderGameOver();
    }

    if (m_player->getInventory().isOpen()) {
        m_player->getInventory().renderFullInventory(m_renderer);
    }

    if (m_debugMode) {
        renderDebugInfo();
    }

    SDL_RenderPresent(m_renderer);
}

void Game::drawWorldObject(SDL_Renderer* renderer, const WorldObject& obj, const Vector2D& camOff) {
    int drawX = (int)(obj.position.x - camOff.x);
    int drawY = (int)(obj.position.y - camOff.y);
    int w = obj.width;
    int h = obj.height;

    using namespace GameConstants;

    // A. Render slanted shadow for all structures (matching the screenshot exactly)
    if (obj.type >= OBJ_BUILDING_A && obj.type <= OBJ_PARKING_LOT) {
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 50);
        for (int i = 0; i < 7; i++) {
            SDL_Rect sh = {drawX - 6 - i * 4, drawY + h - 12 + i * 2, w - 8, 8};
            SDL_RenderFillRect(renderer, &sh);
        }
    }

    // B. Draw high-fidelity axonometric representation of DUT buildings
    switch (obj.type) {
        case OBJ_BUILDING_A: {
            // Grand central white/cream administration block
            SDL_SetRenderDrawColor(renderer, 245, 240, 230, 255);
            SDL_Rect base = {drawX + 4, drawY + h/2, w - 8, h/2};
            SDL_RenderFillRect(renderer, &base);

            // Red pitched roof (Vietnam traditional architecture)
            SDL_SetRenderDrawColor(renderer, 215, 60, 45, 255);
            // Simplistic roof fill
            for (int i = 0; i < h/2 - 8; i++) {
                SDL_RenderDrawLine(renderer, drawX + i * 2, drawY + h/2 - i, drawX + w - i * 2, drawY + h/2 - i);
            }

            // Columns and Logo A badge (as in the screenshot!)
            SDL_SetRenderDrawColor(renderer, 220, 215, 205, 255);
            for (int i = 0; i < 5; i++) {
                SDL_Rect col = {drawX + 16 + i * 24, drawY + h/2 + 6, 8, h/2 - 12};
                SDL_RenderFillRect(renderer, &col);
            }
            // Circular Logo "A"
            SDL_SetRenderDrawColor(renderer, 215, 60, 45, 255);
            SDL_Rect logoBg = {drawX + w/2 - 8, drawY + h/2 - 18, 16, 16};
            SDL_RenderFillRect(renderer, &logoBg);
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            SDL_RenderDrawRect(renderer, &logoBg);
            break;
        }
        case OBJ_BUILDING_B: {
            // Multistory gray and blue classroom block
            SDL_SetRenderDrawColor(renderer, 220, 225, 230, 255);
            SDL_Rect base = {drawX + 6, drawY + 16, w - 12, h - 16};
            SDL_RenderFillRect(renderer, &base);

            // Blue horizontal sun-shields
            SDL_SetRenderDrawColor(renderer, 65, 140, 210, 255);
            SDL_Rect shield1 = {drawX + 2, drawY + 28, w - 4, 6};
            SDL_Rect shield2 = {drawX + 2, drawY + 44, w - 4, 6};
            SDL_RenderFillRect(renderer, &shield1);
            SDL_RenderFillRect(renderer, &shield2);
            break;
        }
        case OBJ_BUILDING_C: {
            // Lecture halls with gray walls and yellow accent lines
            SDL_SetRenderDrawColor(renderer, 195, 200, 205, 255);
            SDL_Rect block = {drawX + 4, drawY + 12, w - 8, h - 12};
            SDL_RenderFillRect(renderer, &block);
            // Yellow accent border
            SDL_SetRenderDrawColor(renderer, 245, 205, 55, 255);
            SDL_Rect border = {drawX + 2, drawY + 12, 4, h - 20};
            SDL_RenderFillRect(renderer, &border);
            break;
        }
        case OBJ_BUILDING_D: {
            // Sprawling D block with modular wings (top-left)
            SDL_SetRenderDrawColor(renderer, 230, 235, 240, 255);
            SDL_Rect wingL = {drawX + 4, drawY + 16, w/2 - 8, h - 16};
            SDL_Rect wingR = {drawX + w/2 + 4, drawY + 16, w/2 - 8, h - 16};
            SDL_RenderFillRect(renderer, &wingL);
            SDL_RenderFillRect(renderer, &wingR);

            // Teal green pitched roofs
            SDL_SetRenderDrawColor(renderer, 45, 155, 145, 255);
            SDL_Rect roofL = {drawX, drawY + 6, w/2 - 2, 10};
            SDL_Rect roofR = {drawX + w/2, drawY + 6, w/2 - 2, 10};
            SDL_RenderFillRect(renderer, &roofL);
            SDL_RenderFillRect(renderer, &roofR);
            break;
        }
        case OBJ_BUILDING_E: {
            // Brick orange/gray building
            SDL_SetRenderDrawColor(renderer, 210, 125, 80, 255);
            SDL_Rect block = {drawX + 4, drawY + 16, w - 8, h - 16};
            SDL_RenderFillRect(renderer, &block);
            // Grey roof
            SDL_SetRenderDrawColor(renderer, 150, 150, 155, 255);
            SDL_Rect roof = {drawX + 1, drawY + 4, w - 2, 12};
            SDL_RenderFillRect(renderer, &roof);
            break;
        }
        case OBJ_BUILDING_F: {
            // Vertical long classroom blocks with a red F Hall auditorium
            SDL_SetRenderDrawColor(renderer, 235, 235, 240, 255);
            SDL_Rect block = {drawX + 8, drawY + 28, w - 16, h - 28};
            SDL_RenderFillRect(renderer, &block);

            // Front auditorium (HỘI TRƯỜNG F) - Red octagonal structure
            SDL_SetRenderDrawColor(renderer, 205, 50, 45, 255);
            SDL_Rect aud = {drawX + 2, drawY + h - 42, w - 4, 38};
            SDL_RenderFillRect(renderer, &aud);
            SDL_SetRenderDrawColor(renderer, 255, 230, 80, 255); // golden yellow trim
            SDL_RenderDrawRect(renderer, &aud);
            break;
        }
        case OBJ_BUILDING_H: {
            // Slanted red-roofed building (represented as angled steps)
            SDL_SetRenderDrawColor(renderer, 235, 230, 220, 255);
            SDL_Rect step1 = {drawX + 4, drawY + 24, w - 8, h - 24};
            SDL_RenderFillRect(renderer, &step1);
            // Red angled roof tiles
            SDL_SetRenderDrawColor(renderer, 220, 55, 45, 255);
            SDL_Rect roof = {drawX, drawY + 10, w, 14};
            SDL_RenderFillRect(renderer, &roof);
            break;
        }
        case OBJ_SMART_BUILDING: {
            // Smart Building - High-tech dark skyscraper with blue glass panel grids
            SDL_SetRenderDrawColor(renderer, 85, 90, 95, 255);
            SDL_Rect body = {drawX + 6, drawY + 12, w - 12, h - 12};
            SDL_RenderFillRect(renderer, &body);

            // Blue horizontal windows strips (smart high-rise look)
            SDL_SetRenderDrawColor(renderer, 95, 210, 255, 255);
            for (int r = 0; r < 4; r++) {
                SDL_Rect win = {drawX + 12, drawY + 22 + r * 20, w - 24, 8};
                SDL_RenderFillRect(renderer, &win);
            }

            // High-tech satellite transmission tower on roof
            SDL_SetRenderDrawColor(renderer, 245, 245, 250, 255);
            SDL_Rect rod = {drawX + w/2 - 2, drawY - 8, 4, 20};
            SDL_RenderFillRect(renderer, &rod);
            SDL_SetRenderDrawColor(renderer, 255, 110, 40, 255); // glowing orange node
            SDL_Rect node = {drawX + w/2 - 4, drawY - 12, 8, 8};
            SDL_RenderFillRect(renderer, &node);
            break;
        }
        case OBJ_RESOURCE_CENTER: {
            // Sleek blue glass study dome library with a rooftop park
            SDL_SetRenderDrawColor(renderer, 95, 145, 195, 255);
            SDL_Rect body = {drawX + 8, drawY + 18, w - 16, h - 18};
            SDL_RenderFillRect(renderer, &body);

            // Circular blue dome skylight roof
            SDL_SetRenderDrawColor(renderer, 80, 180, 240, 255);
            SDL_Rect dome = {drawX + 16, drawY + 4, w - 32, 14};
            SDL_RenderFillRect(renderer, &dome);

            // Rooftop garden greenery
            SDL_SetRenderDrawColor(renderer, 65, 160, 85, 255);
            SDL_Rect bush1 = {drawX + 24, drawY + 14, 16, 8};
            SDL_Rect bush2 = {drawX + w - 40, drawY + 14, 16, 8};
            SDL_RenderFillRect(renderer, &bush1);
            SDL_RenderFillRect(renderer, &bush2);
            break;
        }
        case OBJ_PARKING_LOT: {
            // Covered corrugated iron parking bays for motorbikes
            SDL_SetRenderDrawColor(renderer, 150, 155, 160, 255);
            SDL_Rect roof = {drawX + 4, drawY + 4, w - 8, 14};
            SDL_RenderFillRect(renderer, &roof);

            // Wood support columns
            SDL_SetRenderDrawColor(renderer, 100, 75, 45, 255);
            SDL_Rect colL = {drawX + 6, drawY + 18, 4, h - 18};
            SDL_Rect colR = {drawX + w - 10, drawY + 18, 4, h - 18};
            SDL_RenderFillRect(renderer, &colL);
            SDL_RenderFillRect(renderer, &colR);

            // Miniature colored rectangles representing motorbikes parked inside
            SDL_SetRenderDrawColor(renderer, 220, 60, 45, 255); // red bike
            SDL_Rect bike1 = {drawX + 14, drawY + h - 12, 6, 10};
            SDL_RenderFillRect(renderer, &bike1);
            SDL_SetRenderDrawColor(renderer, 60, 120, 220, 255); // blue bike
            SDL_Rect bike2 = {drawX + 26, drawY + h - 12, 6, 10};
            SDL_RenderFillRect(renderer, &bike2);
            break;
        }
        case OBJ_TREE: {
            // Shadow
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 50);
            SDL_Rect shadow = {drawX + 4, drawY + h - 6, w - 8, 6};
            SDL_RenderFillRect(renderer, &shadow);
            // Trunk
            SDL_SetRenderDrawColor(renderer, 95, 60, 35, 255);
            SDL_Rect trunk = {drawX + w/2 - 4, drawY + h/2, 8, h/2};
            SDL_RenderFillRect(renderer, &trunk);
            // Rounded green foliage (park feel)
            SDL_SetRenderDrawColor(renderer, 70, 165, 80, 255);
            SDL_Rect leaf1 = {drawX + 2, drawY + h/4, w - 4, h/3};
            SDL_Rect leaf2 = {drawX + 6, drawY + 4, w - 12, h/3};
            SDL_RenderFillRect(renderer, &leaf1);
            SDL_RenderFillRect(renderer, &leaf2);
            break;
        }
        case OBJ_NPC_WHITE:
        case OBJ_NPC_BLUE:
        case OBJ_NPC_GREEN: {
            // Slanted shadow
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 45);
            for (int i = 0; i < 5; i++) {
                SDL_Rect sh = {drawX - 4 - i * 3, drawY + h - 8 + i * 2, w - 4, 6};
                SDL_RenderFillRect(renderer, &sh);
            }
            // Colored stick-man body
            Uint8 r=250, g=250, b=250;
            if (obj.type == OBJ_NPC_BLUE)  { r = 60;  g = 150; b = 245; }
            if (obj.type == OBJ_NPC_GREEN) { r = 70;  g = 210; b = 130; }

            // Legs
            SDL_SetRenderDrawColor(renderer, r - 30, g - 20, b - 30, 255);
            SDL_Rect legL = {drawX + 4, drawY + h - 12, 6, 12};
            SDL_Rect legR = {drawX + w - 10, drawY + h - 12, 6, 12};
            SDL_RenderFillRect(renderer, &legL);
            SDL_RenderFillRect(renderer, &legR);

            // Torso
            SDL_SetRenderDrawColor(renderer, r, g, b, 255);
            SDL_Rect torso = {drawX + 3, drawY + h/3, w - 6, h*2/3 - 10};
            SDL_RenderFillRect(renderer, &torso);

            // Head
            SDL_SetRenderDrawColor(renderer, r + 5, g + 5, b + 5, 255);
            SDL_Rect head = {drawX + 4, drawY + 2, w - 8, 16};
            SDL_RenderFillRect(renderer, &head);
            break;
        }
        default: break;
    }
}

void Game::renderMenu() {
    // Sunset gradient
    for (int y = 0; y < GameConstants::SCREEN_HEIGHT; y++) {
        float t = (float)y / GameConstants::SCREEN_HEIGHT;
        SDL_SetRenderDrawColor(m_renderer,
            (Uint8)(10 + t * 45), (Uint8)(10 + t * 25), (Uint8)(25 + t * 15), 255);
        SDL_RenderDrawLine(m_renderer, 0, y, GameConstants::SCREEN_WIDTH, y);
    }

    // Title box
    SDL_SetRenderDrawColor(m_renderer, 205, 50, 45, 230); // red block title
    SDL_Rect titleBox = {GameConstants::SCREEN_WIDTH/2 - 250, 160, 500, 90};
    SDL_RenderFillRect(m_renderer, &titleBox);
    SDL_SetRenderDrawColor(m_renderer, 255, 230, 80, 255);
    SDL_RenderDrawRect(m_renderer, &titleBox);

    float pulse = (SDL_GetTicks() % 2000) / 2000.0f;
    Uint8 alpha = (Uint8)(128 + 127 * std::sin(pulse * 3.14159f * 2.0f));
    SDL_SetRenderDrawColor(m_renderer, 240, 240, 220, alpha);
    SDL_Rect prompt = {GameConstants::SCREEN_WIDTH/2 - 120, 360, 240, 42};
    SDL_RenderFillRect(m_renderer, &prompt);
}

void Game::renderHUD() {
    m_player->getInventory().renderHotbar(m_renderer);

    // Clock
    SDL_SetRenderDrawColor(m_renderer, 20, 20, 25, 220);
    SDL_Rect clockPanel = {GameConstants::SCREEN_WIDTH - 200, 20, 180, 64};
    SDL_RenderFillRect(m_renderer, &clockPanel);
    SDL_SetRenderDrawColor(m_renderer, 85, 85, 95, 255);
    SDL_RenderDrawRect(m_renderer, &clockPanel);

    SDL_SetRenderDrawColor(m_renderer, 80, 180, 240, 255);
    SDL_Rect timeBar = {GameConstants::SCREEN_WIDTH - 198, 22, (int)(176 * m_timeSys.getDayProgress()), 6};
    SDL_RenderFillRect(m_renderer, &timeBar);

    // Stamina
    SDL_SetRenderDrawColor(m_renderer, 20, 20, 25, 220);
    SDL_Rect staminaPanel = {20, 20, 174, 26};
    SDL_RenderFillRect(m_renderer, &staminaPanel);
    float enRatio = m_player->getEnergy() / m_player->getMaxEnergy();
    SDL_SetRenderDrawColor(m_renderer, 65, 175, 90, 255);
    SDL_Rect fill = {23, 23, (int)(168 * enRatio), 20};
    SDL_RenderFillRect(m_renderer, &fill);
    SDL_SetRenderDrawColor(m_renderer, 85, 85, 95, 255);
    SDL_RenderDrawRect(m_renderer, &staminaPanel);

    // Health (HP)
    SDL_SetRenderDrawColor(m_renderer, 20, 20, 25, 220);
    SDL_Rect hpPanel = {20, 52, 174, 26};
    SDL_RenderFillRect(m_renderer, &hpPanel);
    float hpRatio = m_player->getHP() / m_player->getMaxHP();
    SDL_SetRenderDrawColor(m_renderer, 220, 45, 45, 255); // Crimson Red
    SDL_Rect hpFill = {23, 55, (int)(168 * hpRatio), 20};
    SDL_RenderFillRect(m_renderer, &hpFill);
    SDL_SetRenderDrawColor(m_renderer, 85, 85, 95, 255);
    SDL_RenderDrawRect(m_renderer, &hpPanel);
}

void Game::renderDialogueBox() {
    int boxW = 850;
    int boxH = 90;
    int boxX = (GameConstants::SCREEN_WIDTH - boxW) / 2;
    int boxY = GameConstants::SCREEN_HEIGHT - 220;

    SDL_SetRenderDrawColor(m_renderer, 15, 15, 20, 245);
    SDL_Rect box = {boxX, boxY, boxW, boxH};
    SDL_RenderFillRect(m_renderer, &box);
    SDL_SetRenderDrawColor(m_renderer, 255, 225, 80, 255); // beautiful golden-yellow trim
    SDL_RenderDrawRect(m_renderer, &box);

    SDL_SetRenderDrawColor(m_renderer, 255, 225, 80, 100);
    SDL_Rect innerAccent = {boxX + 10, boxY + 10, 8, boxH - 20};
    SDL_RenderFillRect(m_renderer, &innerAccent);
}

void Game::renderPauseOverlay() {
    SDL_SetRenderDrawColor(m_renderer, 0, 0, 0, 160);
    SDL_Rect overlay = {0, 0, GameConstants::SCREEN_WIDTH, GameConstants::SCREEN_HEIGHT};
    SDL_RenderFillRect(m_renderer, &overlay);

    SDL_SetRenderDrawColor(m_renderer, 240, 240, 245, 220);
    SDL_Rect pauseIndicator1 = {GameConstants::SCREEN_WIDTH/2 - 25, GameConstants::SCREEN_HEIGHT/2 - 30, 16, 60};
    SDL_Rect pauseIndicator2 = {GameConstants::SCREEN_WIDTH/2 + 10, GameConstants::SCREEN_HEIGHT/2 - 30, 16, 60};
    SDL_RenderFillRect(m_renderer, &pauseIndicator1);
    SDL_RenderFillRect(m_renderer, &pauseIndicator2);
}

void Game::renderDebugInfo() {
    SDL_SetRenderDrawColor(m_renderer, 0, 0, 0, 200);
    SDL_Rect bg = {GameConstants::SCREEN_WIDTH - 200, 100, 180, 100};
    SDL_RenderFillRect(m_renderer, &bg);
    SDL_SetRenderDrawColor(m_renderer, 50, 255, 50, 255);
    SDL_RenderDrawRect(m_renderer, &bg);

    Vector2D ft = m_player->getFacingTilePos();
    Vector2D cam = m_camera.getPosition();
    SDL_SetRenderDrawColor(m_renderer, 240, 210, 40, 150);
    SDL_Rect ftr = {(int)(ft.x * 32 - cam.x), (int)(ft.y * 32 - cam.y), 32, 32};
    SDL_RenderDrawRect(m_renderer, &ftr);

    SDL_Rect pr = m_player->getCollider();
    pr.x -= (int)cam.x;
    pr.y -= (int)cam.y;
    SDL_SetRenderDrawColor(m_renderer, 50, 240, 80, 200);
    SDL_RenderDrawRect(m_renderer, &pr);
}

void Game::clean() {
    m_enemies.clear();
    TextureManager::getInstance().clearAll();
    if (m_renderer) { SDL_DestroyRenderer(m_renderer); m_renderer = nullptr; }
    if (m_window) { SDL_DestroyWindow(m_window); m_window = nullptr; }
    IMG_Quit();
    SDL_Quit();
}

void Game::spawnEnemy() {
    size_t maxEnemies = m_timeSys.isNight() ? GameConstants::MAX_ACTIVE_ENEMIES * 5 : GameConstants::MAX_ACTIVE_ENEMIES / 2;
    if (m_enemies.size() >= maxEnemies) return;

    // Generate random angle
    float angle = (float)(rand() % 360) * (3.14159265f / 180.0f);
    // Distance from 750px to 900px (outside the viewport edge)
    float distance = 750.0f + (float)(rand() % 150);

    Vector2D playerCenter = m_player->getPosition() + Vector2D(m_player->getWidth() / 2.0f, m_player->getHeight() / 2.0f);
    Vector2D spawnPos = playerCenter + Vector2D(std::cos(angle) * distance, std::sin(angle) * distance);

    // Keep spawn bounds in playable tile limits (160x120 tiles * 32px/tile)
    float mapWidth = m_tileMap.getPixelWidth();
    float mapHeight = m_tileMap.getPixelHeight();
    spawnPos.x = std::clamp(spawnPos.x, 32.0f, mapWidth - 64.0f);
    spawnPos.y = std::clamp(spawnPos.y, 32.0f, mapHeight - 64.0f);

    auto enemy = std::make_unique<Enemy>();
    enemy->init("enemy", spawnPos, GameConstants::PLAYER_WIDTH, GameConstants::PLAYER_HEIGHT);
    m_enemies.push_back(std::move(enemy));
}

void Game::resetGame() {
    m_player->resetHP();
    m_player->fullRestore();
    // Reset player position to Cổng Chính (Main entrance boulevard spawn point)
    m_player->setPosition(Vector2D(80.0f * 32.0f, 103.0f * 32.0f));
    m_enemies.clear();
    m_energyGems.clear();
    m_enemiesHitThisSwing.clear();
    m_enemySpawnTimer = 0.0f;
    std::cout << "[Game] Player respawned. Game restarted." << std::endl;
}

void Game::renderGameOver() {
    // 1. Overlay fade dark crimson
    SDL_SetRenderDrawColor(m_renderer, 35, 10, 10, 210);
    SDL_Rect overlay = {0, 0, GameConstants::SCREEN_WIDTH, GameConstants::SCREEN_HEIGHT};
    SDL_RenderFillRect(m_renderer, &overlay);

    // 2. Large Crimson Box with Golden yellow frame
    SDL_SetRenderDrawColor(m_renderer, 180, 20, 20, 245);
    SDL_Rect goBox = {GameConstants::SCREEN_WIDTH/2 - 250, GameConstants::SCREEN_HEIGHT/2 - 80, 500, 100};
    SDL_RenderFillRect(m_renderer, &goBox);
    SDL_SetRenderDrawColor(m_renderer, 255, 225, 80, 255); // golden yellow border
    SDL_RenderDrawRect(m_renderer, &goBox);

    // Slanted decoration bars in background box
    SDL_SetRenderDrawColor(m_renderer, 255, 255, 255, 30);
    for (int i = 0; i < 10; i++) {
        SDL_RenderDrawLine(m_renderer, goBox.x + i*50, goBox.y, goBox.x + i*50 + 20, goBox.y + goBox.h);
    }

    // 3. Pulse prompt (Press Enter to Restart)
    float pulse = (SDL_GetTicks() % 1500) / 1500.0f;
    Uint8 alpha = (Uint8)(100 + 155 * std::sin(pulse * 3.14159f));
    SDL_SetRenderDrawColor(m_renderer, 255, 225, 80, alpha);
    SDL_Rect prompt = {GameConstants::SCREEN_WIDTH/2 - 180, GameConstants::SCREEN_HEIGHT/2 + 50, 360, 42};
    SDL_RenderFillRect(m_renderer, &prompt);
}

void Game::spawnEnergyGem(const Vector2D& pos, GameConstants::ItemType type) {
    m_energyGems.push_back({
        pos,
        Vector2D::Zero(),
        true,
        10.0f, // restores 10 stamina/energy
        (float)(rand() % 100) * 0.1f,
        type
    });
}

void Game::updateEnergyGems(float dt) {
    Vector2D pCenter(
        m_player->getPosition().x + m_player->getWidth() / 2.0f,
        m_player->getPosition().y + m_player->getHeight() / 2.0f
    );

    for (auto& gem : m_energyGems) {
        if (!gem.active) continue;

        gem.bobTimer += dt;

        Vector2D toPlayer = pCenter - gem.position;
        float dist = toPlayer.magnitude();

        // Magnetic Attraction
        if (dist < 150.0f) {
            if (dist > 5.0f) {
                toPlayer.normalize();
                // Gem flies super fast towards the player
                gem.velocity = toPlayer * 350.0f;
                gem.position += gem.velocity * dt;
            }
        } else {
            // Decelerate if it was moving previously
            gem.velocity *= 0.9f;
            gem.position += gem.velocity * dt;
        }

        // Collision Check (Pick Up Gem/Item)
        if (dist < 20.0f) {
            if (gem.itemType == GameConstants::ITEM_NONE) {
                m_player->restoreEnergy(gem.energyValue);
                gem.active = false;
            } else {
                if (m_player->getInventory().addItem(gem.itemType, 1)) {
                    gem.active = false;
                }
            }
        }
    }

    // Clean up inactive gems
    m_energyGems.erase(std::remove_if(m_energyGems.begin(), m_energyGems.end(),
        [](const EnergyGem& g) { return !g.active; }), m_energyGems.end());
}

void Game::renderEnergyGems(const Vector2D& cameraOffset) {
    for (const auto& gem : m_energyGems) {
        if (!gem.active) continue;

        // Bouncy pulse size using sine bobbing
        float gemPulse = std::sin(gem.bobTimer * 8.0f) * 2.0f;
        int screenX = static_cast<int>(gem.position.x - cameraOffset.x);
        int screenY = static_cast<int>(gem.position.y - cameraOffset.y);

        // Size parameter for the diamond
        int pulse = 6 + static_cast<int>(gemPulse);

        // 1. Draw glowing outer filled diamond based on item type
        if (gem.itemType == GameConstants::ITEM_HEALTH_POTION) {
            SDL_SetRenderDrawColor(m_renderer, 240, 60, 60, 255); // Red
        } else if (gem.itemType == GameConstants::ITEM_SPEED_POTION) {
            SDL_SetRenderDrawColor(m_renderer, 60, 180, 240, 255); // Blue
        } else {
            SDL_SetRenderDrawColor(m_renderer, 50, 240, 200, 255); // Cyan default
        }
        for (int row = -pulse; row <= pulse; ++row) {
            int lineWidth = pulse - std::abs(row);
            SDL_Rect lineRect = {
                screenX - lineWidth,
                screenY + row,
                lineWidth * 2 + 1,
                1
            };
            SDL_RenderFillRect(m_renderer, &lineRect);
        }

        // 2. Draw bright white-cyan inner glowing core diamond
        SDL_SetRenderDrawColor(m_renderer, 230, 255, 255, 255);
        int innerPulse = pulse / 2;
        if (innerPulse < 1) innerPulse = 1;
        for (int row = -innerPulse; row <= innerPulse; ++row) {
            int lineWidth = innerPulse - std::abs(row);
            SDL_Rect lineRect = {
                screenX - lineWidth,
                screenY + row,
                lineWidth * 2 + 1,
                1
            };
            SDL_RenderFillRect(m_renderer, &lineRect);
        }
    }
}
