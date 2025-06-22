#pragma once

#include <array>

class Board;

struct Mino
{
    int x;
    int y;

    Mino(int x = 0,int y = 0) : x(x), y(y) {}
};

enum class TetrominoType { I, J, L, O, S, Z, T };

enum class RotationDirection {CW, CCW};

enum class RotationState {R0 = 0, R = 1, R2 = 2, L = 3};

constexpr int toInt(RotationState rotationState)
{
    return static_cast<int>(rotationState);
}

class Tetromino
{
public:
    Tetromino(TetrominoType type);

    const std::array<Mino, 4>& getMinos() const;
    TetrominoType getType() const;
    RotationState getRotationState() const;

    void move(int dx, int dy);
    void rotate (const Board& board, RotationDirection direction);

private:
    std::array<Mino, 4> minos;
    TetrominoType type;
    RotationState rotationState;

    void initTetromino();
};
