#pragma once

#include "MediaInfo.hpp"

#include <chrono>
#include <string>
#include <wrl.h>

namespace application
{
	using namespace Microsoft::WRL;

	/// <summary>
	/// Wrapper for MF media source.
	/// </summary>
	class MediaSource
	{
	private:

		const uint32_t m_fileSize;

		ComPtr<IMFMediaSource> m_mfMediaSource;

		ComPtr<IMFPresentationDescriptor> GetPresentationDescriptor() const;

	public:

		/// <summary>
		/// Creates a new instance.
		/// </summary>
		/// <param name="mediaFilePath">The path of the media source file.</param>
		MediaSource(const std::wstring& mediaFilePath);

		~MediaSource();

		const ComPtr<IMFMediaSource>& GetMfObject() const
		{
			return m_mfMediaSource;
		}

		/// <summary>
		/// Get the media source duration.
		/// </summary>
		/// <returns>The duration of the media reproduction.</returns>
		std::chrono::nanoseconds GetDuration() const;

		/// <summary>
		/// Get information from this media source.
		/// </summary>
		/// <returns>Audio & video information.</returns>
		MediaInfo GetMediaInfo() const;
	};
}
