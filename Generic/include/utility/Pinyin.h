/**
 * Pinyin related functions
 * @author Shiwei Zhang
 * @date 2014.02.11
 */

#pragma once

#include "utility/Transcode.h"

namespace yiqiding { namespace utility {
	/// Convert Han zi (Kanji) to Pinyin
	std::string toPinyin(const std::string& s, char delimiter = ' ', bool lower_letter = true, CodePage::Charset encode = CodePage::UTF8);
}}
