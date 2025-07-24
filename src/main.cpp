#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Audio.hpp>
#include <optional>
#include <random>
#include <iostream>
#include "Config.hpp"
#include "Board.hpp"
#include "Tetromino.hpp"
#include "TetrominoBag.hpp"
#include "Renderer.hpp"
#include "GhostTetromino.hpp"
#include "GameState.hpp"
#include "GravityTable.hpp"
#include "HighscoreManager.hpp"

int main()
{
    sf::RenderWindow window(sf::VideoMode({WINDOW_WIDTH, WINDOW_HEIGHT}), "Tetris", sf::Style::Titlebar | sf::Style::Close);
    window.setFramerateLimit(60);

    Board board;
    Renderer renderer;
    TetrominoBag bag;
    GameState gameState;
    HighscoreManager highscoreManager("highscore.json");


    if (!renderer.loadTextures("assets/images/minos.png", "assets/images/board.png", "assets/images/ghost_minos.png")) 
    {
        std::cerr << "Texturen konnten nicht geladen werden.\n";
        return 1;
    }

    if (!renderer.loadFont("assets/fonts/Minecraft.ttf")) 
    {
        std::cerr << "Font konnte nicht geladen werden.\n";
        return 1;
    }

    enum class GameMode { Start, Playing, Paused, GameOver };
    GameMode gameMode = GameMode::Start;

    sf::Font font;
    font.openFromFile("assets/fonts/Minecraft.ttf");

    sf::Clock blinkClock;
    const float blinkInterval = 0.5f;
    bool blinkFullColor = true;

    sf::Text startText(font, "Press 'Enter' to Start", 32);
    startText.setFillColor(sf::Color::White);
    startText.setPosition({WINDOW_WIDTH / 2.f - 160.f, WINDOW_HEIGHT / 2.f - 40.f});

    sf::Texture startTexture;
    startTexture.loadFromFile("assets/images/startscreen.png");
    
    sf::Sprite startScreen(startTexture);

    sf::Text pauseText(font, "           Pause\n\n\nPress 'P' to continue", 32);
    pauseText.setFillColor(sf::Color::White);
    pauseText.setPosition({WINDOW_WIDTH / 2.f - 156.f, 60.f});

    sf::RectangleShape pauseOverlay(sf::Vector2f(WINDOW_WIDTH, WINDOW_HEIGHT));
    pauseOverlay.setFillColor(sf::Color(0, 0, 0, 200));

    sf::Text gameoverText(font, "      Game Over!\n\n\nPress 'R' to restart", 32);
    gameoverText.setFillColor(sf::Color::White);
    gameoverText.setPosition({WINDOW_WIDTH / 2.f - 147.f, 60.f});

    Tetromino currentTetromino(bag.getNextTetromino());
    Tetromino ghostTetromino = computeGhostPiece(board, currentTetromino);

    std::optional<Tetromino> holdTetromino;
    bool holdUsedThisTurn = false;
    
    sf::Clock dropClock;
    sf::Clock moveClock;
    sf::Clock softDropClock;

    float dropInterval = GravityTable::getGravity(gameState.getLevel());
    const float moveDelay = 0.07f;
    const float softDropInterval = 0.05f;

    bool movedThisFrame = false;

    sf::Clock lockDelayClock;
    bool onGround = false;
    const float lockDelayTime = 0.5f;

    sf::Music music;
    if (!music.openFromFile("assets/audio/Tetris_Theme_Loop.ogg")) 
    {
        std::cerr << "Musik konnte nicht geladen werden.\n";
        return 1;
    }

    music.setLooping(true);
    music.setVolume(100.f);
    music.play();

    while (window.isOpen())
    {
        movedThisFrame = false;
        dropInterval = GravityTable::getGravity(gameState.getLevel());

        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                window.close();

            if (event->is<sf::Event::KeyPressed>()) 
            {
                auto keyEvent = event->getIf<sf::Event::KeyPressed>();

                if (gameMode == GameMode::Start && keyEvent->code == sf::Keyboard::Key::Enter) 
                {
                    gameMode = GameMode::Playing;
                }

                if(keyEvent->code == sf::Keyboard::Key::P)
                {
                    if(gameMode == GameMode::Playing)
                        gameMode = GameMode::Paused;
                    else if(gameMode == GameMode::Paused)
                        gameMode = GameMode::Playing;
                }

                if(gameMode == GameMode::GameOver && keyEvent->code == sf::Keyboard::Key::R)
                {
                    // Spiel komplett zurücksetzen
                    board.resetBoard();
                    gameState = GameState();
                    bag = TetrominoBag();
                    currentTetromino = Tetromino(bag.getNextTetromino());
                    ghostTetromino = computeGhostPiece(board, currentTetromino);
                    holdTetromino.reset();
                    holdUsedThisTurn = false;

                    // Zurück in den Playing-Modus
                    gameMode = GameMode::Playing;
                }

                if(gameMode == GameMode::Playing)
                {
                Tetromino movedTetromino = currentTetromino;

                if (keyEvent->code == sf::Keyboard::Key::Up) 
                {
                    movedTetromino.rotate(board, RotationDirection::CW);
                    if (board.isValidPosition(movedTetromino))
                    {
                        currentTetromino = movedTetromino;
                        ghostTetromino = computeGhostPiece(board, currentTetromino);
                        movedThisFrame = true;
                        onGround = false;
                        lockDelayClock.restart();
                    }
                }
                else if (keyEvent->code == sf::Keyboard::Key::X) 
                {
                    movedTetromino.rotate(board, RotationDirection::CCW);
                    if (board.isValidPosition(movedTetromino))
                    {
                        currentTetromino = movedTetromino;
                        ghostTetromino = computeGhostPiece(board, currentTetromino);
                        movedThisFrame = true;
                        onGround = false;
                        lockDelayClock.restart();
                    }
                }
                else if (keyEvent->code == sf::Keyboard::Key::Space) 
                {
                    while (true) 
                    {
                        movedTetromino.move(0, 1);
                        if (!board.isValidPosition(movedTetromino))
                            break;
                    }
                    movedTetromino.move(0, -1);
                    board.lockTetromino(movedTetromino);
                    int clearedLines = board.clearCompletedLines();
                    gameState.addClearedLines(clearedLines);
                    dropInterval = GravityTable::getGravity(gameState.getLevel());
                    gameState.addScore(clearedLines);
                    currentTetromino = Tetromino(bag.getNextTetromino());
                    holdUsedThisTurn = false;
                    ghostTetromino = computeGhostPiece(board, currentTetromino);
                    if (board.isGameOver(currentTetromino))
                    {
                        highscoreManager.saveHighscore(gameState.getScore());
                        gameMode = GameMode::GameOver;
                    }
                    dropClock.restart();
                    movedThisFrame = true;
                }
                else if(keyEvent->code == sf::Keyboard::Key::C)
                {
                    if (!holdUsedThisTurn)
                    {
                        TetrominoType currentType = currentTetromino.getType();

                        if (!holdTetromino.has_value())
                        {
                            holdTetromino = Tetromino(currentType);
                            currentTetromino = Tetromino(bag.getNextTetromino());
                        }
                        else
                        {
                            TetrominoType heldType = holdTetromino->getType();
                            holdTetromino = Tetromino(currentType);
                            currentTetromino = Tetromino(heldType);
                        }
                        ghostTetromino = computeGhostPiece(board, currentTetromino);
                        holdUsedThisTurn = true;
                    }
                }
                }
            }
        }
        if(gameMode == GameMode::Playing)
        {
        Tetromino movedTetromino = currentTetromino;
        bool movedSideways = false;

        if (moveClock.getElapsedTime().asSeconds() > moveDelay)
        {
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left))
            {
                movedTetromino.move(-1, 0);
                if (board.isValidPosition(movedTetromino)) 
                {
                    currentTetromino = movedTetromino;
                    ghostTetromino = computeGhostPiece(board, currentTetromino);
                    movedSideways = true;
                    movedThisFrame = true;
                    onGround = false;
                    lockDelayClock.restart();
                }
            }
            else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right))
            {
                movedTetromino = currentTetromino;
                movedTetromino.move(1, 0);
                if (board.isValidPosition(movedTetromino)) 
                {
                    currentTetromino = movedTetromino;
                    ghostTetromino = computeGhostPiece(board, currentTetromino);
                    movedSideways = true;
                    movedThisFrame = true;
                    onGround = false;
                    lockDelayClock.restart();
                }
            }
            if (movedSideways)
                moveClock.restart();
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down))
        {
            if (softDropClock.getElapsedTime().asSeconds() > softDropInterval)
            {
                movedTetromino = currentTetromino;
                movedTetromino.move(0, 1);

                if (board.isValidPosition(movedTetromino)) 
                {
                    currentTetromino = movedTetromino;
                    ghostTetromino = computeGhostPiece(board, currentTetromino);
                    movedThisFrame = true;
                    onGround = false;
                } 
                else 
                {
                    if (!onGround)
                    {
                        onGround = true;
                        lockDelayClock.restart();
                    }
                    else if (lockDelayClock.getElapsedTime().asSeconds() > lockDelayTime)
                    {
                        board.lockTetromino(currentTetromino);
                        int clearedLines = board.clearCompletedLines();
                        gameState.addClearedLines(clearedLines);
                        dropInterval = GravityTable::getGravity(gameState.getLevel());
                        gameState.addScore(clearedLines);
                        currentTetromino = Tetromino(bag.getNextTetromino());
                        holdUsedThisTurn = false;
                        ghostTetromino = computeGhostPiece(board, currentTetromino);
                        if (board.isGameOver(currentTetromino))
                        {
                            highscoreManager.saveHighscore(gameState.getScore());
                            gameMode = GameMode::GameOver;
                        }
                        gameMode = GameMode::GameOver;
                        onGround = false;
                    }
                }
                softDropClock.restart();
                dropClock.restart();
            }
        }
        if (!movedThisFrame && dropClock.getElapsedTime().asSeconds() > dropInterval)
        {
            movedTetromino = currentTetromino;
            movedTetromino.move(0, 1);

            if (board.isValidPosition(movedTetromino)) 
            {
                currentTetromino = movedTetromino;
                ghostTetromino = computeGhostPiece(board, currentTetromino);
                onGround = false;
                dropClock.restart();
            } 
            else 
            {
                if (!onGround) 
                {
                    onGround = true;
                    lockDelayClock.restart();
                } 
                else if (lockDelayClock.getElapsedTime().asSeconds() > lockDelayTime) 
                {
                    board.lockTetromino(currentTetromino);
                    int clearedLines = board.clearCompletedLines();
                    gameState.addClearedLines(clearedLines);
                    dropInterval = GravityTable::getGravity(gameState.getLevel());
                    gameState.addScore(clearedLines);
                    currentTetromino = Tetromino(bag.getNextTetromino());
                    holdUsedThisTurn = false;
                    ghostTetromino = computeGhostPiece(board, currentTetromino);
                    if (board.isGameOver(currentTetromino))
                    {
                        highscoreManager.saveHighscore(gameState.getScore());
                        gameMode = GameMode::GameOver;
                    }
                    onGround = false;
                }
            }

            dropClock.restart();
        }
        }

        if (gameMode == GameMode::Start)
        {
            if (blinkClock.getElapsedTime().asSeconds() >= blinkInterval)
            {
                blinkFullColor = !blinkFullColor;   // Sichtbarkeit umschalten
                blinkClock.restart();               // Timer zurücksetzen
            }

            uint8_t alpha = blinkFullColor ? 255 : 190;
            startText.setFillColor(sf::Color(255, 255, 255, alpha));

            window.clear(sf::Color::Black);

            window.clear(sf::Color::Black);
            window.draw(startScreen);
            window.draw(startText);
            window.display();
            continue;
        }

        if(gameMode == GameMode::Paused)
        {
            window.clear(sf::Color::Black);
            renderer.draw(window, board, currentTetromino, ghostTetromino, bag.getQueue(), holdTetromino, gameState, highscoreManager);

            window.draw(pauseOverlay);
            window.draw(pauseText);

            window.display();
            continue;
        }

        if(gameMode == GameMode::GameOver)
        {
            window.clear(sf::Color::Black);
            renderer.draw(window, board, currentTetromino, ghostTetromino, bag.getQueue(), holdTetromino, gameState, highscoreManager);
            
            window.draw(pauseOverlay);
            window.draw(gameoverText);

            window.display();
            continue;
        }

        window.clear(sf::Color::Black);
        renderer.draw(window, board, currentTetromino, ghostTetromino, bag.getQueue(), holdTetromino, gameState, highscoreManager);
        window.display();
    }
}