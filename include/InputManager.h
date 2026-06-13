#ifndef INPUT_MANAGER_H
#define INPUT_MANAGER_H

#include <SDL2/SDL.h>
#include "Vector2D.h"
#include <unordered_map>

class InputManager {
public:
    static InputManager& getInstance();

    void update();
    void handleEvent(const SDL_Event& event);

    // Keyboard
    bool isKeyDown(SDL_Scancode key) const;
    bool isKeyPressed(SDL_Scancode key) const;   // Just pressed this frame
    bool isKeyReleased(SDL_Scancode key) const;  // Just released this frame

    // Mouse
    bool isMouseButtonDown(int button) const;
    bool isMouseButtonPressed(int button) const;
    bool isMouseButtonReleased(int button) const;
    Vector2D getMousePosition() const;
    int getMouseWheelDelta() const;

    // Quit event
    bool isQuitRequested() const { return m_quitRequested; }

private:
    InputManager();
    ~InputManager() = default;
    InputManager(const InputManager&) = delete;
    InputManager& operator=(const InputManager&) = delete;

    // Keyboard state
    const Uint8* m_keyboardState;
    std::unordered_map<SDL_Scancode, bool> m_prevKeyState;

    // Mouse state
    int m_mouseX, m_mouseY;
    Uint32 m_mouseState;
    Uint32 m_prevMouseState;
    int m_mouseWheelDelta;

    bool m_quitRequested;
};

#endif // INPUT_MANAGER_H
