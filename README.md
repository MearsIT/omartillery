# OmArtillery

An Omarchy plugin bringing classic turn-based artillery combat to your desktop shell. A retro-faithful recreation of Artillery Duel (Xonox, 1983) with physics-based projectile motion, wind, and C64 pixel-art aesthetic.

## Plugin Type

**Bar-Widget** — Click the 🎮 icon in your Omarchy bar to play

## Installation

**Using Omarchy GUI (Recommended)**:

1. Press **Super+Space** (Omarchy launcher)
2. Search for **"Add Plugin"**
3. Enter: `https://github.com/MearsIT/omartillery`
4. Plugin installs automatically
5. A 🎮 icon appears in your Omarchy bar

**Alternative - Command Line**:

```sh
omarchy plugin add https://github.com/MearsIT/omartillery
```

## Usage

**Click the 🎮 icon** in your Omarchy bar to open/close the game.

The game appears as a popup panel from the bar icon. Click the icon again to close.

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

### Pure QML/JavaScript Plugin

Omartillery is a **pure QML and JavaScript plugin** with no compilation required. All game logic is implemented in `GameModel.qml` - no C++ code, no build step, just install and run.

### Validation

```sh
# Validate plugin structure
omarchy plugin validate .

# Lint QML files  
qmllint *.qml qml/*.qml
```

### Testing Changes

After making changes:

```sh
# Reinstall
omarchy plugin add https://github.com/MearsIT/omartillery --force
omarchy-shell shell rescanPlugins

# Launch
omarchy-shell shell summon omartillery '{}'
```
## Project Structure

```
manifest.json           Plugin metadata and entry points
BarWidget.qml          Bar icon and popup manager (entry point)
Panel.qml              Game panel content (loaded by BarWidget)
qml/                   QML UI components
GameModel.qml          Game state singleton
assets/                Pixel-art sprites, sounds, fonts
LICENSE                MIT License
```

## Plugin Architecture

Omartillery is structured as an Omarchy bar-widget plugin:

- **Entry Point**: `BarWidget.qml` declared in `manifest.json`
- **Bar Icon**: 🎮 emoji (future: custom pixel-art icon)
- **Game Panel**: `Panel.qml` loaded as popup from BarWidget
- **QML Module**: `omartillery` with singleton GameModel
- **Pure QML/JS**: No C++ compilation required
- **Lifecycle**: Implements bar-widget API (`open()`, `close()`, `toggle()`, `closeForPopoutSwitch()`)
- **Assets**: Loaded from filesystem paths relative to plugin directory

The plugin shares the Omarchy shell's process and runs unsandboxed with user permissions.

## Assets and Attribution

- Pixel-art sprites, terrain pattern, and sound effects are generated for this project (no external license restrictions)
- Display font is [Press Start 2P](https://fonts.google.com/specimen/Press+Start+2P) by Cody Boisclair, licensed under SIL Open Font License 1.1 — see `assets/fonts/OFL.txt`

## License

MIT License — see `LICENSE` file

## Compatibility

Tested against Qt 6.11 on Omarchy (Arch-based). Plugin manifest schema version 1.
