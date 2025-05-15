// VideoTranscoder.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "CommandLineParsing.hpp"
#include "MediaSession.hpp"
#include "MediaSource.hpp"
#include "MmfLibScope.hpp"
#include "TranscodeProfile.hpp"
#include "TranscodeTopology.hpp"

#include <MinCppXtra/call_stack_access_scope.hpp>
#include <MinCppXtra/seh_translation_scope.hpp>
#include <MinCppXtra/traceable_exception.hpp>
#include <MinCppXtra/win32_api_strings.hpp>

#include <array>
#include <chrono>
#include <iostream>
#include <stdexcept>

using TimePoint = std::chrono::time_point<std::chrono::system_clock>;

namespace application
{
    static const char* GetTimestamp(const time_t timeTicks)
    {
        static std::array<char, 21> timestamp;
        strftime(timestamp.data(), timestamp.size(), "%Y-%b-%d %H:%M:%S", localtime(&timeTicks));
        return timestamp.data();
    }

    /// <summary>
    /// Prints a progress bar given the progress.
    /// </summary>
    /// <param name="progress">Progress of transcoding within range [0,1].</param>
    static void PrintProgressBar(double progress, const TimePoint startTime)
    {
        using std::chrono::minutes;

        // Amount of steps inside the progress bar
        const int qtBarSteps(40);

        static int donePercentage(0);
        static int doneSteps(0);

        // Only update the progress bar if there is a change:
        int updatePercentage = static_cast<int>(progress * 100);
        if (updatePercentage == donePercentage)
            return;
        else
        {
            donePercentage = updatePercentage;
            doneSteps = static_cast<int>(qtBarSteps * progress);
        }

        using std::chrono::system_clock;
        using std::chrono::duration_cast;

        const minutes eta = duration_cast<minutes>(
            (system_clock::now() - startTime) * (1.0 - progress) / progress
        );

        // Print the progress bar:
        if (progress < 1.0)
        {
            std::cout << "\rProgress [";

            for (int idx = 0; idx < doneSteps; ++idx)
                std::cout << '#';

            int remaining = qtBarSteps - doneSteps;
            for (int idx = 0; idx < remaining; ++idx)
                std::cout << '_';

            std::cout << "] " << donePercentage << " % done";
            std::cout << " / Remaining " << eta.count() << " min  " << std::flush;
            return;
        }

        const minutes totalTime = duration_cast<minutes>(system_clock::now() - startTime);
        std::cout
            << "\rTranscoding finished at " << GetTimestamp(time(nullptr))
            << " (total elapsed time was " << totalTime.count() << " minutes)"
            << std::endl << std::endl;
    }

}// end of namespace application

/////////////////
// Entry Point
/////////////////

int main(int argc, char *argv[])
{
    using namespace std::chrono;
    using namespace Microsoft::WRL;

    try
    {
        mincpp::TraceableException::UseColorsOnStackTrace(true);

        application::CmdLineParams params;
        if (!application::ParseCommandLineArgs(argc, argv, params))
            return EXIT_FAILURE;

        mincpp::CallStackAccessScope callStackAccessScope;
        mincpp::SehTranslationScope sehTranslationScope;
        application::MmfLibScope mmfLibScope;

        application::MediaSource mediaSource(
            mincpp::Win32ApiStrings::ToUtf16(params.inputFName)
        );

        auto duration = mediaSource.GetDuration();
        std::cout << std::endl
            << "Input media file is "
            << duration_cast<seconds>(duration).count()
            << " seconds long" << std::endl;

        application::TranscodeProfile transcodeProfile(
            mediaSource.GetMediaInfo(),
            params.encoder,
            params.tgtSize
        );

        application::TranscodeTopology transcodeTopology(
            mediaSource.GetMfObject(),
            transcodeProfile.GetMfObject(),
            params.outputFName
        );

        if (transcodeTopology.IsHardwareAccelerated())
        {
            std::cout << std::endl
                << "Hardware accelerated transcoding detected 👍"
                << std::endl;
        }

        const TimePoint startTime = system_clock::now();
        std::cout << std::endl
            << "Transcoding starting at "
            << application::GetTimestamp(system_clock::to_time_t(startTime))
            << std::endl << std::endl;

        ComPtr<application::MediaSession> mediaSession(new application::MediaSession());
        mediaSession->StartEncodingSession(transcodeTopology.GetMfObject());

        application::PrintProgressBar(0.0, startTime);

        // Loop for transcoding:
        HRESULT asyncResult;
        while ((asyncResult = mediaSession->Wait(milliseconds(500))) == E_PENDING)
        {
            decltype(duration) position = mediaSession->GetEncodingPosition();
            double progress = (double)position.count() / duration.count();
            application::PrintProgressBar(progress, startTime);
        }

        if (FAILED(asyncResult))
            return EXIT_FAILURE;

        application::PrintProgressBar(1.0, startTime);
    }
    catch (mincpp::TraceableException &ex)
    {
        std::cerr << std::endl << ex.Serialize() << std::endl;
        return EXIT_FAILURE;
    }
    catch (std::exception& ex)
    {
        std::cerr << std::endl << "ERROR: " << ex.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
