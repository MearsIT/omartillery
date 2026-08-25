# Bar-Widget Debug Status

**Date**: 2026-08-25  
**Status**: Bar icon not appearing - installation failing after lowercase rename

## Goal

Convert omartillery from panel plugin to bar-widget plugin so it shows an icon in the Omarchy bar (not IPC summon).

## User Preferences (Important!)

- **Always** installs plugins via Super+Space GUI (never manual mkdir/cp)
- **Always** removes plugins via Super+Space (never rm -rf)
- Wants bar icon to click, not terminal commands
- Uses Omarchy desktop environment with Quickshell

## What Works

- ✅ Other bar-widgets work (weather widget confirmed working)
- ✅ Plugin installs and prompts for icon placement (center/left/right)
- ✅ No QML syntax errors (`qmllint` passes)
- ✅ manifest.json is valid JSON

## Current Problem

**Icon doesn't appear in bar** - bar placement dialog shows but no "ART" button appears.

**Latest error** (from journalctl):
```
WARN qml: Plugin widget omartillery failed: file:///home/jandal/.config/omarchy/plugins/omartillery/BarWidget.qml: File name case mismatch
```

After renaming to lowercase `barWidget.qml`, plugin **won't install at all**.

## What We've Tried

### 1. Initial Conversion (worked but no icon)
- Created BarWidget.qml using BarWidget component
- Changed manifest.json to `"kinds": ["bar-widget"]`
- Changed entryPoints from `"bar-widget"` to `"barWidget"`
- Plugin installed, asked for placement, but no icon

### 2. Simplified BarWidget (still no icon)
- Removed all panel loading logic
- Made ultra-minimal: just WidgetButton with text "ART"
- Changed emoji 🎮 to plain text "ART"
- Still no icon after installation

### 3. Removed qmldir (still no icon)
- Backed up qmldir as qmldir.bak
- Removed module definition that Panel.qml imports
- Still no icon

### 4. Removed Panel.qml (still no icon)
- Backed up Panel.qml as Panel.qml.bak
- Left only barWidget.qml in plugin
- Still no icon

### 5. Case Mismatch Investigation (current)
- Logs showed "File name case mismatch" error
- Tried renaming `BarWidget.qml` → `barWidget.qml` (lowercase)
- **Plugin won't install now**

## Current File Structure

Repository root:
```
barWidget.qml           # Minimal bar-widget (lowercase)
manifest.json           # Bar-widget configuration
GameModel.qml           # Game state (not used by minimal widget)
qmldir.bak              # Backed up module definition
Panel.qml.bak           # Backed up panel content
qml/                    # Game components
assets/                 # Fonts, sounds, images
```

## Working Reference

**gameoflife plugin** (confirmed working):
- Has `BarWidget.qml` (capital B)
- No qmldir file
- manifest.json:
  ```json
  "kinds": ["bar-widget"],
  "entryPoints": { "barWidget": "BarWidget.qml" }
  ```

## Current Code

**barWidget.qml**:
```qml
import QtQuick
import Quickshell
import qs.Ui

BarWidget {
  id: root

  WidgetButton {
    bar: root.bar
    text: "ART"
    tooltipText: "Artillery"
  }
}
```

## Next Steps to Try Tomorrow

1. **Revert to capital BarWidget.qml** (like gameoflife)
2. **Remove .bak files** from repo (might confuse validation)
3. **Check Git case sensitivity** settings
4. **Copy exact gameoflife structure** and adapt
5. **Enable Omarchy debug logging** to see exact error

## Git State

Current commit: `0bbff06` - lowercase barWidget.qml (won't install)  
Need to revert or try different approach tomorrow.

---

**Resume command**: Read this file, check latest commit, decide next approach.
