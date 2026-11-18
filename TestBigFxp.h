#pragma once

#include "BigFxp.h"

#include <vector>
#include <cstring>

class TestBigFxp {

	static constexpr size_t AUTOTEST_ROUNDS = 1000u;
	static constexpr size_t PERFORMANCE_TEST_ROUNDS = 100000u;
	static constexpr size_t INT_BITS = sizeof(signed int) * CHAR_BIT;
	static constexpr size_t INT_DEC_DIGITS = INT_BITS / 4u;

	static_assert(INT_DEC_DIGITS > 1, "The autotest is useless!");

#define ITERATE_ANY(func, rounds) { \
	printf("->TestBigFxp::"#func"()\n"); \
	func<8u, 1u>(rounds); \
	func<8u, 2u>(rounds); \
	func<16u, 1u>(rounds); \
	func<16u, 2u>(rounds); \
	func<16u, 3u>(rounds); \
	func<16u, 4u>(rounds); \
	func<32u, 1u>(rounds); \
	func<32u, 2u>(rounds); \
	func<32u, 3u>(rounds); \
	func<32u, 8u>(rounds); \
	func<32u, 9u>(rounds); \
	func<64u, 1u>(rounds); \
	func<64u, 2u>(rounds); \
	func<64u, 8u>(rounds); \
	func<64u, 17u>(rounds); \
	func<64u, 18u>(rounds); \
}

#define ITERATE_INT_COMPATIBLE(func, rounds) { \
	printf("->TestBigFxp::"#func"()\n"); \
	func<INT_BITS * 2u, INT_DEC_DIGITS>(rounds); \
	func<INT_BITS * 2u, INT_DEC_DIGITS - 1u>(rounds); \
	func<INT_BITS * 3u, INT_DEC_DIGITS>(rounds); \
	func<INT_BITS * 3u, INT_DEC_DIGITS - 1u>(rounds); \
}

public:

	TestBigFxp() {
		ITERATE_ANY(test_load_store_string, AUTOTEST_ROUNDS);
		ITERATE_INT_COMPATIBLE(test_range, 0);
	}

private:

	template <size_t W, size_t P>
	void test_load_store_string(size_t rounds) {
		using Type_t = bint::BigFxp<W, P>;
		using TypeInt_t = typename Type_t::IntValue;
		RndIntGen<TypeInt_t> gen;

		Type_t bfxp;

		ASSERT(bfxp.load_str("") == 0);
		ASSERT(bfxp.load_str("+") == 0);
		ASSERT(bfxp.load_str("-") == 0);
		ASSERT(bfxp.load_str(".") == 0);
		ASSERT(bfxp.load_str("++0") == 0);
		ASSERT(bfxp.load_str("--0") == 0);
		ASSERT(bfxp.load_str("..0") == 0);
		ASSERT(bfxp.load_str("x0x") == 0);
		ASSERT(bfxp.load_str("-x") == 0);
		ASSERT(bfxp.load_str("za") == 0);

		test_load_store_string_value<Type_t, TypeInt_t>(bfxp, TypeInt_t(-1));
		test_load_store_string_value<Type_t, TypeInt_t>(bfxp, TypeInt_t(0));
		test_load_store_string_value<Type_t, TypeInt_t>(bfxp, TypeInt_t(1));
		test_load_store_string_value<Type_t, TypeInt_t>(bfxp, TypeInt_t::min());
		test_load_store_string_value<Type_t, TypeInt_t>(bfxp, TypeInt_t::max());

		while(rounds--) {
			test_load_store_string_value(bfxp, gen.next());
		}
	}

	template <typename FxpType_t, typename IntType_t>
	void test_load_store_string_value(FxpType_t& fxp_in, const IntType_t& int_ref) {
		fxp_in = FxpType_t(int_ref, 0);
		const std::string str_in = fxp_in.store_str();
		FxpType_t fxp_out;
		ASSERT(fxp_out.load_str(str_in.c_str()) == str_in.length());
		ASSERT(fxp_in == fxp_out);
	}

	template <size_t W, size_t P>
	static void test_range(unsigned) {
		using Type_t = bint::BigFxp<W, P>;
		ASSERT(Type_t::max() > 1);
		ASSERT(Type_t::min() < -1);
		ASSERT(Type_t::max() >= std::numeric_limits<signed int>::max());
		ASSERT(Type_t::min() <= std::numeric_limits<signed int>::min());
		ASSERT(Type_t::delta() < 1);
	}

};
