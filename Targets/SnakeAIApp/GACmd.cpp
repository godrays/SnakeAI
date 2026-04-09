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
#include <AIXExtensions.hpp>
#include <FontSFNSMono.hpp>
#include <GeneticAlgorithm.hpp>
#include <SnakeGame.hpp>
// External includes
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
// System includes
#include <filesystem>
#include <ranges>
#include <iostream>
#include <limits>
#include <vector>


namespace sai::cmd
{

void GACmd::run(const int argc, const char *argv[])
{
    static constexpr char USAGE[] =
    R"(
    Snake AI - Copyright (c) 2023-Present, Arkin Terli. All rights reserved.

    Usage:
        SnakeAIApp ga play  --modelfile=<name> [--bw=<number> --bh=<number>] [--bls=<number>]
        SnakeAIApp ga train --modelfile=<name> [--bw=<number> --bh=<number>] [--bls=<number>]
                                               [--ps=<number>] [--pr=<number>] [--mp=<number>]
                                               [--tr=<number>] [--cr=<number>] [--sc=<number>]
                                               [--maxGen=<number>]

    Options:

        --modelfile=<name>      Model filename.

        --bw=<number>           Board width in block units.  [Default: 10]
        --bh=<number>           Board height in block units. [Default: 10]
        --bls=<number>          Block size in pixel units.   [Default: 25]

        --ps=number             Population size.            [Default: 50]
        --pr=number             Parent ratio (%).           [Default: 50]
        --mp=number             Mutate probability (%).     [Default: 1]
        --tr=number             Transfer ratio (%).         [Default: 15]
        --cr=number             Crossover (%).              [Default: 50]
        --sc=number             Model sampling count per generation. [Default: 200]
        --maxGen=number         Maximum number of generation for training. [Default: 1000]
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

    if (!validateArguments(args, USAGE))
    {
        return;
    }

    // Execute the command.
    executeCommand(args);
}


bool GACmd::validateArguments(std::map <std::string, docopt::value>& args, const char* USAGE)
{
    // Show help if necessary
    if (args["-h"] || args["--help"])
    {
        std::cout << USAGE << std::endl;
        return false;
    }

    auto CheckRangeLong = [&](const std::string& paramName, const int min, const int max) -> bool
    {
        if (args[paramName] && (args[paramName].asLong() < min || args[paramName].asLong() > max))
        {
            std::cout << "Invalid parameter range: " << paramName
                      << " must be in [" << min << "," << max << "]" << std::endl;
            return false;
        }
        return true;
    };

    // VALIDATE ARGUMENTS

    if (!CheckRangeLong("--bw",  10, 100) ||
        !CheckRangeLong("--bh",  10, 100) ||
        !CheckRangeLong("--bls", 10, 100) ||
        !CheckRangeLong("--ps",  10, 1000000) ||
        !CheckRangeLong("--pr",  0, 100)  ||
        !CheckRangeLong("--mr",  0, 100)  ||
        !CheckRangeLong("--tr",  0, 100)  ||
        !CheckRangeLong("--cr",  0, 100)  ||
        !CheckRangeLong("--sc",  1, 1000000)  ||
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


void GACmd::executeCommand(std::map <std::string, docopt::value> & args)
{
    const std::string modelFilename = args["--modelfile"].asString();

    // Override parameters here
    if (args["--bw"])  m_boardWidth  = args["--bw"].asLong();
    if (args["--bh"])  m_boardHeight = args["--bh"].asLong();
    if (args["--bls"]) m_blockSize   = args["--bls"].asLong();
    if (args["--ps"])  m_gaPopulationSize = args["--ps"].asLong();
    if (args["--pr"])  m_gaParentRatio    = args["--pr"].asLong();
    if (args["--mp"])  m_gaMutateProb     = args["--mp"].asLong();
    if (args["--tr"])  m_gaTransferRatio  = args["--tr"].asLong();
    if (args["--cr"])  m_gaCrossover      = args["--cr"].asLong();
    if (args["--sc"])  m_gaSamplingSize   = args["--sc"].asLong();
    if (args["--maxGen"]) m_maxGeneration = args["--maxGen"].asLong();

    if (args["play"].asBool())
    {
        playModel(modelFilename);
    }
    else if (args["train"].asBool())
    {
        trainModel(modelFilename);
    }
}


void GACmd::playModel(const std::string & modelFilename) {
    aix::NoGradGuard grad;

    std::random_device rndDev;
    const int rndSeed = static_cast<int>(rndDev());
    const int windowWidth  = m_boardWidth  * m_blockSize;
    const int windowHeight = m_boardHeight * m_blockSize;

    // Create a window with a title.
    m_window.create(sf::VideoMode({static_cast<unsigned>(windowWidth),
                                   static_cast<unsigned>(windowHeight)}), "Snake AI Model Play Mode");
    m_window.setFramerateLimit(60);

    // Create font
    sf::Font font;
    if (!font.openFromMemory(FontSFNSMono, sizeof(FontSFNSMono)))
    {
        throw std::runtime_error("Failed to load font from memory");
    }

    // Create a text to render on window.
    sf::Text text(font, "", 10);
    text.setPosition({10, 10});
    text.setFillColor(sf::Color::White);

    // Create a snake game to simulate each step.
    SnakeGame   snakeGame(m_boardWidth, m_boardHeight, rndSeed);

    // Create neural network to determine snakes next steps.
    const auto ffnn = createFFNN();
    aix::nn::load(*ffnn, modelFilename);

    // Initialize blocks to render on windows.
    m_boardBlocks.resize(m_boardWidth * m_boardHeight);
    std::ranges::for_each(m_boardBlocks, [&](sf::RectangleShape & shape)
    {
        shape.setSize({static_cast<float>(m_blockSize), static_cast<float>(m_blockSize)});
        shape.setOutlineThickness(2);
        shape.setOutlineColor(sf::Color::Black);
    });

    sf::Clock  clock;
    float  elapsedTime = 0;
    float  elapsedTimeMax = 0.07;

    // Main loop
    while (m_window.isOpen())
    {
        // Time elapsed between two frames.
        const float deltaTime = clock.restart().asSeconds();

        // Processes window and keypress events.
        processEvents(elapsedTimeMax);

        // Call game update only every one second to slow down the snake movement.
        elapsedTime += deltaTime;
        if (elapsedTime > elapsedTimeMax)
        {
            calculateGameNextStep(snakeGame, ffnn);
            updateGameBoardsDrawableBlocks(snakeGame);
            elapsedTime = 0;
        }

        text.setString("Score: " + std::to_string(snakeGame.getScore()));
        drawGameBoard(text);
    }
}


float GACmd::fitnessFunc(const std::vector<float>& value, const size_t samplingSize, const int rndSeed) const
{
    return simulateSnakeGames(samplingSize, value, rndSeed);
}


void GACmd::trainModel(const std::string & modelFilename)
{
    std::random_device rndDev;
    std::mt19937 rndEngine(rndDev());
    const int rndSeed = static_cast<int>(rndDev());

    std::vector<float> data;
    aix::ext::serializeModule(*createFFNN(), data);
    const auto geneticVectorSize = data.size();

    // Create genetic algorithm to search best weights and biases for a neural network.
    ga::GeneticAlgorithm<float>  ga(m_gaPopulationSize, m_gaParentRatio, m_gaMutateProb, m_gaTransferRatio,
                                    m_gaCrossover, geneticVectorSize);

    // This method will calculate fitness value for each individual. Used bind here since using lambda might have a
    // state copy issue on Windows.
    ga.setFitnessFunc(std::bind(&GACmd::fitnessFunc, this, std::placeholders::_1, m_gaSamplingSize, rndSeed));

    // This method will generate random item (genes) for a genetic vector/material (chromosome).
    ga.setRandomItemFunc([&]() -> float
    {
        // Scale the random fraction to the desired range [min, max]
        constexpr float min = -1.0f;
        constexpr float max =  1.0f;
        return std::uniform_real_distribution<float>(min, max)(rndEngine);
    });

    ga.createInitialPopulation();

    float bestFitness = -std::numeric_limits<float>::max();

    std::cout << "Total Model Parameters: " << geneticVectorSize << std::endl;

    while (ga.getGeneration() < m_maxGeneration)
    {
        // Save the best individual.
        if (const float fitness = ga.getBestIndividual().getFitness(); fitness > bestFitness)
        {
            auto ffnn = createFFNN();
            // Set genes vector (weights and biases) coming from genetic algorithm.
            aix::ext::deserializeModule(*ffnn, ga.getBestIndividual().getValue());
            aix::nn::save(*ffnn, modelFilename);

            bestFitness = fitness;
        }

        std::cout << "Generation: " << ga.getGeneration()
                  << "  Fitness: " << static_cast<ssize_t>(bestFitness) << std::endl;
        ga.createNextPopulation();
    }
}


std::shared_ptr<aix::nn::Sequential> GACmd::createFFNN()
{
    // First determine genetic vector size.
    const auto modelInputSize = SnakeGame::getParameterSize();

    auto model = std::make_shared<aix::nn::Sequential>();
    model->add(new aix::nn::Linear(modelInputSize, modelInputSize));
    model->add(new aix::nn::Tanh());
    model->add(new aix::nn::Linear(modelInputSize, modelInputSize/2));
    model->add(new aix::nn::Tanh());
    model->add(new aix::nn::Linear(modelInputSize/2, 4));

    return model;
}


void GACmd::calculateGameNextStep(SnakeGame& snakeGame, const std::shared_ptr<aix::nn::Sequential>& ffnn)
{
    // Get game parameters to use as inputs to neural network model.
    const auto modelInputs = snakeGame.getParameters();
    const auto inputs = aix::Tensor(modelInputs.data(), modelInputs.size(), aix::DataType::kFloat32,
                                    aix::Shape{1, modelInputs.size()});

    // Make prediction and get new snake directions as model outputs.
    const auto outputs = ffnn->forward(inputs);

    // Determine the best direction from model outputs. The highest value should be the new direction.
    snakeGame.setDirection(determineSnakeDirection(outputs));

    // Update game.
    snakeGame.update();

    // Prepare game board to Render.
    if (snakeGame.getGameState() != SnakeGameState::kRunning)
    {
        snakeGame.reset();
    }
}


void GACmd::updateGameBoardsDrawableBlocks(const SnakeGame& snakeGame)
{
    // Update game board block colors to reflect the changes.
    int blockIndex = 0;
    for (int y=0; y < m_boardHeight; ++y)
    {
        for (int x=0; x < m_boardWidth; ++x)
        {
            auto & block = m_boardBlocks[blockIndex];
            block.setPosition({static_cast<float>(x * m_blockSize), static_cast<float>(y * m_blockSize)});
            switch (snakeGame.getBoardObject(x, y))
            {
                case BoardObjType::kSnakeHead:   block.setFillColor(sf::Color::Yellow);  break;
                case BoardObjType::kSnakeBody:   block.setFillColor(sf::Color::Green);   break;
                case BoardObjType::kApple:       block.setFillColor(sf::Color::Red);     break;
                default:                         block.setFillColor(sf::Color::Black);   break;
            }
            blockIndex++;
        }
    }
}


void GACmd::drawGameBoard(const sf::Text& text)
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


float GACmd::simulateSnakeGames(const std::size_t samplingSize, const std::vector<float> & genesVector,
                                const int rndSeed) const
{
    // Set up a neural network.
    const auto ffnn = createFFNN();

    // Set weights and biases coming from genetic algorithm.
    aix::ext::deserializeModule(*ffnn, genesVector);   // value = genetic material vector = chromosome

    // Create a new snake game.
    SnakeGame snakeGame(m_boardWidth, m_boardHeight, rndSeed);

    float highestScore = 0;
    float avgDeaths = 0;
    float avgSteps = 0;
    float avgLongLoopFails = 0;
    float avgScore = 0;

    // Run the same model N times to assess quality of the individual (chromosome/array of genes/NN Model weights).
    for (std::size_t i=0; i<samplingSize; ++i)
    {
        while (snakeGame.getGameState() == SnakeGameState::kRunning)
        {
            // Get game parameters to use as inputs to neural network model.
            auto modelInputs = snakeGame.getParameters();
            const auto inputs = aix::Tensor(modelInputs.data(), modelInputs.size(), aix::DataType::kFloat32,
                                            aix::Shape{1, modelInputs.size()});

            // Make prediction and get new snake directions as model outputs.
            auto outputs = ffnn->forward(inputs);

            // Determine the best direction from model outputs. The highest value should be the new direction.
            snakeGame.setDirection(determineSnakeDirection(outputs));

            // Update game.
            snakeGame.update();
        }

        if (snakeGame.getGameState() == SnakeGameState::kFailedHitWall ||
            snakeGame.getGameState() == SnakeGameState::kFailedHitItself)
        {
            avgDeaths++;
        }
        if (snakeGame.getGameState() == SnakeGameState::kFailedLongLoop)
        {
            avgLongLoopFails++;
        }

        highestScore = std::max<float>(highestScore, snakeGame.getScore());
        avgSteps += snakeGame.getSteps();
        avgScore += snakeGame.getScore();

        snakeGame.reset();
    }

    // Return fitness value to tell the genetic algorithm how well the neural network has played the game so far.
    // Fitness formula is very important.
    const auto fSamplingSize = static_cast<float>(samplingSize);
    avgSteps  /= fSamplingSize;
    avgDeaths /= fSamplingSize;
    avgScore  /= fSamplingSize;
    avgLongLoopFails /= fSamplingSize;

    return highestScore * 500 + avgScore * 50 - avgDeaths * 15 - avgSteps * 10 - avgLongLoopFails * 100;
}


SnakeDirection GACmd::determineSnakeDirection(const aix::Tensor& outputs)
{
    const auto o0 = outputs.value().getValueAt<float>({0, 0});
    const auto o1 = outputs.value().getValueAt<float>({0, 1});
    const auto o2 = outputs.value().getValueAt<float>({0, 2});
    const auto o3 = outputs.value().getValueAt<float>({0, 3});

    auto newDir = SnakeDirection::kUp;
    float maxValue = o0;

    if (maxValue < o1) { newDir = SnakeDirection::kDown; maxValue = o1; }
    if (maxValue < o2) { newDir = SnakeDirection::kLeft; maxValue = o2; }
    if (maxValue < o3) { newDir = SnakeDirection::kRight; }

    return newDir;
}


void GACmd::processEvents(float& elapsedTimeMax)
{
    // Process events
    while (const auto event = m_window.pollEvent())
    {
        // Close the window when the user clicks the close button.
        if (event->is<sf::Event::Closed>())
        {
            m_window.close();
        }

        // Check if the event is a key pressed event.
        if (event->is<sf::Event::KeyPressed>())
        {
            const auto keyCode = event->getIf<sf::Event::KeyPressed>()->code;

            // Increase or decrease speed of game update.
            if (keyCode == sf::Keyboard::Key::Hyphen && elapsedTimeMax > 0) elapsedTimeMax -= 0.01;
            if (keyCode == sf::Keyboard::Key::Equal  && elapsedTimeMax < 1) elapsedTimeMax += 0.01;
            if (keyCode == sf::Keyboard::Key::Escape)
            {
                m_window.close();
                break;
            }
        }
    }
}

} // namespace sai