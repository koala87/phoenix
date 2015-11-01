/**
 * Romaji related functions
 * @author Shiwei Zhang
 * @date 2014.02.11
 */

#pragma once

#include "utility/Transcode.h"

namespace yiqiding { namespace utility {
	/// Convert Japanese string to Romaji
	std::string toRomaji(const std::string& s, char delimiter = ' ', bool lower_letter = true, CodePage::Charset encode = CodePage::UTF8);
}}
