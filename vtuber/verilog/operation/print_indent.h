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
	bool at_newline;
	bool inline_mode;
	unsigned get_indent_level() {
		assert(not is_first_item_after_indent.empty());
		return is_first_item_after_indent.size() - 1;
	}
	void ensure_text_indent() {
		if (at_newline) {
			const unsigned indent_level = get_indent_level();
			for (unsigned i = 0; i < indent_level; ++i) {
				ost << "\t";
			}
		}
		at_newline = false;
	}
	void put_newline_if_noinline() {
		if (inline_mode) {
			return;
		}
		ost << '\n';
		at_newline = true;
	}
	void put_separator() {
		ost << ',';
		put_newline_if_noinline();
	}
public:
	ost_indent_helper(::std::ostream &ost_):
		ost(ost_),
		is_first_item_after_indent{true},
		at_newline(true),
		inline_mode(false) {}
	void new_nested_object() {
		is_first_item_after_indent.back() = true;
	}
	void open_indent(const char left_char, bool is_inline=false) {
		assert(not inline_mode); // inline mode cannot be nested
		inline_mode = is_inline;
		ensure_text_indent();
		ost << left_char;
		put_newline_if_noinline();
		is_first_item_after_indent.push_back(true);
	}
	void put_newitem() {
		if (is_first_item_after_indent.back()) {
			is_first_item_after_indent.back() = false;
		} else {
			put_separator();
		}
	}
	void close_indent(char right_char) {
		put_newline_if_noinline();
		is_first_item_after_indent.pop_back();
		ensure_text_indent();
		ost << right_char;
		inline_mode = false;
	}
	template<typename T>
	ost_indent_helper& operator<<(const T& t) {
		ensure_text_indent();
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
		helper.new_nested_object();
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
			helper.put_newitem();
			helper << '"' << T::get_name(i) << "\": ";
			print_member_recursively(helper, t.template get<i>());
		}(),
	...);
	helper.close_indent('}');
}

} // namespace detail

} // namespace verilog
