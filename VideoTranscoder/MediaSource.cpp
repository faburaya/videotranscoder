#include "stdafx.h"
#include "MediaSource.hpp"

#include <chrono>
#include <fcntl.h>
#include <io.h>
#include <iostream>
#include <Shlwapi.h>
#include <sstream>

#include "AppException.hpp"
#include "MediaInfo.hpp"

namespace application
{
    static uint32_t GetFileSize(const std::wstring& filePath)
    {
        int fd;
        errno_t rc = _wsopen_s(&fd, filePath.c_str(), _O_RDONLY | _O_BINARY, _SH_DENYNO, 0);

        if (rc != 0)
        {
            char buffer[256];

            if (strerror_s(buffer, sizeof buffer, rc) != 0)
            {
                snprintf(buffer, sizeof buffer,
                    "(secondary failure prevented retrieval of further details about error code %d)", rc);
            }
        
            std::ostringstream oss;
            oss << "Could not open file to assess its size: " << buffer;
            throw AppException(oss.str());
        }

        auto fileSize = _filelength(fd);
        _close(fd);
        return static_cast<uint32_t> (fileSize);
    }

    MediaSource::MediaSource(const std::wstring& mediaFilePath)
        : m_fileSize(GetFileSize(mediaFilePath))
    {
        ComPtr<IMFSourceResolver> sourceResolver;
        CHECK("create source resolver",
            MFCreateSourceResolver(sourceResolver.GetAddressOf()));

        ComPtr<IUnknown> source;
        MF_OBJECT_TYPE objectType = MF_OBJECT_INVALID;

        CHECK("create media source",
            sourceResolver->CreateObjectFromURL(
                mediaFilePath.c_str(),
                MF_RESOLUTION_MEDIASOURCE,
                NULL,
                &objectType,
                source.GetAddressOf()));

        CHECK("get IMFMediaSource interface",
            source->QueryInterface(
                IID_PPV_ARGS(m_mfMediaSource.GetAddressOf())));
    }

    MediaSource::~MediaSource()
    {
        LOG("shutdown media source", m_mfMediaSource->Shutdown());
    }

    ComPtr<IMFPresentationDescriptor> MediaSource::GetPresentationDescriptor() const
    {
        ComPtr<IMFPresentationDescriptor> presentationDescriptor;
        CHECK("create presentation descriptor",
            m_mfMediaSource->CreatePresentationDescriptor(presentationDescriptor.GetAddressOf()));
        return presentationDescriptor;
    }

    std::chrono::nanoseconds MediaSource::GetDuration() const
    {
        ComPtr<IMFPresentationDescriptor> presentationDescriptor = GetPresentationDescriptor();

        uint64_t duration;
        CHECK("get media source duration",
            presentationDescriptor->GetUINT64(MF_PD_DURATION, &duration));

        return std::chrono::nanoseconds(duration * 100);
    }

    MediaInfo MediaSource::GetMediaInfo() const
    {
        MediaInfo info = {};
        ComPtr<IMFPresentationDescriptor> presentationDescriptor = GetPresentationDescriptor();

        DWORD streamCount;
        CHECK("get source stream count",
            presentationDescriptor->GetStreamDescriptorCount(&streamCount));

        // iterate over streams:
        for (DWORD streamIdx = 0; streamIdx < streamCount; ++streamIdx)
        {
            BOOL selected;
            ComPtr<IMFStreamDescriptor> streamDescriptor;
            CHECK("get source stream descriptor",
                presentationDescriptor->GetStreamDescriptorByIndex(
                    streamIdx, &selected, streamDescriptor.GetAddressOf()));

            ComPtr<IMFMediaTypeHandler> mediaTypeHandler;
            CHECK("get media type handler for source stream",
                streamDescriptor->GetMediaTypeHandler(mediaTypeHandler.GetAddressOf()));

            GUID majorType;
            CHECK("get major type of source stream", mediaTypeHandler->GetMajorType(&majorType));

            ComPtr<IMFMediaType> mediaType;
            CHECK("get media type of source stream",
                mediaTypeHandler->GetCurrentMediaType(mediaType.GetAddressOf()));

            if (majorType == MFMediaType_Video)
            {
                CHECK("get info (frame size) from source video stream",
                    MFGetAttributeSize(mediaType.Get(),
                        MF_MT_FRAME_SIZE,
                        &info.videoProfile.frameSize.width,
                        &info.videoProfile.frameSize.height));

                CHECK("get info (frame rate) frou source video stream",
                    MFGetAttributeRatio(mediaType.Get(),
                        MF_MT_FRAME_RATE,
                        &info.videoProfile.frameRate.numerator,
                        &info.videoProfile.frameRate.denominator));
                
                // average bit rate not available?
                if (FAILED(mediaType->GetUINT32(MF_MT_AVG_BITRATE, &info.videoProfile.avgBitrate)))
                {
                    // estimate using file size:
                    info.videoProfile.avgBitrate =
                        static_cast<UINT32> (
                            0.98 * m_fileSize * 8 /
                                std::chrono::duration_cast<std::chrono::seconds>(GetDuration()).count()
                        );

                    std::cout << std::endl
                        << "Average bitrate not available in source: estimated as "
                        << std::fixed << std::setprecision(1)
                        << ((float)info.videoProfile.avgBitrate / (8 * 1024))
                        << " KB/s" << std::endl;
                }
            }
            else if (majorType == MFMediaType_Audio)
            {
                CHECK("get info (bits per sample) from source audio stream",
                    mediaType->GetUINT32(MF_MT_AUDIO_BITS_PER_SAMPLE, &info.audioProfile.bitsPerSample));

                CHECK("get info (samples per sec) from source audio stream",
                    mediaType->GetUINT32(MF_MT_AUDIO_SAMPLES_PER_SECOND, &info.audioProfile.samplesPerSec));

                CHECK("get info (num channels) from source audio stream",
                    mediaType->GetUINT32(MF_MT_AUDIO_NUM_CHANNELS, &info.audioProfile.numChannels));

                CHECK("get info (avg Bps) from source audio stream",
                    mediaType->GetUINT32(MF_MT_AUDIO_AVG_BYTES_PER_SECOND, &info.audioProfile.avgBytesPerSec));
            }
        }

        return info;
    }
}