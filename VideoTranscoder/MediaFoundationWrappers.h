#ifndef MEDIAFOUNDATIONWRAPPERS_H // header guard
#define MEDIAFOUNDATIONWRAPPERS_H

#include <chrono>
#include <cinttypes>
#include <map>
#include <string>
#include <vector>

#include <wrl.h>
#include <d3d11.h>
#include <mfreadwrite.h>

namespace application
{
    using std::string;

    using namespace Microsoft::WRL;

    /// <summary>
    /// Uses RAII to initialize and finalize Microsoft Media Foundation library.
    /// </summary>
    class MediaFoundationLib
    {
    public:

        MediaFoundationLib();
        ~MediaFoundationLib();
    };

    const char *TranslateMFTCategory(const GUID &mftCategory);

    ComPtr<ID3D11Device> GetDeviceDirect3D(const char *gpuDeviceNameKey);

    /// <summary>
    /// Packs the necessary stream metadata from source reader
    /// that needs to be known for setup of the sink writer.
    /// </summary>
    struct DecodedMediaType
    {
        GUID majorType;
        UINT32 originalEncodedDataRate;
        ComPtr<IMFMediaType> mediaType;
    };

    UINT32 EstimateBalanceQualityVsSpeed(DecodedMediaType decoded, double tgtQuality);

    /// <summary>
    /// Enumerates the possible states of a stream being read, that would
    /// require some action in the application loop for transcoding.
    /// </summary>
    enum class ReadStateFlags : DWORD
    {
        EndOfStream = MF_SOURCE_READERF_ENDOFSTREAM,
        NewStreamAvailable = MF_SOURCE_READERF_NEWSTREAM,
        GapFound = MF_SOURCE_READERF_STREAMTICK
    };

    /// <summary>
    /// Wraps Media Foundation Source Reader object.
    /// </summary>
    class MFSourceReader
    {
    private:

        DWORD m_streamCount;
        uint32_t m_fileSize;
        ComPtr<IMFSourceReader> m_mfSourceReader;
        ComPtr<IMFSourceReaderCallback> m_srcReadCallback;

        void ConfigureDecoderTransforms(bool mustReconfigAll);

    public:

        MFSourceReader(const string &url, const ComPtr<IMFDXGIDeviceManager> &mfDXGIDevMan);

		MFSourceReader(const MFSourceReader &) = delete;

        std::chrono::microseconds GetDuration() const;

        void GetOutputMediaTypesFrom(DWORD idxStream, std::map<DWORD, DecodedMediaType> &decodedMTypes) const;

        void ReadSampleAsync();

        ComPtr<IMFSample> GetSample(DWORD &idxStream, DWORD &state);
    };

    enum class Encoder { H264_AVC, H265_HEVC };
    
    /// <summary>
    /// Wraps Media Foundation Sink Writer object.
    /// THIS IS NOT A THREAD SAFE IMPLEMENTATION!
    /// </summary>
    class MFSinkWriter
    {
    private:

        ComPtr<IMFSinkWriter> m_mfSinkWriter;

        enum class MediaDataType : int16_t { Video, Audio, Other };

        struct StreamInfo
        {
            uint16_t outIndex; // output stream index
            MediaDataType mediaDType; // media data type
        };

        /// <summary>
        /// A lookup table for fast conversion of index in source reader
        /// to sink writer. In order to favor performance of contiguos
        /// memory, there might be some unused positions.
        /// </summary>
        std::vector<StreamInfo> m_streamInfoLookupTab;

        /// <summary>
        /// Tracks gap occurences, kept as their timestamps and accessed
        /// in this array by their 0-based stream index.
        /// </summary>
        std::vector<LONGLONG> m_streamsGapsTracking;

        void AddStream(const ComPtr<IMFSinkWriterEx> &sinkWriterAltIntf,
                       DWORD idxDecStream,
                       const DecodedMediaType &decoded,
                       double tgtQuality,
                       Encoder encoder);

    public:

        MFSinkWriter(const string &url,
                     const ComPtr<IMFDXGIDeviceManager> &mfDXGIDevMan,
                     const std::map<DWORD, DecodedMediaType> &decodedMTypes,
                     double tgtQuality,
                     Encoder encoder,
                     bool useHwAcceleration);

		MFSinkWriter(const MFSinkWriter &) = delete;

        ~MFSinkWriter();

        void AddNewStreams(const std::map<DWORD, DecodedMediaType> &decodedMTypes,
                           double tgtQuality,
                           Encoder encoder);

        void EncodeSample(DWORD idxDecStream, const ComPtr<IMFSample> &sample);

        void PlaceGap(DWORD idxDecStream, LONGLONG timestamp);
    };

}// end of namespace application

#endif // end of header guard
