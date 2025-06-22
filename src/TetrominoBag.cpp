#include <random>
#include "TetrominoBag.hpp"

TetrominoBag::TetrominoBag() 
{
    refillBag();
}

void TetrominoBag::refillBag() 
{
    std::vector<TetrominoType> types = 
    {
        TetrominoType::I, 
        TetrominoType::J, 
        TetrominoType::L,
        TetrominoType::O, 
        TetrominoType::S, 
        TetrominoType::T, 
        TetrominoType::Z
    };
    
    std::shuffle(types.begin(), types.end(), std::mt19937(std::random_device{}()));
    
    for (auto& type : types)
        tetrominoQueue.push_back(type);
}

const std::deque<TetrominoType>& TetrominoBag::getQueue() const 
{
    return tetrominoQueue;
}

TetrominoType TetrominoBag::getNextTetromino() 
{
    if (tetrominoQueue.size() < 7)
        refillBag();

    TetrominoType nextTetromino = tetrominoQueue.front();
    tetrominoQueue.pop_front();

    return nextTetromino;
}
