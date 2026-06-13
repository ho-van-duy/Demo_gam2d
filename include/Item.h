#ifndef ITEM_H
#define ITEM_H

#include "Constants.h"
#include <string>
#include <unordered_map>

struct ItemData {
    GameConstants::ItemType type;
    std::string name;
    std::string description;
    int maxStack;
    bool isConsumable;
    int energyRestore;
};

struct ItemStack {
    GameConstants::ItemType type;
    int count;

    ItemStack() : type(GameConstants::ITEM_NONE), count(0) {}
    ItemStack(GameConstants::ItemType t, int c) : type(t), count(c) {}
    bool isEmpty() const { return type == GameConstants::ITEM_NONE || count <= 0; }
};

class ItemDatabase {
public:
    static ItemDatabase& getInstance();
    const ItemData& getItem(GameConstants::ItemType type) const;
    void init();
private:
    ItemDatabase() { init(); }
    std::unordered_map<int, ItemData> m_items;
    ItemData m_empty;
};

#endif
