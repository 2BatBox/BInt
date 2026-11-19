#pragma once

#include "BigFxp.h"

#include <vector>
#include <cstring>

template <size_t W, size_t P>
struct RndFxpGen {
	using Type_t = bint::BigFxp<W, P>;

	static bint::BigFxp<W, P> next() {
		Type_t result(RndIntGen<typename Type_t::IntValue>::next(), 0);
		return result;
	}
};

class TestBigFxp {

	static constexpr size_t AUTOTEST_ROUNDS = 1000u;
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
		ITERATE_ANY(test_relational, AUTOTEST_ROUNDS);
		ITERATE_INT_COMPATIBLE(test_range, 0);
		ITERATE_INT_COMPATIBLE(test_inc_dec, AUTOTEST_ROUNDS);
		ITERATE_INT_COMPATIBLE(test_add_sub, AUTOTEST_ROUNDS);
		ITERATE_INT_COMPATIBLE(test_mult_div, AUTOTEST_ROUNDS);

		test_type_conversion();
	}

private:

	template <size_t W, size_t P>
	void test_load_store_string(size_t rounds) {
		using Type_t = bint::BigFxp<W, P>;
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

		test_load_store_string_value(Type_t(-1));
		test_load_store_string_value(Type_t(0));
		test_load_store_string_value(Type_t(1));
		test_load_store_string_value(Type_t::max());
		test_load_store_string_value(Type_t::min());

		while(rounds--) {
			test_load_store_string_value(RndFxpGen<W, P>::next());
		}
	}

	template <typename FxpType_t>
	void test_load_store_string_value(const FxpType_t& fxp_in) {
		const std::string str_in = fxp_in.store_str();
		FxpType_t fxp_out;
		ASSERT(fxp_out.load_str(str_in.c_str()) == str_in.length());
		ASSERT(fxp_in == fxp_out);
	}

	template <size_t W, size_t P>
	void test_relational(size_t rounds) {
		using Type_t = bint::BigFxp<W, P>;

		while(rounds--) {
			const Type_t x = RndFxpGen<W, P>::next();
			const Type_t y = RndFxpGen<W, P>::next();

			// X vs X
			ASSERT(x == x);
			ASSERT(x <= x);
			ASSERT(x >= x);
			ASSERT(not (x != x));
			ASSERT(not (x > x));
			ASSERT(not (x < x));

			// X vs Y
			ASSERT((x == y) == (x.raw_value() == y.raw_value()));
			ASSERT((y == x) == (y.raw_value() == x.raw_value()));

			ASSERT((x != y) == (x.raw_value() != y.raw_value()));
			ASSERT((y != x) == (y.raw_value() != x.raw_value()));

			ASSERT((x > y) == (x.raw_value() > y.raw_value()));
			ASSERT((x < y) == (x.raw_value() < y.raw_value()));

			ASSERT((x >= y) == (x.raw_value() >= y.raw_value()));
			ASSERT((x <= y) == (x.raw_value() <= y.raw_value()));
		}
	}

	template <size_t W, size_t P>
	static void test_range(size_t) {
		using Type_t = bint::BigFxp<W, P>;
		ASSERT(Type_t::max() > 1);
		ASSERT(Type_t::min() < -1);
		ASSERT(Type_t::max() >= std::numeric_limits<signed int>::max());
		ASSERT(Type_t::min() <= std::numeric_limits<signed int>::min());
		ASSERT(Type_t::delta() < 1);
	}

	template <size_t W, size_t P>
	void test_inc_dec(size_t rounds) {
		using Type_t = bint::BigFxp<W, P>;
		Type_t bfxp;

		test_inc_dec_value(Type_t::max());
		test_inc_dec_value(Type_t::min());

		while(rounds--) {
			test_inc_dec_value(RndFxpGen<W, P>::next());
		}
	}

	template <typename FxpType_t>
	void test_inc_dec_value(const FxpType_t x) {
		FxpType_t tmp = x;
		FxpType_t op = tmp++;
		ASSERT(op == x);
		ASSERT(tmp == (x + 1));

		tmp = x;
		op = ++tmp;
		ASSERT(op == (x + 1));
		ASSERT(tmp == op);

		tmp = x;
		op = tmp--;
		ASSERT(op == x);
		ASSERT(tmp == (x - 1));

		tmp = x;
		op = --tmp;
		ASSERT(op == (x - 1));
		ASSERT(tmp == op);
	}

	template <size_t W, size_t P>
	void test_add_sub(size_t rounds) {
		using Type_t = bint::BigFxp<W, P>;
		Type_t bfxp;

		test_add_sub_values(Type_t::max(), Type_t::max());
		test_add_sub_values(Type_t::max(), Type_t::min());
		test_add_sub_values(Type_t::min(), Type_t::max());
		test_add_sub_values(Type_t::min(), Type_t::min());

		while(rounds--) {
			test_add_sub_values(RndFxpGen<W, P>::next(), RndFxpGen<W, P>::next());
		}
	}

	template <typename FxpType_t>
	void test_add_sub_values(const FxpType_t x, const  FxpType_t y) {
		const FxpType_t res_add = x + y;
		const FxpType_t res_sub = x - y;

		ASSERT(x + 0 == x);
		ASSERT(x - 0 == x);
		ASSERT(0 + x == x);
		ASSERT(0 - x == (-x));

		ASSERT((x + y) == (y + x));
		ASSERT((x - y) == ((-y) + x));

		ASSERT(x + FxpType_t(0.0f) == x);
		ASSERT(x - FxpType_t(0.0f) == x);
		ASSERT(FxpType_t(0.0f) + x == x);
		ASSERT(FxpType_t(0.0f) - x == (-x));

		ASSERT(x + FxpType_t(0.0) == x);
		ASSERT(x - FxpType_t(0.0) == x);
		ASSERT(FxpType_t(0.0) + x == x);
		ASSERT(FxpType_t(0.0) - x == (-x));

		ASSERT(x + FxpType_t(1.0f) == (x + 1));
		ASSERT(x - FxpType_t(1.0f) == (x - 1));
		ASSERT(FxpType_t(1.0f) + x == (1 + x));
		ASSERT(FxpType_t(1.0f) - x == (1 - x));

		ASSERT(x + FxpType_t(1.0) == (x + 1));
		ASSERT(x - FxpType_t(1.0) == (x - 1));
		ASSERT(FxpType_t(1.0) + x == (1 + x));
		ASSERT(FxpType_t(1.0) - x == (1 - x));

		ASSERT((res_add - y) == x);
		ASSERT((res_add - x) == y);
		ASSERT((res_sub + y) == x);
		ASSERT((res_sub - x) == (-y));
		ASSERT((res_add + res_sub) == (x + x));
		ASSERT((res_add - res_sub) == (y + y));

		FxpType_t xop = x;
		xop += y;
		ASSERT(xop == (x + y));

		xop = x;
		xop -= y;
		ASSERT(xop == (x - y));
	}

	template <size_t W, size_t P>
	void test_mult_div(size_t rounds) {
		using Type_t = bint::BigFxp<W, P>;
		Type_t bfxp;

		while(rounds--) {
			test_mult_div_value(RndFxpGen<W, P>::next() / 10);
		}
	}

	template <typename FxpType_t>
	void test_mult_div_value(const FxpType_t x) {
		FxpType_t op = x;
		op = op * 2;
		ASSERT((x == 0) || (op != x));
		op = op / 2;
		ASSERT(op == x);

		op = x;
		op *= 2;
		ASSERT((x == 0) || (op != x));
		op /= 2;
		ASSERT(op == x);

		op = op * 2;
		ASSERT((x == 0) || (op != x));
		op = op / 2;
		ASSERT(op == x);

		op = x;
		op = op * FxpType_t(5.0f);
		ASSERT((x == 0) || (op != x));
		op = op / FxpType_t(5.0f);
		ASSERT(op == x);

		op = x;
		op *= FxpType_t(5.0f);
		ASSERT((x == 0) || (op != x));
		op /= FxpType_t(5.0f);
		ASSERT(op == x);

		op = x;
		op = op * FxpType_t(10.0);
		ASSERT((x == 0) || (op != x));
		op = op / FxpType_t(10.0);
		ASSERT(op == x);

		op = x;
		op *= FxpType_t(10.0);
		ASSERT((x == 0) || (op != x));
		op /= FxpType_t(10.0);
		ASSERT(op == x);
	}


	static void test_arg_unsigned(const unsigned& val) {}
	static void test_arg_float(const float& val) {}
	static void test_arg_double(const double& val) {}

	static void test_type_conversion() {
		printf("->TestBigFxp::test_type_conversion()\n");
		using Type28i4f = bint::BigFxp<32u, 1u>;
		using Type18i14f = bint::BigFxp<32u, 4u>;
		using Type15i17f = bint::BigFxp<32u, 5u>;

		int8_t i8 = 0;
		int16_t i16 = 0;
		int32_t i32 = 0;
		float fl = 0.;
		double dbl = 0.;

		Type28i4f x28i4f;
		Type18i14f x18i14f;
		Type15i17f x15i17f;

		x28i4f = i8;
		x28i4f = i16;
		x28i4f = static_cast<Type28i4f>(i32);

		x18i14f = i8;
		x18i14f = i16;
		x18i14f = static_cast<Type18i14f>(i32);

		x15i17f = i8;
		x15i17f = static_cast<Type15i17f>(i16);
		x15i17f = static_cast<Type15i17f>(i32);

		x28i4f = static_cast<Type28i4f>(fl);
		x18i14f = static_cast<Type18i14f>(fl);
		x15i17f = static_cast<Type15i17f>(fl);

		x28i4f = static_cast<Type28i4f>(dbl);
		x18i14f = static_cast<Type18i14f>(dbl);
		x15i17f = static_cast<Type15i17f>(dbl);

	}

};
