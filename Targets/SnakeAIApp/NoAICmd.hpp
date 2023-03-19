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
// External includes
#include <docopt/docopt.h>
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
};

}
