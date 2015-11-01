/**
 * Smart Vector
 * @author Shiwei Zhang
 * @date 2014.02.11
 */

#pragma once

#include <vector>

namespace yiqiding { namespace container {
	template<class T>
	class SmartVector : public std::vector<T*> {
	public:
		~SmartVector()	{ clear(); }
		void clear() {
			for each (T* t in *this)
				delete t;
			std::vector<T*>::clear();
		};
	};
}}
