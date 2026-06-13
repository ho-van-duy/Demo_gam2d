/**
 * Wildwood Sandbox - A 3/4 Perspective Top-Down Sandbox Exploration Game
 * Built with C++17 and SDL2
 *
 * Controls:
 *   WASD / Arrows  - Move (3/4 Top-down perspective)
 *   Space / LClick - Interact with objects (Signs, Campfires, Chests, Resources)
 *   Q / F          - Consume/Use active hotbar slot
 *   1-8            - Select hotbar slot
 *   Mouse Wheel    - Scroll hotbar
 *   E / Tab        - Toggle Inventory Overlay
 *   ESC            - Pause Game
 *   Enter          - Start Game from Menu
 *   F1             - Toggle Debug Mode
 */

#include "Game.h"
#include "Constants.h"
#include <iostream>

int main(int argc, char* argv[]) {
    Game& game = Game::getInstance();

    if (!game.init(GameConstants::WINDOW_TITLE,
                   GameConstants::SCREEN_WIDTH,
                   GameConstants::SCREEN_HEIGHT)) {
        std::cerr << "Failed to initialize!" << std::endl;
        return 1;
    }

    std::cout << "==========================================================" << std::endl;
    std::cout << "  Wildwood Sandbox - Game Running!" << std::endl;
    std::cout << "==========================================================" << std::endl;
    std::cout << "  WASD/Arrows    - Move around the world" << std::endl;
    std::cout << "  Space / Click  - Interact with signs, chests, campfires" << std::endl;
    std::cout << "  Q / F          - Eat consumable items from hotbar" << std::endl;
    std::cout << "  1-8 / Wheel    - Select items on your hotbar" << std::endl;
    std::cout << "  E / Tab        - Open full 24-slot inventory bag" << std::endl;
    std::cout << "  ESC            - Pause the game" << std::endl;
    std::cout << "  F1             - Toggle Debug grid (Yellow highlights targeted tile)" << std::endl;
    std::cout << "==========================================================" << std::endl;

    game.run();
    game.clean();
    return 0;
}
