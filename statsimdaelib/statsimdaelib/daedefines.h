
#pragma once


//define import / export
#ifndef DLLEXPORT
	#define DLLEXPORT __declspec(dllexport)
#endif
#ifndef DLLIMPORT
	#define DLLIMPORT __declspec(dllimport)
#endif

//define functions
#ifdef _UNICODE
	#ifndef tctime
		#define tctime _wctime
	#endif
	#ifndef tstoi
		#define tstoi _wtoi
	#endif
	#ifndef tstof
		#define tstof _wtof
	#endif

	#ifndef tsprintf
		#define tsprintf swprintf
	#endif

	#ifndef tstrcmp
		#define tstrcmp wcscmp
	#endif

	#ifndef tfopen
		#define tfopen _wfopen
	#endif

	#ifndef tfgets
		#define tfgets fgetws
	#endif

	#ifndef tscpy
		#define tscpy wcscpy_s
	#endif

	#ifndef tslen
		#define tslen wcslen
	#endif

//strings
	#ifndef tstring
		#define tstring std::wstring
	#endif

	#ifndef tifstream
		#define tifstream std::wifstream
	#endif

	#define _TXT(x) L ## x

typedef wchar_t TCHAR;
typedef const wchar_t* LPCTSTR;


#else

	#ifndef tctime
		#define tctime ctime
	#endif
	#ifndef tstoi
		#define tstoi atoi
	#endif
	#ifndef tstof
		#define tstof atof
	#endif
	#ifndef tsprintf
		#define tsprintf sprintf
	#endif
	#ifndef tstrcmp
		#define tstrcmp strcmp
	#endif
	#ifndef tfopen
		#define tfopen fopen
	#endif

	#ifndef tfgets
		#define tfgets fgets
	#endif

	#ifndef tscpy
		#define tscpy strcpy_s
	#endif

	#ifndef tslen
		#define tslen strlen
	#endif

//strings
	#ifndef tstring
		#define tstring std::string
	#endif

	#ifndef tifstream
		#define tifstream std::ifstream

	#endif

	#define _TXT(x) x

    typedef char TCHAR;
    typedef const char* LPCTSTR;
#endif
