#pragma once

#include <deque>
#include "Tetromino.hpp"

class TetrominoBag
{
public:
    TetrominoBag();

    const std::deque<TetrominoType>& getQueue() const;
    TetrominoType getNextTetromino();

private:
    std::deque<TetrominoType> tetrominoQueue;

    void refillBag();
};