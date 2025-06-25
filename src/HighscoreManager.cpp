#include <fstream>
#include "HighscoreManager.hpp"
#include "external/json.hpp"

HighscoreManager::HighscoreManager(const std::string& filename) : _filename(filename) {}

int HighscoreManager::loadHighscore() const
{
    std::ifstream file(_filename);
    if (!file)
        return 0;

    nlohmann::json jsonFile;
    file >> jsonFile;
    return jsonFile.value("highscore", 0);
}

void HighscoreManager::saveHighscore(int score) const
{
    int currentHighscore = loadHighscore();

    if (score > currentHighscore)
    {
        nlohmann::json jsonFile;
        jsonFile["highscore"] = score;

        std::ofstream file(_filename);
        if (file)
        {
            file << jsonFile.dump(4);
        }
    }
}
