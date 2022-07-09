#ifndef COMMAND_LINE_PARSING_H
#define COMMAND_LINE_PARSING_H // header guard

#include "MediaFoundationWrappers.h"
#include <3fd/utils/cmdline.h>

namespace application
{
    struct CmdLineParams
    {
        double tgtSizeFactor;
        Encoder encoder;
        bool disableEncoderHwAcc;
        const char* gpuDevNameKey;
        const char* inputFName;
        const char* outputFName;
    };

    bool ParseCommandLineArgs(int argc, char* argv[], CmdLineParams& params);

}// end of namespace application

#endif // end of header guard
