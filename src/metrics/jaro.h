/*
 * Copyright (C) 2016 deipi.com LLC and contributors. All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

#pragma once

#include "basic_string_metric.h"


/*
 * The Jaro distance.
 *
 * Character-based metric.
 */
class Jaro : public StringMetric<Jaro> {
	friend class StringMetric<Jaro>;
	friend class Jaro_Winkler;

	std::vector<char> get_common_characters(const std::string& str1, const std::string& str2, size_t max_separation) const {
		size_t l_str1 = str1.length();
		std::vector<char> common_chars;
		common_chars.reserve(l_str1);
		std::vector<bool> proc_str1(l_str1);

		// Calculate matching characters.
		size_t i = 0;
		for (const auto& c : str2) {
			size_t start = std::max((int)(i - max_separation), 0);
			for (auto end = std::min(i + max_separation + 1, l_str1); start < end; ++start) {
				if (c == str1[start] && !proc_str1[start]) {
					proc_str1[start] = true;
					common_chars.push_back(c);
					break;
				}
			}
			++i;
		}

		return common_chars;
	}

	double _similarity(const std::string& str1, const std::string& str2) const {
		auto l_str1 = str1.length();
		auto l_str2 = str2.length();

		auto max_separation = std::max((size_t)0, std::max(l_str1, l_str2) / 2 - 1);

		auto common_str1 = get_common_characters(str1, str2, max_separation);
		auto common_str2 = get_common_characters(str2, str1, max_separation);

		auto m1 = common_str1.size(), m2 = common_str2.size();
		if (!m1 || !m2) {
			return 0.0;
		}

		// Calculate character transpositions.
		size_t t = 0;
		if (m1 < m2) {
			auto it = common_str2.begin();
			for (const auto& c : common_str1) {
				t += *it != c;
				++it;
			}
		} else {
			m1 = m2;
			auto it = common_str1.begin();
			for (const auto& c : common_str2) {
				t += *it != c;
				++it;
			}
		}

		return (((double)m1 / l_str1) + ((double)m1 / l_str2) + ((m1 - t / 2.0) / m1)) / 3.0;
	}

	double _similarity(const std::string& str2) const {
		return _similarity(_str, str2);
	}

	double _distance(const std::string& str1, const std::string& str2) const {
		return 1.0 - _similarity(str1, str2);
	}

	double _distance(const std::string& str2) const {
		return 1.0 - _similarity(_str, str2);
	}

	std::string _description() const {
		return "Jaro";
	}

public:
	Jaro(bool icase=true)
		: StringMetric<Jaro>(icase) { }

	template <typename T>
	Jaro(T&& str, bool icase=true)
		: StringMetric<Jaro>(std::forward<T>(str), icase) { }
};
