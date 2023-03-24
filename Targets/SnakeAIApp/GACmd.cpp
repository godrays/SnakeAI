//
//  Copyright © 2023-Present, Arkin Terli. All rights reserved.
//
//  NOTICE:  All information contained herein is, and remains the property of Arkin Terli.
//  The intellectual and technical concepts contained herein are proprietary to Arkin Terli
//  and may be covered by U.S. and Foreign Patents, patents in process, and are protected by
//  trade secret or copyright law. Dissemination of this information or reproduction of this
//  material is strictly forbidden unless prior written permission is obtained from Arkin Terli.

// Project includes
#include "GACmd.hpp"
#include <FFNN.hpp>
#include <FontSFNSMono.hpp>
#include <GeneticAlgorithm.hpp>
#include <SnakeGame.hpp>
// External includes
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
// System includes
#include <filesystem>
#include <iomanip>
#include <iostream>
#include <limits>
#include <vector>


namespace sai::cmd
{

void GACmd::Run(int argc, const char *argv[])
{
    static const char USAGE[] =
    R"(
    Snake AI - Copyright (c) 2023-Present, Arkin Terli. All rights reserved.

    Usage:
        SnakeAIApp ga play  --modelfile=<name> [--bw=<number> --bh=<number>] [--bls=<number>]
        SnakeAIApp ga train --modelfile=<name> [--bw=<number> --bh=<number>] [--bls=<number>] [--maxGen=<number>]

    Options:

        --modelfile=<name>      Model filename.

        --bw=<number>           Board width in block units.  [Default: 10]
        --bh=<number>           Board height in block units. [Default: 10]
        --bls=<number>          Block size in pixel units.   [Default: 25]

        --maxGen=number         Maximum number of generation for training. [Default: 5000]
    )";

    std::map <std::string, docopt::value>  args;

    try
    {
        // Parse cmd-line parameters.
        args = docopt::docopt(USAGE, {argv + 1, argv + argc}, false, "SnakeAIApp 1.0.0");
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


bool GACmd::ValidateArguments(std::map <std::string, docopt::value>& args, const char* USAGE)
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
        !CheckRangeLong("--bls", 10, 100) ||
        !CheckRangeLong("--maxGen", 1, 1000000))
    {
        return false;
    }

    if (args["play"].asBool() && !std::filesystem::exists(args["--modelfile"].asString()))
    {
        std::cout << "Invalid --modelfile value. File does not exist!" << std::endl;
        return false;
    }

    return true;
}


void GACmd::ExecuteCommand(std::map <std::string, docopt::value> & args)
{
    std::string  modelFilename = args["--modelfile"].asString();

    // Override parameters here
    if (args["--bw"])  m_boardWidth  = args["--bw"].asLong();
    if (args["--bh"])  m_boardHeight = args["--bh"].asLong();
    if (args["--bls"]) m_blockSize   = args["--bls"].asLong();
    if (args["--maxGen"]) m_maxGeneration = args["--maxGen"].asLong();

    if (args["play"].asBool())
    {
        PlayModel(modelFilename);
    }
    else if (args["train"].asBool())
    {
        TrainModel(modelFilename);
    }
}


void GACmd::PlayModel(const std::string & modelFilename)
{
    std::random_device rndDev;
    int rndSeed = static_cast<int>(rndDev());

    int windowWidth  = m_boardWidth  * m_blockSize;
    int windowHeight = m_boardHeight * m_blockSize;

    // Create a window with a title.
    m_window.create(sf::VideoMode(windowWidth, windowHeight), "Snake AI Model Play Mode");
    m_window.setFramerateLimit(60);

    // Create font
    sf::Font font;
    font.loadFromMemory(FontSFNSMono, sizeof(FontSFNSMono));

    // Create a text to render on window.
    sf::Text text("", font, 10);
    text.setPosition(10, 10);
    text.setFillColor(sf::Color::White);

    // Create a snake game to simulate each step.
    SnakeGame   snakeGame(m_boardWidth, m_boardHeight, rndSeed);

    // Create neural network to determine snakes next steps.
    FFNN  ffnn;
    ffnn.Load(modelFilename);

    // Initialize blocks to render on windows.
    m_boardBlocks.resize(m_boardWidth * m_boardHeight);
    std::for_each(m_boardBlocks.begin(), m_boardBlocks.end(), [&](sf::RectangleShape & shape)
    {
        shape.setSize({float(m_blockSize), float(m_blockSize)});
        shape.setOutlineThickness(2);
        shape.setOutlineColor(sf::Color::Black);
    });

    sf::Clock  clock;
    float  elapsedTime;
    float  elapsedTimeMax = 0.07;

    // Main loop
    while (m_window.isOpen())
    {
        // Time elapsed between two frames.
        float deltaTime = clock.restart().asSeconds();

        // Processes window and keypress events.
        ProcessEvents(elapsedTimeMax);

        // Call game update only every one second to slow down the snake movement.
        elapsedTime += deltaTime;
        if (elapsedTime > elapsedTimeMax)
        {
            CalculateGameNextStep(snakeGame, ffnn);
            UpdateGameBoardsDrawableBlocks(snakeGame);
            elapsedTime = 0;
        }

        text.setString("Score: " + std::to_string(snakeGame.GetScore()));
        DrawGameBoard(text);
    }
}


void GACmd::TrainModel(const std::string & modelFilename)
{
    std::random_device rndDev;
    std::mt19937 rndEngine(rndDev());
    int rndSeed = static_cast<int>(rndDev());

    // First determine genetic vector size.
    int modelInputSize = static_cast<int>(SnakeGame::GetParameterSize());
    std::vector<int>  ffnnLayers{modelInputSize, modelInputSize, modelInputSize/2, 4};
    FFNN  dummyNet(ffnnLayers);
    auto geneticVectorSize = dummyNet.SerializeAllParameters().size();

    const std::size_t  populationSize    = 50;
    const std::size_t  parentRatio       = 50;      // Percent
    const std::size_t  mutateProbability = 1;       // Percent
    const std::size_t  transferRatio     = 15;      // Percent
    const std::size_t  crossover         = 50;      // Percent
    const std::size_t  samplingSize      = 2000;    // Game sampling size per individual per generation.

    // Create genetic algorithm to search best weights and biases for a neural network.
    ga::GeneticAlgorithm<double>  ga(populationSize, parentRatio, mutateProbability, transferRatio, crossover,
                                     geneticVectorSize);

    // This method will calculate fitness value for each individual.
    ga.SetFitnessFunc([&](const std::vector<double> & chromosome) -> double
    {
        return SimulateSnakeGames(samplingSize, chromosome, ffnnLayers, rndSeed);
    });

    // This method will generate random item (genes) for a genetic vector/material (chromosome).
    ga.SetRandomItemFunc([&]() -> double
    {
        // Scale the random fraction to the desired range [min, max]
        constexpr double min = -1;
        constexpr double max =  1;
        return std::uniform_real_distribution<double>(min, max)(rndEngine);
    });

    ga.CreateInitialPopulation();

    double bestFitness = -std::numeric_limits<double>::max();

    while (ga.GetGeneration() < m_maxGeneration)
    {
        double fitness = ga.GetBestIndividual().GetFitness();

        // Save the best individual.
        if (fitness > bestFitness)
        {
            FFNN  ffnn(ffnnLayers);
            // Set genes vector (weights and biases) coming from genetic algorithm.
            ffnn.DeserializeAllParameters(ga.GetBestIndividual().GetValue());
            ffnn.Save(modelFilename);

            bestFitness = fitness;
        }

        std::cout << "Generation: " << ga.GetGeneration() << "  Fitness: " << bestFitness << "\r";
        ga.CreateNextPopulation();
    }
}


void GACmd::CalculateGameNextStep(SnakeGame& snakeGame, FFNN& ffnn) const
{
    // Get game parameters to use as inputs to neural network model.
    auto modelInputs = snakeGame.GetParameters();
    auto inputs = Eigen::Map<Eigen::RowVectorXd>(modelInputs.data(), modelInputs.size());

    // Make prediction and get new snake directions as model outputs.
    auto outputs = ffnn.Forward(inputs);

    // Determine the best direction from model outputs. The highest value should be the new direction.
    snakeGame.SetDirection(DetermineSnakeDirection(outputs));

    // Update game.
    snakeGame.Update();

    // Prepare game board to Render.
    if (snakeGame.GetGameState() != SnakeGameState::kSnakeGameStateRunning)
    {
        snakeGame.Reset();
    }
}


void GACmd::UpdateGameBoardsDrawableBlocks(SnakeGame& snakeGame)
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


void GACmd::DrawGameBoard(sf::Text& text)
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


double GACmd::SimulateSnakeGames(std::size_t samplingSize, const std::vector<double> & value,
                                 const std::vector<int> & ffnnLayers, int rndSeed)
{
    // Setup a neural network.
    FFNN  ffnn(ffnnLayers);
    // Set weights and biases coming from genetic algorithm.
    ffnn.DeserializeAllParameters(value);   // value = genetic material vector = chromosome

    // Create a new snake game.
    SnakeGame snakeGame(m_boardWidth, m_boardHeight, rndSeed);

    double highestScore = 0;
    double avgDeaths = 0;
    double avgSteps = 0;
    double avgLongLoopFails = 0;
    double avgScore = 0;

    // Run the same model N times to assess quality of the individual (chromosome/array of genes/NN Model weights).
    for (std::size_t i=0; i<samplingSize; ++i)
    {
        while (snakeGame.GetGameState() == SnakeGameState::kSnakeGameStateRunning)
        {
            // Get game parameters to use as inputs to neural network model.
            auto modelInputs = snakeGame.GetParameters();
            auto inputs = Eigen::Map<Eigen::RowVectorXd>(modelInputs.data(), modelInputs.size());

            // Make prediction and get new snake directions as model outputs.
            auto outputs = ffnn.Forward(inputs);

            // Determine the best direction from model outputs. The highest value should be the new direction.
            snakeGame.SetDirection(DetermineSnakeDirection(outputs));

            // Update game.
            snakeGame.Update();
        }

        if (snakeGame.GetGameState() == SnakeGameState::kSnakeGameStateFailedHitWall ||
            snakeGame.GetGameState() == SnakeGameState::kSnakeGameStateFailedHitItself)
        {
            avgDeaths++;
        }
        if (snakeGame.GetGameState() == SnakeGameState::kSnakeGameStateFailedLongLoop)
        {
            avgLongLoopFails++;
        }

        highestScore = std::max<double>(highestScore, snakeGame.GetScore());
        avgSteps += snakeGame.GetSteps();
        avgScore += snakeGame.GetScore();

        snakeGame.Reset();
    }

    // Return fitness value to tell the genetic algorithm how well the neural network has played the game so far.
    // Fitness formula is very important.
    avgSteps /= double(samplingSize);
    avgDeaths /= double(samplingSize);
    avgLongLoopFails /= double(samplingSize);
    avgScore /= double(samplingSize);

    return highestScore * 500 + avgScore * 50 - avgDeaths * 15 - avgSteps * 10 - avgLongLoopFails * 100;
}


SnakeDirection GACmd::DetermineSnakeDirection(const Eigen::MatrixXd& outputs) const
{
    SnakeDirection newDir = SnakeDirection::kSnakeDirUp;

    double maxValue = outputs(0, 0);

    if (maxValue < outputs(0, 1)) { newDir = SnakeDirection::kSnakeDirDown; maxValue = outputs(0, 1); }
    if (maxValue < outputs(0, 2)) { newDir = SnakeDirection::kSnakeDirLeft; maxValue = outputs(0, 2); }
    if (maxValue < outputs(0, 3)) { newDir = SnakeDirection::kSnakeDirRight; }

    return newDir;
}


void GACmd::ProcessEvents(float& elapsedTimeMax)
{
    // Process events
    sf::Event event{};
    while (m_window.pollEvent(event))
    {
        // Close the window when the user clicks the close button
        if (event.type == sf::Event::Closed)
            m_window.close();

        // Check if the event is a key pressed event
        if (event.type == sf::Event::KeyPressed)
        {
            if (event.key.code == sf::Keyboard::Escape)
                m_window.close();
        }
        else if (event.type == sf::Event::KeyReleased)
        {
            // Increase or decrease speed of game update.
            if (event.key.code == sf::Keyboard::Dash  && elapsedTimeMax > 0) elapsedTimeMax -= 0.01;
            if (event.key.code == sf::Keyboard::Equal && elapsedTimeMax < 1) elapsedTimeMax += 0.01;
        }
    }
}

} // namespace sai