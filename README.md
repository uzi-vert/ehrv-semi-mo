# CSOPESY — Operating System Concepts Simulator

[![C++17](https://img.shields.io/badge/C++-17-blue.svg)](https://isocpp.org/)
[![CMake](https://img.shields.io/badge/CMake-3.16+-064F8C?logo=cmake)](https://cmake.org/)
[![OpenGL](https://img.shields.io/badge/OpenGL-3.3+-5586A4?logo=opengl)](https://www.opengl.org/)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

A graphical operating system concepts simulator built with C++17, OpenGL, and Dear ImGui. CSOPESY provides a desktop environment UI for visualizing and interacting with core OS concepts like process scheduling, memory management, and system state.

## Technology Stack

| Category          | Technology                                      |
|-------------------|-------------------------------------------------|
| Language          | C++17 (with `CMAKE_CXX_STANDARD_REQUIRED ON`)   |
| Build System      | CMake 3.16+                                     |
| Windowing         | GLFW 3.3.9                                      |
| Graphics API      | OpenGL 3.3 Core Profile                         |
| OpenGL Loader     | Glad (generated, local in `external/glad/`)     |
| GUI Framework     | Dear ImGui (docking branch)                     |
| Image Loading     | stb_image (single-header, via `nothings/stb`)   |

## Project Architecture

CSOPESY follows a phased application lifecycle with a singleton-driven UI manager pattern:

```
┌─────────────────────────────────────────────────────────┐
│                    GUIApplication                       │
│  ┌──────────┐  ┌──────────┐  ┌──────────┐  ┌─────────┐  │
│  │Bootstrap │→ │KernelInit│→ │  System  │→ │MainLoop │  │
│  │(GLFW/GUI)│  │(UIMgr)   │  │Services  │  │(Render) │  │
│  └──────────┘  └──────────┘  └──────────┘  └─────────┘  │
│                         │                               │
│                    ┌────▼────┐                          │
│                    │Shutdown │                          │
│                    └─────────┘                          │
└─────────────────────────────────────────────────────────┘
```

- **`GUIApplication`** — Top-level orchestrator managing the 5-phase lifecycle
- **`UIManager`** — Singleton that registers, shows, hides, and renders all windows
- **`AWindow`** — Abstract base class for all UI windows (visibility, name, ImGui frame management)
- **`UIConfig`** — Singleton handling DPI-aware scaling based on monitor resolution
- **`Desktop`** — Concrete window component rendering wallpaper + system clock overlay
- **Process States** — `RUNNING`, `READY`, `WAITING`, `TERMINATED` enum for process simulation

## Project Structure

```
csopesy/
├── CMakeLists.txt          # Top-level build configuration
├── external/
│   └── glad/               # Locally generated Glad OpenGL loader
├── src/
│   ├── main.cpp            # Entry point
│   ├── core/
│   │   ├── GUIApplication.h/.cpp   # Lifecycle orchestrator
│   │   ├── AWindow.h/.cpp          # Abstract window base class
│   │   ├── UIManager.h/.cpp        # Singleton window manager
│   │   ├── UIConfig.h/.cpp         # DPI scaling configuration
│   │   └── Types.h                 # Shared types (ProcessState)
│   └── components/
│       └── desktop/
│           ├── Desktop.h/.cpp      # Desktop UI component
├── assets/                 # Static assets (wallpaper, etc.)
├── build/                  # Build output (gitignored)
└── openspec/               # OpenSpec change tracking
```

## Getting Started

### Prerequisites

- **CMake** 3.16 or later
- A **C++17** capable compiler (MSVC, GCC, Clang)
- OpenGL 3.3+ driver support

### Build

```bash
# Configure
cmake -B build -S .

# Build
cmake --build build

# Run
./build/csopesy
```

Dependencies (GLFW, Dear ImGui, stb) are fetched automatically via CMake's `FetchContent`. Glad is included locally under `external/glad/`.

## Key Features

- **Desktop Environment UI** — Full-screen desktop with wallpaper rendering and fallback background
- **Process State Simulation** — Type-safe `ProcessState` enum (`RUNNING`, `READY`, `WAITING`, `TERMINATED`)
- **DPI-Aware Scaling** — Automatic UI scaling based on primary monitor resolution
- **Singleton Window Manager** — Centralized `UIManager` for lifecycle and rendering of all windows
- **Extensible Window System** — Abstract `AWindow` base class for adding new UI panels
- **ImGui Integration** — Debug-style UI with docking branch support

## Development Workflow

The project uses [OpenSpec](https://opencode.ai) for tracking proposed changes. Change specifications live in `openspec/changes/` and are archived after implementation.

### Lifecycle Phases

| Phase           | Description                                     |
|-----------------|-------------------------------------------------|
| Bootstrap       | GLFW window creation, OpenGL context, ImGui init |
| Kernel Init     | Monitor detection, scaling, callback registration |
| System Services | Desktop creation, window registration, show     |
| Main Loop       | Event polling, frame rendering, desktop draw    |
| Shutdown        | ImGui shutdown, window destroy, GLFW terminate  |

## Coding Standards

- **Language**: C++17 with extensions disabled (`CMAKE_CXX_EXTENSIONS OFF`)
- **Naming**: `PascalCase` for classes, `camelCase` for members, `UPPER_CASE` for constants/enums
- **Namespaces**: All code under `csopesy::` namespace
- **Headers**: `#pragma once` include guard style
- **Memory**: Prefer `std::shared_ptr` for shared ownership of UI components
- **Singletons**: Static local initialization in `getInstance()` methods (thread-safe in C++11+)

## Testing

No test framework is currently configured. The project is in early development.

## Contributing

1. Fork the repository
2. Create a feature branch
3. Make changes following existing code conventions
4. Verify the project builds cleanly
5. Submit a pull request

For architecture guidance, refer to the `AWindow` and `UIComponent` patterns in `src/core/`.

## License

Distributed under the MIT License. See [LICENSE](LICENSE) for details.
