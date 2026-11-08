#pragma once

#include "utils/Perf.h"
#include "BigInt.h"

#include <random>
#include <climits>

#define ASSERT(exp) { \
	if(not (exp)) { \
		printf(#exp "\n"); \
		fflush(stdout); \
		fflush(stderr); \
		abort(); \
	} \
};

class TestBigInt {

	static constexpr size_t AUTOTEST_ROUNDS = 100000u;
	static constexpr size_t PERFORMANCE_TEST_ROUNDS = 100000u;

	template <typename T, std::enable_if_t<(std::is_integral_v<T>), int> = 0>
	struct UniformIntGen {
		std::mt19937_64 rnd;
		std::uniform_int_distribution<T> distr;
		UniformIntGen(const T min = std::numeric_limits<T>::min(), const T max = std::numeric_limits<T>::max()) : rnd(0), distr(min, max) {}
		T next() {
			return distr(rnd);
		}
	};

	template <typename BINT>
	struct UniformBigIntGen {
		std::mt19937_64 rnd;
		std::uniform_int_distribution<size_t> gen_size{1u, BINT::NUMBER_WIDTH_BYTES};
		std::uniform_int_distribution<int8_t> gen_data;

		BINT next() {
			BINT result;
			size_t size_bytes = gen_size(rnd);
			while(size_bytes--) {
				result <<= CHAR_BIT;
				result += BINT(gen_data(rnd));
			}
			return result;
		}
	};

	static constexpr size_t CHARS_PER_SHORT = sizeof(signed short);
	static constexpr size_t CHARS_PER_INT = sizeof(signed int);
	static constexpr size_t CHARS_PER_LONG = sizeof(signed long);
	static constexpr size_t SHORTS_PER_INT = sizeof(signed int) / sizeof(signed short);
	static constexpr size_t SHORTS_PER_LONG = sizeof(signed long) / sizeof(signed short);
	static constexpr size_t INT_PER_LONG = sizeof(signed long) / sizeof(signed int);

	using BiChar_t = BigInt<signed char, 1u>;
	using BiShort_t = BigInt<signed char, CHARS_PER_SHORT>;
	using BiInt_t = BigInt<signed char, CHARS_PER_INT>;
	using BiLong_t = BigInt<signed char, CHARS_PER_LONG>;

#define ITERATE_BIGINT_AND_EQUAL_WIDTH_INTEGERS(func, rounds) { \
	printf("->TestBigInt::"#func"()\n"); \
	func<BigInt<signed char, 1u>, signed char>(rounds); \
	func<BigInt<signed char, CHARS_PER_SHORT>, signed short>(rounds); \
	func<BigInt<signed char, CHARS_PER_INT>, signed int>(rounds); \
	func<BigInt<signed char, CHARS_PER_LONG>, signed long>(rounds); \
	func<BigInt<signed short, 1u>, signed short>(rounds); \
	func<BigInt<signed short, SHORTS_PER_INT>, signed int>(rounds); \
	func<BigInt<signed short, SHORTS_PER_LONG>, signed long>(rounds); \
	func<BigInt<signed int, 1u>, signed int>(rounds); \
	func<BigInt<signed int, INT_PER_LONG>, signed long>(rounds); \
	func<BigInt<signed long, 1u>, signed long>(rounds); \
}

#define ITERATE_BIGINT_AND_NARROWER_INTEGERS(func, rounds) { \
	printf("->TestBigInt::"#func"()\n"); \
	func<BigInt<signed char, 2u>, signed char>(rounds); \
	func<BigInt<signed char, CHARS_PER_SHORT + 1u>, signed short>(rounds); \
	func<BigInt<signed char, CHARS_PER_INT + 1u>, signed int>(rounds); \
	func<BigInt<signed char, CHARS_PER_LONG + 1u>, signed long>(rounds); \
	func<BigInt<signed short, 2u>, signed short>(rounds); \
	func<BigInt<signed short, SHORTS_PER_INT + 1u>, signed int>(rounds); \
	func<BigInt<signed short, SHORTS_PER_LONG + 1u>, signed long>(rounds); \
	func<BigInt<signed int, 2u>, signed int>(rounds); \
	func<BigInt<signed int, INT_PER_LONG + 1u>, signed long>(rounds); \
	func<BigInt<signed long, 2u>, signed long>(rounds); \
}

#define ITERATE_BIGINT_AND_EQUAL_WIDTH_BIGINT(func, rounds) { \
	printf("->TestBigInt::"#func"()\n"); \
	func<BigInt<signed char,  1u>, BigInt<signed char,  1u>>(rounds); \
	func<BigInt<signed char,  CHARS_PER_SHORT>, BigInt<signed short, 1u>>(rounds); \
	func<BigInt<signed char,  CHARS_PER_SHORT * 2u>, BigInt<signed short, 2u>>(rounds); \
	func<BigInt<signed char,  CHARS_PER_SHORT * 3u>, BigInt<signed short, 3u>>(rounds); \
	func<BigInt<signed char,  CHARS_PER_INT>, BigInt<signed int, 1u>>(rounds); \
	func<BigInt<signed char,  CHARS_PER_INT * 2u>, BigInt<signed int, 2u>>(rounds); \
	func<BigInt<signed char,  CHARS_PER_INT * 3u>, BigInt<signed int, 3u>>(rounds); \
	func<BigInt<signed char,  CHARS_PER_LONG>, BigInt<signed long, 1u>>(rounds); \
	func<BigInt<signed char,  CHARS_PER_LONG * 2u>, BigInt<signed long, 2u>>(rounds); \
	func<BigInt<signed char,  CHARS_PER_LONG * 3u>, BigInt<signed long, 3u>>(rounds); \
	func<BigInt<signed short,  1u>, BigInt<signed short, 1u>>(rounds); \
	func<BigInt<signed short,  SHORTS_PER_INT>, BigInt<signed int, 1u>>(rounds); \
	func<BigInt<signed short,  SHORTS_PER_INT * 2u>, BigInt<signed int, 2u>>(rounds); \
	func<BigInt<signed short,  SHORTS_PER_INT * 3u>, BigInt<signed int, 3u>>(rounds); \
	func<BigInt<signed short,  SHORTS_PER_LONG>, BigInt<signed long, 1u>>(rounds); \
	func<BigInt<signed short,  SHORTS_PER_LONG * 2u>, BigInt<signed long, 2u>>(rounds); \
	func<BigInt<signed short,  SHORTS_PER_LONG * 3u>, BigInt<signed long, 3u>>(rounds); \
	func<BigInt<signed int,  1u>, BigInt<signed int, 1u>>(rounds); \
	func<BigInt<signed int,  INT_PER_LONG>, BigInt<signed long, 1u>>(rounds); \
	func<BigInt<signed int,  INT_PER_LONG * 2u>, BigInt<signed long, 2u>>(rounds); \
	func<BigInt<signed int,  INT_PER_LONG * 3u>, BigInt<signed long, 3u>>(rounds); \
	func<BigInt<signed long, 1u>, BigInt<signed long, 1u>>(rounds); \
}

#define ITERATE_BIGINT_AND_WIDER_BIGINT(func, rounds) { \
	printf("->TestBigInt::"#func"()\n"); \
	func<BigInt<signed char,  1u>, BigInt<signed char,  2u>>(rounds); \
	func<BigInt<signed char,  CHARS_PER_SHORT>, BigInt<signed short, 2u>>(rounds); \
	func<BigInt<signed char,  CHARS_PER_SHORT * 2u>, BigInt<signed short, 3u>>(rounds); \
	func<BigInt<signed char,  CHARS_PER_SHORT * 3u>, BigInt<signed short, 4u>>(rounds); \
	func<BigInt<signed char,  CHARS_PER_INT>, BigInt<signed int, 2u>>(rounds); \
	func<BigInt<signed char,  CHARS_PER_INT * 2u>, BigInt<signed int, 3u>>(rounds); \
	func<BigInt<signed char,  CHARS_PER_INT * 3u>, BigInt<signed int, 4u>>(rounds); \
	func<BigInt<signed char,  CHARS_PER_LONG>, BigInt<signed long, 2u>>(rounds); \
	func<BigInt<signed char,  CHARS_PER_LONG * 2u>, BigInt<signed long, 3u>>(rounds); \
	func<BigInt<signed char,  CHARS_PER_LONG * 3u>, BigInt<signed long, 4u>>(rounds); \
	func<BigInt<signed short,  1u>, BigInt<signed short, 2u>>(rounds); \
	func<BigInt<signed short,  SHORTS_PER_INT>, BigInt<signed int, 2u>>(rounds); \
	func<BigInt<signed short,  SHORTS_PER_INT * 2u>, BigInt<signed int, 3u>>(rounds); \
	func<BigInt<signed short,  SHORTS_PER_INT * 3u>, BigInt<signed int, 4u>>(rounds); \
	func<BigInt<signed short,  SHORTS_PER_LONG>, BigInt<signed long, 2u>>(rounds); \
	func<BigInt<signed short,  SHORTS_PER_LONG * 2u>, BigInt<signed long, 3u>>(rounds); \
	func<BigInt<signed short,  SHORTS_PER_LONG * 3u>, BigInt<signed long, 4u>>(rounds); \
	func<BigInt<signed int,  1u>, BigInt<signed int, 2u>>(rounds); \
	func<BigInt<signed int,  INT_PER_LONG>, BigInt<signed long, 2u>>(rounds); \
	func<BigInt<signed int,  INT_PER_LONG * 2u>, BigInt<signed long, 3u>>(rounds); \
	func<BigInt<signed int,  INT_PER_LONG * 3u>, BigInt<signed long, 4u>>(rounds); \
	func<BigInt<signed long, 1u>, BigInt<signed long, 2u>>(rounds); \
}

public:

	TestBigInt() {

		// Type conversion tests.
		ITERATE_BIGINT_AND_EQUAL_WIDTH_INTEGERS(test_load_store_int, AUTOTEST_ROUNDS);
		ITERATE_BIGINT_AND_NARROWER_INTEGERS(test_load_store_int, AUTOTEST_ROUNDS);
		ITERATE_BIGINT_AND_EQUAL_WIDTH_BIGINT(test_load_store_big_int, AUTOTEST_ROUNDS);
		ITERATE_BIGINT_AND_WIDER_BIGINT(test_load_store_big_int, AUTOTEST_ROUNDS);
		test_type_conversion_bigint_int();
		test_type_conversion_bigint_bigint();

		// Test BigInt arithmetic operations comparing the results with the equal sized integral integer types.
		ITERATE_BIGINT_AND_EQUAL_WIDTH_INTEGERS(test_min_max, AUTOTEST_ROUNDS);
		ITERATE_BIGINT_AND_EQUAL_WIDTH_INTEGERS(test_bitwise, AUTOTEST_ROUNDS);
		ITERATE_BIGINT_AND_EQUAL_WIDTH_INTEGERS(test_shift, AUTOTEST_ROUNDS);
		ITERATE_BIGINT_AND_EQUAL_WIDTH_INTEGERS(test_relational, AUTOTEST_ROUNDS);
		ITERATE_BIGINT_AND_EQUAL_WIDTH_INTEGERS(test_add_sub, AUTOTEST_ROUNDS);
		ITERATE_BIGINT_AND_EQUAL_WIDTH_INTEGERS(test_mult, AUTOTEST_ROUNDS);
		ITERATE_BIGINT_AND_EQUAL_WIDTH_INTEGERS(test_div_mod, AUTOTEST_ROUNDS);
		ITERATE_BIGINT_AND_EQUAL_WIDTH_INTEGERS(test_sign_change, AUTOTEST_ROUNDS);

		test_constexpr_values();

		test_performance_all<1u>(PERFORMANCE_TEST_ROUNDS);
		test_performance_all<2u>(PERFORMANCE_TEST_ROUNDS);
		test_performance_all<3u>(PERFORMANCE_TEST_ROUNDS);
		test_performance_all<4u>(PERFORMANCE_TEST_ROUNDS);
		test_performance_all<13u>(PERFORMANCE_TEST_ROUNDS);
		test_performance_all<99u>(PERFORMANCE_TEST_ROUNDS);
	}

	template <typename BigType_t, typename Integer>
	void test_load_store_int(size_t rounds) {
		static_assert(sizeof(BigType_t) >= sizeof(Integer));
		UniformIntGen<Integer> gen;
		BigType_t bint;

		test_load_store_int_value<BigType_t, Integer>(bint, -1);
		test_load_store_int_value<BigType_t, Integer>(bint, 0);
		test_load_store_int_value<BigType_t, Integer>(bint, 1);
		test_load_store_int_value<BigType_t, Integer>(bint, std::numeric_limits<Integer>::min());
		test_load_store_int_value<BigType_t, Integer>(bint, std::numeric_limits<Integer>::max());

		while(rounds--) {
			test_load_store_int_value(bint, gen.next());
		}
	}

	template <typename BigType_t, typename Integer>
	void test_load_store_int_value(BigType_t& bint, const Integer& int_ref) {
		bint = int_ref;
		const Integer int_out = bint;
		ASSERT(int_out == int_ref);
	}

	template <typename A, typename B>
	void test_load_store_big_int(size_t rounds) {
		static_assert(A::NUMBER_WIDTH_BYTES <= B::NUMBER_WIDTH_BYTES);
		UniformBigIntGen<A> gen_a;

		test_load_store_big_int_value<A, B>(A(-1));
		test_load_store_big_int_value<A, B>(A(0));
		test_load_store_big_int_value<A, B>(A(1));
		test_load_store_big_int_value<A, B>(A::max());
		test_load_store_big_int_value<A, B>(A::min());

		while(rounds--) {
			test_load_store_big_int_value<A, B>(gen_a.next());
		}
	}

	template <typename A, typename B>
	void test_load_store_big_int_value(const A& a_in) {
		static_assert(A::NUMBER_WIDTH_BYTES <= B::NUMBER_WIDTH_BYTES);
		B b_in(a_in);
		A a_out(b_in);
		ASSERT(a_in == a_out);
		ASSERT(a_out == a_in);

		b_in = a_in;
		a_out = A(b_in);
		ASSERT(a_in == a_out);
		ASSERT(a_out == a_in);
	}

	// ----------------------------------------------------------------------------
	// Type conversion.
	// ----------------------------------------------------------------------------
	static signed char  get_char()   { return 0; }
	static signed short get_short()  { return 0; }
	static signed int   get_int()    { return 0; }
	static signed long  get_long()   { return 0; }

	static void set_char(signed char)   {}
	static void set_short(signed short) {}
	static void set_int(signed int)     {}
	static void set_long(signed long)   {}

	static void test_type_conversion_bigint_int() {
		printf("->TestBigInt::test_type_conversion_bigint_int()\n");

		// Integer to BigInt.

		// Wider to narrower. Explicit conversion.
		BiChar_t bic(get_long());
		BiShort_t bis(get_long());
		BiInt_t bii(get_long());
		BiLong_t bil(get_long());

		// Implicit conversion. Narrower to wider.
		bic = get_char();
		bis = get_char();
		bii = get_char();
		bil = get_char();

//		bic = get_short(); Is not allowed if sizeof(short) > 1u.
		bis = get_short();
		bii = get_short();
		bil = get_short();

//		bic = get_int(); Is not allowed if sizeof(int) > 1u.
//		bis = get_int(); Is not allowed if sizeof(int) > sizeof(short).
		bii = get_int();
		bil = get_int();

//		bic = get_long(); Is not allowed if sizeof(long) > 1u.
//		bis = get_long(); Is not allowed if sizeof(long) > sizeof(short).
//		bii = get_long(); Is not allowed if sizeof(long) > sizeof(int).
		bil = get_long();

		// Explicit conversion.
		bic = BiChar_t(get_short());
		bic = BiChar_t(get_int());
		bic = BiChar_t(get_long());
		bis = BiShort_t(get_int());
		bis = BiShort_t(get_long());
		bii = BiInt_t(get_long());
		bil = BiLong_t(get_long());

		bic = static_cast<BiChar_t>(get_short());
		bic = static_cast<BiChar_t>(get_int());
		bic = static_cast<BiChar_t>(get_long());
		bis = static_cast<BiShort_t>(get_int());
		bis = static_cast<BiShort_t>(get_long());
		bii = static_cast<BiInt_t>(get_long());
		bil = static_cast<BiLong_t>(get_long());

		// BigInt to integer.
		set_char(bic);
		set_short(bic);
		set_int(bic);
		set_long(bic);

//		set_char(bis); Is not allowed if sizeof(short) > 1u.
		set_short(bis);
		set_int(bis);
		set_long(bis);

//		set_char(bii); Is not allowed if sizeof(int) > 1u.
//		set_short(bii); Is not allowed if sizeof(int) > sizeof(short).
		set_int(bii);
		set_long(bii);

//		set_char(bil); Is not allowed if sizeof(long) > 1u.
//		set_short(bil); Is not allowed if sizeof(long) > sizeof(short).
//		set_int(bil); Is not allowed if sizeof(long) > sizeof(int).
		set_long(bil);
	}

	static BiChar_t  get_bi_char()  { return BiChar_t(0);  }
	static BiShort_t get_bi_short() { return BiShort_t(0); }
	static BiInt_t   get_bi_int()   { return BiInt_t(0);   }
	static BiLong_t  get_bi_long()  { return BiLong_t(0);  }

	static void set_bi_char(BiChar_t)   {}
	static void set_bi_short(BiShort_t) {}
	static void set_bi_int(BiInt_t )    {}
	static void set_bi_long(BiLong_t)    {}

	static void test_type_conversion_bigint_bigint() {
		printf("->TestBigInt::test_type_conversion_bigint_bigint()\n");

		BiChar_t bic;
		BiShort_t bis;
		BiInt_t bii;
		BiLong_t bil;

		set_bi_char(bic);
//		set_bi_char(bis); Is not allowed if sizeof(short) > 1u.
//		set_bi_char(bii); Is not allowed if sizeof(int) > 1u.
//		set_bi_char(bil); Is not allowed if sizeof(long) > 1u.

		set_bi_short(bic);
		set_bi_short(bis);
//		set_bi_short(bii); Is not allowed if sizeof(int) > sizeof(short).
//		set_bi_short(bil); Is not allowed if sizeof(long) > sizeof(short).

		set_bi_int(bic);
		set_bi_int(bis);
		set_bi_int(bii);
//		set_bi_int(bil); Is not allowed if sizeof(long) > sizeof(int).

		set_bi_long(bic);
		set_bi_long(bis);
		set_bi_long(bii);
		set_bi_long(bil);

	}

	template <typename BigType_t, typename Integer>
	void test_min_max(size_t) {
		static_assert(BigType_t::NUMBER_WIDTH_BYTES == sizeof(Integer));
		const auto bmin = BigType_t::min();
		const auto bmax = BigType_t::max();
		Integer imin = bmin;
		Integer imax = bmax;
		ASSERT(imin == std::numeric_limits<Integer>::min());
		ASSERT(imax == std::numeric_limits<Integer>::max());
	}

	template <typename BigType_t, typename Integer>
	void test_bitwise(size_t rounds) {
		static_assert(BigType_t::NUMBER_WIDTH_BYTES == sizeof(Integer));
		UniformIntGen<Integer> gen;
		BigType_t bx;
		BigType_t by;
		BigType_t bop;
		while(rounds--) {
			const Integer ix = gen.next();
			const Integer iy = gen.next();
			Integer iop;
			bx = ix;
			by = iy;
			test_equal<BigType_t, Integer>(bx, ix);
			test_equal<BigType_t, Integer>(by, iy);
			test_equal<BigType_t, Integer>(~bx, ~ix);
			test_equal<BigType_t, Integer>(~by, ~iy);
			test_equal<BigType_t, Integer>((bx | by), (ix | iy));
			test_equal<BigType_t, Integer>((bx & by), (ix & iy));
			test_equal<BigType_t, Integer>((bx ^ by), (ix ^ iy));

			bop = bx;
			iop = ix;
			bop |= by;
			iop |= iy;
			test_equal<BigType_t, Integer>(bop, iop);

			bop = bx;
			iop = ix;
			bop &= by;
			iop &= iy;
			test_equal<BigType_t, Integer>(bop, iop);

			bop = bx;
			iop = ix;
			bop ^= by;
			iop ^= iy;
			test_equal<BigType_t, Integer>(bop, iop);
		}
	}

	template <typename BigType_t, typename Integer>
	void test_shift(size_t rounds) {
		static_assert(BigType_t::NUMBER_WIDTH_BYTES == sizeof(Integer));
		UniformIntGen<Integer> gen;
		UniformIntGen<size_t> gen_shift(0, (sizeof(Integer) * CHAR_BIT) - 1u);
		BigType_t bx;
		BigType_t bop;
		while(rounds--) {
			const Integer ix = gen.next();
			Integer iop;
			const size_t shift = gen_shift.next();
			bx = ix;
			test_equal<BigType_t, Integer>(bx, ix);
			test_equal<BigType_t, Integer>(bx << shift, ix << shift);

			bop = bx;
			iop = ix;
			bop <<= shift;
			iop <<= shift;
			test_equal<BigType_t, Integer>(bop, iop);

			test_equal<BigType_t, Integer>(bx << BigType_t::NUMBER_WIDTH_BITS, BigType_t(0));
			test_equal<BigType_t, Integer>(bx.shr(BigType_t::NUMBER_WIDTH_BITS), BigType_t(0));

			// Since right shift operation of a signed integer produces a platform dependent result,
			// we cannot compare BigInt behaviour with the integral integer types.

//			test_equal<BigType_t, Integer>(bx >> shift, ix >> shift);
//
//			bop = bx;
//			iop = ix;
//			bop >>= shift;
//			iop >>= shift;
//			test_equal<BigType_t, Integer>(bop, iop);
		}
	}

	template <typename BigType_t, typename Integer>
	void test_relational(size_t rounds) {
		static_assert(BigType_t::NUMBER_WIDTH_BYTES == sizeof(Integer));
		UniformIntGen<Integer> gen;
		BigType_t bx;
		BigType_t by;
		while(rounds--) {
			const Integer ix = gen.next();
			const Integer iy = gen.next();
			bx = ix;
			by = iy;
			test_equal<BigType_t, Integer>(bx, ix);
			test_equal<BigType_t, Integer>(by, iy);

			ASSERT((bx == by) == (ix == iy));
			ASSERT((by == bx) == (iy == ix));
			ASSERT((bx != by) == (ix != iy));
			ASSERT((by != bx) == (iy != ix));
			ASSERT((bx > by) == (ix > iy));
			ASSERT((by > bx) == (iy > ix));
			ASSERT((bx < by) == (ix < iy));
			ASSERT((by < bx) == (iy < ix));
			ASSERT((bx >= by) == (ix >= iy));
			ASSERT((by >= bx) == (iy >= ix));
			ASSERT((bx <= by) == (ix <= iy));
			ASSERT((by <= bx) == (iy <= ix));

		}
	}

	template <typename BigType_t, typename Integer>
	void test_add_sub(size_t rounds) {
		static_assert(BigType_t::NUMBER_WIDTH_BYTES == sizeof(Integer));
		UniformIntGen<Integer> gen;
		BigType_t bx;
		BigType_t by;
		BigType_t bop;
		while(rounds--) {
			const Integer ix = gen.next();
			const Integer iy = gen.next();
			Integer iop;
			bx = ix;
			by = iy;
			test_equal<BigType_t, Integer>(bx, ix);
			test_equal<BigType_t, Integer>(by, iy);
			test_equal<BigType_t, Integer>(bx + by, ix + iy);
			test_equal<BigType_t, Integer>(by + bx, iy + ix);
			test_equal<BigType_t, Integer>(bx - by, ix - iy);
			test_equal<BigType_t, Integer>(by - bx, iy - ix);

			bop = bx;
			iop = ix;
			bop += by;
			iop += iy;
			test_equal<BigType_t, Integer>(bop, iop);

			bop = bx;
			iop = ix;
			bop -= by;
			iop -= iy;
			test_equal<BigType_t, Integer>(bop, iop);
		}
	}

	template <typename BigType_t, typename Integer>
	void test_mult(size_t rounds) {
		static_assert(BigType_t::NUMBER_WIDTH_BYTES == sizeof(Integer));
		UniformIntGen<Integer> gen;
		BigType_t bx;
		BigType_t by;
		BigType_t bop;
		while(rounds--) {
			const Integer ix = gen.next();
			const Integer iy = gen.next();
			Integer iop;
			bx = ix;
			by = iy;
//			printf("ix=%s iy=%s\n", std::to_string(ix).c_str(), std::to_string(iy).c_str());
			test_equal<BigType_t, Integer>(bx, ix);
			test_equal<BigType_t, Integer>(by, iy);
			test_equal<BigType_t, Integer>(bx * by, ix * iy);
			test_equal<BigType_t, Integer>(by * bx, iy * ix);

			bop = bx;
			iop = ix;
			bop *= by;
			iop *= iy;
			test_equal<BigType_t, Integer>(bop, iop);
		}
	}

	template <typename BigType_t, typename Integer>
	void test_div_mod(size_t rounds) {
		static_assert(BigType_t::NUMBER_WIDTH_BYTES == sizeof(Integer));
		UniformIntGen<Integer> gen;
		UniformIntGen<Integer> gen_sqrt(-std::sqrt(std::numeric_limits<Integer>::max()), std::sqrt(std::numeric_limits<Integer>::max()));
		BigType_t bx;
		BigType_t by;
		BigType_t bop;
		while(rounds--) {
			const Integer ix = gen.next();
			Integer iy = gen_sqrt.next();
			if(iy == 0) {
				--iy;
			}
			Integer iop;
			bx = ix;
			by = iy;
//			printf("ix=%s / iy=%s 0x%X / 0x%x\n", std::to_string(ix).c_str(), std::to_string(iy).c_str(), ix, iy);
			test_equal<BigType_t, Integer>(bx, ix);
			test_equal<BigType_t, Integer>(by, iy);
			test_equal<BigType_t, Integer>(bx / by, ix / iy);
			test_equal<BigType_t, Integer>(bx % by, ix % iy);

			bop = bx;
			iop = ix;
			bop /= by;
			iop /= iy;
			test_equal<BigType_t, Integer>(bop, iop);

			bop = bx;
			iop = ix;
			bop %= by;
			iop %= iy;
			test_equal<BigType_t, Integer>(bop, iop);
		}
	}

	template <typename BigType_t, typename Integer>
	void test_sign_change(size_t rounds) {
		static_assert(BigType_t::NUMBER_WIDTH_BYTES == sizeof(Integer));
		UniformIntGen<Integer> gen;
		BigType_t bx;
		while(rounds--) {
			const Integer ix = gen.next();
			bx = ix;
			test_equal<BigType_t, Integer>(bx, ix);
			test_equal<BigType_t, Integer>(bx.abs(), std::abs(ix));
			test_equal<BigType_t, Integer>(-bx, -ix);
		}
	}

	template <typename BigType_t, typename Integer>
	static void test_equal(const BigType_t& big_int, const Integer& int_ref) {
		Integer int_out = big_int;
		ASSERT(int_out == int_ref);
		ASSERT(big_int == int_ref);
	}

	void test_constexpr_values() {
		printf("->TestBigInt::test_constexpr_values()\n");
		static constexpr BiChar_t BI_CHAR = 'A';
		static constexpr BiShort_t BI_SHORT = short(-1);
		static constexpr BiInt_t BI_INT = 0x7F;
		static constexpr BiLong_t BI_LONG = 0x0ABC;

		static_assert((BI_CHAR + BiChar_t(1)) == 'B');
		static_assert(BI_SHORT + BiShort_t(1) == BiShort_t(0));
		static_assert((BI_INT - 1) == 0x7E);
		static_assert((BI_INT * 16) == 0x7F0);
		static_assert((BI_INT / 16) == 0x7);
		static_assert((BI_INT ^ 0x7F) == 0);
		static_assert((BI_INT | 0x80) == 0xFF);
		static_assert((BI_INT & 0x80) == 0);
		static_assert(BI_LONG >> 4u == 0xAB);
		static_assert(BI_LONG << 4u == 0xABC0);
	}

	// -----------------------------------------------------------------------
	// Performance test.
	// -----------------------------------------------------------------------
	template <size_t CHUNK_WIDTH>
	void test_performance_all(size_t rounds) {
		test_performance<BigInt<signed char, CHUNK_WIDTH>>(rounds);
		test_performance<BigInt<signed short, CHUNK_WIDTH>>(rounds);
		test_performance<BigInt<signed int, CHUNK_WIDTH>>(rounds);
		test_performance<BigInt<signed long, CHUNK_WIDTH>>(rounds);
		test_performance<BigInt<signed long long, CHUNK_WIDTH>>(rounds);
	}

	template <typename BigType_t>
	void test_performance(size_t rounds) {
		printf(
			"Performance test BigInt<%zu, %zu>. %zu bit signed integer.\n",
			BigType_t::CHUNK_WIDTH_BITS,
			BigType_t::NUMBER_WIDTH_CHUNKS,
			BigType_t::NUMBER_WIDTH_BITS
		);

		size_t result = 0;
		result += test_performance_add_sub<BigType_t>(rounds);
		result += test_performance_mult<BigType_t>(rounds);
		result += test_performance_div<BigType_t>(rounds);

		printf("Performance test finishes with checksum %zu.\n\n", result);
	}

	template <typename BigType_t>
	size_t test_performance_add_sub(const size_t rounds) {
		size_t result = 0;
		Perf perf_add("add()          ");
		Perf perf_sub("sub()          ");

		UniformBigIntGen<BigType_t> gen;

		std::vector<BigType_t> x_ref;
		std::vector<BigType_t> x;
		std::vector<BigType_t> y;

		x_ref.resize(rounds);
		x.resize(rounds);
		y.resize(rounds);

		for(size_t idx = 0; idx < rounds; ++idx) {
			x[idx] = gen.next();
			y[idx] = gen.next();
			x_ref[idx] = x[idx];
		}

		perf_add.start();
		for(size_t idx = 0; idx < rounds; ++idx) {
			x[idx] += y[idx];
		}
		perf_add.stop();

		perf_sub.start();
		for(size_t idx = 0; idx < rounds; ++idx) {
			x[idx] -= y[idx];
		}
		perf_sub.stop();

		for(size_t idx = 0; idx < rounds; ++idx) {
			result += x[idx].msbit();
		}

		perf_add.print(rounds);
		perf_sub.print(rounds);

		return result;
	}

	template <typename BigType_t>
	size_t test_performance_mult(const size_t rounds) {
		size_t result = 0;
		Perf perf_mult("mult()         ");

		UniformBigIntGen<BigType_t> gen;

		std::vector<BigType_t> x_ref;
		std::vector<BigType_t> x;
		std::vector<BigType_t> y;

		x_ref.resize(rounds);
		x.resize(rounds);
		y.resize(rounds);

		for(size_t idx = 0; idx < rounds; ++idx) {
			x[idx] = gen.next();
			do {
				y[idx] = (gen.next() >> (BigType_t::NUMBER_WIDTH_BITS / 2u));
			} while(y[idx] == BigType_t(0));
			x_ref[idx] = x[idx];
		}

		perf_mult.start();
		for(size_t idx = 0; idx < rounds; ++idx) {
			x[idx] *= y[idx];
		}
		perf_mult.stop();

		for(size_t idx = 0; idx < rounds; ++idx) {
			result += x[idx].msbit();
		}

		perf_mult.print(rounds);

		return result;
	}

	template <typename BigType_t>
	size_t test_performance_div(const size_t rounds) {
		size_t result = 0;
		Perf perf_div("div()          ");

		UniformBigIntGen<BigType_t> gen;

		std::vector<BigType_t> x_ref;
		std::vector<BigType_t> x;
		std::vector<BigType_t> y;

		x_ref.resize(rounds);
		x.resize(rounds);
		y.resize(rounds);

		for(size_t idx = 0; idx < rounds; ++idx) {
			x[idx] = gen.next();
			do {
				y[idx] = (gen.next() >> (BigType_t::NUMBER_WIDTH_BITS / 2u));
			} while(y[idx] == BigType_t(0));
			x_ref[idx] = x[idx];
		}

		perf_div.start();
		for(size_t idx = 0; idx < rounds; ++idx) {
			x[idx] /= y[idx];
		}
		perf_div.stop();

		for(size_t idx = 0; idx < rounds; ++idx) {
			result += x[idx].msbit();
		}

		perf_div.print(rounds);

		return result;
	}


};
