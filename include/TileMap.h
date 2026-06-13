#ifndef TILEMAP_H
#define TILEMAP_H

#include <SDL2/SDL.h>
#include <string>
#include <vector>
#include "Vector2D.h"
#include "Constants.h"

struct WorldObject {
    GameConstants::ObjectType type;
    int row, col;
    Vector2D position;
    int width, height;
    bool active;
    bool opened;
    std::string message;

    WorldObject()
        : type(GameConstants::OBJ_NONE), row(0), col(0),
          width(32), height(32), active(true), opened(false) {}

    WorldObject(GameConstants::ObjectType t, int r, int c, int w, int h, const std::string& msg = "")
        : type(t), row(r), col(c), width(w), height(h), active(true), opened(false), message(msg) {
        // Offset Y-coordinate so the base of the tall building stands on the tile (axonometric view)
        position = Vector2D((float)(c * GameConstants::TILE_SIZE),
                            (float)(r * GameConstants::TILE_SIZE - (h - GameConstants::TILE_SIZE)));
    }

    SDL_Rect getCollider() const {
        // Larger structural buildings have solid foot colliders matching their base
        int baseH = 24; // base footprint
        if (type == GameConstants::OBJ_SMART_BUILDING || type == GameConstants::OBJ_RESOURCE_CENTER) {
            baseH = 40;
        }
        return {
            static_cast<int>(position.x + 4),
            static_cast<int>(position.y + height - baseH),
            width - 8,
            baseH
        };
    }
};

class TileMap {
public:
    TileMap();
    ~TileMap();

    void generateSandboxWorld(int rows, int cols);
    void renderGround(SDL_Renderer* renderer, const Vector2D& cameraOffset);

    // Dynamic objects for Y-sorting
    std::vector<WorldObject>& getObjects() { return m_objects; }
    const std::vector<WorldObject>& getObjects() const { return m_objects; }

    // Sandbox Interactions
    bool interactWithObject(int row, int col, class Player& player, std::string& dialogueOut);

    // Collision detection
    bool isSolid(int row, int col) const;
    bool isInBounds(int row, int col) const;
    std::vector<SDL_Rect> getSolidCollidersNear(const SDL_Rect& area) const;

    // Getters
    int getRows() const { return m_rows; }
    int getCols() const { return m_cols; }
    int getTileSize() const { return m_tileSize; }
    int getPixelWidth() const { return m_cols * m_tileSize; }
    int getPixelHeight() const { return m_rows * m_tileSize; }

    GameConstants::GroundType getGroundType(int row, int col) const;

private:
    std::vector<std::vector<GameConstants::GroundType>> m_ground;
    std::vector<WorldObject> m_objects;
    int m_rows, m_cols, m_tileSize;

    void spawnObject(GameConstants::ObjectType type, int r, int c, int w, int h, const std::string& msg = "");
};

#endif
