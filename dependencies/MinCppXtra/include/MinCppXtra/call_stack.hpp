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

#include <string>

namespace mincpp
{
	/// <summary>
	/// Provides call stack information.
	/// </summary>
	class CallStack
	{
	public:

		/// <summary>
		/// Creates a trace of the current stack.
		/// </summary>
		/// <param name="isConsole"> Whether the text should be visual appealing for the console.</param>
		/// <returns>The current call stack trace, UTF-8 encoded.</returns>
		static std::string GetTrace(bool isConsole = false);

		/// <summary>
		/// Creates the stack trace from the given context.
		/// </summary>
		/// <param name="currentContextHandle">The system handle for the current context.</param>
		/// <param name="isConsole"> Whether the text should be visual appealing for the console.</param>
		/// <returns>The current call stack trace, UTF-8 encoded.</returns>
		static std::string GetTrace(
			const void* currentContextHandle, bool isConsole = false);
	};
}
