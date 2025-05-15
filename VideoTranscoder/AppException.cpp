#include "stdafx.h"
#include "AppException.hpp"

#include <comdef.h>
#include <MinCppXtra/win32_api_strings.hpp>
#include <iostream>
#include <sstream>
#include <string>

namespace application
{
	static std::string CreateErrorMessage(HRESULT hr, const char* what, const char* where)
	{
		_com_error comError(hr);
		std::ostringstream oss;
		oss << "Failed to " << what << ": "
			<< mincpp::Win32ApiStrings::ToUtf8(comError.ErrorMessage())
			<< " (HRESULT 0x" << std::hex << hr << ") in " << where;

		return oss.str();
	}

	AppException::AppException(int32_t hr, const char* what, const char* where)
		: TraceableException(CreateErrorMessage(hr, what, where))
		, m_hresult(hr)
	{
	}

	AppException::AppException(const std::string& what)
		: TraceableException(what)
	{
	}

	void AppException::CheckResult(
		int32_t hr,
		const char* what,
		const char* where,
		bool throwOnError)
	{
		if (FAILED(hr))
		{
			AppException exception(hr, what, where);
			if (throwOnError)
				throw exception;

			std::cerr << exception.Serialize();
		}
	}
}
