#include "InputManager.h"

InputManager& InputManager::getInstance() {
    static InputManager instance;
    return instance;
}

InputManager::InputManager()
    : m_keyboardState(nullptr), m_mouseX(0), m_mouseY(0),
      m_mouseState(0), m_prevMouseState(0), m_mouseWheelDelta(0),
      m_quitRequested(false)
{
    m_keyboardState = SDL_GetKeyboardState(nullptr);
}

void InputManager::update() {
    // Store previous key states for pressed/released detection
    int numKeys;
    const Uint8* state = SDL_GetKeyboardState(&numKeys);
    for (int i = 0; i < numKeys; i++) {
        SDL_Scancode sc = static_cast<SDL_Scancode>(i);
        m_prevKeyState[sc] = state[i];
    }

    // Store previous mouse state
    m_prevMouseState = m_mouseState;
    m_mouseWheelDelta = 0;
}

void InputManager::handleEvent(const SDL_Event& event) {
    switch (event.type) {
        case SDL_QUIT:
            m_quitRequested = true;
            break;
        case SDL_MOUSEWHEEL:
            m_mouseWheelDelta = event.wheel.y;
            break;
    }

    // Update mouse state
    m_mouseState = SDL_GetMouseState(&m_mouseX, &m_mouseY);
    m_keyboardState = SDL_GetKeyboardState(nullptr);
}

bool InputManager::isKeyDown(SDL_Scancode key) const {
    return m_keyboardState[key] != 0;
}

bool InputManager::isKeyPressed(SDL_Scancode key) const {
    auto it = m_prevKeyState.find(key);
    bool wasPressedBefore = (it != m_prevKeyState.end()) ? it->second : false;
    return m_keyboardState[key] && !wasPressedBefore;
}

bool InputManager::isKeyReleased(SDL_Scancode key) const {
    auto it = m_prevKeyState.find(key);
    bool wasPressedBefore = (it != m_prevKeyState.end()) ? it->second : false;
    return !m_keyboardState[key] && wasPressedBefore;
}

bool InputManager::isMouseButtonDown(int button) const {
    return (m_mouseState & SDL_BUTTON(button)) != 0;
}

bool InputManager::isMouseButtonPressed(int button) const {
    return (m_mouseState & SDL_BUTTON(button)) && !(m_prevMouseState & SDL_BUTTON(button));
}

bool InputManager::isMouseButtonReleased(int button) const {
    return !(m_mouseState & SDL_BUTTON(button)) && (m_prevMouseState & SDL_BUTTON(button));
}

Vector2D InputManager::getMousePosition() const {
    return Vector2D(static_cast<float>(m_mouseX), static_cast<float>(m_mouseY));
}

int InputManager::getMouseWheelDelta() const {
    return m_mouseWheelDelta;
}
