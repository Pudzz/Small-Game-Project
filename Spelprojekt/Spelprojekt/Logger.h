#pragma once
#include <fcntl.h>
#include <io.h>
#include <iostream>
#include <fstream>
#include <Windows.h>
#include <string>

// https://stackoverflow.com/questions/32185512/output-to-console-from-a-win32-gui-application-on-windows-10
#define ENABLE_CONSOLE_OUTPUT 1
#define ENABLE_TEXT_OUTPUT 0
#define TEXT_FILENAME "test_console_output.txt"

enum class LOG_LEVEL
{
	Info = 0,
	Error = 1,
	Warning = 2,
	Debug = 3,
	WTF = 4
};

namespace Logger
{
	static bool _iocopen = false;
	static std::string LOG_LEVEL_NAMES[] = { "info", "error", "warning", "debug", "WTF" };
	static std::ofstream filestream;

	// writes a string to the console
	inline void Write(LOG_LEVEL logLevel, const std::string& msg)
	{
		std::string lineString = "[" + LOG_LEVEL_NAMES[static_cast<int>(logLevel)] + "] " + msg + "\n";

#if ENABLE_CONSOLE_OUTPUT
		std::cout << lineString;
#endif

#if ENABLE_TEXT_OUTPUT
		filestream.open(TEXT_FILENAME, std::ios::out | std::ios_base::app);

		filestream.exceptions(filestream.exceptions() | std::ios::failbit | std::ifstream::badbit);
		filestream << lineString;
		filestream.close();
#endif
	}

	inline void Write(const std::string& msg)
	{
		Write(LOG_LEVEL::Info, msg);
	}
	// adds a \n to the output console
	inline void Space()
	{
#if ENABLE_CONSOLE_OUTPUT
		std::cout << std::endl;
#endif

#if ENABLE_TEXT_OUTPUT
		filestream.open(TEXT_FILENAME, std::ios::out | std::ios_base::app);

		filestream.exceptions(filestream.exceptions() | std::ios::failbit | std::ifstream::badbit);
		filestream << std::endl;
		filestream.close();
#endif
	}

	// initializes and opens the console
	inline void Open()
	{
		if (!_iocopen)
		{
			std::string ss = "Console open\t\t";

#if ENABLE_TEXT_OUTPUT
			ss += "Writing to file: " + std::string(TEXT_FILENAME);
#endif			

#if ENABLE_CONSOLE_OUTPUT
			AllocConsole();
			HANDLE stdHandle = GetStdHandle(STD_OUTPUT_HANDLE);
			int hConsole = _open_osfhandle((intptr_t)stdHandle, _O_TEXT);
			FILE* fp = _fdopen(hConsole, "w");
			freopen_s(&fp, "CONOUT$", "w", stdout);
#endif

			Space();
			Write(LOG_LEVEL::Info, ss);
			_iocopen = true;
		}
	}

	// closes the console
	inline void Close()
	{
		Write(LOG_LEVEL::Info, "Closing console");
		_iocopen = false;
		filestream.close();
	}
}