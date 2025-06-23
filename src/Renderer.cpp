#include <sstream>
#include <SFML/Graphics.hpp>
#include "Config.hpp"
#include "UIHelpers.hpp"
#include "Renderer.hpp"
#include "GameState.hpp"

Renderer::Renderer()
{
    initBorder();
}

void Renderer::initBorder()
{
    border.setSize(sf::Vector2f(BOARD_WIDTH * MINO_SIZE, VISIBLE_ROWS * MINO_SIZE));
    border.setFillColor(sf::Color::Transparent);
    border.setOutlineColor(sf::Color::White);
    border.setOutlineThickness(5);
    border.setPosition({BOARD_OFFSET_LEFT_X, BOARD_OFFSET_Y});
}

bool Renderer::loadFont(const std::string& fontPath)
{
    if (!font.openFromFile(fontPath)) 
        return false;

    holdLabel.emplace(font);
    holdLabel->setFont(font);
    holdLabel->setString("HOLD");
    holdLabel->setCharacterSize(40);
    holdLabel->setFillColor(sf::Color::White);
    holdLabel->setPosition({19.f, 10.f});

    nextLabel.emplace(font);
    nextLabel->setFont(font);
    nextLabel->setString("NEXT");
    nextLabel->setCharacterSize(40);
    nextLabel->setFillColor(sf::Color::White);
    nextLabel->setPosition({475.f, 10.f});

    return true;
}

bool Renderer::loadTextures(const std::string& minosPath, const std::string& boardPath, const std::string& ghostMinosPath)
{
    if (!minoTexture.loadFromFile(minosPath)) 
        return false;
    
    minoSprite.emplace(minoTexture);

    if (!boardTexture.loadFromFile(boardPath)) 
        return false;
    
    boardSprite.emplace(boardTexture);
    boardSprite->setPosition({BOARD_OFFSET_LEFT_X, BOARD_OFFSET_Y});

    if (!ghostMinoTexture.loadFromFile(ghostMinosPath)) 
        return false;
    
    ghostMinoSprite.emplace(ghostMinoTexture);

    return true;
}

void Renderer::draw(sf::RenderWindow& window, const Board& board, const Tetromino& activeTetromino, const Tetromino& ghostTetromino, const std::deque<TetrominoType>& queue, const std::optional<Tetromino>& holdTetromino, const GameState& gameState)
{
    drawStaticBoard(window, board);

    drawFont(window);

    drawGhostTetromino(window, ghostTetromino);

    drawActiveTetromino(window, activeTetromino);

    drawQueueTetrominos(window, queue);

    drawHoldTetromino(window, holdTetromino);

    drawGameInfo(window, gameState);
}

void Renderer::drawStaticBoard(sf::RenderWindow& window, const Board& board)
{
    if (boardSprite)
        window.draw(*boardSprite);

    window.draw(border);

    for (int y = HIDDEN_ROWS; y < BOARD_HEIGHT; ++y) 
    {
        for (int x = 0; x < BOARD_WIDTH; ++x) 
        {
            int cell = board.getCellValue(x, y);
            if (cell != 0) 
                drawMino(window, x, y, cell);
        }
    }
}

void Renderer::drawFont(sf::RenderWindow& window)
{
    if (holdLabel) 
        window.draw(*holdLabel);

    if (nextLabel) 
        window.draw(*nextLabel);
}

void Renderer::drawMino(sf::RenderWindow& window, int x, int y, int type)
{
    int textureOffset = (type - 1) * MINO_SIZE;

    minoSprite->setTextureRect({{textureOffset, 0}, {MINO_SIZE, MINO_SIZE}});

    minoSprite->setPosition({BOARD_OFFSET_LEFT_X + x * MINO_SIZE, BOARD_OFFSET_Y + (y - HIDDEN_ROWS) * MINO_SIZE});
    window.draw(*minoSprite);
}

void Renderer::drawActiveTetromino(sf::RenderWindow& window, const Tetromino& activeTetromino)
{
    for (const auto& mino : activeTetromino.getMinos()) 
    {
        int x = mino.x;
        int y = mino.y;

        if(y >= HIDDEN_ROWS)
        {
            int type = static_cast<int>(activeTetromino.getType()) + 1;
            drawMino(window, x, y, type);
        }
    }
}

void Renderer::drawGhostTetromino(sf::RenderWindow& window, const Tetromino& ghostTetromino)
{
    for (const auto& mino : ghostTetromino.getMinos()) 
    {
        int x = mino.x;
        int y = mino.y- HIDDEN_ROWS;

        int type = static_cast<int>(ghostTetromino.getType()) + 1;
        int textureBlockOffset = (type - 1) * MINO_SIZE;

        ghostMinoSprite->setTextureRect(sf::IntRect({textureBlockOffset, 0}, {MINO_SIZE, MINO_SIZE}));
        ghostMinoSprite->setPosition({x * MINO_SIZE + BOARD_OFFSET_LEFT_X, y * MINO_SIZE + BOARD_OFFSET_Y});
        window.draw(*ghostMinoSprite);
    }
}

void Renderer::drawQueueTetrominos(sf::RenderWindow& window, const std::deque<TetrominoType>& queue)
{
    for (int i = 0; i < std::min((int)queue.size(), MAX_PREVIEW); ++i)
    {
        TetrominoType type = queue[i];
        Tetromino preview(type);

        int pixelXOffset = getPreviewXOffset(type);

        float pixelYOffset = UPPER_LABEL_OFFSET_Y + i * MINO_SIZE * 3;

        sf::Vector2f previewOffset(BOARD_OFFSET_RIGHT_X + 5 + pixelXOffset, pixelYOffset);

        int textureBlockOffset = static_cast<int>(type) * MINO_SIZE;

        for (const auto& mino : preview.getMinos())
        {
            minoSprite->setTextureRect(sf::IntRect({textureBlockOffset, 0}, {MINO_SIZE, MINO_SIZE}));
            minoSprite->setPosition({previewOffset.x + mino.x * MINO_SIZE, previewOffset.y + mino.y * MINO_SIZE});
            
            window.draw(*minoSprite);
        }
    }
}

void Renderer::drawHoldTetromino(sf::RenderWindow& window, const std::optional<Tetromino>& holdTetromino)
{
    if (!holdTetromino.has_value())
        return;

    Tetromino hold(holdTetromino->getType());

    int pixelXOffset = getPreviewXOffset(hold.getType());

    sf::Vector2f holdOffset(pixelXOffset, UPPER_LABEL_OFFSET_Y);

    for (const auto& mino : hold.getMinos())
    {
        int type = static_cast<int>(hold.getType()) + 1;
        int textureBlockOffset = (type - 1) * MINO_SIZE;

        minoSprite->setTextureRect(sf::IntRect({textureBlockOffset, 0}, {MINO_SIZE, MINO_SIZE}));
        minoSprite->setPosition({holdOffset.x + mino.x * MINO_SIZE, holdOffset.y + mino.y * MINO_SIZE});

        window.draw(*minoSprite);
    }
}

void Renderer::drawGameInfo(sf::RenderWindow& window, const GameState& gameState)
{
    std::ostringstream levelString;
    levelString << gameState.getLevel();

    std::ostringstream linesString;
    linesString << gameState.getTotalLinesCleared();

    std::ostringstream scoreString;
    scoreString << gameState.getScore();

    sf::Vector2f boxPositionLevel(25.f, 350.f);
    sf::Vector2f boxPositionLines(25.f, 430.f);
    sf::Vector2f boxPositionScore(25.f, 510.f);
    sf::Vector2f boxSize(120.f, 30.f);

    sf::RectangleShape levelBox(boxSize);
    levelBox.setFillColor(sf::Color::White);
    levelBox.setPosition(boxPositionLevel);

    sf::Text levelLabel(font);
    levelLabel.setCharacterSize(24);
    levelLabel.setFillColor(sf::Color::Black);
    levelLabel.setString("LEVEL");
    levelLabel.setPosition({boxPositionLevel.x + 10, boxPositionLevel.y + 1});

    sf::Text levelText(font);
    levelText.setCharacterSize(20);
    levelText.setFillColor(sf::Color::White);
    levelText.setString(levelString.str());

    sf::FloatRect boundsLevel = levelText.getLocalBounds();
    levelText.setOrigin({boundsLevel.size.x, 0});
    levelText.setPosition({135.f, 390.f});

    sf::RectangleShape linesBox(boxSize);
    linesBox.setFillColor(sf::Color::White);
    linesBox.setPosition(boxPositionLines);

    sf::Text linesLabel(font);
    linesLabel.setCharacterSize(24);
    linesLabel.setFillColor(sf::Color::Black);
    linesLabel.setString("LINES");
    linesLabel.setPosition({boxPositionLines.x + 10, boxPositionLines.y + 1});

    sf::Text linesText(font);
    linesText.setCharacterSize(20);
    linesText.setFillColor(sf::Color::White);
    linesText.setString(linesString.str());

    sf::FloatRect boundsLines = linesText.getLocalBounds();
    linesText.setOrigin({boundsLines.size.x, 0});
    linesText.setPosition({135.f, 470.f});

    sf::RectangleShape scoreBox(boxSize);
    scoreBox.setFillColor(sf::Color::White);
    scoreBox.setPosition(boxPositionScore);

    sf::Text scoreLabel(font);
    scoreLabel.setCharacterSize(24);
    scoreLabel.setFillColor(sf::Color::Black);
    scoreLabel.setString("SCORE");
    scoreLabel.setPosition({boxPositionScore.x + 10, boxPositionScore.y + 1});

    sf::Text scoreText(font);
    scoreText.setCharacterSize(20);
    scoreText.setFillColor(sf::Color::White);
    scoreText.setString(scoreString.str());

    sf::FloatRect boundsScore = scoreText.getLocalBounds();
    scoreText.setOrigin({boundsScore.size.x, 0});
    scoreText.setPosition({135.f, 550.f});

    window.draw(levelBox);
    window.draw(levelLabel);
    window.draw(levelText);

    window.draw(linesBox);
    window.draw(linesLabel);
    window.draw(linesText);

    window.draw(scoreBox);
    window.draw(scoreLabel);
    window.draw(scoreText);
}