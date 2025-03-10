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
#include "GACmd.hpp"
// External includes
// System includes
#include <exception>
#include <iostream>
#include <memory>
#include <vector>


int main(int argc, const char* argv[])
{
    static const char USAGE[] =
    R"(
    Snake AI - Copyright (c) 2023-Present, Arkin Terli. All rights reserved.

    Usage:
        SnakeAIApp <command> [<args>...]

    Options:

        -h, --help            Show this screen.

    Commands:

        noai                  User controlled snake game. No AI is used.

        ga                    Use genetic algorithm to train or to play.

    Use 'SnakeAIApp <command> -h' for more information on a specific command.
    )";

    try
    {
        std::vector<std::string>    args{ argv + 1, argv + argc };

        if (args.empty() || args[0] == "-h" || args[0] == "--help")
        {
            std::cout << USAGE << std::endl;
        }
        else if (args[0] == "noai")
        {
            sai::cmd::NoAICmd cmd;
            cmd.Run(argc, argv);
        }
        else if (args[0] == "ga")
        {
            sai::cmd::GACmd cmd;
            cmd.Run(argc, argv);
        }
        else
        {
            std::cerr << "Invalid commandline parameter usage. Please use '--help' parameter for more information." << std::endl;
            return -1;
        }
    }
    catch (std::exception & e)
    {
        std::cout << "EXCEPTION: " << e.what() << std::endl;
        return -1;
    }

    return 0;
}
