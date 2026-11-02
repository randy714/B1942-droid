# B1942-droid 🪖

# B1942-droid

B1942-droid is a standalone native engine runtime and portrait tactical launcher for Battlefield 1942 on Android devices.

## Features
- **Standalone Native Engine**: Includes C++ Refractor archive loader (`.rfa`) and OpenGL ES 2.0 3D renderer pipeline (`libb1942_engine.so`).
- **Tactical Portrait Launcher**: Military-inspired Dark Espresso UI (`#0D0B0A`, `#1E1612`, `#D48C46`).
- **10-Button Touch Control Engine**: Features octagonal action buttons and a circular red primary fire button.
- **Interactive Control Layout Editor**: Full drag, drop, and resize configuration for all 10 on-screen buttons saved to persistent preferences.
- **External Data Linking**: Runs engine natively while parsing game data archives directly from storage (`/sdcard/B1942-droid/game/`).

## Engine Requirements
- Operating System: Android 8.0 (API level 26) up to Android 16 (API level 36).
- System Memory: 3 GB to 8 GB RAM.
- System Architecture: ARM64-v8a or ARMEABI-v7a.

## Setup Instructions
1. Download and install `B1942-droid-v2.5.apk` from the GitHub Release section.
2. Grant Storage Access permissions upon launching the launcher.
3. Copy your PC Battlefield 1942 game folder containing `BF1942.exe` and `Mods/bf1942/Archives/*.rfa` files into `/sdcard/B1942-droid/game/`.
4. Tap **Controls Editor** to adjust layout positions if needed, then tap **Launch Game**.
