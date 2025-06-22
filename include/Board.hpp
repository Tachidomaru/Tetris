#pragma once

#include <array>
#include "Config.hpp"
#include "Tetromino.hpp"

class Board 
{
public:
    Board();

    int getCellValue(int x, int y) const;

    bool isValidPosition(const Tetromino& tetromino) const;
    bool isGameOver(const Tetromino& tetromino) const;

    void lockTetromino(const Tetromino& tetromino);

    int clearCompletedLines();
    
    void resetBoard();

private:
    std::array<std::array<int, BOARD_WIDTH>, BOARD_HEIGHT> grid;
};