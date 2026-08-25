# Omartillery Plugin Structure Analysis

**Date**: 2026-08-25  
**Comparison Reference**: [gameoflife](https://github.com/guillechuma/gameoflife) - Working Omarchy Plugin

## Executive Summary

The omartillery plugin is **correctly architected as an Omarchy panel plugin**. The comparison with gameoflife revealed these are fundamentally different plugin types by design:

- **gameoflife**: Bar-widget plugin (icon in shell bar → popup panel)
- **omartillery**: Panel plugin (floating window summoned via IPC)

### Critical Issues Fixed

Three critical bugs were preventing the panel from displaying:

1. ✅ **Asset path resolution in Theme.qml** - Font path fixed from `assets/` to `../assets/`
2. ✅ **Missing terrainHeights property in GameModel** - Added property alias
3. ✅ **Asset path resolution in GameBoard.qml** - Terrain pattern path fixed

### Remaining: Plugin Installation

The plugin needs to be installed to `~/.config/omarchy/plugins/omartillery/` and enabled in shell configuration.

---

## Plugin Architecture Comparison

### Bar-Widget vs Panel Plugins

| Aspect | Bar-Widget (gameoflife) | Panel (omartillery) |
|--------|------------------------|---------------------|
| **Purpose** | Persistent UI in shell bar | Standalone floating window |
| **Visibility** | Always (bar icon) | On-demand (IPC summon) |
| **Files Required** | BarWidget.qml + Panel.qml | Panel.qml only |
| **Shell Integration** | Embedded in bar | Independent surface |
| **Lifecycle** | Loaded with shell | Loaded on first summon |
| **Entry Point** | `"bar-widget": "BarWidget.qml"` | `"panel": "Panel.qml"` |
| **Use Cases** | Status widgets, quick tools | Games, modals, complex UIs |

### Why These Differences Matter

The gameoflife reference is a **bar-widget** - it has:
- `BarWidget.qml` with icon button
- Popup panel on click
- Always visible in bar
- `manifest.json` with `"kinds": ["bar-widget"]`

Omartillery is a **panel** - it has:
- `Panel.qml` only (no BarWidget)
- Summoned via IPC command
- Hidden until explicitly summoned
- `manifest.json` with `"kinds": ["panel"]`

**Both architectures are valid** - they serve different purposes. Omartillery's panel design is appropriate for a game that doesn't need persistent bar presence.

---

## Bugs Fixed

### Bug #1: Font Path in Theme.qml ✅

**File**: `qml/Theme.qml:7`

**Problem**: Theme.qml is in the `qml/` subdirectory, but referenced assets using paths relative to the repository root.

```qml
// BEFORE (broken)
readonly property FontLoader fontLoader: FontLoader {
    source: "assets/fonts/c64_font.ttf"  // Resolves to qml/assets/fonts/...
}

// AFTER (fixed)
readonly property FontLoader fontLoader: FontLoader {
    source: "../assets/fonts/c64_font.ttf"  // Correct: goes up one level
}
```

**Impact**: Font loading failure caused QML runtime error, preventing panel display.

---

### Bug #2: Missing terrainHeights Property ✅

**File**: `GameModel.qml:35`

**Problem**: GameBoard.qml referenced `GameModel.terrainHeights`, but only `terrain` existed.

```qml
// GameBoard.qml:26
const heights = GameModel.terrainHeights;  // Property not defined!
```

**Fix**: Added property alias in GameModel.qml:

```qml
property var terrain: []
readonly property alias terrainHeights: root.terrain  // Added
```

**Impact**: Runtime error accessing undefined property prevented panel rendering.

---

### Bug #3: Terrain Pattern Path in GameBoard.qml ✅

**File**: `qml/GameBoard.qml:42`

**Problem**: Same as Theme.qml - path relative to qml/ directory instead of repository root.

```qml
// BEFORE (broken)
ctx.fillStyle = ctx.createPattern(
    "assets/terrain_pattern.png",  // Resolves to qml/assets/...
    "repeat");

// AFTER (fixed)
ctx.fillStyle = ctx.createPattern(
    "../assets/terrain_pattern.png",  // Correct path
    "repeat");
```

**Impact**: Canvas pattern loading failure, terrain rendering broken.

---

## Asset Path Rules

### Rule: Paths are Relative to the QML File Location

- **Panel.qml** (at repository root): Uses `"assets/sounds/fire.wav"` ✅
- **qml/Theme.qml** (in subdirectory): Must use `"../assets/fonts/..."` ✅
- **qml/GameBoard.qml** (in subdirectory): Must use `"../assets/..."` ✅

### Verified Asset References

All asset paths verified as correct:

| File | Line | Asset Path | Status |
|------|------|------------|--------|
| Panel.qml | 38 | `assets/sounds/fire.wav` | ✅ Correct (root level) |
| Panel.qml | 44 | `assets/sounds/explosion.wav` | ✅ Correct (root level) |
| qml/Theme.qml | 7 | `../assets/fonts/c64_font.ttf` | ✅ Fixed |
| qml/GameBoard.qml | 42 | `../assets/terrain_pattern.png` | ✅ Fixed |

---

## Installation Steps

### Step 1: Clean Build Artifacts (Optional)

```bash
cd /home/jandal/Projects/omartillery
rm -rf build/
```

The `build/` directory contains C++ artifacts from the pre-conversion architecture and is no longer needed.

### Step 2: Install Plugin to Omarchy

```bash
# Create plugins directory if it doesn't exist
mkdir -p ~/.config/omarchy/plugins/

# Copy plugin to Omarchy plugins directory
cp -r /home/jandal/Projects/omartillery ~/.config/omarchy/plugins/omartillery

# Optional: Remove .git directory to reduce clutter
rm -rf ~/.config/omarchy/plugins/omartillery/.git
```

### Step 3: Enable Plugin in Shell Configuration

Edit `~/.config/omarchy/shell.json`:

```json
{
  "plugins": {
    "omartillery": {
      "enabled": true
    }
  }
}
```

If the file doesn't exist, create it with the above content. If it exists, merge the plugin configuration.

### Step 4: Reload Omarchy

```bash
# Rescan plugins to load omartillery
omarchy-shell shell rescanPlugins
```

### Step 5: Test Plugin

```bash
# Summon the game panel
omarchy-shell shell summon omartillery '{}'

# Expected: Window appears with "ARTILLERY DUEL" main menu
# - Click "2 PLAYER" to start a game
# - Use angle/power controls and Space to fire
# - Game should work with proper graphics and sounds

# Hide the panel
omarchy-shell shell hide omartillery

# Toggle the panel
omarchy-shell shell toggle omartillery '{}'
```

---

## File Structure Verification

After installation, verify the structure:

```bash
cd ~/.config/omarchy/plugins/omartillery/
tree -L 2
```

Expected structure:

```
.
├── Panel.qml              # Main panel component
├── GameModel.qml          # Game state singleton
├── qmldir                 # QML module definition
├── manifest.json          # Plugin metadata
├── LICENSE
├── README.md
├── VERSION.NFO
├── qml/                   # QML components
│   ├── Explosion.qml
│   ├── GameBoard.qml
│   ├── GameOverScreen.qml
│   ├── HUD.qml
│   ├── MainMenu.qml
│   ├── PlayerControls.qml
│   ├── Projectile.qml
│   ├── RetroButton.qml
│   ├── Tank.qml
│   └── Theme.qml
└── assets/                # Game assets
    ├── fonts/
    │   └── c64_font.ttf
    ├── sounds/
    │   ├── explosion.wav
    │   └── fire.wav
    └── terrain_pattern.png
```

---

## Debugging Guide

### If Panel Still Doesn't Display

#### Check Plugin Loading

```bash
# List all loaded plugins
omarchy-shell shell listPlugins | grep omartillery

# Should show: omartillery (enabled)
```

#### Validate QML Syntax

```bash
cd ~/.config/omarchy/plugins/omartillery/

# Check all QML files for syntax errors
qmllint Panel.qml
qmllint GameModel.qml
qmllint qml/*.qml

# Expected: No errors
```

#### Monitor Omarchy Logs

```bash
# Watch logs for QML errors
journalctl --user -u omarchy-shell -f | grep -i "error\|qml\|omartillery"

# Then in another terminal, summon the panel
omarchy-shell shell summon omartillery '{}'
```

#### Verify Asset Files Exist

```bash
cd ~/.config/omarchy/plugins/omartillery/

# Check font
ls -lh assets/fonts/c64_font.ttf

# Check sounds
ls -lh assets/sounds/*.wav

# Check terrain pattern
ls -lh assets/terrain_pattern.png
```

#### Test with Minimal Panel

If the panel still won't display, create a minimal test:

```bash
# Backup current Panel.qml
cp Panel.qml Panel.qml.backup

# Create minimal test Panel.qml
cat > Panel.qml << 'EOF'
import QtQuick

Item {
    id: root
    anchors.fill: parent

    function open() { root.visible = true; }
    function close() { root.visible = false; }
    function toggle() { root.visible = !root.visible; }
    function closeForPopoutSwitch() { root.visible = false; }

    Rectangle {
        anchors.fill: parent
        color: "red"
        
        Text {
            anchors.centerIn: parent
            text: "OMARTILLERY TEST"
            font.pixelSize: 48
            color: "white"
        }
    }
}
EOF

# Rescan and test
omarchy-shell shell rescanPlugins
omarchy-shell shell summon omartillery '{}'

# If this works: The problem is in the game logic
# If this fails: The problem is in Omarchy plugin loading

# Restore original
mv Panel.qml.backup Panel.qml
```

---

## Comparison: What gameoflife Does Differently

### Simpler Module Structure

gameoflife uses **plain JavaScript import** instead of qmldir:

```qml
// gameoflife Panel.qml
import "GameOfLife.js" as Engine

// Uses: Engine.step(grid, cols, rows, wrap)
```

vs. omartillery's **QML singleton**:

```qml
// omartillery Panel.qml
import omartillery

// Uses: GameModel.property
```

**Trade-off**: gameoflife is simpler but stateless. Omartillery's singleton is more powerful for game state management.

### No Asset Dependencies

gameoflife has:
- No fonts
- No sounds  
- No images

Making it easier to debug path issues. Omartillery's rich assets require careful path management.

### Bar-Widget Architecture

gameoflife splits UI into:
- BarWidget.qml (bar icon + popup trigger)
- Panel.qml (popup content)

Omartillery uses Panel.qml only, appropriate for non-bar-widget plugins.

---

## Key Takeaways

1. ✅ **Omartillery is correctly structured as a panel plugin** - no need to change to bar-widget
2. ✅ **Critical bugs fixed** - asset paths and missing property resolved
3. ⏳ **Installation required** - plugin must be copied to `~/.config/omarchy/plugins/`
4. 📝 **Different plugin types** - gameoflife (bar-widget) vs omartillery (panel) serve different purposes
5. 🎯 **Asset paths matter** - Files in subdirectories need `../` prefix for root-level assets

---

## Next Steps

1. **Commit these fixes to git**:
   ```bash
   git add qml/Theme.qml qml/GameBoard.qml GameModel.qml
   git commit -m "fix: resolve asset paths and add terrainHeights property alias"
   ```

2. **Install the plugin**:
   ```bash
   cp -r /home/jandal/Projects/omartillery ~/.config/omarchy/plugins/omartillery
   ```

3. **Configure and test**:
   - Edit `~/.config/omarchy/shell.json` to enable plugin
   - Run `omarchy-shell shell rescanPlugins`
   - Test with `omarchy-shell shell summon omartillery '{}'`

4. **Optional: Create a bar-widget variant** if you want the game accessible from the shell bar

---

## Future Enhancements

Once the panel is working:

- [ ] Add keyboard shortcut for summon/hide
- [ ] Create `preview.png` for plugin gallery
- [ ] Add responsive scaling for different screen sizes
- [ ] Consider `"allowMultiple": true` for multiple game windows
- [ ] Publish to Omarchy plugin repository

---

## References

- **gameoflife plugin**: https://github.com/guillechuma/gameoflife
- **Omartillery repository**: https://github.com/MearsIT/omartillery
- **Omarchy documentation**: https://omarchy.org/docs/plugins
