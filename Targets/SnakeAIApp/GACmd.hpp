//
//  Copyright © 2023-Present, Arkin Terli. All rights reserved.
//
//  NOTICE:  All information contained herein is, and remains the property of Arkin Terli.
//  The intellectual and technical concepts contained herein are proprietary to Arkin Terli
//  and may be covered by U.S. and Foreign Patents, patents in process, and are protected by
//  trade secret or copyright law. Dissemination of this information or reproduction of this
//  material is strictly forbidden unless prior written permission is obtained from Arkin Terli.

#pragma once

#define SFML_STATIC

// Project includes
#include "BaseCmd.hpp"
#include "SFML/Graphics.hpp"
#include "SnakeGame.hpp"
// External includes
#include <aix.hpp>
#include <docopt/docopt.h>
// System includes
#include <map>


namespace sai::cmd
{

class GACmd final : public BaseCmd
{
public:
    // Constructor
    GACmd() = default;

    // Destructor
    ~GACmd() override = default;

    void run(int argc, const char * argv[]) override;

protected:
    // Validate required arguments.
    static bool validateArguments(std::map<std::string, docopt::value> & args, const char * USAGE);

    // Executes the command based on the given commandline parameter options.
    void executeCommand(std::map<std::string, docopt::value> & args);

    void playModel(const std::string & modelFilename);
    void trainModel(const std::string & modelFilename);

    // Creates and returns a pre-configured FFNN Model object.
    static std::shared_ptr<aix::nn::Sequential> createFFNN();

    float simulateSnakeGames(std::size_t samplingSize, const std::vector<float> & genesVector, int rndSeed) const;

    // Calculates game's next step.
    static void calculateGameNextStep(SnakeGame& snakeGame, const std::shared_ptr<aix::nn::Sequential>& ffnn) ;

    // Draws game board.
    void drawGameBoard(const sf::Text& text);

    // Determine direction of the snake from ML model outputs.
    static SnakeDirection determineSnakeDirection(const aix::Tensor& outputs) ;

    // Updates position of the drawable game board blocks.
    void updateGameBoardsDrawableBlocks(const SnakeGame& snakeGame);

    // Processes window and keypress events.
    void processEvents(float &elapsedTimeMax);

    float fitnessFunc(const std::vector<float>& value, size_t samplingSize, int rndSeed) const;

private:
    int m_boardWidth{10};
    int m_boardHeight{10};
    int m_blockSize{25};

    std::size_t m_gaPopulationSize{50};
    std::size_t m_gaParentRatio{50};
    std::size_t m_gaMutateProb{1};
    std::size_t m_gaTransferRatio{15};
    std::size_t m_gaCrossover{50};
    std::size_t m_gaSamplingSize{200};
    std::size_t m_maxGeneration{1000};

    sf::RenderWindow   m_window;
    std::vector<sf::RectangleShape>  m_boardBlocks;
};

}
