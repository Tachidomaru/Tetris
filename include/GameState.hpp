#pragma once

class GameState 
{
public:
    GameState();

    int getLevel() const;
    int getTotalLinesCleared() const;
    int getScore() const;

    void addClearedLines(int countLines);
    void addScore(int linesCleared);

private:
    int totalLinesCleared;
    int level;
    int score;

    void updateLevel();
};
