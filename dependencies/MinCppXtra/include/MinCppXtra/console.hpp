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

namespace mincpp
{
    /// <summary>
    /// Helper for printing in the console.
    /// </summary>
    class Console
    {
    public:

        /// <summary>
        /// Gathers ANSI color codes for the background.
        /// </summary>
        struct Background
        {
        private:

            const bool m_enabled;

        public:

            const char* Black() const { return m_enabled ? "\033[40m" : ""; }
            const char* Red() const { return m_enabled ? "\033[41m" : ""; }
            const char* Green() const { return m_enabled ? "\033[42m" : ""; }
            const char* Yellow() const { return m_enabled ? "\033[43m" : ""; }
            const char* Blue() const { return m_enabled ? "\033[44m" : ""; }
            const char* Magenta() const { return m_enabled ? "\033[45m" : ""; }
            const char* Cyan() const { return m_enabled ? "\033[46m" : ""; }
            const char* White() const { return m_enabled ? "\033[47m" : ""; }

            const char* BrightBlack() const { return m_enabled ? "\033[100m" : ""; }
            const char* BrightRed() const { return m_enabled ? "\033[101m" : ""; }
            const char* BrightGreen() const { return m_enabled ? "\033[102m" : ""; }
            const char* BrightYellow() const { return m_enabled ? "\033[103m" : ""; }
            const char* BrightBlue() const { return m_enabled ? "\033[104m" : ""; }
            const char* BrightMagenta() const { return m_enabled ? "\033[105m" : ""; }
            const char* BrightCyan() const { return m_enabled ? "\033[106m" : ""; }
            const char* BrightWhite() const { return m_enabled ? "\033[107m" : ""; }

            /// <summary>
            /// Creates an instance of this class.
            /// </summary>
            /// <param name="enabled">Set to false if you wish to disable color output.</param>
            Background(bool enabled = true)
                : m_enabled(enabled) {}
        };

        /// <summary>
        /// Gathers ANSI color codes for the text.
        /// </summary>
        class Color
        {
        private:

            const bool m_enabled;

        public:

            const char* Black() const { return m_enabled ? "\033[30m" : ""; }
            const char* Red() const { return m_enabled ? "\033[31m" : ""; }
            const char* Green() const { return m_enabled ? "\033[32m" : ""; }
            const char* Yellow() const { return m_enabled ? "\033[33m" : ""; }
            const char* Blue() const { return m_enabled ? "\033[34m" : ""; }
            const char* Magenta() const { return m_enabled ? "\033[35m" : ""; }
            const char* Cyan() const { return m_enabled ? "\033[36m" : ""; }
            const char* White() const { return m_enabled ? "\033[37m" : ""; }
            const char* Reset() const { return m_enabled ? "\033[0m" : ""; }

            const char* BrightBlack() const { return m_enabled ? "\033[90m" : ""; }
            const char* BrightRed() const { return m_enabled ? "\033[91m" : ""; }
            const char* BrightGreen() const { return m_enabled ? "\033[92m" : ""; }
            const char* BrightYellow() const { return m_enabled ? "\033[93m" : ""; }
            const char* BrightBlue() const { return m_enabled ? "\033[94m" : ""; }
            const char* BrightMagenta() const { return m_enabled ? "\033[95m" : ""; }
            const char* BrightCyan() const { return m_enabled ? "\033[96m" : ""; }
            const char* BrightWhite() const { return m_enabled ? "\033[97m" : ""; }

            /// <summary>
            /// Creates an instance of this class.
            /// </summary>
            /// <param name="enabled">Set to false if you wish to disable color output.</param>
            Color(bool enabled)
                : m_enabled(enabled) {}
        };
    };
}
