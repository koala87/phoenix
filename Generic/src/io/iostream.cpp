/**
 * iostream implementation
 * @author Shiwei Zhang
 * @date 2014.01.06
 */

#include <cctype>
#include "io/iostream.h"
using namespace yiqiding::io;

//
// ios
//
void ios::clear() {
	is_eof = false;
}

//
// istream
//
// note: this method WILL remove whitespace in the original source after str
istream& istream::operator>>(std::string& str) {
	int c;

	str.clear();

	// skip whitespace
	while ((c = get()) != EOF) {
		if (!isspace(c))
			break;
	}
	// fill string
	while (!eof() && !isspace(c)) {
		str.push_back(c);
		c = get();
	}

	return *this;
}

istream& istream::operator>>(char* str) {
	int c;

	// skip whitespace
	while ((c = get()) != EOF) {
		if (!isspace(c))
			break;
	}
	// fill string
	while (!eof() && !isspace(c)) {
		*str++ = c;
		c = get();
	}
	*str = 0;

	return *this;
}

istream& istream::operator>>(char& ch) {
	int c;

	// skip whitespace
	while ((c = get()) != EOF) {
		if (!isspace(c)) {
			ch = c;
			break;
		}
	}

	return *this;
}

istream& istream::operator>>(std::vector<char>& v) {
	int c;

	v.clear();
	while ((c = get()) != EOF)
		v.push_back(c);

	return *this;
}

int istream::get() {
	char c;
	read(&c, 1);
	if (eof())
		return EOF;
	else
		return c;
}

istream& istream::getline(std::string& str, char delim) {
	int c;

	str.clear();
	while ((c = get()) != EOF && c != delim)
		str.push_back(c);

	return *this;
}

//
// ostream
//
ostream& ostream::operator<<(const std::string& str) {
	return write(str.c_str(), str.size());
}

ostream& ostream::operator<<(const char* str) {
	return write(str, strlen(str));
}

ostream& ostream::operator<<(char c) {
	return write(&c, 1);
}

ostream& ostream::operator<<(const std::vector<char>& v) {
	if (v.empty())
		return *this;
	else
		return write(&v[0], v.size());
}

ostream& ostream::operator<<(ostream& (*callback)(ostream&)) {
	return callback(*this);
}
