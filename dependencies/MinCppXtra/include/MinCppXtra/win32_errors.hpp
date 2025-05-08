/*
 * MinCppXtra - A minimalistic C++ utility library
 *
 * Author: Felipe Vieira Aburaya, 2025
 * License: The Unlicense (public domain)
 * Repository: https://github.com/faburaya/MinCppXtra
 *
 * This software is released into the public domain.
 * You can freely use, modify, and distribute it without restrictions.
 *
 * For more details, see: https://unlicense.org
 */

#pragma once

#include <cinttypes>
#include <ostream>

namespace mincpp
{
	class Win32Errors
	{
    public:

        static std::ostream& AppendErrorMessage(
            uint32_t errCode,
            const char* funcName,
            std::ostream& oss);

        static std::string GetErrorMessage(uint32_t errCode, const char* funcName);
	};
}
