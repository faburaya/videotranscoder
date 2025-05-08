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

#include <memory>

namespace mincpp
{
	/// <summary>
	/// Creates a scope for access of the call stack.
	/// </summary>
	class CallStackAccessScope
	{
	private:

		class Impl;
		std::unique_ptr<Impl> m_pimpl;

	public:

		/// <summary>
		/// Creates the scope.
		/// </summary>
		CallStackAccessScope();

		~CallStackAccessScope();
	};
}
