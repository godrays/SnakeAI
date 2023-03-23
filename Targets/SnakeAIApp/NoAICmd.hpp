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
#include "SnakeGame.hpp"
// External includes
#include <docopt/docopt.h>
#include <SFML/Graphics.hpp>
// System includes
#include <map>


namespace sai::cmd
{

class NoAICmd : public BaseCmd
{
public:
    // Constructor
    NoAICmd() = default;

    // Destructor
    virtual ~NoAICmd() = default;

    void Run(int argc, const char * argv[]) final;

protected:
    // Validate required arguments.
    bool ValidateArguments(std::map<std::string, docopt::value> & args, const char * USAGE);

    // Executes the command based on the given commandline parameter options.
    void ExecuteCommand(std::map<std::string, docopt::value> & args);

    // Draws game board.
    void DrawGameBoard(sf::Text& text);

    // Updates position of the drawable game board blocks.
    void UpdateGameBoardsDrawableBlocks(SnakeGame& snakeGame);

private:
    int m_boardWidth{10};
    int m_boardHeight{10};
    int m_blockSize{25};

    sf::RenderWindow   m_window;
    std::vector<sf::RectangleShape>  m_boardBlocks;
};

}
