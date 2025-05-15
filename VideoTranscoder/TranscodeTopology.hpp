#pragma once

#include <mfobjects.h>
#include <wrl.h>

#include <string>

namespace application
{
	using namespace Microsoft::WRL;

	class TranscodeTopology
	{
	private:

		ComPtr<IMFTopology> m_mfTopology;

		bool m_hasHardwareAcceleration;

	public:

		TranscodeTopology(
			const ComPtr<IMFMediaSource>& mfMediaSource,
			const ComPtr<IMFTranscodeProfile>& mfTranscodeProfile,
			const std::string& outputFilePath);

		const ComPtr<IMFTopology>& GetMfObject() const
		{
			return m_mfTopology;
		}

		const bool IsHardwareAccelerated() const
		{
			return m_hasHardwareAcceleration;
		}
	};
}
