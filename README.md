# AimSmoothLab
Offline C++ Win32 aim-smoothing simulator for Windows 11. No game access, no memory reads, no injected input, no anti-cheat bypass.

## Features
- Moving simulated bot with head target
- Click-to-assign activation key (keyboard or mouse buttons)
- Hold/toggle modes
- Smooth speed slider 1–100
- Settings saved in AimSmoothLab.ini beside executable

## Build without local C++ installation
Upload repository files to GitHub (including `.github/workflows/build.yml`), open Actions → Build Windows EXE → Run workflow, then download `AimSmoothLab-Windows11` artifact.

## Local build
Visual Studio Developer Command Prompt:
`cl /EHsc /std:c++17 /O2 main.cpp /link /SUBSYSTEM:WINDOWS user32.lib gdi32.lib comctl32.lib`

## Usage
Click inside arena to reposition the simulated crosshair. Press the configured activation key while the app is focused to follow the moving bot's head. The app never controls the actual system mouse cursor.
