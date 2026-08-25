# Artillery Duel

An Omarchy plugin bringing classic turn-based artillery combat to your desktop shell. A retro-faithful recreation of Artillery Duel (Xonox, 1983) with physics-based projectile motion, wind, and C64 pixel-art aesthetic.

## Plugin Type

**Panel** — Floating game window summoned via IPC

## Installation

### From Source Repository

```sh
omarchy plugin add https://github.com/jandal/artillery-duel
```

### Manual Installation

1. Clone or copy this repository to `~/.config/omarchy/plugins/io.github.jandal.artillery-duel/`
2. Enable the plugin by adding it to `~/.config/omarchy/shell.json`:

```json
{
  "plugins": {
    "io.github.jandal.artillery-duel": {
      "enabled": true
    }
  }
}
```

3. Restart Omarchy shell or run:

```sh
omarchy-shell shell rescanPlugins
```

## Usage

### Summon the Game Panel

```sh
omarchy-shell shell summon io.github.jandal.artillery-duel '{}'
```

### Hide the Panel

```sh
omarchy-shell shell hide io.github.jandal.artillery-duel
```

### Toggle Visibility

```sh
omarchy-shell shell toggle io.github.jandal.artillery-duel '{}'
```

You can bind these commands to keyboard shortcuts in your Omarchy configuration.

## Game Modes

- **2-Player** — Hot-seat: players alternate turns on one machine
- **vs Computer** — Single player against a formulaic AI opponent

## Gameplay

Adjust **angle** (0-90 degrees) and **power** (0-100), then fire. Wind changes every turn. First side to knock the opposing gun's health to zero wins.

### Controls

- Sliders or `Left`/`Right` arrows to adjust angle
- `Up`/`Down` arrows to adjust power
- `Space` or the Fire button to fire

## Requirements

### Omarchy Shell

This plugin runs inside the Omarchy shell process and requires:

- Omarchy 4.0+ (Qt 6.4+ based shell)
- Linux with glibc 2.34+

### Audio (Optional)

For sound effects, Qt Multimedia requires FFmpeg or GStreamer:

```sh
# Arch Linux
pacman -S ffmpeg

# Ubuntu/Debian
apt install ffmpeg
```

The game degrades gracefully and plays silently if audio libraries are missing.

## Development

### Building the QML Extension Plugin

```sh
cmake -B build -G Ninja
cmake --build build
```

This builds `libartillery_core.so` — the QML extension plugin that Omarchy shell loads.

### Testing

```sh
ctest --test-dir build --output-on-failure
```

### Validation

Validate plugin structure and manifest:

```sh
omarchy plugin validate

# Lint QML files
qmllint qml/*.qml
```

## Project Structure

```
manifest.json           Plugin metadata and entry points
Panel.qml              Main panel component (entry point)
qml/                   QML UI components
src/                   C++ game logic (engine, physics, AI) built as QML extension
assets/                Pixel-art sprites, sounds, fonts
LICENSE                MIT License
```

## Plugin Architecture

Artillery Duel is structured as an Omarchy panel plugin:

- **Entry Point**: `Panel.qml` declared in `manifest.json`
- **QML Module**: `io.github.jandal.artillery-duel` (reverse-domain URI)
- **C++ Extension**: Game logic exposed via QML types (GameEngine, Player, PhysicsEngine, AIOpponent)
- **Lifecycle**: Implements panel API (`open()`, `close()`, `toggle()`, `closeForPopoutSwitch()`)
- **Assets**: Loaded from filesystem paths relative to plugin directory

The plugin shares the Omarchy shell's process and runs unsandboxed with user permissions.

## Assets and Attribution

- Pixel-art sprites, terrain pattern, and sound effects are generated for this project (no external license restrictions)
- Display font is [Press Start 2P](https://fonts.google.com/specimen/Press+Start+2P) by Cody Boisclair, licensed under SIL Open Font License 1.1 — see `assets/fonts/OFL.txt`

## License

MIT License — see `LICENSE` file

## Compatibility

Tested against Qt 6.11 on Omarchy (Arch-based). Plugin manifest schema version 1.
