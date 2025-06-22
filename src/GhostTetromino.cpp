#include "GhostTetromino.hpp"

Tetromino computeGhostPiece(const Board& board, const Tetromino& activeTetromino)
{
    Tetromino ghostTetromino = activeTetromino;

    while(true) 
    {
        Tetromino testTetromino = ghostTetromino;

        testTetromino.move(0, 1);

        if (board.isValidPosition(testTetromino))
            ghostTetromino = testTetromino;
        else
            break;
    }
    return ghostTetromino;
}