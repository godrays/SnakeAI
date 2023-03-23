//
//  Copyright © 2023-Present, Arkin Terli. All rights reserved.
//
//  NOTICE:  All information contained herein is, and remains the property of Arkin Terli.
//  The intellectual and technical concepts contained herein are proprietary to Arkin Terli
//  and may be covered by U.S. and Foreign Patents, patents in process, and are protected by
//  trade secret or copyright law. Dissemination of this information or reproduction of this
//  material is strictly forbidden unless prior written permission is obtained from Arkin Terli.

// Project includes
#include "NoAICmd.hpp"
#include <FontSFNSMono.hpp>
#include <SnakeGame.hpp>
// External includes
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
// System includes
#include <iostream>
#include <vector>


namespace sai::cmd
{

void NoAICmd::Run(int argc, const char *argv[])
{
    static const char USAGE[] =
    R"(
    Usage:
        SnakeAIApp noai [--usestep]

    Options:

        --bw=<number>           Board width in block units.  [Default: 10]
        --bh=<number>           Board height in block units. [Default: 10]
        --bls=<number>          Block size in pixel units.   [Default: 25]

        --usestep               Update game per keypress only.
    )";

    std::map <std::string, docopt::value> args;

    try
    {
        // Parse cmd-line parameters.
        args = docopt::docopt(USAGE, {argv + 1, argv + argc}, false, "SnakeAI 1.0.0");
    }
    catch (...)
    {
        std::cerr << "Invalid commandline parameter usage. Please use '--help' parameter for more information."
                  << std::endl;
        return;
    }

    if (!ValidateArguments(args, USAGE))
    {
        return;
    }

    // Execute the command.
    ExecuteCommand(args);
}


bool NoAICmd::ValidateArguments(std::map <std::string, docopt::value> &args, const char *USAGE)
{
    // Show help if necessary
    if (args["-h"] || args["--help"])
    {
        std::cout << USAGE << std::endl;
        return false;
    }

    auto CheckRangeLong = [&](const std::string& paramName, int min, int max) -> bool
    {
        if (args[paramName] && (args[paramName].asLong() < min || args[paramName].asLong() > max))
        {
            std::cout << "Invalid parameter range: " << paramName
                      << " must be in [" << min << "," << max << "]" << std::endl;
            return false;
        }
        return true;
    };

    // VALIDATE REQUIRED ARGUMENTS

    if (!CheckRangeLong("--bw",  10, 100) ||
        !CheckRangeLong("--bh",  10, 100) ||
        !CheckRangeLong("--bls", 10, 100))
    {
        return false;
    }

    return true;
}


void NoAICmd::ExecuteCommand(std::map <std::string, docopt::value> & args)
{
    std::random_device rndDev;
    int rndSeed = static_cast<int>(rndDev());

    // Override parameters here
    if (args["--bw"])  m_boardWidth  = args["--bw"].asLong();
    if (args["--bh"])  m_boardHeight = args["--bh"].asLong();
    if (args["--bls"]) m_blockSize   = args["--bls"].asLong();
    bool useStep = args["--usestep"].asBool();

    int windowWidth  = m_boardWidth  * m_blockSize;
    int windowHeight = m_boardHeight * m_blockSize;

    // Create a window with a title
    m_window.create(sf::VideoMode(windowWidth, windowHeight), "Snake AI - Manual");
    m_window.setFramerateLimit(60);

    sf::Font font;
    font.loadFromMemory(FontSFNSMono, sizeof(FontSFNSMono));

    sf::Text text("", font, 10);
    text.setPosition(10, 10);
    text.setFillColor(sf::Color::White);

    SnakeGame   snakeGame(m_boardWidth, m_boardHeight, rndSeed);
    m_boardBlocks.resize(m_boardWidth * m_boardHeight);

    std::for_each(m_boardBlocks.begin(), m_boardBlocks.end(), [&](sf::RectangleShape & shape)
    {
        shape.setSize({float(m_blockSize), float(m_blockSize)});
        shape.setOutlineThickness(2);
        shape.setOutlineColor(sf::Color::Black);
    });

    sf::Clock clock;
    float  elapsedTime = 10;

    // Main loop
    while (m_window.isOpen())
    {
        bool updateGame = !useStep;

        // Time elapsed between two frames.
        float deltaTime = clock.restart().asSeconds();

        // Processes window and keypress events.
        ProcessEvents(snakeGame, updateGame);

        elapsedTime += deltaTime;

        // Call game update only every one second to slow down the snake movement.
        if (elapsedTime >= 10 || (elapsedTime > 0.25 && updateGame))
        {
            snakeGame.Update();
            if (snakeGame.GetGameState() != SnakeGameState::kSnakeGameStateRunning)
            {
                snakeGame.Reset();
            }

            UpdateGameBoardsDrawableBlocks(snakeGame);

            elapsedTime = 0;
        }

        text.setString("Score: " + std::to_string(snakeGame.GetScore()));
        DrawGameBoard(text);
    }
}


void NoAICmd::UpdateGameBoardsDrawableBlocks(SnakeGame& snakeGame)
{
    // Update game board block colors to reflect the changes.
    int blockIndex = 0;
    for (int y=0; y < m_boardHeight; ++y)
    {
        for (int x=0; x < m_boardWidth; ++x)
        {
            auto & block = m_boardBlocks[blockIndex];
            block.setPosition(float(x * m_blockSize), float(y * m_blockSize));
            switch (snakeGame.GetBoardObject(x, y))
            {
                case BoardObjType::kBoardObjSnakeHead:   block.setFillColor(sf::Color::Yellow);  break;
                case BoardObjType::kBoardObjSnakeBody:   block.setFillColor(sf::Color::Green);   break;
                case BoardObjType::kBoardObjApple:       block.setFillColor(sf::Color::Red);     break;
                default:                                 block.setFillColor(sf::Color::Black);   break;
            }
            blockIndex++;
        }
    }
}


void NoAICmd::DrawGameBoard(sf::Text& text)
{
    // Clear the window with a black color
    m_window.clear(sf::Color::Black);

    // Draw the board.
    for (const auto & block : m_boardBlocks)
    {
        m_window.draw(block);
    }

    m_window.draw(text);

    // Display the window content on the screen
    m_window.display();
}


void NoAICmd::ProcessEvents(SnakeGame& snakeGame, bool & updateGame)
{
    // Process events
    sf::Event event{};
    while (m_window.pollEvent(event))
    {
        // Close the window when the user clicks the close button
        if (event.type == sf::Event::Closed)
        {
            m_window.close();
        }

        // Check if the event is a key pressed event
        if (event.type == sf::Event::KeyReleased)
        {
            updateGame = true;
            // Check if the pressed key is the space key
            switch (event.key.code)
            {
                case sf::Keyboard::Escape:     m_window.close();                                        break;
                case sf::Keyboard::Left:       snakeGame.SetDirection(SnakeDirection::kSnakeDirLeft);   break;
                case sf::Keyboard::Right:      snakeGame.SetDirection(SnakeDirection::kSnakeDirRight);  break;
                case sf::Keyboard::Up:         snakeGame.SetDirection(SnakeDirection::kSnakeDirUp);     break;
                case sf::Keyboard::Down:       snakeGame.SetDirection(SnakeDirection::kSnakeDirDown);   break;
                default: break;
            }
        }
    }
}


} // namespace sai
