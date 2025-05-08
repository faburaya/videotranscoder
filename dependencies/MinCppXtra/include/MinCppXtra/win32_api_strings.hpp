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
	/// Handles common tasks for text encoding when dealing with Win32 API.
	/// </summary>
	class Win32ApiStrings
	{
	public:

		/// <summary>
		/// Performs initialization tasks for the application.
		/// </summary>
		struct StaticInitializer
		{
			StaticInitializer();
		};

		/// <summary>
		/// Transcodes UTF-16 text to UTF-8.
		/// </summary>
		/// <param name="utf16str">A wide-char string with UTF-16 encoded text and 0-terminated.</param>
		/// <returns>A UTF-8 encoded string.</returns>
		static std::string ToUtf8(const wchar_t* utf16str);

		/// <summary>
		/// Transcodes UTF-16 text to UTF-8.
		/// </summary>
		/// <param name="utf16str">A wide-char string with UTF-16 encoded text.</param>
		/// <param name="wideCharCount">
		/// The count of wide chars in the input text (final '\0' excluded if present, = array size - 1).
		/// </param>
		/// <returns>A UTF-8 encoded string.</returns>
		static std::string ToUtf8(const wchar_t* utf16str, size_t wideCharCount);

		/// <summary>
		/// Transcodes UTF-8 text to UTF-16.
		/// </summary>
		/// <param name="utf8str">A string with UTF-8 encoded text.</param>
		/// <returns>A UTF-16 encoded wide-string.</returns>
		static std::wstring ToUtf16(const std::string_view utf8str);

		/// <summary>
		/// Transcodes UTF-8 text to UTF-16.
		/// </summary>
		/// <param name="utf8str">A string with UTF-8 encoded text.</param>
		/// <returns>A UTF-16 encoded wide-string.</returns>
		static std::wstring ToUtf16(const std::u8string_view utf8str);

		/// <summary>
		/// Transcodes UTF-8 text to UTF-16.
		/// </summary>
		/// <param name="utf8str">A string with UTF-8 encoded text and 0-terminated.</param>
		/// <returns>A UTF-16 encoded wide-string.</returns>
		static std::wstring ToUtf16(const char* utf8str);

		/// <summary>
		/// Transcodes UTF-8 text to UTF-16.
		/// </summary>
		/// <param name="utf8str">A string with UTF-8 encoded text.</param>
		/// <param name="charCount">
		/// The count of chars in the input text (final '\0' excluded if present, = array size - 1).
		/// </param>
		/// <returns>A UTF-16 encoded wide-string.</returns>
		static std::wstring ToUtf16(const char* utf8str, size_t charCount);

	private:

		StaticInitializer s_initializer;
	};
}