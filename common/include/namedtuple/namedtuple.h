#pragma once
// Direct include
// C system headers
// C++ standard library headers
#include <type_traits>
#include <utility>
// Other libraries' .h files.
#include <boost/preprocessor/seq/for_each_i.hpp>
#include <boost/preprocessor/seq/size.hpp>
#include <boost/preprocessor/stringize.hpp>
#include <boost/preprocessor/tuple/to_seq.hpp>
// Your project's .h files.

#if __cplusplus >= 201402L
#  define _DEFINE_NAMETUPLE_14 typedef ::std::make_integer_sequence<unsigned, num_members> indices_t;
#else
#  define _DEFINE_NAMETUPLE_14
#endif

#define _CREATE_NAMEDTUPLE_GETTERS(dummy1, dummy2, i, name)\
	auto& get(::std::integral_constant<unsigned, i>) { return name; }\
	const auto& get(::std::integral_constant<unsigned, i>) const { return name; }
#define _CREATE_NAMEDTUPLE_NAMES(dummy1, dummy2, dummy3, name) BOOST_PP_STRINGIZE(name),
#define MAKE_NAMEDTUPLE_SEQ(members)\
	static constexpr unsigned num_members = BOOST_PP_SEQ_SIZE(members);\
	_DEFINE_NAMETUPLE_14\
	BOOST_PP_SEQ_FOR_EACH_I(_CREATE_NAMEDTUPLE_GETTERS, dummy, members)\
	static const char* get_name(unsigned i) {\
		static const char *names[num_members+1u] = {\
			BOOST_PP_SEQ_FOR_EACH_I(_CREATE_NAMEDTUPLE_NAMES, dummy, members) ""\
		};\
		return names[i];\
	}\
	template<unsigned x> auto& get() { return get(::std::integral_constant<unsigned, x>{}); }\
	template<unsigned x> const auto& get() const { return get(::std::integral_constant<unsigned, x>{}); }
#define MAKE_NAMEDTUPLE(...) MAKE_NAMEDTUPLE_SEQ(BOOST_PP_TUPLE_TO_SEQ((__VA_ARGS__)))
