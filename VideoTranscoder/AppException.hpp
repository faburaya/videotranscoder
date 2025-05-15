#pragma once

#include <MinCppXtra/traceable_exception.hpp>
#include <optional>

#define CHECK(description, com_call) AppException::CheckResult((com_call), description, #com_call, true);
#define LOG(description, com_call)   AppException::CheckResult((com_call), description, #com_call, false);

namespace application
{
	class AppException : public mincpp::TraceableException
	{
	private:

		std::optional<int32_t> m_hresult;

	public:

		static void CheckResult(
			int32_t hr,
			const char* what,
			const char* where,
			bool throwOnError);

		AppException(int32_t hr, const char* what, const char* where);

		AppException(const std::string& what);

		std::optional<int32_t> GetHResult() const
		{
			return m_hresult;
		}
	};
}

