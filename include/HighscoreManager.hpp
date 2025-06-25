#pragma once
#include <string>

class HighscoreManager
{
public:
    HighscoreManager(const std::string& filename);

    int loadHighscore() const;
    void saveHighscore(int score) const;

private:
    std::string _filename;
};
