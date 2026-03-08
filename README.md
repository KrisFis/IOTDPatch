# IOTDPatch

A patch for *The I of the Dragon* aimed at improving performance and user experience, implemented via DLL injection.

## Features

- **Faster UI loading** — Reduces load time when opening any UI from ~10 seconds to under half a second.
- **Improved camera sensitivity** — Separates sensitivity settings for camera rotation mode vs. auto-follow mode.

## How It Works

The patch is implemented through DLL injection. It works by reverse engineering the game's code and patching the `InputSystem`. The game uses DirectInput and dimap, which are intercepted and patched by the injected DLL.

## Installation

The DLL must be injected as the **first DLL** when the game starts. The recommended approach is to start the game in a suspended state, inject the DLL, then resume. You can use the author's [DLLInjector](https://github.com/KrisFis/DLLInjector) to do this easily.

> A dedicated launcher UI or integration of the injector into this repo may be added in the future.

## Configuration

Place `IOTDPatch.ini` in the same directory as the DLL. The file will be created with defaults on first run if not present.

```ini
[Input]
; Sensitivity when in "control mode" (rotating camera around the dragon)
ControlSensitivity=0.16

[Debugging]
; Spawns a console window with a real-time log from the DLL
EnableConsole=TRUE
; Writes all logs to IOTDPatch.log
EnableLogFile=TRUE
; Minimum logging level: Error > Warning > Info > Verbose > VeryVerbose
MinimalLogVerbosity=VeryVerbose
```

## Known Issues

- **Keybind UI** — When the patch is enabled, the keybind options UI may not display all inputs correctly. You can still update bindings through the UI (or do so before enabling the patch).
- **Rotation sensitivity** — The `ControlSensitivity` setting cannot be exposed through the game's native UI and must be configured via the INI file.

## Building

The project is written in pure C++ and uses CMake.

**Dependencies:**
- [MinHook](https://github.com/TsudaKageyu/minhook) — Hook and patching library
- [ASTD](https://github.com/your-username/astd) — Author's personal standard library

```bash
cmake -B build
cmake --build build
```

## License

See [LICENSE](LICENSE) for details.