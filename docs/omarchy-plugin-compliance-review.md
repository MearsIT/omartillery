# Omarchy Plugin Compliance Review

**Date:** 2026-08-25  
**Repository:** `/home/jandal/Projects/omartillery`  
**Branch:** `feat/artillery-duel-game`  
**Review Type:** Architecture compliance against Omarchy plugin standards  
**Verdict:** ❌ **Not ready** — Fundamental architecture mismatch

---

## Executive Summary

Artillery Duel is currently a well-structured standalone Qt/QML desktop game, but has **zero Omarchy plugin compatibility**. Every architectural layer (build system, application lifecycle, QML root, asset loading, C++ integration) is structured for standalone execution and must be completely rebuilt for the Omarchy plugin model.

**Key Findings:**
- 16 violations identified (4 P0 Critical, 5 P1 High, 4 P2 Moderate, 3 P3 Low)
- No plugin manifest or directory structure
- Uses standalone executable with QGuiApplication
- ApplicationWindow root incompatible with plugin surfaces
- Binary resource bundling instead of filesystem paths
- Missing plugin lifecycle API

---

## Standards Reviewed

This review compares the codebase against Omarchy plugin development requirements from:

1. **https://omarchyplugins.com/develop.html** — Core plugin development guidelines
2. **https://github.com/basecamp/omarchy/blob/quattro/shell/README.md** — Shell plugin architecture
3. **https://github.com/basecamp/omarchy/tree/quattro/shell/plugins** — Reference implementations

---

## P0 — Critical Architecture Violations

### #1: Standalone Executable Architecture
**File:** `src/main.cpp:6-21`  
**Issue:** Creates QGuiApplication and QQmlApplicationEngine  
**Confidence:** 100%

Omarchy plugins run **inside the omarchy-shell process**, not as separate executables. The `main.cpp` creates its own QGuiApplication/QQmlApplicationEngine, which is fundamentally incompatible.

**Fix:** Remove `src/main.cpp` entirely. Convert to pure QML/JS plugin loaded by the shell. C++ game logic must be exposed via a QML module that the shell can import, not a standalone binary.

---

### #2: Missing manifest.json
**File:** `(missing)`  
**Issue:** No manifest.json file in repository root  
**Confidence:** 100%

Every Omarchy plugin **must** have a `manifest.json` at its root defining plugin metadata and entry points.

**Fix:** Create `manifest.json` with required fields:

```json
{
  "schemaVersion": 1,
  "id": "com.yourname.artillery-duel",
  "name": "Artillery Duel",
  "version": "0.1.0",
  "author": "Your Name",
  "license": "MIT",
  "description": "Turn-based artillery combat game",
  "kinds": ["panel"],
  "entryPoints": {
    "panel": "Panel.qml"
  }
}
```

**Required manifest fields:**
- `schemaVersion: 1`
- `id` (reverse-domain notation, e.g., `io.github.username.plugin-name`)
- `name`, `version`, `author`, `license`, `description`
- `kinds` array (at least one valid type)
- `entryPoints` object mapping kinds to QML files

---

### #3: ApplicationWindow Root Component
**File:** `qml/main.qml:6`  
**Issue:** Uses ApplicationWindow as root instead of plugin component  
**Confidence:** 100%

Plugins receive surfaces from the shell (panel windows, overlay surfaces, bar sections). Using `ApplicationWindow` creates independent window chrome that conflicts with the shell's window management.

**Fix:** Replace `qml/main.qml`'s `ApplicationWindow` with plugin-appropriate root:
- For **panel** kind: bare `Item` or `Rectangle` as root
- For **overlay** kind: fullscreen `Item`
- For **bar-widget** kind: compact `Item` that fits in bar section

---

### #4: Executable Build Target
**File:** `CMakeLists.txt:56`  
**Issue:** Builds qt_add_executable instead of QML module plugin  
**Confidence:** 100%

```cmake
qt_add_executable(artillery-duel src/main.cpp)
```

Omarchy loads plugins as QML modules into the shared shell process. Building a standalone executable defeats the plugin architecture.

**Fix:** Remove the executable target. Keep only the `qt_add_qml_module(artillery_core ...)` and ensure it's loadable as `import ArtilleryDuel 1.0` from within the shell.

---

## P1 — High-Impact Plugin Contract Violations

### #5: No Plugin Installation Path Structure
**File:** `(structure)`  
**Issue:** Flat repository structure incompatible with plugin layout  
**Confidence:** 100%

Plugins must live in `~/.config/omarchy/plugins/<plugin-id>/` with all resources self-contained.

**Fix:** Reorganize to plugin layout:

```
~/.config/omarchy/plugins/com.yourname.artillery-duel/
├── manifest.json
├── Panel.qml  (or whatever entryPoints specifies)
├── README.md
├── LICENSE
├── Model.js (optional business logic)
└── assets/  (sprites, sounds, fonts)
```

---

### #6: Missing LICENSE File
**File:** `(missing)`  
**Issue:** No LICENSE file in repository  
**Confidence:** 100%

Omarchy plugin guidelines state "LICENSE – recommended licensing file."

**Fix:** Add LICENSE file with chosen license (manifest should declare the same license type).

---

### #7: No moduleName Declarations
**File:** `qml/*.qml`  
**Issue:** QML files lack moduleName pragma  
**Confidence:** 100%

Every plugin QML file must declare `pragma ModuleName: "<plugin-id>"` matching the manifest.json `id` field.

**Fix:** Add to top of each QML file:

```qml
pragma ModuleName: "com.yourname.artillery-duel"
```

---

### #8: QML Module URI Format Violation
**File:** `CMakeLists.txt:19-20`  
**Issue:** URI 'ArtilleryDuel' doesn't follow reverse-domain notation  
**Confidence:** 100%

```cmake
qt_add_qml_module(artillery_core
    URI ArtilleryDuel
    ...
```

Plugins must use reverse-domain notation matching the manifest `id`.

**Fix:** Change to:

```cmake
qt_add_qml_module(artillery_core
    URI com.yourname.artillery-duel
    ...
```

---

### #9: Missing Panel Lifecycle Methods
**File:** `(missing)`  
**Issue:** Panel plugins require lifecycle API  
**Confidence:** 100%

If targeting `"kinds": ["panel"]`, the plugin must expose these methods:
- `open()`
- `close()`
- `toggle()`
- `closeForPopoutSwitch()`

**Fix:** Add these methods to the root Panel.qml component or use a Controller pattern with Loader as shown in reference plugins.

---

## P2 — Moderate Plugin Compliance Issues

### #10: No Plugin-Oriented Documentation
**File:** `(missing plugin README)`  
**Issue:** README describes standalone app, not plugin installation  
**Confidence:** 100%

While README.md exists, it covers building a standalone app. Plugin README should cover installation via `omarchy plugin add`, configuration, and usage within Omarchy shell.

**Fix:** Rewrite README for plugin audience, or create separate PLUGIN-README.md.

---

### #11: C++ Engine Not Plugin-Compatible
**File:** `src/*.cpp`  
**Issue:** C++ game logic compiled as static library linked to executable  
**Confidence:** 100%

The src/*.cpp files (gameengine, physics, AI) are compiled into a static library linked to an executable. Plugins can't carry executables.

**Fix:** Either:
- **(a)** Port all logic to QML/JavaScript, or
- **(b)** Build the C++ as a **QML extension plugin** (.so shared library) that the shell can dlopen. This requires registering C++ types with `qmlRegisterType<GameEngine>(...)` in a plugin init function.

---

### #12: Binary Assets in qrc Bundle
**File:** `CMakeLists.txt:42-50`  
**Issue:** Assets embedded in compiled resource bundle  
**Confidence:** 75%

The CMake qrc embedding packages assets into the executable:

```cmake
RESOURCES
    assets/tank_left.png
    assets/tank_right.png
    assets/terrain_pattern.png
    ...
```

Omarchy plugins load from `~/.config/omarchy/plugins/<id>/` filesystem paths.

**Fix:** Remove assets from qrc, reference them via relative filesystem paths in QML (e.g., `source: "assets/tank_left.png"`).

---

### #13: QML Resource URL Paths
**File:** `qml/main.qml:17-27`  
**Issue:** SoundEffect uses qrc:// resource URLs  
**Confidence:** 75%

```qml
SoundEffect {
    source: "qrc:/ArtilleryDuel/assets/sounds/fire.wav"
}
```

Plugins can't use qrc compiled into an executable.

**Fix:** Change to relative filesystem paths:

```qml
SoundEffect {
    source: "assets/sounds/fire.wav"
}
```

---

## P3 — Low-Priority / Advisory

### #14: Plugin ID Namespace Planning
**File:** `(id)`  
**Issue:** Need production plugin ID before publishing  
**Confidence:** 75%

Must choose a permanent reverse-domain ID (e.g., `io.github.yourname.artillery-duel` if hosting on GitHub). During development, `yourname.artillery-duel` is acceptable.

**Restriction:** Cannot use `omarchy.*` prefix (reserved for first-party plugins).

---

### #15: Optional Preview Image Missing
**File:** `(missing)`  
**Issue:** No preview.png for plugin gallery  
**Confidence:** 50%

A `preview.png` screenshot helps users discover the plugin in galleries.

**Fix:** Add preview.png showing the game in action (optional but recommended).

---

### #16: No Validation Workflow
**File:** `(validation)`  
**Issue:** Missing validation step in development workflow  
**Confidence:** 75%

Should run `omarchy plugin validate` and `qmllint` on QML files as part of development.

**Fix:** Add validation commands:

```bash
omarchy plugin validate
qmllint qml/*.qml
```

---

## Conversion Roadmap

To convert Artillery Duel from a standalone app to an Omarchy plugin:

### Phase 1: Plugin Structure (Required)

1. ✅ Create `manifest.json` with panel kind and proper reverse-domain ID
2. ✅ Add LICENSE file
3. ✅ Rename/refactor `qml/main.qml` → `Panel.qml` with bare Item root
4. ✅ Add moduleName pragmas to all QML files
5. ✅ Reorganize into `~/.config/omarchy/plugins/<id>/` layout

### Phase 2: Remove Standalone Architecture (Required)

6. ✅ Delete `src/main.cpp` (no standalone executable)
7. ✅ Remove `qt_add_executable` from CMakeLists.txt
8. ✅ Change ApplicationWindow to plugin-appropriate root component
9. ✅ Remove qrc resource bundling, use filesystem asset paths

### Phase 3: C++ Integration (Required if keeping C++ logic)

10. ✅ Convert C++ game engine to QML extension plugin (.so)
11. ✅ Register C++ types with qmlRegisterType in plugin init
12. ✅ Ensure shell can dlopen the extension without linking executable

### Phase 4: Plugin API (Required for panel kind)

13. ✅ Implement open(), close(), toggle(), closeForPopoutSwitch() lifecycle methods
14. ✅ Handle IPC summon/hide/toggle from omarchy-shell command

### Phase 5: Validation & Polish (Recommended)

15. ✅ Rewrite README for plugin installation/usage
16. ✅ Add preview.png
17. ✅ Run `omarchy plugin validate` and fix errors
18. ✅ Test: `omarchy plugin add <repo>`, enable in shell.json, summon via IPC

---

## Omarchy Plugin Kinds Reference

For future reference, the six available plugin types:

| Kind | Entry Point Key | QML File | Purpose |
|------|-----------------|----------|---------|
| bar-widget | barWidget | BarWidget.qml | Bar item |
| panel | panel | Panel.qml | Floating surface |
| overlay | overlay | Overlay.qml | Fullscreen surface |
| menu | menu | Menu.qml | Summoned menu |
| service | service | Service.qml | Headless singleton |
| bar | bar | Bar.qml | Full bar replacement |

**Recommended for Artillery Duel:** `panel` kind (floating game window summoned via IPC)

---

## Security Considerations

From Omarchy plugin guidelines:

> "Plugins share the long-running Omarchy shell process" and "run unsandboxed with your user permissions."

**Implications:**
- Must review all dependencies
- Avoid unnecessary privileges
- Never launch additional shell processes
- Plugin code runs with full user access

**Current status:** Artillery Duel has no external dependencies beyond Qt, so security posture is good for conversion.

---

## Testing Checklist

After conversion, validate with:

```bash
# 1. Validate manifest and structure
omarchy plugin validate

# 2. Lint QML files
qmllint Panel.qml

# 3. List plugins (confirm discovery)
omarchy plugin list --json

# 4. Test panel lifecycle
omarchy-shell shell summon com.yourname.artillery-duel '{}'
omarchy-shell shell hide com.yourname.artillery-duel
omarchy-shell shell toggle com.yourname.artillery-duel '{}'

# 5. Test enable/disable
# Edit ~/.config/omarchy/shell.json
omarchy-shell shell rescanPlugins

# 6. Test panel open/close/escape
# (manual interaction testing)
```

---

## Alternative: Remain Standalone

**If Artillery Duel should remain a standalone game:**

This review is **not applicable**. The current code is correct for a standalone Qt application. The Omarchy plugin model is fundamentally different and should only be pursued if:

1. You want the game integrated into Omarchy desktop shell
2. You want to summon it via IPC (`omarchy-shell shell summon artillery-duel`)
3. You want it to share the shell's process and lifecycle

Otherwise, continue developing as a standalone Qt/QML game and ignore this review.

---

## Verdict Summary

**Status:** Not ready for Omarchy plugin use  
**Violations:** 16 total (4 P0, 5 P1, 4 P2, 3 P3)  
**Architecture:** Standalone executable ❌ Plugin module ✅  
**Estimated Conversion Effort:** High (complete restructuring required)  

**Recommendation:**

- **If plugin form is intended:** Follow Conversion Roadmap, start with Phase 1
- **If standalone game is intended:** Mark this review as inapplicable, continue normal development

---

## Coverage

**Standards checked:**
- ✅ manifest.json schema requirements
- ✅ Plugin kinds and entry points
- ✅ File structure and naming conventions
- ✅ Lifecycle API for panel plugins
- ✅ Security model (unsandboxed execution)
- ✅ IPC contract
- ✅ Asset loading patterns
- ✅ QML module structure

**Not checked:**
- ❌ Runtime behavior in actual Omarchy shell (requires plugin structure first)
- ❌ QML binding performance
- ❌ Panel sizing/positioning in shell environment
- ❌ Multi-instance support (if `allowMultiple: true` in manifest)

**Protected artifacts respected:**
- docs/plans/ preserved (no deletion recommendations)

---

**Review completed:** 2026-08-25  
**Reviewer:** Claude Code (automated plugin compliance check)
