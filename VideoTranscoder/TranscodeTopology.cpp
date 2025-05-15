#include "stdafx.h"
#include "TranscodeTopology.hpp"
#include "AppException.hpp"

#include <MinCppXtra/win32_api_strings.hpp>

#include <Mferror.h>

namespace application
{
	static bool HasHardwareAcceleration(const ComPtr<IMFTopologyNode>& mfTopoNode)
	{
		const size_t bufferSize = 128;
		wchar_t buffer[bufferSize];
		HRESULT hr = mfTopoNode->GetString(
			MFT_ENUM_HARDWARE_URL_Attribute, buffer, bufferSize, nullptr);

		if (hr == S_OK)
			return true;
		else if (hr == MF_E_ATTRIBUTENOTFOUND)
			return false;
		else
			throw AppException(hr,
				"Failed to read attribute from topology node",
				NAMEOF(IMFTopologyNode::GetString));
	}

	TranscodeTopology::TranscodeTopology(
		const ComPtr<IMFMediaSource>& mfMediaSource,
		const ComPtr<IMFTranscodeProfile>& mfTranscodeProfile,
		const std::string& outputFilePath)
		: m_hasHardwareAcceleration(false)
	{
		std::wstring wOutFilePath = mincpp::Win32ApiStrings::ToUtf16(outputFilePath);
		CHECK("create transcode topology",
			MFCreateTranscodeTopology(
				mfMediaSource.Get(),
				wOutFilePath.c_str(),
				mfTranscodeProfile.Get(),
				m_mfTopology.GetAddressOf()));

		WORD nodeCount;
		CHECK("get topology nodes count", m_mfTopology->GetNodeCount(&nodeCount));
		for (WORD idxNode = 0; idxNode < nodeCount; ++idxNode)
		{
			ComPtr<IMFTopologyNode> mfTopoNode;
			CHECK("get topology node", m_mfTopology->GetNode(idxNode, mfTopoNode.GetAddressOf()));

			MF_TOPOLOGY_TYPE type;
			CHECK("get topology node type", mfTopoNode->GetNodeType(&type));
			if (type != MF_TOPOLOGY_TRANSFORM_NODE)
				continue;

			if (HasHardwareAcceleration(mfTopoNode))
			{
				m_hasHardwareAcceleration = true;
				break;;
			}
		}
	}
}
