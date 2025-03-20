#pragma once

#include <tuple>
#include <string>
#include <vector>
#include <format>

namespace null_values {

	template <class T>
	T get() {
		if constexpr (std::is_same_v<T, int>)
			return std::numeric_limits<int>::min();
		if constexpr (std::is_same_v<T, std::string>)
			return "_null";
		if constexpr (std::is_same_v<T, bool>)
			return "false";
	}

	bool is_null(const auto & x) {
		return get<std::remove_cvref_t<decltype(x)>>() == x;
	}

	template<class... T>
	void _fill_tuple_null(std::tuple<T...> & tuple) {
		std::apply([&](auto&... elem) {
			((elem = get<T>()), ...);
		}, tuple);
	}

}

template <class D, class Tuple> requires std::_Is_specialization_v<Tuple, std::tuple>
struct BasicTypeDB{

	using tuple_t = Tuple;
	static constexpr int tuple_size = std::tuple_size_v<Tuple>;

	BasicTypeDB() {
		null_values::_fill_tuple_null(tp);
	}

// virtual abstract
	static std::string tuple_info_name() { return D::tuple_info_name_override(); }
	static std::string field_info(int field) { return  D::field_info_override(field); }

//just vritual static
	static std::string tuple_info_custom_select() {
		if constexpr ( requires{ D::tuple_info_custom_select_override(); })
			return D::tuple_info_custom_select_override();
		else
			return "";
	}
	static bool auto_increment_first() {
		if constexpr ( requires{ D::auto_increment_first_override(); })
			return D::auto_increment_first_override();
		else
			return true;
	}

	Tuple tp;
};
