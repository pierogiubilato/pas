//------------------------------------------------------------------------------
// PAS package-wide console addons (actually from CAKE library)  			  --
// (C) Piero Giubilato 2010-2012, Padova University						      --
//------------------------------------------------------------------------------

//______________________________________________________________________________
// [File name]		"pasConsole.h"
// [Author]			"Piero Giubilato"
// [Version]		"1.2"
// [Modified by]	"Piero Giubilato"
// [Date]			"27 May 2011"
// [Language]		"C++"
// [Project]		"CAKE"
//______________________________________________________________________________


// Overloading check
#ifndef pasConsole_H
#define pasConsole_H


#include <iostream>
#include <sstream>
#include <iomanip>

// All subsequent statements are valid if _WIN32 || _WIN64 (MSVC default define)
// _____________________________________________________________________________
#if defined(_WIN32) || defined(_WIN64)
	
	// This library calls many others windows libs. In case ROOT is enabled the
	// root modified version must be loaded to avoid names conflicts.
	#include <windows.h>
		
	// Color constants definitions.
	enum COL {
		
		// Modifiers.
		L		= FOREGROUND_INTENSITY,
		FG		= 0,
		BG		= 16,
		US		= COMMON_LVB_UNDERSCORE,
		REV		= COMMON_LVB_REVERSE_VIDEO,
		DEFAULT	= 65535,
		
		// Standard colors.
		BLACK	= 0,
		BLUE	= FOREGROUND_BLUE,
		GREEN	= FOREGROUND_GREEN,
		CYAN	= FOREGROUND_GREEN | FOREGROUND_BLUE,
		RED		= FOREGROUND_RED,
		PURPLE	= FOREGROUND_RED | FOREGROUND_BLUE,
		YELLOW	= FOREGROUND_RED | FOREGROUND_GREEN,
		WHITE	= FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE,
		LBLACK	= L | BLACK,
		LBLUE	= L | BLUE,
		LGREEN	= L | GREEN,
		LCYAN	= L | CYAN,
		LRED	= L | RED,
		LPURPLE	= L | PURPLE,
		LYELLOW	= L | YELLOW,
		LWHITE	= L | WHITE
	};
		
	// Console handles.
	struct kConsole {bool cStarted; HANDLE cHandle; CONSOLE_SCREEN_BUFFER_INFO cSBI;};
	static kConsole _Console;

	// Overloads the << operator in case the color keyword is found,
	// setting the new color for the console standard output
	template <class _Elem, class _Traits>
	inline std::basic_ostream<_Elem,_Traits>& operator <<(std::basic_ostream<_Elem,_Traits>& out_Stream, COL c)
	{
		// If it is the first call, store current console properties.
		if (_Console.cStarted == false) {
			_Console.cStarted = true;
			_Console.cHandle = GetStdHandle(STD_OUTPUT_HANDLE);
			GetConsoleScreenBufferInfo(_Console.cHandle, &_Console.cSBI);
		}

		// Call for default settings.
		if (c == 65535) {
			SetConsoleTextAttribute(_Console.cHandle, _Console.cSBI.wAttributes);
		
		// Otherwise, set the new color.
		} else {
			SetConsoleTextAttribute(_Console.cHandle, c);
		}
   
		// Return the rightmost stream object to the next operator(s)
		return out_Stream;
	};
	

// All subsequent statements are valid in LINUX (Mac to be tested)
// _____________________________________________________________________________
#else

	// Color constants definitions (just for compatibility, not actually used.)
	enum COL {
		FG = 0, BG = 16, L = 0, DEFAULT = 255,BLACK	= 0,BLUE = 1, GREEN = 2,
		CYAN = 3, RED = 4, PURPLE = 5, YELLOW = 6, WHITE = 7, LBLACK = 8,
		LBLUE = 9,LGREEN = 10, LCYAN = 11, LRED = 12, LPURPLE = 13, 
		LYELLOW = 14, LWHITE = 15
	};
	
	// Overload << operator for col structure
	inline std::ostream& operator<<(std::ostream& o, const COL& c)
	{
		/*! Uses console escape commands to set a color. This is a quick
			and dirty way which works just for Linux consoles. */
	  	switch (c) {
			case DEFAULT: std::cout	<< "\033[0m"; break;
			case BLACK: std::cout	<< "\033[22;30m"; break;
			case BLUE:	std::cout	<< "\033[22;34m"; break;
			case GREEN: std::cout	<< "\033[22;32m"; break;
			case CYAN:	std::cout	<< "\033[22;36m"; break;
			case RED:	std::cout	<< "\033[22;31m"; break;
			case PURPLE: std::cout	<< "\033[22;35m"; break;
			case YELLOW: std::cout	<< "\033[22;33m"; break;
			case WHITE:	std::cout	<< "\033[22;37m"; break;
			case LBLACK: std::cout	<< "\033[01;30m"; break;
			case LBLUE: std::cout	<< "\033[01;34m"; break;
			case LGREEN: std::cout	<< "\033[01;32m"; break;
			case LCYAN: std::cout	<< "\033[01;36m"; break;
			case LRED:	std::cout	<< "\033[01;31m"; break;
			case LPURPLE: std::cout	<< "\033[01;35m"; break;
			case LYELLOW: std::cout	<< "\033[01;33m"; break;
			case LWHITE: std::cout	<< "\033[01;37m"; break;
			default: std::cout << "";
		}

		// Return
		return o;
	}

// End of _WIN32 || _WIN64 defined IF
#endif

// Overloading check
#endif
