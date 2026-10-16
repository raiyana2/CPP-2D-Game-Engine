# CMPUT 350 Project Assignment 1B

Current design, state flow, contributions status and AI prompts are documented in
[project.txt](project.txt). The Part 1A notes below are historical; their test results
and description of limited AI use do not describe this Part 1B update.

## Build and run

```sh
cmake -S . -B build
cmake --build build
./build/Project1
```

Press **5**, then **1** to start. Move with **A/D** or **Left/Right**, and shoot with
**Space**. Inserting a coin displays "CREDIT 1 - PRESS 1 TO START"; the player
appears after starting. The window fits the desktop while retaining a 768×1024
logical game canvas. `gUseCollisionDemo` in `galaga/main.cpp` selects the optional collision demo.

CMake before 3.28 uses the bundled Linux x86-64 SFML libraries intended for the lab.
CMake 3.28 or newer fetches SFML 3.1.0; a first configure requires network access.
The lab build still needs to be verified on a lab machine.

## Part 1B verification

```sh
cmake --build build
ctest --test-dir build --output-on-failure
```

Verified on macOS on October 16, 2026: `cmake --build build -j 4` succeeded;
`ctest --test-dir build --output-on-failure` passed 1/1 test executable (all four
regression groups). No graphical playthrough or Linux lab build was performed.

Automated regression coverage: rotated rectangle/circle/line geometry; finite,
tangent, collinear, and zero-length line contacts; Bezier boundaries and distance
traversal; notification subscription changes and killed listeners; game-over cleanup,
restart, empty incoming waves, and final-enemy/player simultaneous death.
These tests use a headless EngineView substitute for state checks. They do not test
the SFML window, pixel output, actual keyboard events, or the complete engine loop.

AI was used for Part 1B code and documentation. See project.txt for the actual
prompts, affected files, and a draft technical reflection. Team names/contributions
and the teammates' own review/reflection still require their input.

---

# CMPUT 350 Project Assignment 1A

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

## AI Usage Disclosure


### Areas Where AI Was Used

AI was used in a limited supporting role during this project, mainly for:

- Clarifying C++ concepts and syntax
- Debugging compiler and build errors
- Checking small helper functions and edge cases
- Understanding smart pointers and memory management
- Interpreting Valgrind output

AI suggestions were reviewed and tested before being used in the final code.


---

## Example AI Prompts Used

Examples of prompts used during development include:

- "What concepts of C++ do I need to understand for this project?"
- "How should I implement a bounding box collision check?"
- "Why is `mLoc` not declared in my Enemy class?"
- "How do I limit the player to two bullets using `weak_ptr`?"
- "How should a bullet update its previous and current position?"
- "How should `Rect::operator&=` represent a null intersection?"
- "Can you write the documentation?"

Some conversations also involved asking AI to review small sections of code for correctness or explain errors produced by the compiler.

---

## Reflection on AI Usage

AI was most useful as a learning and debugging tool rather than as a replacement for implementing the project.

It helped explain unfamiliar C++ concepts and made it easier to understand why certain parts of the engine were designed in a particular way. It was useful for understanding smart pointers, inheritance, collision handling and debugging build errors.

However, AI suggestions were not always immediately appropriate for the assignment. Some suggestions needed to be adjusted after comparing them with the project specification and professor clarifications.

The team reviewed and tested the code that was influenced by AI and made sure that both team members understood the final implementation.

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
