#ifndef INVENTORY_H
#define INVENTORY_H

#include "Item.h"
#include "Constants.h"
#include <SDL2/SDL.h>
#include <vector>

class Inventory {
public:
    Inventory();

    bool addItem(GameConstants::ItemType type, int count = 1);
    bool removeItem(int slotIndex, int count = 1);
    bool hasItem(GameConstants::ItemType type, int count = 1) const;
    void swapSlots(int a, int b);

    // Hotbar selection
    void setSelectedSlot(int slot);
    int getSelectedSlot() const { return m_selectedSlot; }
    ItemStack& getSelectedItem();
    void scrollHotbar(int direction);

    // Consumption / Usage
    bool useSelected(class Player& player);

    // Accessors
    ItemStack& getSlot(int index);
    const ItemStack& getSlot(int index) const;
    int getSlotCount() const { return static_cast<int>(m_slots.size()); }

    // Rendering
    void renderHotbar(SDL_Renderer* renderer);
    void renderFullInventory(SDL_Renderer* renderer);

    bool isOpen() const { return m_isOpen; }
    void toggleOpen() { m_isOpen = !m_isOpen; }

private:
    std::vector<ItemStack> m_slots;
    int m_selectedSlot;
    bool m_isOpen;

    void renderSlot(SDL_Renderer* renderer, const ItemStack& item,
                    int x, int y, int size, bool selected);
};

#endif
