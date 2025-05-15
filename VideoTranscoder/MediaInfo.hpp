#pragma once

#include <cinttypes>

namespace application
{
	struct MediaInfo
	{
        struct AudioProfile
        {
            uint32_t bitsPerSample;
            uint32_t samplesPerSec;
            uint32_t numChannels;
            uint32_t avgBytesPerSec;
        }
        audioProfile;

        struct VideoProfile
        {
            struct
            {
                uint32_t width;
                uint32_t height;
            }
            frameSize;

            struct
            {
                uint32_t numerator;
                uint32_t denominator;
            }
            frameRate;

            uint32_t avgBitrate;
        }
        videoProfile;
	};
}
