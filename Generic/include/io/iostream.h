/**
 * iostream extracted
 * @author Shiwei Zhang
 * @date 2014.01.08
 */

#pragma once

#include <string>
#include <vector>

namespace yiqiding { namespace io {
	class ios {
	protected:
		bool is_eof;
	public:
		ios() : is_eof(false) {};
		virtual ~ios() {};
		inline bool eof() const { return is_eof; }
		inline operator bool() const { return !eof(); }
		virtual void clear();
	};

	class istream : virtual public ios {
	public:
		virtual istream& read(char* s, size_t n) = 0;
		virtual istream& operator>>(std::string&);
		virtual istream& operator>>(char*);
		virtual istream& operator>>(char&);
		virtual istream& operator>>(std::vector<char>&);
		virtual int get();
		virtual istream& getline(std::string& str, char delim = 0);
	};

	class ostream : virtual public ios {
	public:
		virtual ostream& write(const char* s, size_t n) = 0;
		virtual ostream& operator<<(const std::string&);
		virtual ostream& operator<<(const char*);
		virtual ostream& operator<<(char);
		virtual ostream& operator<<(const std::vector<char>&);
		virtual ostream& operator<<(ostream& (*callback)(ostream&));
	};

	class iostream : public istream, public ostream {};

	// inline functions
	inline istream& getline(istream& is, std::string& str, char delim = 0) {
		return is.getline(str,delim);
	}

	inline ostream& endl(ostream& is) {
		return is << "\r\n";	// windows style
	}
}}
