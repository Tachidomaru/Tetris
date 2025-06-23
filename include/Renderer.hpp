#pragma once

#include <SFML/Graphics.hpp>
#include <optional>
#include "Board.hpp"
#include "Tetromino.hpp"
#include "GameState.hpp"

class Renderer 
{
public:
    Renderer();

    bool loadFont(const std::string& fontPath);
    bool loadTextures(const std::string& blocksPath, const std::string& boardPath, const std::string& ghostPath);

    void draw(sf::RenderWindow& window, const Board& board, const Tetromino& activeTetromino, const Tetromino& ghostTetromino, const std::deque<TetrominoType>& queue, const std::optional<Tetromino>& holdTetromino, const GameState& gameState);

private:
    sf::Texture minoTexture;
    std::optional<sf::Sprite> minoSprite;

    sf::Texture ghostMinoTexture;
    std::optional<sf::Sprite> ghostMinoSprite;

    sf::Texture boardTexture;
    std::optional<sf::Sprite> boardSprite;

    sf::RectangleShape border;

    sf::Font font;
    std::optional<sf::Text> holdLabel;
    std::optional<sf::Text> nextLabel;

    void initBorder();

    void drawStaticBoard(sf::RenderWindow& window, const Board& board);
    void drawFont(sf::RenderWindow& window);
    void drawMino(sf::RenderWindow& window, int x, int y, int type);
    void drawActiveTetromino(sf::RenderWindow& window, const Tetromino& activeTetromino);
    void drawGhostTetromino(sf::RenderWindow& window, const Tetromino& ghostTetromino);
    void drawQueueTetrominos(sf::RenderWindow& window, const std::deque<TetrominoType>& queue);
    void drawHoldTetromino(sf::RenderWindow& window, const std::optional<Tetromino>& holdTetromino);
    void drawGameInfo(sf::RenderWindow& window, const GameState& gameState);
};