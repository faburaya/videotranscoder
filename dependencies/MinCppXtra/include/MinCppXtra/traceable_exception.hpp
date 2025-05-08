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
#include <stdexcept>
#include <string>
#include <optional>

namespace mincpp
{
	/// <summary>
	/// Represents an exception with call stack trace.
	/// </summary>
	class TraceableException : public std::runtime_error
	{
	private:

		static bool s_useColorsOnStackTrace;

		class Impl;
		std::shared_ptr<Impl> m_pimpl;

		virtual std::string_view GetTypeName() const final;

	public:

		/// <summary>
		/// Enable/disable use of (ANSI encoded) colors in the stack trace.
		/// </summary>
		/// <param name="enable">Whether the feature should be enabled.</param>
		static void UseColorsOnStackTrace(bool enable);

		/// <summary>
		/// Creates a new instance.
		/// </summary>
		/// <param name="message">The exception message.</param>
		/// <param name="innerException">The inner/preceding exception.</param>
		TraceableException(
			const std::string& message,
			std::optional<std::exception>&& innerException = std::nullopt);

		/// <summary>
		/// Creates a new instance.
		/// </summary>
		/// <param name="message">The exception message.</param>
		/// <param name="exceptionContextHandle">
		/// The system handle for the stack context when the exception was thrown.
		/// </param>
		/// <param name="isStackTraceEnabled">
		/// Whether the implementation is allowed to walk the call stack to produce a trace.
		/// </param>
		TraceableException(
			const std::string& message,
			const void* exceptionContextHandle,
			bool isStackTraceEnabled = true);

		virtual ~TraceableException();

		/// <summary>
		/// Gets the trace of the all stack when the exception was thrown.
		/// (It requires the loading of debug symbols.)
		/// </summary>
		/// <returns>The call stack trace encoded in UTF-8.</returns>
		const std::string& GetCallStackTrace() const;

		/// <summary>
		/// Gets the inner/preceding exception.
		/// </summary>
		/// <returns>The inner exception, if available.</returns>
		const std::optional<std::exception>& GetInnerException() const;

		/// <summary>
		/// Serializes this exception into a text representation.
		/// </summary>
		/// <returns>This exception as UTF-8 encoded text.</returns>
		std::string Serialize() const;
	};
}
