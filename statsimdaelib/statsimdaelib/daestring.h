#pragma once

/*

	Ultra-lightweight string additions. (Should be) UNICODE compatible.

	dae::daestring str;

	// gee this is a contrived example !
	str.Format("   %  ",1);
	str.TrimRight();
	str.TrimLeft();

	// there is no implicit conversion operator
	Cdaestring cs = str.c_str();

*/
#include "stdafx.h"


#ifndef _ANSI
	#define _ANSI(x) x
#endif

#ifndef DAESTRING
	#define DAESTRING


#include <string>
#include <algorithm>
#include <vector>
#include <stdio.h>
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#ifdef _UNICODE
	#ifndef tstring
		#define tstring std::wstring
	#endif
#else
	#ifndef tstring
		#define tstring std::string
	#endif
#endif

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
using namespace std;

class daestring : public string
{
	// size of Format() buffer.
	enum { _MAX_CHARS = 8096 };

	public:

	daestring()	{}
	~daestring()	{}

	// various constructor flavours
	daestring(const daestring& arg)	{ assign(arg.c_str()); }
	daestring(const string& arg)	{ assign(arg); }
	daestring(const char* pArg)		{ assign(pArg); }

	// various assignment operator flavours
	daestring& operator=(const char* pArg)		{ assign(pArg); return (*this); }
	daestring& operator=(const string& arg)	{ assign(arg); return (*this); }
	daestring& operator=(const daestring& arg)	{ assign(arg.c_str()); return (*this); }

	// the essential MFC member functions
	int format(const char* szFormat,...)
	{
		std::vector<char> _buffer(_MAX_CHARS);
		va_list argList;
		va_start(argList,szFormat);
		int ret = _vsnprintf(&_buffer[0],_MAX_CHARS,szFormat,argList);

/*
#ifdef _UNICODE
		int ret = _vsnwprintf(&_buffer[0],_MAX_CHARS,szFormat,argList);
#else
		int ret = _vsnprintf(&_buffer[0],_MAX_CHARS,szFormat,argList);
#endif
*/
		va_end(argList);
		assign(&_buffer[0],ret);
		return ret;
	}

	// this returns a reference so you can do things like this:
	// daestring query = "abc";
	// query += daestring().Format("%d %s",1,"abc");
	daestring& formatex(const char* szFormat,...)
	{
		std::vector<char> _buffer(_MAX_CHARS);
		va_list argList;
		va_start(argList,szFormat);
		int ret = _vsnprintf(&_buffer[0],_MAX_CHARS,szFormat,argList);

/*
#ifdef _UNICODE
		int ret = _vsnwprintf(&_buffer[0],_MAX_CHARS,szFormat,argList);
#else
		int ret = _vsnprintf(&_buffer[0],_MAX_CHARS,szFormat,argList);
#endif
*/
		va_end(argList);
		assign(&_buffer[0],ret);
		return (*this);
	}
	// trim the right hand of the daestring of whitespace characters
	daestring& trimright(const char* pszDelimiters = (LPCSTR) _ANSI(" \t\r\n"), int off = 0)
	{
		//this->erase(this->find_last_not_of(_ANSI(" "))+1);
		//return *this;

		size_type idx = npos;
		if (!off)
			idx = this->find_last_not_of(pszDelimiters);
		else
			idx = this->find_last_not_of(pszDelimiters, off);

		if (idx != string::npos)
		{
			this->erase(idx+1);
		}
		else {
			this->erase();
		}

		//MessageBox(0,this->c_str(), L"trim", MB_OK);

		return *this;
	}
	
	//remove trailing characters
    // trim the right hand of the daestring of whitespace characters
    daestring& trimleft(const char* pszDelimiters = (LPCSTR) _ANSI(" \t\r\n"), int off = 0)
    {
		//unsafe but no other opt
		this->erase(0, this->find_first_not_of(_ANSI(" ")));
		return *this;

		size_type idx = npos;
		if (!off)
			idx = this->find_first_not_of(pszDelimiters);
		else
			idx = this->find_first_not_of(pszDelimiters, off);

        if (idx != string::npos)
        {
            this->erase(0,idx);
        }
        else
        {
			this->erase();
        }
        return *this;
    }

    // trim leading and trailing whitespace
    daestring& trim(const char* pszDelimiters = (LPCSTR) _ANSI(" \t\r\n"), int off = 0)
    {
		return this->trimleft(pszDelimiters, off).trimright(pszDelimiters, off);
    }

	// convert to upper and lower case
	daestring& makeupper()	{ std::transform(begin(),end(),begin(),toupper); return (*this); }
	daestring& makelower()	{ std::transform(begin(),end(),begin(),tolower); return (*this); }

	// get first nchar of the string
    daestring left(int nchar)
    {

		return substr(0, nchar);
    }
	
	// get last nchar of the string
	daestring right(int nchar)
    {
		int off = length() - nchar;

		if (off<0)
			return *this;

		return substr(off, nchar);

    }

	// another version of substr
    daestring mid(int off, int nchar)
    {

		return substr(off, nchar);
    }

	size_type replace(const string& from, const string& to)
	{
		string::size_type cnt(string::npos);
        
		if(from != to && !from.empty()) {
			string::size_type pos1(0);
            string::size_type pos2(0);
			const string::size_type from_len(from.size());
			const string::size_type to_len(to.size());
			cnt = 0;
			
			while((pos1 = string::find(from, pos2)) != string::npos) {
				string::replace(pos1, from_len, to);
				pos2 = pos1 + to_len;
				++cnt;
			}
		}
		
		return cnt;
	}


};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

#endif
