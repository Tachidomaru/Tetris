#pragma once

#include "Board.hpp"
#include "Tetromino.hpp"

Tetromino computeGhostPiece(const Board& board, const Tetromino& activeTetromino);