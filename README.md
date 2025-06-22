# Tetris (C++ & SFML)

A modern Tetris clone built with **C++**, **CMake**, and **SFML**.  
Cross-platform compatible and ideal for learning or extending classic game mechanics.

## ✨ Features

- ✔️ Classic Tetris gameplay
- 🧩 7 Tetromino shapes (I, J, L, O, S, Z, T)
- 🔄 Super Rotation System (SRS)
- 👻 Ghost piece
- 🕹️ Hold & Next queue system
- 🎨 Visual rendering via **SFML**
- 🧱 Modular codebase for easy extensions

## 📸 Screenshots

--- MAKE SCREENSHOTS WHEN GAME IS FINISHED ---

## 🛠️ Build & Run

### 📦 Requirements
- C++17 compatible compiler (e.g. `g++`, `clang`, MSVC)
- [CMake](https://cmake.org/) (>= 3.10)
- [SFML](https://www.sfml-dev.org/) (>= 3.0)

---

### 🔧 Building from Command Line (Linux/macOS/WSL)

```bash
git clone https://github.com/Tachidomaru/Tetris.git
cd Tetris
mkdir build && cd build
cmake ..
make
./tetris
```

## 🗂️ Project Strucure

├── src/           # Game logic
├── include/       # Header files
├── assets/        # Images, fonts, etc.
├── CMakeLists.txt # Build configuration
└── main.cpp       # Entry point

## 🎮 Controls

| Key           | Action                    |
|---------------|---------------------------|
| ← (Left)      | Move tetromino left       |
| → (Right)     | Move tetromino right      |
| ↑ (Up)        | Rotate clockwise          |
| ↓ (Down)      | Soft drop                 |
|   (Space)     | Hard drop                 |
| C             | Hold / Swap tetromino     |

## 📄 License

This project is licensed under the MIT License – see LICENSE for details.