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

int main()
{
    sf::RenderWindow window(sf::VideoMode({WINDOW_WIDTH, WINDOW_HEIGHT}), "Tetris", sf::Style::Titlebar | sf::Style::Close);
    window.setFramerateLimit(60);

    Board board;
    Renderer renderer;
    TetrominoBag bag;
    GameState gameState;

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
                        std::cout << "Game Over!\n";
                        window.close();
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
                            window.close();
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
                        window.close();
                    onGround = false;
                }
            }

            dropClock.restart();
        }

        window.clear(sf::Color::Black);
        renderer.draw(window, board, currentTetromino, ghostTetromino, bag.getQueue(), holdTetromino, gameState);
        window.display();
    }
}