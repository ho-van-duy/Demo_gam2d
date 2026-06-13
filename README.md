# Shadow Engine 2D

A professional 2D game base project built with **C++17** and **SDL2**.

## Project Structure

```
game2D/
├── include/              # Header files
│   ├── Animation.h       # Sprite animation system
│   ├── Camera.h          # Camera with smooth follow & shake
│   ├── Constants.h       # Game configuration
│   ├── Game.h            # Main game class
│   ├── GameObject.h      # Base entity class
│   ├── InputManager.h    # Keyboard & mouse input
│   ├── Player.h          # Player with platformer physics
│   ├── TextureManager.h  # Texture loading & caching
│   ├── TileMap.h         # Tile-based level system
│   ├── Timer.h           # Delta time & FPS
│   └── Vector2D.h        # 2D math vector
├── src/                  # Source files
│   ├── main.cpp          # Entry point
│   ├── Game.cpp          # Game loop & rendering
│   ├── Timer.cpp         # Frame timing
│   ├── TextureManager.cpp
│   ├── InputManager.cpp
│   ├── Animation.cpp
│   ├── Camera.cpp
│   ├── GameObject.cpp
│   ├── Player.cpp
│   └── TileMap.cpp
├── assets/               # Game resources
│   ├── textures/
│   ├── fonts/
│   ├── audio/
│   └── maps/
├── Makefile
├── build.bat
└── README.md
```

## Features

- **Game State Machine** - Menu, Playing, Paused, Game Over
- **Platformer Physics** - Gravity, friction, coyote time, jump buffering
- **Camera System** - Smooth lerp follow, screen shake, zoom
- **Tile Map** - File loading, collision detection, frustum culling
- **Animation System** - Named states, sprite sheet support
- **Input Manager** - Press/release detection, mouse support
- **Texture Manager** - Singleton cache, sprite sheet rendering

## Controls

| Key | Action |
|-----|--------|
| A/D, ←/→ | Move left/right |
| Space, W, ↑ | Jump |
| ESC | Pause / Quit |
| Enter | Start game |
| F1 | Toggle debug mode |

## Build

```bash
# Using build script
build.bat

# Using Makefile (with MinGW make)
mingw32-make

# Run
game2d.exe
```

## Adding Your Own Assets

1. Place sprite sheets in `assets/textures/`
2. Load them via `TextureManager::loadTexture("id", "path", renderer)`
3. Use `drawFrame()` for animated sprites
