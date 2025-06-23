#include "GameState.hpp"

GameState::GameState() : totalLinesCleared(0), level(1) {}

int GameState::getLevel() const 
{
    return level;
}

int GameState::getTotalLinesCleared() const 
{
    return totalLinesCleared;
}

int GameState::getScore() const
{
    return score;
}

void GameState::addClearedLines(int countLines) 
{
    totalLinesCleared += countLines;
    updateLevel();
}

void GameState::addScore(int linesCleared)
{
    int factor = getLevel() + 1;

    switch (linesCleared)
    {
        case 1: score += 40 * factor; 
            break;
        case 2: score += 100 * factor; 
            break;
        case 3: score += 300 * factor; 
            break;
        case 4: score += 1200 * factor; 
            break;
        default: 
            break;
    }
}

void GameState::updateLevel() 
{
    level = totalLinesCleared / 10 + 1;
}