#ifndef _GCM_STRING_H
# define _GCM_STRING_H

#include <string>
#include <Object.h>
#include <GC.h>
#include <stdio.h>
#include <vector>

namespace GCM {

class String;

class GCM_API cString: public Object, public std::string {
	public:
		cString(): Object(), std::string() {}
		cString(const std::string &str): Object(), std::string(str) {}
		cString(const std::string &str, size_t pos, size_t n = std::string::npos): Object(), std::string(str, pos, n) {}
		cString(const char *s, size_t n): Object(), std::string(s, n) {}
		cString(const char *s): Object(), std::string(s) {}
		cString(size_t n, char c): Object(), std::string(n, c) {}

		String replace(String needle, String replacement);

		bool equals(const char *string);
		bool equals(String string);
		bool startsWith(const char *string);
		bool startsWith(String string);
		bool endsWith(const char *string);
		bool endsWith(String string);
		String substring(int start, int length = 0);
		
		std::string &append(const std::string &s);
		std::string &append(const std::string &s, size_t pos, size_t n);
		std::string &append(const char *s, size_t n);
		std::string &append(const char *s);
		std::string &append(size_t n, char c);

		template <class InputIterator>
		std::string &append(InputIterator first, InputIterator last) {
			return std::string::append(first, last);
		}

		String append(String string);
		String toLowerCase();
		String toUpperCase();
};

class GCM_API String: public GC<cString> {
	public:
		String(): GC<cString>(new cString()) {}
		String(cString *str): GC<cString>(str) {}
		String(const std::string &str): GC<cString>(new cString(str)) {}
		String(const std::string &str, size_t pos, size_t n = std::string::npos): GC<cString>(new cString(str, pos, n)) {}
		String(const char *s, size_t n): GC<cString>(new cString(s, n)) {}
		String(const char *s): GC<cString>(new cString(s)) { }
		String(size_t n, char c): GC<cString>(new cString(n, c)) {}
		String(const String &s): GC<cString>(s) { }
		String(const String s, size_t pos, size_t n = std::string::npos): GC<cString>(new cString(*(s.getObj()), pos, n)) {}

		bool operator==(const char *str) const { return *(this->mObj) == str; }
		bool operator==(const std::string &str) const { return *(this->mObj) == str; }
		bool operator==(const String &str) const { return *(str.mObj) == *(this->mObj); }
		bool operator<(const char *str) const { return *(this->mObj) < str; }
		bool operator<(const std::string &str) const { return *(this->mObj) < str; }
		bool operator<(const String &str) const { return *(this->mObj) < *(str.mObj); }
		static String format(const char *format, ...);
		static String vformat(const char *format, va_list ap);
};

class sStringList: public std::vector<String>, public Object {
};

class StringList: public GC<sStringList> {
	public:
		StringList(): GC<sStringList>(new sStringList()) {}
		typedef std::vector<String>::iterator iterator;
		typedef std::vector<String>::const_iterator const_iterator;
		typedef std::vector<String>::const_reverse_iterator const_reverse_iterator;
		typedef std::vector<String>::reverse_iterator reverse_iterator;
};

}

#endif
