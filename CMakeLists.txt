cmake_minimum_required(VERSION 3.10)
project(TetrisSFML)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(SFML 3 REQUIRED COMPONENTS Graphics Window System Audio)

add_executable(tetris
    src/main.cpp
    src/Board.cpp
    src/Tetromino.cpp
    src/GhostTetromino.cpp
    src/TetrominoBag.cpp
    src/Renderer.cpp
    src/GameState.cpp
    src/UIHelpers.cpp
    src/HighscoreManager.cpp
)

target_include_directories(tetris PRIVATE include)

target_link_libraries(tetris PRIVATE
    SFML::Graphics
    SFML::Window
    SFML::System
    SFML::Audio
)

add_custom_command(TARGET tetris POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/assets $<TARGET_FILE_DIR:tetris>/assets
)