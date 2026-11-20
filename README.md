

## Build and run

```sh
cmake --preset debug
cmake --build build
./build/Project1
```

---



## Project Overview

This project implements a basic object-oriented 2D game engine in C++ using SFML, along with a simple Galaga-style game built on top of the engine.

The engine supports:

- Game object lifecycle management
- Adding and removing objects during runtime
- Update and late-update loops
- Keyboard input handling
- Background and foreground rendering
- Axis-aligned bounding box collision detection
- Shared, weak, and raw pointer usage where appropriate

The Galaga-style game includes:

- A player that moves left and right
- Player shooting
- A maximum of two active player bullets
- 40 enemies
- Bullet/enemy collision handling
- Enemy removal after being hit
- A star background
- A bouncing-ball demo used to validate the engine


---

## Testing

The project was tested by:

- Running the Galaga-style game
- Verifying player movement
- Verifying player shooting
- Verifying the two-bullet limit
- Verifying bullet/enemy collisions
- Verifying enemy removal
- Running the bouncing-ball demo
- Performing clean CMake builds
- Running Valgrind to check for memory leaks

Valgrind reported:

```text
definitely lost: 0 bytes
indirectly lost: 0 bytes
possibly lost: 0 bytes
ERROR SUMMARY: 0 errors
```
