#include "Board.hpp"

Board::Board() 
{
    for(auto& row : grid)
        row.fill(0);
}

int Board::getCellValue(int x, int y) const
{
    if(x < 0 || x >= BOARD_WIDTH || y < 0 || y >= BOARD_HEIGHT)
        return -1;

    return grid[y][x];
}

bool Board::isValidPosition(const Tetromino& tetromino) const
{
    for(const auto& mino : tetromino.getMinos())
    {
        int x = mino.x;
        int y = mino.y;

        if(x < 0 || x >= BOARD_WIDTH || y >= BOARD_HEIGHT)
            return false;

        if (y >= 0 && grid[y][x] != 0)
            return false;
    }
    return true;
}

bool Board::isGameOver(const Tetromino& tetromino) const
{
    return !isValidPosition(tetromino);
}

void Board::lockTetromino(const Tetromino& tetromino)
{
    int color = static_cast<int>(tetromino.getType()) + 1;

    for(const auto& mino : tetromino.getMinos()) 
    {
        int x = mino.x;
        int y = mino.y;

        if (y >= 0 && y < BOARD_HEIGHT && x >= 0 && x < BOARD_WIDTH)
            grid[y][x] = color;
    }
}

int Board::clearCompletedLines()
{
    int clearedLines = 0;

    for(int y = BOARD_HEIGHT - 1; y >= 0; --y) 
    {
        bool fullRow = true;

        for(int x = 0; x < BOARD_WIDTH; ++x) 
        {
            if(grid[y][x] == 0) 
            {
                fullRow = false;
                break;
            }
        }

        if(fullRow) 
        {
            for(int row = y; row > 0; --row) 
            {
                grid[row] = grid[row - 1];
            }

            grid[0].fill(0);

            ++clearedLines;
            ++y;
        }
    }
    return clearedLines;
}

void Board::resetBoard()
{
    for(auto& row : grid)
        row.fill(0);
}