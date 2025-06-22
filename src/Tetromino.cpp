#include <cmath>
#include "WallkickData.hpp"
#include "Tetromino.hpp"
#include "Board.hpp"

Tetromino::Tetromino(TetrominoType type) : type(type), rotationState(RotationState::R0)
{
    initTetromino();

    for(auto& mino : minos) 
    {
        mino.x += 3;
    }
}

void Tetromino::initTetromino() 
{
    switch(type) 
    {
        case TetrominoType::I:
            minos = {{ {0, 1}, {1, 1}, {2, 1}, {3, 1} }};
            break;
        case TetrominoType::J:
            minos = {{ {0, 0}, {0, 1}, {1, 1}, {2, 1} }};
            break;
        case TetrominoType::L:
            minos = {{ {2, 0}, {0, 1}, {1, 1}, {2, 1} }};
            break;
        case TetrominoType::O:
            minos = {{ {1, 0}, {2, 0}, {1, 1}, {2, 1} }};
            break;
        case TetrominoType::S:
            minos = {{ {1, 0}, {2, 0}, {0, 1}, {1, 1} }};
            break;
        case TetrominoType::Z:
            minos = {{ {0, 0}, {1, 0}, {1, 1}, {2, 1} }};
            break;
        case TetrominoType::T:
            minos = {{ {1, 0}, {0, 1}, {1, 1}, {2, 1} }};
            break;
    }
}

const std::array<Mino, 4>& Tetromino::getMinos() const 
{
    return minos;
}

TetrominoType Tetromino::getType() const 
{
    return type;
}

RotationState Tetromino::getRotationState() const 
{
    return rotationState;
}

void Tetromino::move(int dx, int dy)
{
    for(auto& mino : minos)
    {
        mino.x += dx;
        mino.y += dy;
    }
}

void Tetromino::rotate(const Board& board, RotationDirection direction)
{
    if(type == TetrominoType::O)
        return;

    auto oldRotationState = rotationState;
    rotationState = static_cast<RotationState>((toInt(rotationState) + (direction == RotationDirection::CW ? 1 : 3)) % 4);

    float rotationCenterX;
    float rotationCenterY;

    if(type == TetrominoType::I)
    {
        rotationCenterX = static_cast<float>(minos[1].x);
        rotationCenterY = static_cast<float>(minos[1].y);
    }
    else if(type == TetrominoType::S)
    {
        rotationCenterX = static_cast<float>(minos[3].x);
        rotationCenterY = static_cast<float>(minos[3].y);
    }
    else
    {
        rotationCenterX = static_cast<float>(minos[2].x);
        rotationCenterY = static_cast<float>(minos[2].y);
    }

    for(auto& mino : minos)
    {
        float dx = mino.x - rotationCenterX;
        float dy = mino.y - rotationCenterY;

        if(direction == RotationDirection::CW)
        {
            mino.x = std::round(rotationCenterX - dy);
            mino.y = std::round(rotationCenterY + dx);
        }
        else
        {
            mino.x = std::round(rotationCenterX + dy);
            mino.y = std::round(rotationCenterY - dx);
        }
    }

    int from = toInt(oldRotationState);
    int index = (direction == RotationDirection::CW) ? from * 2 : from * 2 + 1;

    const auto& wallkickData = (type == TetrominoType::I) ? wallkickData_I : wallkickData_JLSTZ;

    auto originalMinos = minos;

    for(auto [dx, dy] : wallkickData[index]) 
    {
        for(auto& mino : minos)
        {
            mino.x += dx;
            mino.y += dy;
        }

        if(board.isValidPosition(*this))
        {
            return;
        }
        minos = originalMinos;
    }
    minos = originalMinos;
    rotationState = oldRotationState;
}