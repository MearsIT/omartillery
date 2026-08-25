# Artillery Duel

A retro-faithful recreation of Artillery Duel (Xonox, 1983) as a Qt/QML
desktop game for Linux. Turn-based artillery combat with physics-based
projectile motion, wind, and a C64 pixel-art aesthetic.

## Modes

- **2-Player** — hot-seat: players alternate turns on one machine
- **vs AI** — single player against a formulaic AI opponent (beatable, not perfect)

## Gameplay

Adjust **angle** (0-90 degrees) and **power** (0-100), then fire. Wind
changes every turn. First side to knock the opposing gun's health to zero
wins.

- Sliders or `Left`/`Right` (angle), `Up`/`Down` (power)
- `Space` or the Fire button to fire

## Requirements

- Linux with glibc 2.34+
- CMake 3.21+
- A C++17 compiler
- Qt 6.4 or newer with the following modules:
  - `Qt6::Quick` / `Qt6::QuickControls2`
  - `Qt6::Multimedia`
- For sound playback, the Qt Multimedia FFmpeg backend
  (`ffmpeg` on Arch: `pacman -S ffmpeg`). The game degrades gracefully
  and plays silently if audio libraries are missing.

Tested against Qt 6.11 on Omarchy (Arch-based).

## Build

```sh
cmake -B build -G Ninja
cmake --build build
./build/artillery-duel
```

## Run Tests

```sh
ctest --test-dir build --output-on-failure
```

## Project Layout

```
src/     C++ game logic (engine, physics, AI)
qml/     QML UI (board, controls, screens)
assets/  Pixel-art sprites, sounds, fonts
tests/   Qt Test unit/integration tests
```

## Assets and Attribution

- Pixel-art sprites, terrain pattern, and sound effects are generated
  for this project (no external license restrictions).
- The display font is [Press Start 2P](https://fonts.google.com/specimen/Press+Start+2P)
  by Cody Boisclair, licensed under the SIL Open Font License 1.1 — see
  `assets/fonts/OFL.txt`.
