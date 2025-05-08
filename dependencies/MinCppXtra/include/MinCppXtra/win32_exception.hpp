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

#include "traceable_exception.hpp"

namespace mincpp
{
	/// <summary>
	/// Represents a Win32 exception that has been translated from SEH to C++.
	/// </summary>
	class Win32Exception : public TraceableException
	{
	public:

		/// <summary>
		/// Creates an instance of Win32Exception.
		/// </summary>
		/// <param name="exceptionPointers">
		/// This is provided by the caught Win32 exception.
		/// (The erased type is PEXCEPTION_POINTERS).
		/// </param>
		Win32Exception(const void* exceptionPointers);
	};
}