//
//  Copyright © 2023-Present, Arkin Terli. All rights reserved.
//
//  NOTICE:  All information contained herein is, and remains the property of Arkin Terli.
//  The intellectual and technical concepts contained herein are proprietary to Arkin Terli
//  and may be covered by U.S. and Foreign Patents, patents in process, and are protected by
//  trade secret or copyright law. Dissemination of this information or reproduction of this
//  material is strictly forbidden unless prior written permission is obtained from Arkin Terli.

#pragma once

// Project includes
#include "BaseCmd.hpp"
#include "SFML/Graphics.hpp"
#include "SnakeGame.hpp"
#include "FFNN.hpp"
// External includes
#include <docopt/docopt.h>
// System includes
#include <map>


namespace sai::cmd
{

class GACmd : public BaseCmd
{
public:
    // Constructor
    GACmd() = default;

    // Destructor
    virtual ~GACmd() = default;

    void Run(int argc, const char * argv[]) final;

protected:
    // Validate required arguments.
    bool ValidateArguments(std::map<std::string, docopt::value> & args, const char * USAGE);

    // Executes the command based on the given commandline parameter options.
    void ExecuteCommand(std::map<std::string, docopt::value> & args);

    void PlayModel(const std::string & modelFilename);
    void TrainModel(const std::string & modelFilename);

    // Creates and returns a pre-configured FFNN object.
    FFNN CreateFFNN();

    double SimulateSnakeGames(std::size_t samplingSize, const std::vector<double> & genesVector, int rndSeed);

    // Calculates game's next step.
    void CalculateGameNextStep(SnakeGame& snakeGame, FFNN& ffnn) const;

    // Draws game board.
    void DrawGameBoard(sf::Text& text);

    // Determine direction of the snake from ML model outputs.
    SnakeDirection DetermineSnakeDirection(const Eigen::MatrixXd& outputs) const;

    // Updates position of the drawable game board blocks.
    void UpdateGameBoardsDrawableBlocks(SnakeGame& snakeGame);

    // Processes window and keypress events.
    void ProcessEvents(float &elapsedTimeMax);

private:
    int m_boardWidth{10};
    int m_boardHeight{10};
    int m_blockSize{25};

    std::size_t m_gaPopulationSize{50};
    std::size_t m_gaParentRatio{50};
    std::size_t m_gaMutateProb{1};
    std::size_t m_gaTransferRatio{15};
    std::size_t m_gaCrossover{50};
    std::size_t m_gaSamplingSize{2000};
    std::size_t m_maxGeneration{1000};

    sf::RenderWindow   m_window;
    std::vector<sf::RectangleShape>  m_boardBlocks;
};

}
