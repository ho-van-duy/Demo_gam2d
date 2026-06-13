#include "TileMap.h"
#include "TextureManager.h"
#include "Item.h"
#include "Player.h"
#include "Game.h"
#include <iostream>
#include <algorithm>
#include <cstdlib>

TileMap::TileMap() : m_rows(0), m_cols(0), m_tileSize(GameConstants::TILE_SIZE) {}
TileMap::~TileMap() {}

void TileMap::spawnObject(GameConstants::ObjectType type, int r, int c, int w, int h, const std::string& msg) {
    if (isInBounds(r, c)) {
        m_objects.emplace_back(type, r, c, w, h, msg);
    }
}

void TileMap::generateSandboxWorld(int rows, int cols) {
    m_rows = rows;
    m_cols = cols;
    
    // Initialize whole map to light green grass, with random dark grass patches for visual variety
    m_ground.resize(m_rows, std::vector<GameConstants::GroundType>(m_cols, GameConstants::GROUND_GRASS_LIGHT));
    for (int r = 0; r < m_rows; r++) {
        for (int c = 0; c < m_cols; c++) {
            if ((std::rand() % 100) < 15) {
                m_ground[r][c] = GameConstants::GROUND_GRASS_DARK;
            }
        }
    }
    m_objects.clear();

    using namespace GameConstants;

    // 1. Procedural University Campus Ground Layout (Expanded to 160x120)
    for (int r = 0; r < m_rows; r++) {
        for (int c = 0; c < m_cols; c++) {
            
            // --- MAIN HIGHWAYS & STREETS ---
            // Nguyễn Lương Bằng Street (Bottom horizontal highway, 5 tiles wide)
            if (r >= 107 && r <= 111) {
                m_ground[r][c] = GROUND_ROAD_ASPHALT;
            }
            // Ngô Sĩ Liên Street (Right vertical street, 4 tiles wide)
            else if (c >= 140 && c <= 143) {
                m_ground[r][c] = GROUND_ROAD_ASPHALT;
            }
            
            // --- INTERNAL RING ROAD SYSTEM (3 tiles wide, connects all zones) ---
            // Horizontal Ring Road
            else if (r >= 45 && r <= 47 && c >= 15 && c <= 139) {
                m_ground[r][c] = GROUND_ROAD_ASPHALT;
            }
            // Vertical Ring Road West
            else if (c >= 47 && c <= 49 && r >= 15 && r <= 106) {
                m_ground[r][c] = GROUND_ROAD_ASPHALT;
            }
            // Vertical Ring Road East
            else if (c >= 105 && c <= 107 && r >= 15 && r <= 106) {
                m_ground[r][c] = GROUND_ROAD_ASPHALT;
            }
            
            // --- REGIONAL PAVEMENTS & PLAZAS ---
            // Main Gate Entry Pavement Boulevard
            else if (c >= 76 && c <= 84 && r >= 101 && r <= 106) {
                m_ground[r][c] = GROUND_CAMPUS_PAVEMENT;
            }
            // Central Plaza & Admission Region (Quảng trường trung tâm)
            else if (r >= 48 && r <= 70 && c >= 60 && c <= 100) {
                m_ground[r][c] = GROUND_CAMPUS_PAVEMENT;
            }
            // Central Avenue (Đại lộ trung tâm) connecting Main Gate to Plaza
            else if (c >= 76 && c <= 84 && r >= 70 && r <= 100) {
                m_ground[r][c] = GROUND_CAMPUS_PAVEMENT;
            }
            
            // --- BUILDING FOOTPRINT PAVEMENTS (To prevent grass growing inside structures) ---
            // Building A Pavement Base (Central Admin)
            else if (r >= 35 && r <= 47 && c >= 72 && c <= 88) {
                m_ground[r][c] = GROUND_CAMPUS_PAVEMENT;
            }
            // Building B Pavement Base (Lecture B)
            else if (r >= 74 && r <= 84 && c >= 68 && c <= 84) {
                m_ground[r][c] = GROUND_CAMPUS_PAVEMENT;
            }
            // Building C Pavement Base (Lecture C)
            else if (r >= 60 && r <= 70 && c >= 116 && c <= 128) {
                m_ground[r][c] = GROUND_CAMPUS_PAVEMENT;
            }
            // Building E Pavement Base (Lecture E)
            else if (r >= 38 && r <= 48 && c >= 116 && c <= 128) {
                m_ground[r][c] = GROUND_CAMPUS_PAVEMENT;
            }
            // Smart Building Pavement Base (Northeast Lab)
            else if (r >= 10 && r <= 26 && c >= 116 && c <= 132) {
                m_ground[r][c] = GROUND_CAMPUS_PAVEMENT;
            }
            // Building D Pavement Base (Northwest Engineering)
            else if (r >= 10 && r <= 22 && c >= 20 && c <= 36) {
                m_ground[r][c] = GROUND_CAMPUS_PAVEMENT;
            }
            // Building D & H connector pavement
            else if (r >= 22 && r <= 28 && c >= 26 && c <= 30) {
                m_ground[r][c] = GROUND_CAMPUS_PAVEMENT;
            }
            // Building H Pavement Base (Northwest Architecture)
            else if (r >= 28 && r <= 38 && c >= 20 && c <= 36) {
                m_ground[r][c] = GROUND_CAMPUS_PAVEMENT;
            }
            // Building F Pavement Base (West Campus Seminar)
            else if (r >= 50 && r <= 62 && c >= 24 && c <= 36) {
                m_ground[r][c] = GROUND_CAMPUS_PAVEMENT;
            }
            // Learning Resource Center (LRC) Pavement Base (Southwest Library)
            else if (r >= 86 && r <= 98 && c >= 18 && c <= 36) {
                m_ground[r][c] = GROUND_CAMPUS_PAVEMENT;
            }
            // Parking Lot B Pavement Base
            else if (r >= 84 && r <= 92 && c >= 50 && c <= 62) {
                m_ground[r][c] = GROUND_CAMPUS_PAVEMENT;
            }
            
            // --- WATER BODIES ---
            // Central Plaza Fountain / Water Pool (Inside Central Plaza)
            else if (r >= 54 && r <= 60 && c >= 74 && c <= 86) {
                m_ground[r][c] = GROUND_WATER_POND;
            }
            // West Lake F (Next to Building F)
            else if (r >= 56 && r <= 68 && c >= 14 && c <= 24) {
                m_ground[r][c] = GROUND_WATER_POND;
            }
            
            // --- SPORTS ARENA & FUTURE EXPANSION FIELDS ---
            // Large Sandbox Playground / Football Field (Far left, West side)
            else if (r >= 30 && r <= 80 && c >= 2 && c <= 12) {
                m_ground[r][c] = GROUND_SAND_PLAYGROUND;
            }
        }
    }

    // 2. Spawn Axonometric Buildings (precisely positioned within their designated zone pavements)
    // Building A ("KHU A" - Central Admin - North of Plaza)
    spawnObject(OBJ_BUILDING_A, 38, 76, 256, 160, "Building A: Central Administration & Enrollment Bureau. Proud symbol of DUT.");

    // Building B ("KHU B" - Central Lecture Complex)
    spawnObject(OBJ_BUILDING_B, 78, 72, 192, 112, "Building B: School of Information & Communication Technology (SoICT) and main classroom block.");

    // Building C ("KHU C" - East Campus General Sciences Lecture Complex)
    spawnObject(OBJ_BUILDING_C, 64, 120, 128, 160, "Building C: General Lecture Complex. Lots of first-year and second-year classes here.");

    // Building D ("KHU D" - Northwest Campus Mechanical & Electrical)
    spawnObject(OBJ_BUILDING_D, 18, 22, 256, 120, "Building D: Faculty of Mechanical Engineering & Faculty of Electrical Engineering. Workshops nearby.");

    // Building E ("KHU E" - East Campus Chemical & Environmental)
    spawnObject(OBJ_BUILDING_E, 40, 120, 128, 140, "Building E: Faculty of Chemical Engineering and Faculty of Environment & Biology.");

    // Building F ("KHU F" - West Campus Seminar Complex)
    spawnObject(OBJ_BUILDING_F, 54, 27, 96, 240, "Building F: Hall F Auditorium and student activity seminar complexes.");

    // Building H ("KHU H" - Northwest Campus Architecture & Civil Engineering)
    spawnObject(OBJ_BUILDING_H, 32, 24, 192, 128, "Building H: Center of Civil Engineering, Roadways & Architecture. Excellent slanted drawing tables inside!");

    // Smart Building ("TÒA NHÀ THÔNG MINH" - Northeast High-Tech Zone)
    spawnObject(OBJ_SMART_BUILDING, 20, 120, 160, 224, "Smart Building: High-Tech R&D, AI Research Center and Postgraduate Academy.");

    // Resource Center ("TRUNG TÂM HỌC LIỆU / KHU R" - Southwest Library)
    spawnObject(OBJ_RESOURCE_CENTER, 92, 20, 224, 160, "Learning Resource Center (LRC): Massive digital library, computer stations, and quiet research rooms.");

    // Parking Lot ("NHÀ XE KHU B")
    spawnObject(OBJ_PARKING_LOT, 88, 52, 128, 64, "Parking Lot Khu B: Motorbike Parking. Keep your ticket safe!");

    // 3. Spawn Interactive University NPCs in Segmented Zones
    // Bác bảo vệ (White capsule) near Main Gate
    spawnObject(OBJ_NPC_WHITE, 102, 80, 24, 46, "Guard Guard: Welcome to DUT! Freshmen should follow the RED dotted path for enrollment at Building A.");

    // Tân sinh viên Nam (Blue capsule) in central avenue
    spawnObject(OBJ_NPC_BLUE, 76, 82, 24, 46, "Freshman Nam: I just got admitted! The campus is so huge and beautiful, I am exploring!");

    // Sinh viên năm cuối Huy (Green capsule) near LRC / Library
    spawnObject(OBJ_NPC_GREEN, 92, 32, 24, 46, "Senior Huy: Preparing my graduation thesis here. Bách Khoa engineering is tough but rewarding!");

    // Giảng viên Bách Khoa inside Building A
    spawnObject(OBJ_NPC_WHITE, 42, 80, 24, 46, "Professor Lan: Congratulations on joining our university! Your future begins right here.");
    
    // Additional NPCs for the new zones
    // Chemistry Lecturer near Building E
    spawnObject(OBJ_NPC_WHITE, 42, 118, 24, 46, "Dr. Minh: Don't forget your safety goggles in the lab. We are conducting organic synthesis today!");
    
    // Mechanical Engineering Student near Building D
    spawnObject(OBJ_NPC_BLUE, 16, 25, 24, 46, "Engineering Student Khoa: We are building a solar car prototype in the workshop! Want to see the CAD designs?");
    
    // Lake Janitor near West Lake F
    spawnObject(OBJ_NPC_GREEN, 64, 22, 24, 46, "Janitor Ba: Enjoy the beautiful breeze near Lake F, but please don't litter! Let's keep DUT green.");
    
    // PhD Researcher near Smart Building
    spawnObject(OBJ_NPC_WHITE, 22, 116, 24, 46, "PhD Candidate Vy: Our deep learning model is analyzing drone images of the campus. Extremely exciting research!");

    // 4. Campus Regional Signboards
    spawnObject(OBJ_SIGN, 105, 83, 32, 32, "Main Gate: Welcome to Danang University of Technology (DUT) - Expanded Sandbox Edition!");
    spawnObject(OBJ_SIGN, 65, 80, 32, 32, "Central Square: North (Building A Admin), South (Building B SoICT), West (Lake F Park), East (East Campus).");
    spawnObject(OBJ_SIGN, 88, 25, 32, 32, "Khu R: Learning Resource Center (LRC). Quiet study area and digital libraries. Southwest Campus.");
    spawnObject(OBJ_SIGN, 68, 112, 32, 32, "East Campus: Building C (General Sciences Complex) and Building E (Chemical Engineering).");
    spawnObject(OBJ_SIGN, 22, 112, 32, 32, "Northeast District: High-Tech R&D and Postgraduate Center (Smart Building).");
    spawnObject(OBJ_SIGN, 58, 23, 32, 32, "West Campus: Building F and Scenic Lake F. Student activities seminar complex.");
    spawnObject(OBJ_SIGN, 35, 14, 32, 32, "DUT Athletic Arena: Sand sports court and future athletic complexes. West border.");
    spawnObject(OBJ_SIGN, 22, 25, 32, 32, "Northwest District: Technical Workshops. Building D (Mechanical & Electrical) and Building H (Civil & Architecture).");

    // Spawn trees and grass detail all over the campus parks (avoiding pavements, roads, water, sand)
    for (int r = 2; r < m_rows - 5; r += 4) {
        for (int c = 2; c < m_cols - 2; c += 4) {
            // Avoid pavements, water, and building footprints
            if (m_ground[r][c] == GROUND_ROAD_ASPHALT || m_ground[r][c] == GROUND_CAMPUS_PAVEMENT ||
                m_ground[r][c] == GROUND_SAND_PLAYGROUND || m_ground[r][c] == GROUND_WATER_POND)
                continue;

            // Buffer zone to avoid spawning directly in front of the main gate boulevard path
            if (c >= 73 && c <= 87 && r >= 44 && r <= 112)
                continue;

            int randVal = std::rand() % 100;
            if (randVal < 38) {
                spawnObject(OBJ_TREE, r, c, 32, 60);
            }
        }
    }

    std::cout << "[TileMap] DUT University Campus (Expanded 160x120 & Segmented Zones) generated successfully." << std::endl;
}

void TileMap::renderGround(SDL_Renderer* renderer, const Vector2D& cameraOffset) {
    using namespace GameConstants;
    int startCol = std::max(0, static_cast<int>(cameraOffset.x) / m_tileSize);
    int startRow = std::max(0, static_cast<int>(cameraOffset.y) / m_tileSize);
    int endCol = std::min(m_cols, startCol + GameConstants::SCREEN_WIDTH / m_tileSize + 2);
    int endRow = std::min(m_rows, startRow + GameConstants::SCREEN_HEIGHT / m_tileSize + 2);

    for (int r = startRow; r < endRow; r++) {
        for (int c = startCol; c < endCol; c++) {
            int x = c * m_tileSize - static_cast<int>(cameraOffset.x);
            int y = r * m_tileSize - static_cast<int>(cameraOffset.y);
            SDL_Rect rect = {x, y, m_tileSize, m_tileSize};

            switch (m_ground[r][c]) {
                case GROUND_GRASS_LIGHT:
                    // Soft light yellow-green grass lawns matching the screenshot park spaces
                    SDL_SetRenderDrawColor(renderer, 178, 222, 120, 255);
                    SDL_RenderFillRect(renderer, &rect);
                    break;
                case GROUND_GRASS_DARK:
                    SDL_SetRenderDrawColor(renderer, 140, 195, 88, 255);
                    SDL_RenderFillRect(renderer, &rect);
                    break;
                case GROUND_ROAD_ASPHALT:
                    // Dark grey streets with yellow/white dividing lines
                    SDL_SetRenderDrawColor(renderer, 115, 115, 120, 255);
                    SDL_RenderFillRect(renderer, &rect);
                    // Add asphalt textures/cracks
                    SDL_SetRenderDrawColor(renderer, 245, 245, 245, 60);
                    // Main highway lane stripes
                    if (r == 109 && c % 4 == 0) {
                        SDL_Rect stripe = {x, y + m_tileSize / 2 - 1, m_tileSize / 2, 2};
                        SDL_RenderFillRect(renderer, &stripe);
                    }
                    // Ring road horizontal lane stripes
                    if (r == 46 && c % 4 == 0) {
                        SDL_Rect stripe = {x, y + m_tileSize / 2 - 1, m_tileSize / 2, 2};
                        SDL_RenderFillRect(renderer, &stripe);
                    }
                    break;
                case GROUND_CAMPUS_PAVEMENT:
                    // Soft light grey/cream university boulevard pavement
                    SDL_SetRenderDrawColor(renderer, 230, 228, 222, 255);
                    SDL_RenderFillRect(renderer, &rect);
                    SDL_SetRenderDrawColor(renderer, 205, 203, 197, 255);
                    SDL_RenderDrawRect(renderer, &rect);
                    break;
                case GROUND_SAND_PLAYGROUND:
                    // Lighter sandy/yellow dirt field for sandbox sports
                    SDL_SetRenderDrawColor(renderer, 228, 218, 170, 255);
                    SDL_RenderFillRect(renderer, &rect);
                    break;
                case GROUND_WATER_POND:
                    // Soft cyan fountain/pond
                    SDL_SetRenderDrawColor(renderer, 100, 200, 245, 255);
                    SDL_RenderFillRect(renderer, &rect);
                    SDL_SetRenderDrawColor(renderer, 140, 220, 255, 200);
                    SDL_RenderDrawLine(renderer, x, y, x + m_tileSize, y + m_tileSize);
                    break;
                default:
                    break;
            }

            // 3. Render Colorful Dotted Pathways ("Lối tiếp cận" from the screenshot)
            if (m_ground[r][c] == GROUND_CAMPUS_PAVEMENT) {
                // RED Dotted Path (Enrollment Route straight to Building A)
                if (c == 80 && r >= 48 && r <= 106 && r % 2 == 0) {
                    SDL_SetRenderDrawColor(renderer, 235, 45, 45, 255);
                    SDL_Rect dot = {x + m_tileSize/2 - 3, y + m_tileSize/2 - 3, 6, 6};
                    SDL_RenderFillRect(renderer, &dot);
                }
                // BLUE Dotted Path (Academic Route to Building F and LRC)
                else if (((c == 78 && r >= 75 && r <= 106) || 
                          (r == 74 && c >= 30 && c <= 78) || 
                          (c == 30 && r >= 62 && r <= 86)) && (r + c) % 2 == 0) {
                    SDL_SetRenderDrawColor(renderer, 45, 195, 235, 255);
                    SDL_Rect dot = {x + m_tileSize/2 - 3, y + m_tileSize/2 - 3, 6, 6};
                    SDL_RenderFillRect(renderer, &dot);
                }
                // ORANGE Dotted Path (Technological Route to East Complex & Smart Building)
                else if (((c == 82 && r >= 75 && r <= 106) || 
                          (r == 74 && c >= 82 && c <= 122) || 
                          (c == 122 && r >= 26 && r <= 74)) && (r + c) % 2 == 0) {
                    SDL_SetRenderDrawColor(renderer, 255, 110, 40, 255);
                    SDL_Rect dot = {x + m_tileSize/2 - 3, y + m_tileSize/2 - 3, 6, 6};
                    SDL_RenderFillRect(renderer, &dot);
                }
            }
        }
    }
}

bool TileMap::interactWithObject(int row, int col, Player& player, std::string& dialogueOut) {
    using namespace GameConstants;
    for (auto& obj : m_objects) {
        if (!obj.active) continue;

        // Account for wide structural building boundaries
        bool hit = (row >= obj.row && row <= obj.row + obj.height/GameConstants::TILE_SIZE &&
                    col >= obj.col && col <= obj.col + obj.width/GameConstants::TILE_SIZE);

        if (hit || (obj.row == row && obj.col == col)) {
            dialogueOut = obj.message;
            return true;
        }
    }
    return false;
}

bool TileMap::isSolid(int row, int col) const {
    using namespace GameConstants;
    if (!isInBounds(row, col)) return true;

    // Boundary check
    for (const auto& obj : m_objects) {
        if (!obj.active) continue;
        // Fences and large buildings are solid obstacles
        if (obj.type >= OBJ_BUILDING_A && obj.type <= OBJ_RESOURCE_CENTER) {
            int blockCols = obj.width / GameConstants::TILE_SIZE;
            int blockRows = 1; // base collision layer
            if (row >= obj.row && row < obj.row + blockRows &&
                col >= obj.col && col < obj.col + blockCols) {
                return true;
            }
        }
        else if (obj.row == row && obj.col == col) {
            if (obj.type == GameConstants::OBJ_TREE || obj.type == GameConstants::OBJ_ROCK) {
                return true;
            }
        }
    }
    return false;
}

bool TileMap::isInBounds(int row, int col) const {
    return row >= 0 && row < m_rows && col >= 0 && col < m_cols;
}

std::vector<SDL_Rect> TileMap::getSolidCollidersNear(const SDL_Rect& area) const {
    using namespace GameConstants;
    std::vector<SDL_Rect> colliders;

    // Dynamic colliders for buildings (axonometric foot box)
    for (const auto& obj : m_objects) {
        if (!obj.active) continue;
        if (obj.type >= OBJ_BUILDING_A && obj.type <= OBJ_PARKING_LOT) {
            SDL_Rect objCol = obj.getCollider();
            if (SDL_HasIntersection(&area, &objCol)) {
                colliders.push_back(objCol);
            }
        }
        else if (obj.type == GameConstants::OBJ_TREE || obj.type == GameConstants::OBJ_ROCK) {
            SDL_Rect objCol = obj.getCollider();
            if (SDL_HasIntersection(&area, &objCol)) {
                colliders.push_back(objCol);
            }
        }
    }

    return colliders;
}

GameConstants::GroundType TileMap::getGroundType(int row, int col) const {
    if (isInBounds(row, col)) return m_ground[row][col];
    return GameConstants::GROUND_GRASS_LIGHT;
}
