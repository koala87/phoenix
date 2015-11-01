/**
 * MD5 Calculator
 * Lohas Network Technology Co., Ltd
 * @author Shiwei Zhang
 * @date 2014.02.19
 * @note imported from NBS library
 */

#pragma once

#include <vector>
#include <string>
#include <istream>

namespace yiqiding { namespace crypto {
	class MD5 {
	private:
		typedef unsigned int uint4;
		typedef unsigned short int uint2;
		typedef unsigned char uint1;

		uint4 state[4];
		uint4 count[2];
		uint1 buffer[64];
		uint1 digest[16];
		bool finalised;
	public:
		MD5();

		// friendly constructors. all finalised.
		MD5(const char*, uint4);
		MD5(std::istream&);
		MD5(const char*);
		MD5(const std::string&);
		MD5(const std::vector<char>&);

		// control functions
		void update(const uint1*, uint4);
		void update(std::istream&);
		void update(const char*);
		void update(const std::string&);
		void update(const std::vector<char>&);
		void finalise();

		// output functions
		const uint1* raw() const;
		std::string hex() const;
		std::string HEX() const;
		std::vector<char> vector() const;

		// inline functions
		inline void update(const char* buf, uint4 len) {
			update((const unsigned char*)buf, len);
		}

		// stream output
		friend std::ostream& operator<<(std::ostream& o, const MD5& m) {
			return o << m.hex();
		}
	private:
		void assert_finalised() const;
		void assert_not_finalised() const;
		void init();
		void transform(const uint1 *buffer);

		// static functions
		static void encode(uint1 *dest, const uint4 *src, uint4 length);
		static void decode(uint4 *dest, const uint1 *src, uint4 length);
		static void memcpy(uint1 *dest, const uint1 *src, uint4 length);
		static void memset(uint1 *start, uint1 val, uint4 length);

		static inline uint4 rotate_left(uint4 x, uint4 n);
		static inline uint4 F(uint4 x, uint4 y, uint4 z);
		static inline uint4 G(uint4 x, uint4 y, uint4 z);
		static inline uint4 H(uint4 x, uint4 y, uint4 z);
		static inline uint4 I(uint4 x, uint4 y, uint4 z);
		static inline void FF(uint4& a, uint4 b, uint4 c, uint4 d,
			uint4 x, uint4 s, uint4 ac);
		static inline void GG(uint4& a, uint4 b, uint4 c, uint4 d,
			uint4 x, uint4 s, uint4 ac);
		static inline void HH(uint4& a, uint4 b, uint4 c, uint4 d,
			uint4 x, uint4 s, uint4 ac);
		static inline void II(uint4& a, uint4 b, uint4 c, uint4 d,
			uint4 x, uint4 s, uint4 ac);
	};
}}
