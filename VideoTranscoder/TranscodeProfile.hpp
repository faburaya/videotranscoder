#pragma once

#include <wrl.h>

#include "Encoder.hpp"
#include "MediaInfo.hpp"

namespace application
{
	using namespace Microsoft::WRL;

	/// <summary>
	/// Wrapper for MF transcode profile.
	/// </summary>
	class TranscodeProfile
	{
	private:

		ComPtr<IMFTranscodeProfile> m_transcodeProfile;

	public:

		/// <summary>
		/// Create new instance.
		/// </summary>
		/// <param name="sourceInfo">Media source information.</param>
		/// <param name="videoEncoder">The video encoder to use.</param>
		/// <param name="targetSizeFactor">
		/// The target size of the video output, as a fraction of the source data rate.
		/// </param>
		TranscodeProfile(
			const MediaInfo& sourceInfo,
			Encoder videoEncoder,
			double targetSizeFactor);

		const ComPtr<IMFTranscodeProfile>& GetMfObject() const
		{
			return m_transcodeProfile;
		}
	};
}
