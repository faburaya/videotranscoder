#include "stdafx.h"
#include "TranscodeProfile.hpp"

#include <array>
#include <codecapi.h>
#include <iomanip>
#include <iostream>

#include "AppException.hpp"

namespace application
{
    /// <summary>
    /// Estimates a value for the "quality vs speed" configurable parameter for the encoders.
    /// </summary>
    /// <remarks>
    /// In order to maintain good quality, the calculation has been based
    /// on empirical data of real videos using the H.264 encoder.
    /// </remarks>
    /// <returns>An integer in [1,100] for the "quality vs speed" parameter.</returns>
    static uint32_t EstimateBalanceQualityVsSpeed(
        const MediaInfo::VideoProfile& videoInfo, double targetSizeFactor)
    {
        // calculate Bps/pixel:
        const auto rpp =
            static_cast<float> (videoInfo.avgBitrate / 8)
            / (videoInfo.frameSize.width * videoInfo.frameSize.height);

        // The smaller the output has to be, the greater is the encoding complexity to maintain quality.
        // Moreover, take into consideration that when the input is already efficiently encoded (empiric
        // data points to Bps/pixel around 0.5), the complexity is from start expected to be high:
        _ASSERTE(targetSizeFactor > 0.0 && targetSizeFactor <= 1.0);
        if (rpp > 0)
        {
            auto complexity = static_cast<uint32_t> (67 + (1 - targetSizeFactor) * 133 * rpp);
            return std::min(100U, complexity);
        }
        else
            return 80U;
    }

    static uint32_t CalculateAudioTargetBps(const MediaInfo::AudioProfile& sourceInfo)
    {
        // Find a possibly lower data rate for the audio stream:
        const auto enumBpsVals = std::to_array<uint32_t>({ 12000, 16000, 20000, 24000 });
        auto iterEnumBps =
            std::lower_bound(enumBpsVals.begin(), enumBpsVals.end(), sourceInfo.avgBytesPerSec);

        if (iterEnumBps != enumBpsVals.begin()
            && (*iterEnumBps != sourceInfo.avgBytesPerSec || *iterEnumBps > 16000))
        {
            --iterEnumBps;
        }

        return *iterEnumBps;
    }

    static ComPtr<IMFAttributes> CreateAudioProfileAttributes(const MediaInfo::AudioProfile& sourceInfo)
    {
        ComPtr<IMFAttributes> attributes;

        CHECK("create attributes for audio profile",
            MFCreateAttributes(attributes.GetAddressOf(), 6));

        CHECK("set audio media subtype",
            attributes->SetGUID(MF_MT_SUBTYPE, MFAudioFormat_AAC));
        
        CHECK("set audio bits per sample",
            attributes->SetUINT32(MF_MT_AUDIO_BITS_PER_SAMPLE, sourceInfo.bitsPerSample));

        CHECK("set audio samples per sec",
            attributes->SetUINT32(MF_MT_AUDIO_SAMPLES_PER_SECOND, sourceInfo.samplesPerSec));

        CHECK("set audio num channels",
            attributes->SetUINT32(MF_MT_AUDIO_NUM_CHANNELS, sourceInfo.numChannels));

        CHECK("set audio block alignment",
            attributes->SetUINT32(MF_MT_AUDIO_BLOCK_ALIGNMENT, 1));

        uint32_t avgBps = CalculateAudioTargetBps(sourceInfo);
        CHECK("set audio avg Bps",
            attributes->SetUINT32(MF_MT_AUDIO_AVG_BYTES_PER_SECOND, avgBps));

        CHECK("set audio quality vs speed",
            attributes->SetUINT32(MF_TRANSCODE_QUALITYVSSPEED, 80));

        return attributes;
    }

    static ComPtr<IMFAttributes> CreateVideoProfileAttributes(
        const MediaInfo::VideoProfile& sourceInfo,
        Encoder encoder,
        double targetSizeFactor)
    {
        ComPtr<IMFAttributes> attributes;

        CHECK("create attributes for video profile",
            MFCreateAttributes(attributes.GetAddressOf(), 6));

        switch (encoder)
        {
        case Encoder::H264_AVC:
            CHECK("set video encoder", attributes->SetGUID(MF_MT_SUBTYPE, MFVideoFormat_H264));

            CHECK("set video encoder profile",
                attributes->SetUINT32(MF_MT_MPEG2_PROFILE, eAVEncH264VProfile_High));
            break;

        case Encoder::H265_HEVC:
            CHECK("set video encoder", attributes->SetGUID(MF_MT_SUBTYPE, MFVideoFormat_HEVC));

            CHECK("set video encoder profile",
                attributes->SetUINT32(MF_MT_VIDEO_PROFILE, eAVEncH265VProfile_Main_420_8));

            CHECK("set video interlace mode",
                attributes->SetUINT32(MF_MT_INTERLACE_MODE, MFVideoInterlace_Progressive));
            break;

        default:
            throw AppException("Encoder is not supported!");
        }

        CHECK("set video frame size",
            MFSetAttributeSize(attributes.Get(), MF_MT_FRAME_SIZE,
                sourceInfo.frameSize.width, sourceInfo.frameSize.height));

        CHECK("set video frame rate",
            MFSetAttributeRatio(attributes.Get(), MF_MT_FRAME_RATE,
                sourceInfo.frameRate.numerator, sourceInfo.frameRate.denominator));

        const auto videoAvgBitrate =
            static_cast<uint32_t> (sourceInfo.avgBitrate * targetSizeFactor);

        std::cout << std::endl
            << "Targeted video data rate is "
            << std::fixed << std::setprecision(1) << ((float)videoAvgBitrate / (8 * 1024))
            << " KB/s" << std::endl;

        CHECK("set video bitrate",
            attributes->SetUINT32(MF_MT_AVG_BITRATE, videoAvgBitrate));

        uint32_t qvs = EstimateBalanceQualityVsSpeed(sourceInfo, targetSizeFactor);
        std::cout << std::endl << "Encoder 'quality vs. speed' set to " << qvs << '%' << std::endl;
        CHECK("set video quality vs speed",
            attributes->SetUINT32(MF_TRANSCODE_QUALITYVSSPEED, qvs));

        return attributes;
    }

    TranscodeProfile::TranscodeProfile(
        const MediaInfo& sourceInfo,
        Encoder videoEncoder,
        double targetSizeFactor)
    {
        CHECK("create transcode profile",
            MFCreateTranscodeProfile(m_transcodeProfile.GetAddressOf()));

        ComPtr<IMFAttributes> audioAttrs =
            CreateAudioProfileAttributes(sourceInfo.audioProfile);

        CHECK("set audio profile", m_transcodeProfile->SetAudioAttributes(audioAttrs.Get()));

        ComPtr<IMFAttributes> videoAttrs =
            CreateVideoProfileAttributes(
                sourceInfo.videoProfile, videoEncoder, targetSizeFactor);

        CHECK("set video profile", m_transcodeProfile->SetVideoAttributes(videoAttrs.Get()));

        ComPtr<IMFAttributes> container;
        CHECK("create container attributes",
            MFCreateAttributes(container.GetAddressOf(), 1));

        CHECK("set container type",
            container->SetGUID(MF_TRANSCODE_CONTAINERTYPE, MFTranscodeContainerType_MPEG4));

        // allow hardware acceleration:
        CHECK("set topology mode",
            container->SetUINT32(
                MF_TRANSCODE_TOPOLOGYMODE, MF_TRANSCODE_TOPOLOGYMODE_HARDWARE_ALLOWED));

        CHECK("set container", m_transcodeProfile->SetContainerAttributes(container.Get()));
    }
}