# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## What This Is

LK8000 is a **tactical flight computer** — aerial navigation software for gliders, paragliders, and other aircraft. It is a multi-platform C++20 application targeting Linux (desktop, Raspberry Pi, Kobo e-reader), Windows (PC/PCX64), and Android.

## Build Commands

The primary build system is GNU Make. The default target is `LINUX`.

```bash
# Standard Linux build (builds then installs to ~/LK8000/)
make -j4 TARGET=LINUX install

# Debug build (no optimization, includes debug symbols)
make -j4 TARGET=LINUX DEBUG=y install

# Windows (requires MinGW cross-compiler)
make -j4 TARGET=PC distrib       # 32-bit, creates Distrib/PC/
make -j4 TARGET=PCX64 distrib    # 64-bit

# Kobo e-reader (greyscale, memory canvas — no OpenGL)
make -j4 TARGET=KOBO KOBO=/opt/kobo-rootfs

# Raspberry Pi (cross-compile)
make -j4 TARGET=PI PI=/opt/rpi-rootfs

# Greyscale Linux build (like Kobo)
make -j4 TARGET=LINUX OPENGL=n GREYSCALE=y DITHER=y clean
make -j4 TARGET=LINUX OPENGL=n GREYSCALE=y DITHER=y install

# Android
cd android-studio && ./gradlew app:assembleRelease
```

Key build flags: `OPENGL=[y|n]`, `GLES2=[y|n]`, `USE_EGL=[y|n]`, `USE_SDL=[y|n]`, `ENABLE_MESA_KMS=[y|n]`, `CLANG=[y|n]`, `OPENMP=[y|n]`.

Compiler settings: C++20, `-O2 -g`, `-Wall -Werror` (warnings are errors).

**Required packages (Ubuntu/Debian):**
```
libzzip-dev libfreetype6-dev libboost-dev libpng-dev libgeographiclib-dev
xsltproc imagemagick fonts-dejavu zip libgl1-mesa-dev libegl1-mesa-dev
libx11-dev libasound2-dev libsndfile1-dev zlib1g-dev libcurl4-openssl-dev libjpeg-dev
```

## Other Make Targets

```bash
make cppcheck   # Static analysis with cppcheck
make tags       # Generate TAGS file for editor navigation
make clean      # Remove build artifacts (also clears Distrib/ for bitmap rebuilds)
```

## Testing

LK8000 uses **Doctest** (in `lib/doctest/`). Tests are compiled into the application binary. There is no separate test runner; run the application itself to execute embedded tests. Contest/task validation tests live in `Common/Utils/TestContest/`.

## Code Architecture

### Directory Layout

- `Common/Source/` — All C++ source (see subsystems below)
- `Common/Header/` — Shared headers (`.h`, `.hpp`)
- `Common/Data/` — Icons, bitmaps, XML dialog definitions, config samples
- `android-studio/` — Android Gradle project and Java wrapper
- `build/` — Make include fragments (per-subsystem: `xcs_screen.mk`, `lk_os.mk`, etc.)
- `lib/` — Git submodules: `doctest`, `glm`, `json` (nlohmann)
- `Bin/<TARGET>/` — Object files (build output)
- `Distrib/<TARGET>/` — Installed/packaged output

### Major Subsystems (under `Common/Source/`)

| Directory | Purpose |
|---|---|
| `Screen/` | Canvas abstraction with multiple backends: OpenGL, GLES2, SDL, framebuffer, in-memory |
| `Window/` | Window/widget system (XCSoar-derived); custom drawing framework |
| `Dialogs/` | All dialog implementations; XML layouts in `Common/Data/` (`dlg*.xml`) |
| `Form/` | UI form controls (buttons, lists, data fields) |
| `Draw/` | Core rendering: overlays, gauges, traffic, symbols |
| `MapDraw/` | Map layer rendering: terrain, topology, airspace, waypoints |
| `Calc/` | Flight calculations engine; `Task/` subdirectory for task planning |
| `Comm/` | Serial/NMEA communication layer; device I/O |
| `Devices/` | Hardware device drivers (GPS, variometers, etc.) |
| `Airspace/` | Airspace parsing, boundary checking, sonar |
| `Waypoints/` | Waypoint/airfield database, search, parsing |
| `Terrain/` | Elevation data loading and lookup |
| `Topology/` | Topology layers via shapelib (roads, rivers, etc.) |
| `Logger/` | IGC flight logging |
| `Tracking/` | Live tracking via HTTP/CURL |
| `Sound/` | Audio playback (ALSA/libsndfile on Linux) |
| `Settings/` | Profile/configuration persistence |
| `NMEA/` | GPS sentence parsing and state |
| `OS/` | OS abstraction (Linux vs. Windows file/time/thread) |
| `Android/` | JNI glue code for Android |
| `xcs/` | XCSoar-derived support code: input handling, OS layer, screen primitives |
| `Library/` | Bundled third-party code (POCO threading, rapidxml, GLU tessellator) |
| `utils/` | Miscellaneous utility functions |

### Entry Points

- **Linux/Desktop:** `Common/Source/lk8000.cpp`
- **Android JNI:** `Common/Source/Android/Main.cpp`
- **Android Java:** `android-studio/app/src/main/java/org/LK8000/`

### Dialog System

Dialogs are defined in XML (`Common/Data/Dialogs/dlg*.xml`, with `_L` and `_P` suffixes for landscape/portrait variants) and implemented in `Common/Source/Dialogs/dlg*.cpp`. The `Form/` layer handles control binding. Dialog layout for overlays: landscape uses a right-side vertical strip; portrait uses a bottom panel.

### Platform Abstraction

Graphics backends are selected at compile time via Make flags. The `Screen/` layer exposes a unified canvas API regardless of whether the backend is OpenGL, GLES2, SDL, or a memory framebuffer. The `xcs/` subtree contains the XCSoar-derived OS and event abstractions. Android uses JNI bindings in `Common/Source/Java/` and `Common/Source/Android/`.

### Internationalisation

Strings use a custom i18n system. Translation files live in `Common/Data/Language/`. When adding new user-visible strings, follow existing patterns in nearby dialog code for `gettext`-style lookups.
