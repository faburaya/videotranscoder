// VideoTranscoder.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "MediaFoundationWrappers.h"
#include "CommandLineParsing.hpp"
#include <3fd\core\runtime.h>
#include <3fd\core\exceptions.h>
#include <3fd\core\callstacktracer.h>
#include <3fd\core\logger.h>
#include <3fd\utils\cmdline.h>

#include <array>
#include <codecvt>
#include <iostream>
#include <mfapi.h>

using TimePoint = std::chrono::time_point<std::chrono::system_clock>;

namespace application
{
    using namespace _3fd::core;

    const char* GetTimestamp(const time_t timeTicks)
    {
        static std::array<char, 21> timestamp;
        strftime(timestamp.data(), timestamp.size(), "%Y-%b-%d %H:%M:%S", localtime(&timeTicks));
        return timestamp.data();
    }

    /// <summary>
    /// Prints a progress bar given the progress.
    /// </summary>
    /// <param name="progress">Progress of transcoding within range [0,1].</param>
    void PrintProgressBar(double progress, const TimePoint startTime)
    {
        using std::chrono::minutes;

        // Amount of steps inside the progress bar
        const int qtBarSteps(30);

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
                std::cout << ' ';

            std::cout << "] " << donePercentage << " % done";
            std::cout << " / Remaining " << eta.count() << " min  " << std::flush;
            return;
        }

        const minutes totalTime = duration_cast<minutes>(system_clock::now() - startTime);
        std::cout << "\rTranscoding finished at " << GetTimestamp(time(nullptr))
                  << " (total elapsed time was " << totalTime.count() << " min)\n" << std::endl;
    }

}// end of namespace application

/////////////////
// Entry Point
/////////////////

using namespace Microsoft::WRL;

int main(int argc, char *argv[])
{
    using namespace std::chrono;
    using namespace _3fd::core;

    FrameworkInstance frameworkInstance(FrameworkInstance::ComMultiThreaded);

    CALL_STACK_TRACE;

    try
    {
        SetConsoleOutputCP(CP_UTF8);

        application::CmdLineParams params;
        if (application::ParseCommandLineArgs(argc, argv, params) == STATUS_FAIL)
            return EXIT_FAILURE;

        application::MediaFoundationLib msmflib;

        // Create an instance of the Microsoft DirectX Graphics Infrastructure (DXGI) Device Manager:
        ComPtr<IMFDXGIDeviceManager> mfDXGIDevMan;
        UINT dxgiResetToken;
        HRESULT hr = MFCreateDXGIDeviceManager(&dxgiResetToken, mfDXGIDevMan.GetAddressOf());
        if (FAILED(hr))
        {
            WWAPI::RaiseHResultException(hr,
                "Failed to create Microsoft DXGI Device Manager object",
                "MFCreateDXGIDeviceManager");
        }

        // Get Direct3D device and associate with DXGI device manager:
        auto d3dDevice = application::GetDeviceDirect3D(params.gpuDevNameKey);
        mfDXGIDevMan->ResetDevice(d3dDevice.Get(), dxgiResetToken);

        // Load media source and select decoders
        application::MFSourceReader sourceReader(params.inputFName, mfDXGIDevMan);

        // Start reading early to avoid waiting
        sourceReader.ReadSampleAsync();

        // Gather info about reader output decoded streams:
        std::map<DWORD, application::DecodedMediaType> srcReadDecStreams;
        sourceReader.GetOutputMediaTypesFrom(0UL, srcReadDecStreams);
        
        if (srcReadDecStreams.empty())
        {
            std::cout << "\nInput media file had no video or audio streams to decode!\n" << std::endl;
            return EXIT_SUCCESS;
        }

        auto duration = sourceReader.GetDuration();
        std::cout << "\nInput media file is " << duration_cast<seconds>(duration).count() << " seconds long\n";

        // Prepare media sink and select encoders
        application::MFSinkWriter sinkWriter(params.outputFName,
                                             mfDXGIDevMan,
                                             srcReadDecStreams,
                                             params.tgtSizeFactor,
                                             params.encoder,
                                             !params.disableEncoderHwAcc);

        const TimePoint startTime = system_clock::now();
        std::cout << "\nTranscoding starting at "
                  << application::GetTimestamp(system_clock::to_time_t(startTime))
                  << '\n' << std::endl;

        application::PrintProgressBar(0.0, startTime);

        try
        {
            DWORD state;

            // Loop for transcoding (decoded source reader output goes to sink writer input):
            do
            {
                DWORD idxStream;
                auto sample = sourceReader.GetSample(idxStream, state);
                sourceReader.ReadSampleAsync();

                if (!sample)
                    continue;

                LONGLONG timestampIn100ns;
                sample->GetSampleTime(&timestampIn100ns);
                double progress = timestampIn100ns / (duration.count() * 10.0);
                application::PrintProgressBar(progress, startTime);

                if ((state & static_cast<DWORD> (application::ReadStateFlags::GapFound)) != 0)
                {
                    sinkWriter.PlaceGap(idxStream, timestampIn100ns);
                }
                else if ((state & static_cast<DWORD> (application::ReadStateFlags::NewStreamAvailable)) != 0)
                {
                    auto prevLastIdxStream = srcReadDecStreams.rbegin()->first;
                    sourceReader.GetOutputMediaTypesFrom(prevLastIdxStream + 1, srcReadDecStreams);
                    sinkWriter.AddNewStreams(srcReadDecStreams, params.tgtSizeFactor, params.encoder);
                }

                sinkWriter.EncodeSample(idxStream, sample);

            } while ((state & static_cast<DWORD> (application::ReadStateFlags::EndOfStream)) == 0);
        }
        catch (HResultException &ex)
        {
            // In case of "GPU device removed" error, log some additional info:
            if (ex.GetErrorCode() == DXGI_ERROR_DEVICE_REMOVED)
            {
                std::ostringstream oss;
                oss << "There is a failure related to the GPU device: "
                    << WWAPI::GetDetailsFromHResult(d3dDevice->GetDeviceRemovedReason());

                Logger::Write(oss.str(), Logger::PRIO_FATAL);
            }

            throw;
        }

        application::PrintProgressBar(1.0, startTime);
    }
    catch (IAppException &ex)
    {
        Logger::Write(ex, Logger::PRIO_FATAL);
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
