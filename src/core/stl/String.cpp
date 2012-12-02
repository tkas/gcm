#include <stl/String.h>
#include <stdarg.h>
#include <string.h>
#include <util/Log.h>

using namespace GCM;

String GCM_API cString::replace(String needle, String replacement) {
	size_t pos = 0;
	int occurences = 0;

	while ((pos = this->find(*(needle.getObj()), pos)) != std::string::npos) {
		occurences++;
		pos++;
	}

	// String is not modified.
	if (occurences == 0) {
		return String(*this);
	}

	// Modify the string.
	String out;
	//out->resize(this->length() - occurences * needle->length() + occurences * replacement->length());

	pos = 0;
	size_t lastpos = 0;
	while ((pos = this->find(*(needle.getObj()), pos)) != std::string::npos) {
		if (lastpos != pos) {
			out->append(*this, lastpos, pos - lastpos);
		}

		out->append(replacement);

		pos += needle->length();
		lastpos = pos;
	}

	if (lastpos != pos) {
		out->append(*this, lastpos, pos - lastpos);
	}

	return out;
}

bool GCM_API cString::equals(const char *string) {
	return *this == string;
}

bool GCM_API cString::equals(String string) {
	return *this == *(string.getObj());
}

bool GCM_API cString::startsWith(const char *string) {
	size_t len = strlen(string);
	return this->compare(0, len, string) == 0;
}

bool GCM_API cString::startsWith(String string) {
	return this->compare(0, string->length(), *(string.getObj())) == 0;
}

bool GCM_API cString::endsWith(const char *string) {
	size_t len = strlen(string);
	if (len <= this->length()) {
		return this->compare(
			this->length() - len,
			len,
			string
		) == 0;
	} else {
		return false;
	}
}

bool GCM_API cString::endsWith(String string) {
	if (string->length() <= this->length()) {
		return this->compare(
			this->length() - string->length(),
			string->length(),
			*(string.getObj())
		) == 0;
	} else {
		return false;
	}
}

String GCM_API cString::toLowerCase() {
	String out = cString(*this);
	cString &me = *(out.getObj());
	for (size_t i = 0; i < out->length(); i++) {
		me[i] = tolower(me[i]);
	}
	return out;
}

String GCM_API cString::toUpperCase() {
	String out = cString(*this);
	cString &me = *(out.getObj());
	for (size_t i = 0; i < out->length(); i++) {
		me[i] = toupper(me[i]);
	}
	return out;
}

String GCM_API cString::substring(int start, int len) {
	if (start < 0) {
		start = this->length() + start;
		if (start < 0) start = 0;
	}
	
	if (len < 0) {
		len = this->length() + len - start;
		if (len < 0) len = 0;
	}
	
	if (len == 0) {
		len = this->length() - start;
		if (len < 0) len = 0;
	}
	
	return this->substr(start, len);
}

std::string GCM_API &cString::append(const std::string &s) {
	return std::string::append(s);
}

std::string GCM_API &cString::append(const std::string &s, size_t pos, size_t n) {
	return std::string::append(s, pos, n);
}

std::string GCM_API &cString::append(const char *s, size_t n) {
	return std::string::append(s, n);
}

std::string GCM_API &cString::append(const char *s) {
	return std::string::append(s);
}

std::string GCM_API &cString::append(size_t n, char c) {
	return std::string::append(n, c);
}

String GCM_API cString::append(String string) {
	this->append(*(string.getObj()));
	return this;
}

String GCM_API String::format(const char *format, ...) {
	va_list ap;
	int size;
	char *buffer;
	va_start(ap, format);
		size = vsnprintf(NULL, 0, format, ap) + 1;
		buffer = (char *)malloc(sizeof(char) * size);
	va_end(ap);

	va_start(ap, format);
		vsnprintf(buffer, size, format, ap);
	va_end(ap);

	String out(buffer);
	free(buffer);
	return out;
}

// For MSVC
#ifndef va_copy
# ifdef _MSC_VER
#  define va_copy(d,s) ((d) = (s))
# endif
#endif

String GCM_API String::vformat(const char *format, va_list ap) {
	va_list sec;
	int size;
	char *buffer;

	va_copy(sec, ap);
		size = vsnprintf(NULL, 0, format, sec) + 1;
		buffer = (char *)malloc(sizeof(char) * size);
	va_end(sec);

	vsnprintf(buffer, size, format, ap);

	String out(buffer);
	free(buffer);
	return out;
}