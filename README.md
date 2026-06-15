# Shadow Engine 2D

A 2D game framework developed in **C++17** using **SDL2**, created as a personal learning project to explore game engine architecture, rendering systems, platformer physics, animation pipelines, and software design patterns.

---

## About The Project

Shadow Engine 2D is a reusable foundation for developing 2D platformer games.

The project was built to practice:

* Object-Oriented Programming (OOP)
* Game Loop Architecture
* State Machine Design
* Resource Management
* Collision Detection
* Camera Systems
* Animation Systems
* Platformer Physics
* SDL2 Development
* Software Engineering Principles

The goal is not only to create a playable game but also to understand how a small-scale game engine is structured internally.

---

## Technologies

* C++17
* SDL2
* SDL2_image
* SDL2_ttf
* SDL2_mixer
* MinGW
* Makefile Build System

---

## Project Structure

```text
game2D/
├── include/
│   ├── Animation.h
│   ├── Camera.h
│   ├── Constants.h
│   ├── Game.h
│   ├── GameObject.h
│   ├── InputManager.h
│   ├── Player.h
│   ├── TextureManager.h
│   ├── TileMap.h
│   ├── Timer.h
│   └── Vector2D.h
│
├── src/
│   ├── main.cpp
│   ├── Game.cpp
│   ├── Timer.cpp
│   ├── TextureManager.cpp
│   ├── InputManager.cpp
│   ├── Animation.cpp
│   ├── Camera.cpp
│   ├── GameObject.cpp
│   ├── Player.cpp
│   └── TileMap.cpp
│
├── assets/
│   ├── textures/
│   ├── audio/
│   ├── fonts/
│   └── maps/
│
├── build.bat
├── Makefile
└── README.md
```

---

## Core Features

### Game State Machine

Supports multiple game states:

* Main Menu
* Playing
* Pause
* Game Over

This structure makes the project easier to scale and maintain.

---

### Platformer Physics

Implemented mechanics include:

* Gravity
* Friction
* Acceleration
* Velocity
* Jump Buffering
* Coyote Time

These features help create responsive and enjoyable movement.

---

### Camera System

Features:

* Smooth Follow
* Lerp Movement
* Camera Shake
* Zoom Support

---

### Tile Map System

Supports:

* Tile-based level design
* Collision detection
* Map loading
* Frustum culling

---

### Animation System

Features:

* Sprite Sheet Animation
* Named Animation States
* Frame Timing Control
* State Switching

---

### Resource Management

TextureManager provides:

* Centralized texture loading
* Resource caching
* Reduced memory usage

---

### Input Management

Handles:

* Keyboard Input
* Mouse Input
* Press / Hold / Release Detection

---

## Controls

| Key           | Action            |
| ------------- | ----------------- |
| A / D         | Move Left / Right |
| ← / →         | Move Left / Right |
| W / ↑ / Space | Jump              |
| ESC           | Pause / Quit      |
| Enter         | Start Game        |
| F1            | Debug Mode        |

---

## Build Instructions

### Windows (MinGW)

```bash
mingw32-make
```

or

```bash
build.bat
```

### Run

```bash
game2d.exe
```

---

## Learning Objectives

This project was developed to gain practical experience in:

* Game Programming
* Engine Architecture
* Data Structures
* Memory Management
* Design Patterns
* Software Development Workflow
* Git & GitHub Collaboration

---

## Third-Party Assets

This project uses third-party assets for educational purposes.

Credits to their respective creators.

### Graphics & Animations

* Author: [AUTHOR NAME]
* Source: [ITCH.IO LINK]

Usage follows the license terms provided by the original creator.

---

## AI Assistance Disclosure

Artificial Intelligence tools were used during the development process as learning and productivity aids.

Examples include:

* Code explanation
* Debugging assistance
* Documentation writing
* Architecture discussions
* Refactoring suggestions

All design decisions, implementation choices, testing, and final integration were performed by the project author.

---

## Author

**Hồ Văn Duy**

* Information Technology Student
* University of Da Nang
* GitHub: https://github.com/ho-van-duy

---

## License

This repository is published for educational and portfolio purposes.

Source code belongs to the project author.

Third-party assets remain the property of their respective creators and are subject to their own licenses.
