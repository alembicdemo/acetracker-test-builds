/*
** Dn-FamiTracker - NES/Famicom sound tracker
** Copyright (C) 2020-2025 D.P.C.M.
** FamiTracker Copyright (C) 2005-2020 Jonathan Liss
** 0CC-FamiTracker Copyright (C) 2014-2018 HertzDevil
**
** This program is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program. If not, see https://www.gnu.org/licenses/.
*/


#pragma once

#include <iterator>
#include <string>
#include <string_view>
#include "../to_sv.h"
#include "utf8_conv.hpp"

namespace conv {

namespace std = ::std;

namespace details {

template <typename CharT, typename ForwardIt>
constexpr auto codepoint_from_utf(ForwardIt first, ForwardIt last) noexcept {
	if constexpr (sizeof(CharT) == sizeof(char))
		return utf8_to_codepoint(std::move(first), std::move(last));
	else if constexpr (sizeof(CharT) == sizeof(char16_t))
		return utf16_to_codepoint(std::move(first), std::move(last));
	else if constexpr (sizeof(CharT) == sizeof(char32_t))
		return utf32_to_codepoint(std::move(first), std::move(last));
	else
		static_assert(!sizeof(ForwardIt), "Invalid character size");
}

template <typename CharT, typename OutputIt>
constexpr auto utf_from_codepoint(OutputIt it, char32_t c) noexcept {
	if constexpr (sizeof(CharT) == sizeof(char))
		return codepoint_to_utf8(std::move(it), c);
	else if constexpr (sizeof(CharT) == sizeof(char16_t))
		return codepoint_to_utf16(std::move(it), c);
	else if constexpr (sizeof(CharT) == sizeof(char32_t))
		return codepoint_to_utf32(std::move(it), c);
	else
		static_assert(!sizeof(OutputIt), "Invalid character size");
}

template <typename To, typename From>
std::basic_string<To> utf_convert(std::basic_string_view<From> sv) {
	std::basic_string<To> str;
	auto b = sv.begin();
	auto e = sv.end();
	while (b != e) {
		auto [it, ch] = codepoint_from_utf<From>(b, e);
		b = it;
		utf_from_codepoint<To>(std::back_inserter(str), ch);
	}
	return str;
}

template <typename To, typename From>
auto to_utf_string(From&& str) {
	if constexpr (std::is_same_v<std::decay_t<From>, To>)
		return To(std::forward<From>(str));
	else
		return details::utf_convert<typename To::value_type>(to_sv(str));
}

} // namespace details



template <typename T>
auto to_utf8(T&& str) {
	return details::to_utf_string<std::string>(std::forward<T>(str));
}
template <typename T>
auto to_utf16(T&& str) {
	return details::to_utf_string<std::u16string>(std::forward<T>(str));
}
template <typename T>
auto to_utf32(T&& str) {
	return details::to_utf_string<std::u32string>(std::forward<T>(str));
}
template <typename T>
auto to_wide(T&& str) {
	return details::to_utf_string<std::wstring>(std::forward<T>(str));
}

// defined by nyanpasu64:

typedef std::basic_string<TCHAR> tstring;

template <typename T>
auto to_t(T&& str) {
	return details::to_utf_string<tstring>(std::forward<T>(str));
}

} // namespace conv
