#include "stdafx.h"
#include "CommandLineParsing.hpp"
#include <CLI11/CLI11.hpp>

#include <iostream>
#include <iomanip>
#include <string>

namespace application
{
    bool ParseCommandLineArgs(int argc, char* argv[], CmdLineParams& params)
    {
        CLI::App app("Hardware accelerated video transcoder");

        app.add_option("-i,--input", params.inputFName, "Input video file")->required();
        app.add_option("-o,--output", params.outputFName, "Output MP4 file")->required();

        std::string encoderName;
        app.add_option("-e,--encoder", encoderName,
            "Video encoder to use (from Microsoft Media Foundation)")
            ->required()
            ->check(CLI::IsMember({ "hevc", "h264" }));

        app.add_option("-t,--tsf", params.tgtSize, "Target size factor")
            ->required()
            ->check(CLI::Range(0.0, 1.0));

        app.allow_windows_style_options();

        try
        {
            app.parse(argc, argv);
        }
        catch (CLI::ParseError&ex)
        {
            app.exit(ex);
            std::cout << std::endl;
            return false;
        };

        std::cout << std::endl << std::setw(25) << "input = " << params.inputFName;
        std::cout << std::endl << std::setw(25) << "output = " << params.outputFName;
        std::cout << std::endl << std::setw(25) << "encoder = " << encoderName;

        if (encoderName == "h264")
            params.encoder = Encoder::H264_AVC;
        else if (encoderName == "hevc")
            params.encoder = Encoder::H265_HEVC;
        else
            _ASSERTE(false);

        std::cout
            << std::endl << std::setw(25)
            << "target size factor = " << params.tgtSize
            << std::endl;

        return true;
    }

}// end of namespace application
