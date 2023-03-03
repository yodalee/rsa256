#pragma once
// Direct include
// C system headers
// C++ standard library headers
#include <cassert>
#include <ostream>
#include <utility>
#include <vector>
// Other libraries' .h files.
// Your project's .h files.
#include "verilog/dtype_base.h"

namespace verilog {

namespace detail {

class ost_indent_helper {
	::std::ostream &ost;
	::std::vector<bool> is_first_item_after_indent;
	bool inline_mode;
	bool skip_next_new_line;
	void add_new_line() {
		if (inline_mode) {
			return;
		}
		if (skip_next_new_line) {
			skip_next_new_line = false;
			return;
		}
		ost << "\n";
		const unsigned indent_level = get_indent_level();
		for (unsigned i = 0; i < indent_level; ++i) {
			ost << "\t";
		}
	}
public:
	ost_indent_helper(::std::ostream &ost_):
		ost(ost_),
		is_first_item_after_indent{true},
		inline_mode(false),
		skip_next_new_line(true) {}
	void force_skip_next_new_line() {
		skip_next_new_line = true;
	}
	unsigned get_indent_level() {
		assert(not is_first_item_after_indent.empty());
		return is_first_item_after_indent.size() - 1;
	}
	void open_indent(const char left_char, bool is_inline=false) {
		add_new_line();
		assert(not inline_mode); // inline mode cannot be nested
		inline_mode = is_inline;
		is_first_item_after_indent.push_back(true);
		ost << left_char;
	}
	void add_new_item() {
		if (is_first_item_after_indent.back()) {
			is_first_item_after_indent.back() = false;
		} else {
			ost << ",";
		}
	}
	void close_indent(char right_char) {
		is_first_item_after_indent.pop_back();
		add_new_line();
		ost << right_char;
		inline_mode = false;
	}
	template<typename T>
	ost_indent_helper& operator<<(const T& t) {
		ost << t;
		return *this;
	}
};

// forward declaration
template<typename T, unsigned ...i>
void vstruct_print_with_helper_expander(
	ost_indent_helper& helper, const T& t,
	::std::integer_sequence<unsigned, i...> idx
);

template<typename T_>
void print_member_recursively(ost_indent_helper& helper, const T_& t) {
	typedef ::std::remove_reference_t<T_> T;
	static_assert(is_dtype_v<T>, "T must be a verilog type");
	if constexpr (is_vint_v<T>) {
		helper << t;
	} else {
		helper.force_skip_next_new_line();
		t.print_with_helper(helper);
	}
}

template<typename T, unsigned ...i>
void vstruct_print_with_helper_expander(
	ost_indent_helper& helper, const T& t,
	::std::integer_sequence<unsigned, i...> idx
) {
	helper.open_indent('{');
	(
		[&]() {
			helper.add_new_item();
			helper << '"' << T::get_name(i) << "\": ";
			print_member_recursively(helper, t.template get<i>());
		}(),
	...);
	helper.close_indent('}');
}

} // namespace detail

} // namespace verilog
