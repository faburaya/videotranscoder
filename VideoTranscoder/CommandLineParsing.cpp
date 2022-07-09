#include "stdafx.h"
#include "CommandLineParsing.hpp"
#include <3fd/utils/text.h>
#include <iostream>
#include <iomanip>

namespace application
{
    bool ParseCommandLineArgs(int argc, char* argv[], CmdLineParams& params)
    {
        CALL_STACK_TRACE;

        using _3fd::core::CommandLineArguments;

        CommandLineArguments cmdLineArgs(120,
                                         CommandLineArguments::ArgOptionSign::Slash,
                                         CommandLineArguments::ArgValSeparator::Colon,
                                         false);

        enum {
            ArgValEncoder,
            ArgOptHwAcc,
            ArgValTgtQuality, 
            ArgValGpuDev, 
            ArgValsListIO
        };

        cmdLineArgs.AddExpectedArgument(CommandLineArguments::ArgDeclaration{
            ArgValEncoder,
            CommandLineArguments::ArgType::OptionWithReqValue,
            CommandLineArguments::ArgValType::EnumString,
            'e', "encoder",
            "What video encoder to use (powered by Microsoft Media Foundation)"
        }, { "h264", "hevc" });

        cmdLineArgs.AddExpectedArgument(CommandLineArguments::ArgDeclaration{
            ArgOptHwAcc,
            CommandLineArguments::ArgType::OptionSwitch,
            CommandLineArguments::ArgValType::None,
            's', "nohwacc",
            "Disable hardware acceleration of encoder in order to prevent distortion"
            });

        cmdLineArgs.AddExpectedArgument(CommandLineArguments::ArgDeclaration{
            ArgValTgtQuality,
            CommandLineArguments::ArgType::OptionWithReqValue,
            CommandLineArguments::ArgValType::RangeFloat,
            'q', "quality",
            "The targetted quality of the video stream output"
        }, { 0.5, 0.001, 1.0 });

        cmdLineArgs.AddExpectedArgument(CommandLineArguments::ArgDeclaration{
            ArgValGpuDev,
            CommandLineArguments::ArgType::OptionWithReqValue,
            CommandLineArguments::ArgValType::String,
            'g', "gpu",
            "Which GPU device to use for hardware acceleration (any part of the name)"
        });

        cmdLineArgs.AddExpectedArgument(CommandLineArguments::ArgDeclaration{
            ArgValsListIO,
            CommandLineArguments::ArgType::ValuesList,
            CommandLineArguments::ArgValType::String,
            0, "input output",
            "input & output files"
        }, { (uint16_t)2, (uint16_t)2 });

        const char* usageMessage("\nUsage:\n\n VideoTranscoder [/e:encoder] [/s] [/q:quality] input output\n\n");

        if (cmdLineArgs.Parse(argc, argv) == STATUS_FAIL)
        {
            std::cerr << usageMessage;
            cmdLineArgs.PrintArgsInfo();
            return STATUS_FAIL;
        }

        bool isPresent;
        const char* encoderLabel = cmdLineArgs.GetArgValueString(ArgValEncoder, isPresent);
        std::cout << '\n' << std::setw(25)
            << "encoder = "
            << _3fd::utils::to_upper(encoderLabel)
            << (isPresent ? " " : " (default)");
        
        if (strcmp(encoderLabel, "h264") == 0)
            params.encoder = Encoder::H264_AVC;
        else if (strcmp(encoderLabel, "hevc") == 0)
            params.encoder = Encoder::H265_HEVC;
        else
            _ASSERTE(false);

        params.tgtQuality = cmdLineArgs.GetArgValueFloat(ArgValTgtQuality, isPresent);
        std::cout << '\n' << std::setw(25) << "targetted quality = " << params.tgtQuality
            << (isPresent ? " " : " (default)");

        params.gpuDevNameKey = cmdLineArgs.GetArgValueString(ArgValGpuDev, isPresent);
        std::cout << '\n' << std::setw(25) << "accelerator GPU = ";
        if (isPresent)
        {
            std::cout << '"' << params.gpuDevNameKey << "\" (try to match name)";
        }
        else
        {
            params.gpuDevNameKey = "";
            std::cout << "(default)";
        }

        params.disableEncoderHwAcc = cmdLineArgs.GetArgSwitchOptionValue(ArgOptHwAcc);
        std::cout << '\n' << std::setw(25) << "encoder acceleration = "
            << (params.disableEncoderHwAcc ? "disabled" : "try");

        std::vector<const char*> filesNames;
        cmdLineArgs.GetArgListOfValues(filesNames);

        if (filesNames.size() != 2)
        {
            std::cerr << "\nMust provide input & output files!\n" << usageMessage;
            cmdLineArgs.PrintArgsInfo();
            return STATUS_FAIL;
        }

        params.inputFName = filesNames[0];
        std::cout << '\n' << std::setw(25)
            << "input = " << params.inputFName;

        params.outputFName = filesNames[1];
        std::cout << '\n' << std::setw(25)
            << "output = " << params.outputFName << '\n' << std::endl;

        return STATUS_OKAY;
    }

}// end of namespace application