//
//  Copyright © 2023-Present, Arkin Terli. All rights reserved.
//
//  NOTICE:  All information contained herein is, and remains the property of Arkin Terli.
//  The intellectual and technical concepts contained herein are proprietary to Arkin Terli
//  and may be covered by U.S. and Foreign Patents, patents in process, and are protected by
//  trade secret or copyright law. Dissemination of this information or reproduction of this
//  material is strictly forbidden unless prior written permission is obtained from Arkin Terli.

#include <FontSFNSMono.hpp>
#include <SnakeGame.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

#include <vector>
#include <iostream>


int main()
{
    int windowWidth  = 500;
    int windowHeight = 500;

    // Create a window with a resolution of 800x600 and a title
    sf::RenderWindow   window(sf::VideoMode(windowWidth, windowHeight), "Snake - No AI");
    window.setFramerateLimit(60);

    sf::Font font;
    if (!font.loadFromMemory(FontSFNSMono, sizeof(FontSFNSMono)))
    {
        std::cerr << "Failed to load font!" << std::endl;
        return -1;
    }

    sf::Text text("", font, 10);
    text.setPosition(10, 10);
    text.setFillColor(sf::Color::White);

    int blockWidth  = 50;
    int blockHeight = 50;

    int boardWidth  = windowWidth  / blockWidth;
    int boardHeight = windowHeight / blockHeight;

    SnakeGame   snakeGame(boardWidth, boardHeight);
    std::vector<sf::RectangleShape>  boardBlocks(boardWidth * boardHeight);

    sf::Clock clock;
    float  elapsedTime;

    // Main loop
    while (window.isOpen())
    {
        // Time elapsed between two frames.
        float deltaTime = clock.restart().asSeconds();

        // Process events
        sf::Event event{};
        while (window.pollEvent(event))
        {
            // Close the window when the user clicks the close button
            if (event.type == sf::Event::Closed)
            {
                window.close();
            }

            // Check if the event is a key pressed event
            if (event.type == sf::Event::KeyPressed)
            {
                // Check if the pressed key is the space key
                if (event.key.code == sf::Keyboard::Escape)
                {
                    window.close();
                }
                else if (event.key.code == sf::Keyboard::Left)
                {
                    snakeGame.SetDirection(SnakeDirection::kSnakeDirLeft);
                }
                else if (event.key.code == sf::Keyboard::Right)
                {
                    snakeGame.SetDirection(SnakeDirection::kSnakeDirRight);
                }
                else if (event.key.code == sf::Keyboard::Up)
                {
                    snakeGame.SetDirection(SnakeDirection::kSnakeDirUp);
                }
                else if (event.key.code == sf::Keyboard::Down)
                {
                    snakeGame.SetDirection(SnakeDirection::kSnakeDirDown);
                }
            }
        }

        // Clear the window with a black color
        window.clear(sf::Color::Black);

        // Call game update only every one second to slow down the snake movement.
        elapsedTime += deltaTime;
        if (elapsedTime > 0.25)
        {
            snakeGame.Update();
            elapsedTime = 0;

            auto gameBoard = snakeGame.GetBoard();
            int blockIndex = 0;

            // Render game board.
            for (int y=0; y<boardHeight; ++y)
            {
                for (int x=0; x<boardWidth; ++x)
                {
                    auto & block = boardBlocks[blockIndex];
                    block.setSize({float(blockWidth), float(blockHeight)});
                    block.setPosition(float(x*blockWidth), float(y*blockHeight));

                    auto boardObj = gameBoard[y][x];

                    if (boardObj == BoardObjType::kBoardObjSnakeHead)
                    {
                        block.setFillColor(sf::Color::Yellow);
                    }
                    else if (boardObj == BoardObjType::kBoardObjSnakeBody)
                    {
                        block.setFillColor(sf::Color::Green);
                    }
                    else if (boardObj == BoardObjType::kBoardObjApple)
                    {
                        block.setFillColor(sf::Color::Red);
                    }
                    else
                    {
                        block.setFillColor(sf::Color::Black);
                    }

                    blockIndex++;
                }
            }
        }

        // Draw the board.
        for (auto & block : boardBlocks)
        {
            window.draw(block);
        }

        // Draw text
        std::string gameStateStr;
        switch (snakeGame.GetGameState())
        {
            case SnakeGameState::kSnakeGameStateRunning:
                gameStateStr = "Running";
                break;
            case SnakeGameState::kSnakeGameStateWon:
                gameStateStr = "Won";
                break;
            case SnakeGameState::kSnakeGameStateFailed:
                gameStateStr = "Failed";
                break;
            default:
                gameStateStr = "Invalid";
                break;
        }

        text.setString("Delta Time (ms): " + std::to_string(deltaTime*1000) +
                       "\nGame State: " + gameStateStr +
                       "\nGame Score: " + std::to_string(snakeGame.GetScore()*100));
        window.draw(text);

        // Display the window content on the screen
        window.display();
    }

    return 0;
}
