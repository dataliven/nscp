/**************************************************************************
*   Copyright (C) 2004-2007 by Michael Medin <michael@medin.name>         *
*                                                                         *
*   This code is part of NSClient++ - http://trac.nakednuns.org/nscp      *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
*                                                                         *
*   This program is distributed in the hope that it will be useful,       *
*   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
*   GNU General Public License for more details.                          *
*                                                                         *
*   You should have received a copy of the GNU General Public License     *
*   along with this program; if not, write to the                         *
*   Free Software Foundation, Inc.,                                       *
*   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
***************************************************************************/
#pragma once

#include <tchar.h>
#include <windows.h>
#include <sstream>
#include <iomanip>
#include <string>
#include <utility>
#include <list>
#include <functional>
#include <time.h>
#ifdef _DEBUG
#include <iostream>
#endif

namespace strEx {

	inline void append_list(std::wstring &lst, std::wstring &append) {
		if (!lst.empty())
			lst += _T(", ");
		lst += append;
	}

	inline std::string wstring_to_string( const wchar_t* pStr, int len) {
		//ASSERT_PTR( pStr ) ; 
		//ASSERT( len >= 0 || len == -1 , _T("Invalid string length: ") << len ) ; 

		// figure out how many narrow characters we are going to get 
		int nChars = WideCharToMultiByte( CP_ACP , 0 , pStr , len , NULL , 0 , NULL , NULL ) ; 
		if ( len == -1 )
			-- nChars ; 
		if ( nChars == 0 )
			return "" ;

		// convert the wide string to a narrow string
		// nb: slightly naughty to write directly into the string like this
		std::string buf ;
		buf.resize( nChars ) ;
		WideCharToMultiByte( CP_ACP , 0 , pStr , len , 
			const_cast<char*>(buf.c_str()) , nChars , NULL , NULL ) ; 

		return buf ; 
	}
	inline std::string wstring_to_string( const std::wstring& str ) {
		return wstring_to_string(str.c_str(), static_cast<int>(str.length()));
	}

	inline std::wstring string_to_wstring( const char* pStr , int len ) {
		//ASSERT_PTR( pStr ) ; 
		//ASSERT( len >= 0 || len == -1 , _T("Invalid string length: ") << len ) ; 

		// figure out how many wide characters we are going to get 
		int nChars = MultiByteToWideChar( CP_ACP , 0 , pStr , len , NULL , 0 ) ; 
		if ( len == -1 )
			-- nChars ; 
		if ( nChars == 0 )
			return L"" ;

		// convert the narrow string to a wide string 
		// nb: slightly naughty to write directly into the string like this
		std::wstring buf ;
		buf.resize( nChars ) ; 
		MultiByteToWideChar( CP_ACP , 0 , pStr , len , const_cast<wchar_t*>(buf.c_str()) , nChars ) ; 

		return buf ;
	}
	inline std::wstring string_to_wstring( const std::string& str ) {
		return string_to_wstring(str.c_str(), static_cast<int>(str.length())) ;
	}

	inline std::wstring format_buffer(const wchar_t* buf, unsigned int len) {
		std::wstringstream ss;
		std::wstring chars;
		for (unsigned int i=0;i<len;i++) {
			ss << std::hex << buf[i];
			ss << _T(", ");
			if (buf[i] >= ' ' && buf[i] <= 'z')
				chars += buf[i];
			else
				chars += '?';
			if (i%32==0) {
				ss << chars;
				ss << _T("\n");
				chars = _T("");
			}
		}
		return ss.str();
	}
	inline std::string format_buffer(const char* buf, unsigned int len) {
		std::stringstream ss;
		std::string chars;
		for (unsigned int i=0;i<len;i++) {
			if (i%32==0) {
				if (i > 0) {
					ss << chars;
					ss << "\n";
				}
				chars = "";
				ss << std::hex << std::setw(8) << std::setfill('0') << i;
				ss << ": ";
			}
			ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(static_cast<unsigned char>(buf[i]));
			ss << ", ";
			if (buf[i] < 30 || buf[i] == 127)
				chars += '?';
			else
				chars += buf[i];
		}
		return ss.str();
	}

	inline std::wstring format_date(time_t time, std::wstring format = _T("%Y-%m-%d %H:%M:%S")) {
		TCHAR buf[51];
		struct tm nt; // = new struct tm;
#if (_MSC_VER > 1300)  // 1300 == VC++ 7.0
		if (gmtime_s(&nt, &time) != 0)
			return _T("");
#else
		nt = gmtime(&time);
		if (nt == NULL)
			return "";
#endif
		size_t l = wcsftime(buf, 50, format.c_str(), &nt);
		if (l <= 0 || l >= 50)
			return _T("");
		buf[l] = 0;
		return buf;
	}
	inline std::wstring format_date(SYSTEMTIME &time, std::wstring format = _T("%Y-%m-%d %H:%M:%S")) {
		TCHAR buf[51];

		struct tm tmTime;
		memset(&tmTime, 0, sizeof(tmTime));

		tmTime.tm_sec = time.wSecond; // seconds after the minute - [0,59]
		tmTime.tm_min = time.wMinute; // minutes after the hour - [0,59]
		tmTime.tm_hour = time.wHour;  // hours since midnight - [0,23]
		tmTime.tm_mday = time.wDay;  // day of the month - [1,31]
		tmTime.tm_mon = time.wMonth-1; // months since January - [0,11]
		tmTime.tm_year = time.wYear-1900; // years since 1900
		tmTime.tm_wday = time.wDayOfWeek; // days since Sunday - [0,6]

		size_t l = wcsftime(buf, 50, format.c_str(), &tmTime);
		if (l <= 0 || l >= 50)
			return _T("");
		buf[l] = 0;
		return buf;
	}

	static const __int64 SECS_BETWEEN_EPOCHS = 11644473600;
	static const __int64 SECS_TO_100NS = 10000000;
	inline std::wstring format_filetime(unsigned long long filetime, std::wstring format) {
		filetime -= (SECS_BETWEEN_EPOCHS * SECS_TO_100NS);
		filetime /= SECS_TO_100NS;
		return format_date(static_cast<time_t>(filetime), format);
	}

	inline void replace(std::wstring &string, std::wstring replace, std::wstring with) {
		std::wstring::size_type pos = string.find(replace);
		std::wstring::size_type len = replace.length();
		while (pos != std::wstring::npos) {
			string = string.substr(0,pos)+with+string.substr(pos+len);
			pos = string.find(replace, pos+1);
		}
	}
	inline std::wstring itos(unsigned int i) {
		std::wstringstream ss;
		ss << i;
		return ss.str();
	}
	inline std::wstring itos(int i) {
		std::wstringstream ss;
		ss << i;
		return ss.str();
	}
	inline std::wstring itos(unsigned long long i) {
		std::wstringstream ss;
		ss << i;
		return ss.str();
	}
	inline std::wstring itos(__int64 i) {
		std::wstringstream ss;
		ss << i;
		return ss.str();
	}
	inline std::wstring itos(unsigned long i) {
		std::wstringstream ss;
		ss << i;
		return ss.str();
	}
	inline std::wstring itos(double i) {
		std::wstringstream ss;
		ss << i;
		return ss.str();
	}
	inline std::wstring itos(float i) {
		std::wstringstream ss;
		ss << i;
		return ss.str();
	}
	inline std::wstring ihextos(unsigned int i) {
		std::wstringstream ss;
		ss << std::hex << i;
		return ss.str();
	}
	inline int stoi(std::wstring s) {
		return _wtoi(s.c_str());
	}
	inline double stod(std::wstring s) {
		return _wtof(s.c_str());
	}
	inline long long stoi64(std::wstring s) {
		return _wtoi64(s.c_str());
	}
	inline unsigned stoui_as_time(std::wstring time, unsigned int smallest_unit = 1000) {
		std::wstring::size_type p = time.find_first_of(_T("sSmMhHdDwW"));
		unsigned int value = _wtoi(time.c_str());
		if (p == std::wstring::npos)
			return value * smallest_unit;
		else if ( (time[p] == 's') || (time[p] == 'S') )
			return value * 1000;
		else if ( (time[p] == 'm') || (time[p] == 'M') )
			return value * 60 * 1000;
		else if ( (time[p] == 'h') || (time[p] == 'H') )
			return value * 60 * 60 * 1000;
		else if ( (time[p] == 'd') || (time[p] == 'D') )
			return value * 24 * 60 * 60 * 1000;
		else if ( (time[p] == 'w') || (time[p] == 'W') )
			return value * 7 * 24 * 60 * 60 * 1000;
		return value * smallest_unit;
	}

	inline unsigned long long stoi64_as_time(std::wstring time, unsigned int smallest_unit = 1000) {
		std::wstring::size_type p = time.find_first_of(_T("sSmMhHdDwW"));
		unsigned long long value = _wtoi64(time.c_str());
		if (p == std::wstring::npos)
			return value * smallest_unit;
		else if ( (time[p] == 's') || (time[p] == 'S') )
			return value * 1000;
		else if ( (time[p] == 'm') || (time[p] == 'M') )
			return value * 60 * 1000;
		else if ( (time[p] == 'h') || (time[p] == 'H') )
			return value * 60 * 60 * 1000;
		else if ( (time[p] == 'd') || (time[p] == 'D') )
			return value * 24 * 60 * 60 * 1000;
		else if ( (time[p] == 'w') || (time[p] == 'W') )
			return value * 7 * 24 * 60 * 60 * 1000;
		return value * smallest_unit;
	}

#define MK_FORMAT_FTD(min, key, val) \
	if (mtm->tm_year > min) \
		strEx::replace(format, key, strEx::itos(val));  \
	else  \
		strEx::replace(format, key, _T("0"));

	inline std::wstring format_time_delta(std::wstring format, struct tm *mtm) {
		// "Date: %Y-%m-%d %H:%M:%S"
		MK_FORMAT_FTD(70, _T("%Y"), mtm->tm_year);
		MK_FORMAT_FTD(0, _T("%m"), mtm->tm_mon);
		MK_FORMAT_FTD(0, _T("%d"), mtm->tm_mday-1);
		MK_FORMAT_FTD(0, _T("%H"), mtm->tm_hour);
		MK_FORMAT_FTD(0, _T("%M"), mtm->tm_min);
		MK_FORMAT_FTD(0, _T("%S"), mtm->tm_sec);
		return format;
	}

#define WEEK	(7 * 24 * 60 * 60 * 1000)
#define DAY		(24 * 60 * 60 * 1000)
#define HOUR	(60 * 60 * 1000)
#define MIN		(60 * 1000)
#define SEC		(1000)
	inline std::wstring itos_as_time(unsigned long long time) {
		if (time > WEEK) {
			unsigned int w = static_cast<unsigned int>(time/WEEK);
			unsigned int d = static_cast<unsigned int>((time-(w*WEEK))/DAY);
			unsigned int h = static_cast<unsigned int>((time-(w*WEEK)-(d*DAY))/HOUR);
			unsigned int m = static_cast<unsigned int>((time-(w*WEEK)-(d*DAY)-(h*HOUR))/MIN);
			return itos(w) + _T("w ") + itos(d) + _T("d ") + itos(h) + _T(":") + itos(m);
		}
		else if (time > DAY) {
			unsigned int d = static_cast<unsigned int>((time)/DAY);
			unsigned int h = static_cast<unsigned int>((time-(d*DAY))/HOUR);
			unsigned int m = static_cast<unsigned int>((time-(d*DAY)-(h*HOUR))/MIN);
			return itos(d) + _T("d ") + itos(h) + _T(":") + itos(m);
		}
		else if (time > HOUR) {
			unsigned int h = static_cast<unsigned int>((time)/HOUR);
			unsigned int m = static_cast<unsigned int>((time-(h*HOUR))/MIN);
			return itos(h) + _T(":") + itos(m);
		} else if (time > MIN) {
			return _T("0:") + itos(static_cast<unsigned int>(time/(60 * 1000)));
		} else if (time > SEC)
			return itos(static_cast<unsigned int>(time/(1000))) + _T("s");
		return itos(static_cast<unsigned int>(time));
	}

	inline long long stoi64_as_BKMG(std::wstring s) {
		std::wstring::size_type p = s.find_first_of(_T("BMKGT"));
		if (p == std::wstring::npos)
			return _wtoi64(s.c_str());
		else if (s[p] == 'B') 
			return _wtoi64(s.c_str());
		else if (s[p] == 'K') 
			return _wtoi64(s.c_str())*1024;
		else if (s[p] == 'M') 
			return _wtoi64(s.c_str())*1024*1024;
		else if (s[p] == 'G') 
			return _wtoi64(s.c_str())*1024*1024*1024;
		else if (s[p] == 'T') 
			return _wtoi64(s.c_str())*1024*1024*1024*1024;
		else
			return _wtoi64(s.c_str());
	}
#define BKMG_RANGE "BKMGTP"
#define BKMG_SIZE 5

	inline std::wstring itos_as_BKMG(unsigned __int64 i) {
		double cpy = static_cast<double>(i);
		TCHAR postfix[] = _T(BKMG_RANGE);
		int idx = 0;
		while ((cpy > 999)&&(idx<BKMG_SIZE)) {
			cpy/=1024;
			idx++;
		}
		std::wstringstream ss;
		ss << std::setprecision(3);
		ss << cpy;
		std::wstring ret = ss.str(); // itos(cpy);
		ret += postfix[idx];
		return ret;
	}
	inline std::wstring format_BKMG(unsigned __int64 i, std::wstring unit) {
		double cpy = static_cast<double>(i);
		TCHAR postfix[] = _T(BKMG_RANGE);
		if (unit.length() != 1)
			return itos(cpy);
		for (int i=0;i<BKMG_SIZE;i++) {
			if (unit[0] == postfix[i]) {
				std::wstringstream ss;
				ss << std::setiosflags(std::ios::fixed) << std::setprecision(3) << cpy;
				std::wstring s = ss.str();
				std::wstring::size_type pos = s.find_last_not_of(_T("0"));
				if (pos != std::string::npos) {
					s = s.substr(0,pos);
				}
				return s;
			}
			cpy/=1024;
		}
		return itos(cpy);
	}
	inline std::wstring find_proper_unit_BKMG(unsigned __int64 i) {
		double cpy = static_cast<double>(i);
		TCHAR postfix[] = _T(BKMG_RANGE);
		int idx = 0;
		while ((cpy > 999)&&(idx<BKMG_SIZE)) {
			cpy/=1024;
			idx++;
		}
		return std::wstring(1, postfix[idx]);
	}

	typedef std::list<std::wstring> splitList;
	inline splitList splitEx(std::wstring str, std::wstring key) {
		splitList ret;
		std::wstring::size_type pos = 0, lpos = 0;
		while ((pos = str.find(key, pos)) !=  std::wstring::npos) {
			ret.push_back(str.substr(lpos, pos-lpos));
			lpos = ++pos;
		}
		if (lpos < str.size())
			ret.push_back(str.substr(lpos));
		return ret;
	}
	inline std::wstring joinEx(splitList lst, std::wstring key) {
		std::wstring ret;
		for (splitList::const_iterator it = lst.begin(); it != lst.end(); ++it) {
			if (!ret.empty())
				ret += key;
			ret += *it;
		}
		return ret;
	}

	inline std::pair<std::wstring,std::wstring> split(std::wstring str, std::wstring key) {
		std::wstring::size_type pos = str.find(key);
		if (pos == std::wstring::npos)
			return std::pair<std::wstring,std::wstring>(str, _T(""));
		return std::pair<std::wstring,std::wstring>(str.substr(0, pos), str.substr(pos+key.length()));
	}
	typedef std::pair<std::wstring,std::wstring> token;
	// foo bar "foo \" bar" foo -> foo, bar "foo \" bar" foo -> bar, "foo \" bar" foo -> 
	// 
	inline token getToken(std::wstring buffer, char split, bool escape = false) {
		std::wstring::size_type pos = std::wstring::npos;
		if ((escape) && (buffer[0] == '\"')) {
			do {
				pos = buffer.find('\"');
			}
			while (((pos != std::wstring::npos)&&(pos > 1))&&(buffer[pos-1] == '\\'));
		} else
			pos = buffer.find(split);
		if (pos == std::wstring::npos)
			return token(buffer, _T(""));
		if (pos == buffer.length()-1)
			return token(buffer.substr(0, pos), _T(""));
		return token(buffer.substr(0, pos-1), buffer.substr(++pos));
	}


	template<class _E>
	struct blind_traits : public std::char_traits<_E>
	{
		static bool eq(const _E& x, const _E& y) {
			return tolower( x ) == tolower( y ); 
		}
		static bool lt(const _E& x, const _E& y) {
			return tolower( x ) < tolower( y ); 
		}

		static int compare(const _E *x, const _E *y, size_t n) { 
			return _wcsnicmp( x, y, n );
		}

		//  There's no memichr(), so we roll our own.  It ain't rocket science.
		static const _E * __cdecl find(const _E *buf, size_t len, const _E& ch) {
			//  Jerry says that x86s have special mojo for memchr(), so the 
			//  memchr() calls end up being reasonably efficient in practice.
			const _E *pu = (const _E *)memchr(buf, ch, len);
			const _E *pl = (const _E *)memchr(buf, tolower( ch ), len);
			if ( ! pu )
				return pl;  //  Might be NULL; if so, NULL's the word.
			else if ( ! pl )
				return pu;
			else
				//  If either one was NULL, we return the other; if neither is 
				//  NULL, we return the lesser of the two.
				return ( pu < pl ) ? pu : pl;
		}

		//  I'm reasonably sure that this is eq() for wide characters.  Maybe.
		static bool eq_int_type(const int_type& ch1, const int_type& ch2) { 
			return char_traits<_E>::eq_int_type( tolower( ch1 ), tolower( ch2 ) ); 
		}
	};

	//  And here's our case-blind string class.
	//typedef std::basic_string<char, blind_traits<char>, std::allocator<char> >  blindstr;
	typedef std::basic_string<wchar_t, blind_traits<wchar_t>, std::allocator<wchar_t> >  blindstr;

	struct case_blind_string_compare : public std::binary_function<std::wstring, std::wstring, bool>
	{
		bool operator() (const std::wstring& x, const std::wstring& y) const {
			return _wcsicmp( x.c_str(), y.c_str() ) < 0;
		}
	};

#ifdef _DEBUG
	inline void test_getToken(std::wstring in1, char in2, std::wstring out1, std::wstring out2) {
		token t = getToken(in1, in2);
		std::wcout << _T("strEx::test_getToken(") << in1 << _T(", ") << in2 << _T(") : ");
		if ((t.first == out1) && (t.second == out2))
			std::wcout << _T("Succeeded") << std::endl;
		else
			std::wcout << _T("Failed [") << out1 << _T("=") << t.first << _T(", ") << out2 << _T("=") << t.second << _T("]") << std::endl;
	}
	inline void run_test_getToken() {
		test_getToken(_T(""), '&', _T(""), _T(""));
		test_getToken(_T("&"), '&', _T(""), _T(""));
		test_getToken(_T("&&"), '&', _T(""), _T("&"));
		test_getToken(_T("foo"), '&', _T("foo"), _T(""));
		test_getToken(_T("foo&"), '&', _T("foo"), _T(""));
		test_getToken(_T("foo&bar"), '&', _T("foo"), _T("bar"));
		test_getToken(_T("foo&bar&test"), '&', _T("foo"), _T("bar&test"));
	}

	inline void test_replace(std::wstring source, std::wstring replace, std::wstring with, std::wstring out) {
		std::wcout << _T("strEx::test_replace(") << source << _T(", ") << replace << _T(", ") << with << _T(") : ");
		std::wstring s = source;
		strEx::replace(s, replace, with);
		if (s == out)
			std::wcout << _T("Succeeded") << std::endl;
		else
			std::wcout << _T("Failed [") << s << _T("=") << out << _T("]") << std::endl;
	}
	inline void run_test_replace() {
		test_replace(_T(""), _T(""), _T(""), _T(""));
		test_replace(_T("foo"), _T(""), _T(""), _T("foo"));
		test_replace(_T("foobar"), _T("foo"), _T(""), _T("bar"));
		test_replace(_T("foobar"), _T("foo"), _T("bar"), _T("barbar"));
	}

#endif
}