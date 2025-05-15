#pragma once

#include "Encoder.hpp"
#include <string>

namespace application
{
    struct CmdLineParams
    {
        Encoder encoder;
        double tgtSize;
        std::string inputFName;
        std::string outputFName;
    };

    bool ParseCommandLineArgs(int argc, char* argv[], CmdLineParams& params);
}
