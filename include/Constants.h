#ifndef CONSTANTS_H
#define CONSTANTS_H

namespace GameConstants {
    // Window settings
    const int SCREEN_WIDTH  = 1280;
    const int SCREEN_HEIGHT = 720;
    const char* const WINDOW_TITLE = "DUT Campus Sandbox Explorer";

    // Frame rate
    const int TARGET_FPS = 60;
    const float FRAME_DELAY = 1000.0f / TARGET_FPS;

    // Tile map (Axonometric 3/4 high angle) - Expanded & Segmented
    const int TILE_SIZE = 32;
    const int MAP_COLS  = 160;
    const int MAP_ROWS  = 120;

    // Player capsule sizes
    const float PLAYER_SPEED   = 220.0f;
    const int   PLAYER_WIDTH   = 24;
    const int   PLAYER_HEIGHT  = 46;

    // Enemy Settings (Brotato-like arena)
    const float ENEMY_SPEED          = 120.0f;
    const float ENEMY_DAMAGE         = 10.0f;
    const float ENEMY_SPAWN_INTERVAL = 2.0f; // spawn every 2 seconds
    const int   MAX_ACTIVE_ENEMIES   = 40;

    // Camera
    const float CAMERA_LERP_SPEED = 6.0f;

    // Time system (Day/Night cycle)
    const float SECONDS_PER_GAME_MINUTE = 0.5f;
    const int   DAY_START_HOUR   = 6;
    const int   NIGHT_START_HOUR = 19;

    const int   MAX_INVENTORY_SLOTS = 24;
    const int   HOTBAR_SLOTS = 8;

    // Directions
    enum Direction { DIR_DOWN = 0, DIR_LEFT, DIR_RIGHT, DIR_UP };

    // Ground tile types (Tilted 3D campus terrain)
    enum GroundType {
        GROUND_GRASS_LIGHT = 0, // Green campus lawns
        GROUND_GRASS_DARK,      // Shade grass lawns
        GROUND_ROAD_ASPHALT,    // Nguyễn Lương Bằng & Ngô Sĩ Liên streets
        GROUND_CAMPUS_PAVEMENT, // Main entrance boulevard & internal paths
        GROUND_SAND_PLAYGROUND, // Large dirt/sand playground on the left
        GROUND_WATER_POND,      // Beautiful campus lake / fountain
        GROUND_COUNT
    };

    // Object types (placed on top, Y-sorted)
    enum ObjectType {
        OBJ_NONE = 0,
        OBJ_TREE,
        OBJ_ROCK,
        OBJ_SIGN,
        OBJ_CAMPFIRE,
        // Campus specific buildings
        OBJ_BUILDING_A,
        OBJ_BUILDING_B,
        OBJ_BUILDING_C,
        OBJ_BUILDING_D,
        OBJ_BUILDING_E,
        OBJ_BUILDING_F,
        OBJ_BUILDING_H,
        OBJ_SMART_BUILDING,
        OBJ_RESOURCE_CENTER,
        OBJ_PARKING_LOT,
        // Interactive NPCs
        OBJ_NPC_WHITE,          // Professor / Guard NPC
        OBJ_NPC_BLUE,           // Freshman Student NPC
        OBJ_NPC_GREEN,          // Senior Student NPC
        OBJ_COUNT
    };

    // Item types for gathering & sandbox
    enum ItemType {
        ITEM_NONE = 0,
        ITEM_WOOD,
        ITEM_STONE,
        ITEM_IRON_ORE,
        ITEM_COIN,
        ITEM_CHEST_KEY,
        ITEM_ANCIENT_RELIC,
        ITEM_WILD_BERRY,
        ITEM_HEALTH_POTION,
        ITEM_SPEED_POTION,
        ITEM_COUNT
    };
}

#endif
