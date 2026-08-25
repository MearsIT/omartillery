# Bar-Widget Conversion

**Date**: 2026-08-25  
**Version**: 0.2.0

## Summary

Converted omartillery from a **panel plugin** to a **bar-widget plugin** to provide a persistent icon in the Omarchy bar.

## What Changed

### Plugin Type
- **Before**: Panel plugin (summoned via IPC command)
- **After**: Bar-widget plugin (icon in Omarchy bar)

### User Experience
- **Before**: No bar presence, launch via `omarchy-shell shell summon omartillery '{}'`
- **After**: 🎮 icon in Omarchy bar, click to open/close game

### Files Added
- **BarWidget.qml** - Bar icon and popup management

### Files Modified
- **manifest.json** - Changed from `"panel"` to `"bar-widget"`, entry point now `BarWidget.qml`
- **Panel.qml** - Added bar-widget integration properties
- **Version** - Bumped to 0.2.0

## Architecture

```
BarWidget.qml (in Omarchy bar)
  ├─ WidgetButton (🎮 icon)
  └─ Loader
      └─ Panel.qml (game content, shown as popup)
```

### How It Works

1. **BarWidget.qml** creates a button in the Omarchy bar with a 🎮 icon
2. User clicks the icon
3. `toggle()` method shows/hides the Panel.qml popup
4. **Panel.qml** contains the full game (unchanged game logic)

### Integration Properties

BarWidget injects these properties into Panel:
- `barWidget` - Reference to the BarWidget
- `button` - Reference to the bar button
- `bar` - Reference to the Omarchy bar
- `hostWidget` - Reference to the host widget

## Installation

**With Omarchy GUI (Recommended)**:
1. Press Super+Space
2. Search for "Add Plugin"
3. Enter: `https://github.com/MearsIT/omartillery`
4. Plugin installs automatically
5. 🎮 icon appears in your Omarchy bar

**To Remove**:
1. Press Super+Space
2. Search for "Remove Plugin"
3. Select "Omartillery"

## User Preference

The user (Martin) prefers:
- ✅ Super+Space menu for plugin management (never manual mkdir/cp)
- ✅ Bar icon to launch plugins (not IPC commands)
- ✅ Click icon to open/close (not terminal commands)

This bar-widget design matches those preferences.

## Migration from v0.1.0

If you had the panel version (v0.1.0) installed:

1. Remove the old panel version:
   - Super+Space → Remove Plugin → Omartillery

2. Install the new bar-widget version:
   - Super+Space → Add Plugin → `https://github.com/MearsIT/omartillery`

3. The 🎮 icon will appear in your bar

## Technical Details

### manifest.json Changes
```json
{
  "version": "0.2.0",
  "kinds": ["bar-widget"],
  "entryPoints": {
    "bar-widget": "BarWidget.qml"
  }
}
```

### Panel.qml Changes
Added properties for bar-widget integration:
```qml
property bool popoutSwitchClosing: false
property var barWidget: null
property var button: null
property var bar: null
property var hostWidget: null
```

Updated `closeForPopoutSwitch()` to track transition state.

### BarWidget.qml (New File)
- Uses `Quickshell.Ui.WidgetButton` for bar icon
- Uses `Loader` to load Panel.qml dynamically
- Manages open/close/toggle lifecycle
- Injects bar references into Panel

## Icon

Currently using emoji: 🎮

Future enhancement: Replace with custom pixel-art icon matching C64 theme.

## Compatibility

- **Omarchy**: Requires Quickshell framework
- **QML**: Uses Qt Quick 6.x
- **Previous Versions**: Not compatible with v0.1.0 (different plugin type)

## Next Steps

- [ ] Test in Omarchy with Super+Space installation
- [ ] Create custom pixel-art icon for bar
- [ ] Add configuration options (icon size, position)
- [ ] Document keyboard shortcuts
