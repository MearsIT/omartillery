# Omartillery Plugin - Current Status

**Last Updated**: 2026-08-25  
**Current Issue**: Bar-widget icon not appearing  
**Git Commit**: `d48636d`

---

## Current Goal

Get the Omarchy bar-widget working so a clickable icon appears in the Omarchy bar.

## User Preferences ⭐

- **Always** uses Super+Space GUI to add/remove plugins (never manual commands)
- **Wants** bar icon to click (not IPC terminal commands)
- **Uses** Omarchy desktop environment with Quickshell

---

## What's Working ✅

- Other bar-widgets work (weather widget confirmed)
- Plugin installs and asks for icon placement
- No QML syntax errors
- Asset paths all fixed (fonts, images, sounds)

## What's Broken ❌

**Icon doesn't appear in bar** even though:
- Plugin installs successfully
- Placement dialog appears (chose center)
- No visible errors in basic logs

### Error Found in Logs

```
WARN qml: Plugin widget omartillery failed: 
file:///home/jandal/.config/omarchy/plugins/omartillery/BarWidget.qml: 
File name case mismatch
```

After renaming to lowercase `barWidget.qml` → plugin won't install at all.

---

## What We've Tried (Chronologically)

### 1. Initial Bar-Widget Conversion
- Created BarWidget.qml from gameoflife reference
- Changed manifest to `"bar-widget"` kind
- Plugin installed but no icon appeared

### 2. Simplified BarWidget
- Removed all panel loading
- Made minimal: just WidgetButton with "ART" text
- Changed emoji 🎮 to plain text
- Still no icon

### 3. Removed qmldir
- Backed up as qmldir.bak
- Still no icon

### 4. Removed Panel.qml
- Backed up as Panel.qml.bak  
- Only barWidget.qml left
- Still no icon

### 5. Fixed manifest key
- Changed `"bar-widget"` → `"barWidget"` in entryPoints
- Still no icon, but now seeing case mismatch error

### 6. Tried Lowercase Filename
- Renamed `BarWidget.qml` → `barWidget.qml`
- **Plugin won't install now**

---

## Current File Structure

```
/home/jandal/Projects/omartillery/
├── barWidget.qml        # Minimal (lowercase - won't install)
├── manifest.json        # Bar-widget config
├── GameModel.qml        # Game logic (not used by minimal widget)
├── qmldir.bak          # Backed up module definition
├── Panel.qml.bak       # Backed up panel UI
├── qml/                # Game components
└── assets/             # Fonts, sounds, images
```

## Working Reference: gameoflife

**Confirmed working bar-widget plugin:**
- Filename: `BarWidget.qml` (capital B)
- No qmldir file
- manifest.json:
  ```json
  "kinds": ["bar-widget"],
  "entryPoints": { "barWidget": "BarWidget.qml" }
  ```
- Repo: https://github.com/guillechuma/gameoflife

---

## Current Code

### barWidget.qml (current - won't install)

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

### manifest.json

```json
{
  "schemaVersion": 1,
  "id": "omartillery",
  "name": "Omartillery",
  "version": "0.2.0",
  "author": "Martin Mears",
  "license": "MIT",
  "description": "Turn-based artillery combat game",
  "kinds": ["bar-widget"],
  "entryPoints": {
    "barWidget": "barWidget.qml"
  }
}
```

---

## Next Steps to Try

### Priority 1: Revert to Capital Case
gameoflife uses `BarWidget.qml` (capital) even though manifest key is lowercase.

```bash
git mv barWidget.qml BarWidget.qml
# Update manifest to: "barWidget": "BarWidget.qml"
```

### Priority 2: Remove .bak Files
Omarchy might be scanning all QML files and failing on Panel.qml.bak imports.

```bash
rm qmldir.bak Panel.qml.bak
```

### Priority 3: Check Git Case Sensitivity
Might be a Git tracking issue:

```bash
git config core.ignorecase  # Check setting
df -T /  # Check filesystem type
```

### Priority 4: Copy Exact gameoflife Structure
1. Download gameoflife BarWidget.qml
2. Adapt only button text/tooltip
3. Test if that exact structure works

### Priority 5: Enable Debug Logging
```bash
journalctl --user -u omarchy-shell -f
# Watch while installing plugin
```

---

## Historical Context (For Reference)

### Asset Path Fixes (Already Applied)

These were fixed in earlier commits:
- qml/Theme.qml: `"assets/"` → `"../assets/"` for fonts
- qml/GameBoard.qml: `"assets/"` → `"../assets/"` for terrain pattern
- qml/Explosion.qml, Projectile.qml, Tank.qml: Similar fixes
- GameModel.qml: Added missing `terrainHeights` property

**Rule**: Files in `qml/` subdirectory need `../` prefix to reach root-level `assets/`.

### Panel vs Bar-Widget

We started as a **panel plugin** (IPC summon), converted to **bar-widget** (bar icon) because user wants clickable icon, not terminal commands.

---

## Commands for Next Session

**Check current state:**
```bash
cd /home/jandal/Projects/omartillery
git status
git log --oneline -5
```

**Check installed plugin:**
```bash
ls -la ~/.config/omarchy/plugins/omartillery/
```

**Remove plugin:**
```
Super+Space → "Remove Plugin" → Omartillery
```

**Add plugin:**
```
Super+Space → "Add Plugin" → https://github.com/MearsIT/omartillery
```

**Check logs:**
```bash
journalctl --user -b --no-pager | grep -i "omarchy\|quickshell" | tail -50
```

---

## Success Criteria

✅ "ART" button appears in Omarchy bar center section  
✅ Clicking button opens game panel  
✅ Game works with proper graphics/sounds
