#include "Inventory.h"
#include "Constants.h"
#include "Player.h"
#include "Player.h"
#include <algorithm>
#include <iostream>

Inventory::Inventory()
    : m_selectedSlot(0), m_isOpen(false)
{
    m_slots.resize(GameConstants::MAX_INVENTORY_SLOTS);
    // Starter items for exploration
    m_slots[0] = ItemStack(GameConstants::ITEM_CHEST_KEY, 1);
    m_slots[1] = ItemStack(GameConstants::ITEM_WILD_BERRY, 5);
}

bool Inventory::addItem(GameConstants::ItemType type, int count) {
    auto& db = ItemDatabase::getInstance();
    int maxStack = db.getItem(type).maxStack;

    // Try stack
    for (auto& slot : m_slots) {
        if (slot.type == type && slot.count < maxStack) {
            int added = std::min(count, maxStack - slot.count);
            slot.count += added;
            count -= added;
            if (count <= 0) return true;
        }
    }

    // Try empty slot
    for (auto& slot : m_slots) {
        if (slot.isEmpty()) {
            slot.type = type;
            slot.count = std::min(count, maxStack);
            count -= slot.count;
            if (count <= 0) return true;
        }
    }
    return count <= 0;
}

bool Inventory::removeItem(int slotIndex, int count) {
    if (slotIndex < 0 || slotIndex >= (int)m_slots.size()) return false;
    auto& slot = m_slots[slotIndex];
    if (slot.count < count) return false;
    slot.count -= count;
    if (slot.count <= 0) {
        slot.type = GameConstants::ITEM_NONE;
        slot.count = 0;
    }
    return true;
}

bool Inventory::hasItem(GameConstants::ItemType type, int count) const {
    int total = 0;
    for (const auto& slot : m_slots) {
        if (slot.type == type) {
            total += slot.count;
            if (total >= count) return true;
        }
    }
    return total >= count;
}

void Inventory::swapSlots(int a, int b) {
    if (a >= 0 && a < (int)m_slots.size() && b >= 0 && b < (int)m_slots.size())
        std::swap(m_slots[a], m_slots[b]);
}

void Inventory::setSelectedSlot(int slot) {
    m_selectedSlot = std::max(0, std::min(slot, GameConstants::HOTBAR_SLOTS - 1));
}

ItemStack& Inventory::getSelectedItem() {
    return m_slots[m_selectedSlot];
}

void Inventory::scrollHotbar(int direction) {
    m_selectedSlot += direction;
    if (m_selectedSlot < 0) m_selectedSlot = GameConstants::HOTBAR_SLOTS - 1;
    if (m_selectedSlot >= GameConstants::HOTBAR_SLOTS) m_selectedSlot = 0;
}

bool Inventory::useSelected(Player& player) {
    auto& held = getSelectedItem();
    if (held.isEmpty()) return false;

    auto& db = ItemDatabase::getInstance();
    const auto& info = db.getItem(held.type);

    if (info.isConsumable) {
        if (info.type == GameConstants::ITEM_HEALTH_POTION) {
            player.heal(50.0f);
            std::cout << "[Inventory] Drank Health Potion, restoring 50 HP." << std::endl;
        } else if (info.type == GameConstants::ITEM_SPEED_POTION) {
            player.applySpeedBoost(10.0f);
            std::cout << "[Inventory] Drank Speed Potion, speed boosted for 10s." << std::endl;
        } else {
            player.restoreEnergy((float)info.energyRestore);
            std::cout << "[Inventory] Ate " << info.name << " restoring " << info.energyRestore << " energy." << std::endl;
        }
        removeItem(m_selectedSlot, 1);
        return true;
    }
    return false;
}

ItemStack& Inventory::getSlot(int index) { return m_slots[index]; }
const ItemStack& Inventory::getSlot(int index) const { return m_slots[index]; }

void Inventory::renderSlot(SDL_Renderer* renderer, const ItemStack& item,
                            int x, int y, int size, bool selected) {
    // Outer highlight if selected
    if (selected) {
        SDL_SetRenderDrawColor(renderer, 255, 215, 0, 255);
        SDL_Rect highlight = {x - 3, y - 3, size + 6, size + 6};
        SDL_RenderFillRect(renderer, &highlight);
    }

    // Inner slot background
    SDL_SetRenderDrawColor(renderer, 30, 30, 35, 230);
    SDL_Rect bg = {x, y, size, size};
    SDL_RenderFillRect(renderer, &bg);
    SDL_SetRenderDrawColor(renderer, 70, 70, 75, 255);
    SDL_RenderDrawRect(renderer, &bg);

    if (item.isEmpty()) return;

    // Render simple colored icon matching the item type
    using namespace GameConstants;
    Uint8 r = 180, g = 180, b = 180;
    switch (item.type) {
        case ITEM_WOOD:          r = 139; g = 90;  b = 43;  break; // Brown
        case ITEM_STONE:         r = 120; g = 120; b = 125; break; // Gray
        case ITEM_IRON_ORE:      r = 190; g = 110; b = 80;  break; // Reddish Iron
        case ITEM_CHEST_KEY:     r = 210; g = 180; b = 80;  break; // Brass
        case ITEM_WILD_BERRY:    r = 220; g = 40;  b = 90;  break; // Pink/crimson
        case ITEM_ANCIENT_RELIC: r = 80;  g = 220; b = 190; break; // Cyan Glow
        case ITEM_COIN:          r = 255; g = 215; b = 0;   break; // Bright Gold
        case ITEM_HEALTH_POTION: r = 240; g = 60;  b = 60;  break; // Red
        case ITEM_SPEED_POTION:  r = 60;  g = 180; b = 240; break; // Light Blue
        default: break;
    }

    SDL_SetRenderDrawColor(renderer, r, g, b, 255);
    int pad = size / 5;
    SDL_Rect icon = {x + pad, y + pad, size - pad * 2, size - pad * 2};
    SDL_RenderFillRect(renderer, &icon);

    // Render simple stack indicator
    if (item.count > 1) {
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 180);
        SDL_Rect barBg = {x + size - 14, y + size - 10, 12, 8};
        SDL_RenderFillRect(renderer, &barBg);
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 200);
        int dotCount = std::min(item.count, 6);
        for (int i = 0; i < dotCount; i++) {
            SDL_Rect dot = {x + size - 12 + i * 2, y + size - 7, 1, 3};
            SDL_RenderFillRect(renderer, &dot);
        }
    }
}

void Inventory::renderHotbar(SDL_Renderer* renderer) {
    int size = 52;
    int pad = 6;
    int totalW = GameConstants::HOTBAR_SLOTS * (size + pad) - pad;
    int sx = (GameConstants::SCREEN_WIDTH - totalW) / 2;
    int sy = GameConstants::SCREEN_HEIGHT - size - 20;

    // Hotbar panel bg
    SDL_SetRenderDrawColor(renderer, 15, 15, 18, 200);
    SDL_Rect panel = {sx - 10, sy - 10, totalW + 20, size + 20};
    SDL_RenderFillRect(renderer, &panel);
    SDL_SetRenderDrawColor(renderer, 80, 80, 90, 255);
    SDL_RenderDrawRect(renderer, &panel);

    for (int i = 0; i < GameConstants::HOTBAR_SLOTS; i++) {
        int x = sx + i * (size + pad);
        renderSlot(renderer, m_slots[i], x, sy, size, i == m_selectedSlot);
    }
}

void Inventory::renderFullInventory(SDL_Renderer* renderer) {
    if (!m_isOpen) return;

    // Semi-transparent backdrop
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 140);
    SDL_Rect backdrop = {0, 0, GameConstants::SCREEN_WIDTH, GameConstants::SCREEN_HEIGHT};
    SDL_RenderFillRect(renderer, &backdrop);

    int size = 54;
    int pad = 6;
    int cols = 8;
    int rows = GameConstants::MAX_INVENTORY_SLOTS / cols;
    int totalW = cols * (size + pad) - pad;
    int totalH = rows * (size + pad) - pad;
    int sx = (GameConstants::SCREEN_WIDTH - totalW) / 2;
    int sy = (GameConstants::SCREEN_HEIGHT - totalH) / 2;

    // Full Panel Bg
    SDL_SetRenderDrawColor(renderer, 25, 25, 28, 240);
    SDL_Rect panel = {sx - 20, sy - 20, totalW + 40, totalH + 40};
    SDL_RenderFillRect(renderer, &panel);
    SDL_SetRenderDrawColor(renderer, 95, 95, 105, 255);
    SDL_RenderDrawRect(renderer, &panel);

    for (int i = 0; i < (int)m_slots.size(); i++) {
        int col = i % cols;
        int row = i / cols;
        int x = sx + col * (size + pad);
        int y = sy + row * (size + pad);
        renderSlot(renderer, m_slots[i], x, y, size, i == m_selectedSlot);
    }

    // Interactive Hint
    SDL_SetRenderDrawColor(renderer, 200, 200, 205, 200);
    SDL_Rect hintBar = {sx, sy + totalH + 10, totalW, 4};
    SDL_RenderFillRect(renderer, &hintBar);
}
