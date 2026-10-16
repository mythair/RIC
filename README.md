# Roblox Icon Changer

A lightweight Windows utility that replaces the Roblox player window icon and lets you set a custom window title.

It runs in the background, watches for the Roblox window, and applies your icon (`myicon.ico`) + custom title every 500ms. Useful for customization, streaming (hide "Roblox" from taskbar / window title), or theming.

Built with plain Win32 C++ — no dependencies, no installer.

## Features

- Replaces Roblox window icon (big + small, including child windows)
- Sets a custom window title (default: `ROBLOX`)
- Forces taskbar refresh via `WM_SETICON` + `RedrawWindow`
- Assigns a distinct `AppUserModel_ID` (`MyRobloxApp`) so the taskbar doesn't reuse the stock Roblox icon
- Simple GUI: textbox + Apply button to change the title live
- Background updater thread (500 ms poll via `FindWindowA`)

## Requirements

- Windows 10/11 (x64 recommended)
- Visual Studio 2022 (toolset v143) or compatible to build
- A file named `myicon.ico` next to the `.exe`

## Usage

1. Download the latest release from the [Releases](../../releases) page.
2. Place your custom icon as `myicon.ico` in the same folder as `Roblox icon changer.exe`.
   - Must contain 16x16 and 32x32 sizes.
3. Launch `Roblox icon changer.exe` **before or after** starting Roblox.
4. Type your desired window title in the textbox (default is `ROBLOX`) and click **Apply Title**.
5. Leave the app running while you play. Close it to stop overriding.

> If you see "Failed to load myicon.ico!", the `.ico` file is missing or in the wrong folder.

## Build from source

1. Clone the repo:
   ```bash
   git clone https://github.com/<your-username>/roblox-icon-changer.git
   ```
2. Open `Roblox icon changer.sln` in Visual Studio 2022.
3. Set configuration to `Release | x64`.
4. Build → output in `x64/Release/`.
5. Copy your `myicon.ico` next to the built `.exe` and run.

## How it works

`Roblox icon changer.cpp` does the following:

- `main()` loads `myicon.ico` (32px → `hBig`, 16px → `hSmall`) and creates a small Win32 GUI (`RobloxIconChanger` class).
- `IconUpdater()` runs on a detached thread: `FindWindowA(NULL, "Roblox")` (fallback: current custom title) → `SetTaskbarIcon()` + `SetWindowTextA()` every 500 ms.
- `SetTaskbarIcon()` sends `WM_SETICON` (big/small), updates `GCLP_HICON` / `GCLP_HICONSM`, recurses into child windows, forces a redraw, and sets `PKEY_AppUserModel_ID` to `MyRobloxApp` via `SHGetPropertyStoreForWindow`.
- `WndProc()` handles the textbox (`ID_TEXTBOX`) and Apply button (`ID_BUTTON`) to update `currentTitle` live.

## Limitations / Notes

- Windows only.
- Matches the window by exact title (`"Roblox"` or your custom title). If Roblox changes its window title scheme, the app may need an update.
- Does not modify Roblox files or memory — window-icon/title override only.
- Some overlays / recording software may still show the original icon.

## Disclaimer

Unofficial third-party tool. Not affiliated with, endorsed, or sponsored by Roblox Corporation. Use at your own risk.

## License

Add a license (e.g. MIT) here if you plan to accept contributions. Otherwise all rights reserved by default.
