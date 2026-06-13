#include "Item.h"

ItemDatabase& ItemDatabase::getInstance() {
    static ItemDatabase instance;
    return instance;
}

void ItemDatabase::init() {
    using namespace GameConstants;
    m_empty = {ITEM_NONE, "Empty", "", 0, false, 0};

    auto add = [this](ItemType t, const std::string& name, const std::string& desc,
                      int stack, bool consumable = false, int energy = 0) {
        m_items[(int)t] = {t, name, desc, stack, consumable, energy};
    };

    // Gatherable materials
    add(ITEM_WOOD,          "Wood",           "A clean branch suitable for crafting.", 99);
    add(ITEM_STONE,         "Stone",          "A rough pebble. Heavy but solid.", 99);
    add(ITEM_IRON_ORE,      "Iron Ore",       "Raw metallic ore. Glows with potential.", 99);

    // Sandbox consumables / items
    add(ITEM_WILD_BERRY,    "Wild Berry",     "Sweet forest berry. Restores 15 energy.", 99, true, 15);
    add(ITEM_CHEST_KEY,     "Old Chest Key",  "Rusty brass key. Might unlock something ancient.", 5);
    add(ITEM_ANCIENT_RELIC, "Ancient Relic",  "A shiny mysterious artifact of a lost era.", 10);
    add(ITEM_COIN,          "Ancient Coin",   "Gold coin found buried. Shines brightly.", 999);
    add(ITEM_HEALTH_POTION, "Health Potion",  "A red potion. Restores 50 HP.", 99, true, 0);
    add(ITEM_SPEED_POTION,  "Speed Potion",   "A blue potion. Temporarily increases movement speed.", 99, true, 0);
}

const ItemData& ItemDatabase::getItem(GameConstants::ItemType type) const {
    auto it = m_items.find((int)type);
    if (it != m_items.end()) return it->second;
    return m_empty;
}
