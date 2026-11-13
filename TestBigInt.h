#pragma once

#include "BigInt.h"
#include "test_environment.h"

#include <vector>
#include <cstdlib>
#include <climits>
#include <limits>
#include <cstdio>
#include <chrono>

class TestBigInt {

	static_assert(sizeof(signed long long) > 1u, "The autotest is useless!");

	static constexpr size_t AUTOTEST_ROUNDS = 100000u;
	static constexpr size_t PERFORMANCE_TEST_ROUNDS = 100000u;

	template <typename T, size_t BYTES_MAX = sizeof(T), typename std::enable_if<(BYTES_MAX <= sizeof(T)), int>::type = 0>
	struct RndIntGen {
		static_assert(RAND_MAX > 0xFF, "rand() range is narrower than one byte.");

		T next() {
			T result = T(0);
			size_t bytes = (std::abs(rand()) % BYTES_MAX) + 1u;
			while(bytes--) {
				result <<= CHAR_BIT;
				result += T(rand() & 0xFF);
			}
			if(rand() > (RAND_MAX / 2)) {
				result = -result;
			}
			return result;
		}
	};

	template <size_t WIDTH>
	using NotWiderInteger =
		typename std::conditional<(sizeof(signed long long) <= WIDTH), signed long long,
			typename std::conditional<(sizeof(signed long) <= WIDTH), signed long,
				typename std::conditional<(sizeof(signed int) <= WIDTH), signed int,
					typename std::conditional<(sizeof(signed short) <= WIDTH), signed short,
						typename std::conditional<(sizeof(signed char) <= WIDTH), signed char, void>::type
					>::type
				>::type
			>::type
		>::type;

	using BiChar_t = BigInt<CHAR_BIT>;
	using BiShort_t = BigInt<sizeof(signed short) * CHAR_BIT>;
	using BiInt_t = BigInt<sizeof(signed int) * CHAR_BIT>;
	using BiLong_t = BigInt<sizeof(signed long) * CHAR_BIT>;
	using BiLongLong_t = BigInt<sizeof(signed long long) * CHAR_BIT>;

#define ITERATE_BIGINT_AND_EQUAL_WIDTH_INTEGER(func, rounds) { \
	printf("->TestBigInt::"#func"()\n"); \
	func<BiChar_t, signed char>(rounds); \
	func<BiShort_t, signed short>(rounds); \
	func<BiInt_t, signed int>(rounds); \
	func<BiLong_t, signed long>(rounds);\
	func<BiLongLong_t, signed long long>(rounds); \
}

#define ITERATE_BIGINT_AND_NOT_WIDER_INTEGER(func, rounds) { \
	printf("->TestBigInt::"#func"()\n");                        \
    func<BigInt<(1u * CHAR_BIT)>, NotWiderInteger<1u>>(rounds); \
	func<BigInt<(2u * CHAR_BIT)>, NotWiderInteger<2u>>(rounds); \
	func<BigInt<(3u * CHAR_BIT)>, NotWiderInteger<3u>>(rounds); \
	func<BigInt<(4u * CHAR_BIT)>, NotWiderInteger<4u>>(rounds); \
	func<BigInt<(5u * CHAR_BIT)>, NotWiderInteger<5u>>(rounds); \
	func<BigInt<(6u * CHAR_BIT)>, NotWiderInteger<6u>>(rounds); \
	func<BigInt<(7u * CHAR_BIT)>, NotWiderInteger<7u>>(rounds); \
	func<BigInt<(8u * CHAR_BIT)>, NotWiderInteger<8u>>(rounds); \
	func<BigInt<(9u * CHAR_BIT)>, NotWiderInteger<9u>>(rounds); \
	func<BigInt<(10u * CHAR_BIT)>, NotWiderInteger<10u>>(rounds); \
	func<BigInt<(11u * CHAR_BIT)>, NotWiderInteger<11u>>(rounds); \
	func<BigInt<(12u * CHAR_BIT)>, NotWiderInteger<12u>>(rounds); \
	func<BigInt<(13u * CHAR_BIT)>, NotWiderInteger<13u>>(rounds); \
	func<BigInt<(14u * CHAR_BIT)>, NotWiderInteger<14u>>(rounds); \
	func<BigInt<(15u * CHAR_BIT)>, NotWiderInteger<15u>>(rounds); \
	func<BigInt<(16u * CHAR_BIT)>, NotWiderInteger<16u>>(rounds); \
	func<BigInt<(17u * CHAR_BIT)>, NotWiderInteger<17u>>(rounds); \
}


#define ITERATE_BIGINT_AND_WIDER_BIGINT(func, rounds) { \
printf("->TestBigInt::"#func"()\n"); \
	func<BigInt<CHAR_BIT>, BigInt<CHAR_BIT * 2u>>(rounds); \
	func<BigInt<CHAR_BIT>, BigInt<CHAR_BIT * 3u>>(rounds); \
	func<BigInt<CHAR_BIT>, BigInt<CHAR_BIT * 4u>>(rounds); \
	func<BigInt<CHAR_BIT * 2u>, BigInt<CHAR_BIT * 3u>>(rounds); \
	func<BigInt<CHAR_BIT * 2u>, BigInt<CHAR_BIT * 4u>>(rounds); \
	func<BigInt<CHAR_BIT * 2u>, BigInt<CHAR_BIT * 5u>>(rounds); \
	func<BigInt<CHAR_BIT * 2u>, BigInt<CHAR_BIT * 8u>>(rounds); \
	func<BigInt<CHAR_BIT * 7u>, BigInt<CHAR_BIT * 8u>>(rounds); \
	func<BigInt<CHAR_BIT * 7u>, BigInt<CHAR_BIT * 10u>>(rounds); \
	func<BigInt<CHAR_BIT * 7u>, BigInt<CHAR_BIT * 13u>>(rounds); \
	func<BigInt<CHAR_BIT * 7u>, BigInt<CHAR_BIT * 64u>>(rounds); \
}
public:

	TestBigInt() {
		srand(0);

		// Type conversion tests.
		ITERATE_BIGINT_AND_EQUAL_WIDTH_INTEGER(test_load_store_int, AUTOTEST_ROUNDS);
		ITERATE_BIGINT_AND_NOT_WIDER_INTEGER(test_load_store_int, AUTOTEST_ROUNDS);
		ITERATE_BIGINT_AND_WIDER_BIGINT(test_load_store_big_int, AUTOTEST_ROUNDS);
		test_type_conversion_int_to_bigint();

		// Test BigInt arithmetic operations comparing the results with the equal sized integral integer types.
		ITERATE_BIGINT_AND_EQUAL_WIDTH_INTEGER(test_cmp_int_min_max, AUTOTEST_ROUNDS);
		ITERATE_BIGINT_AND_EQUAL_WIDTH_INTEGER(test_cmp_int_bitwise, AUTOTEST_ROUNDS);
		ITERATE_BIGINT_AND_EQUAL_WIDTH_INTEGER(test_cmp_int_shift, AUTOTEST_ROUNDS);
		ITERATE_BIGINT_AND_EQUAL_WIDTH_INTEGER(test_cmp_int_relational, AUTOTEST_ROUNDS);
		ITERATE_BIGINT_AND_EQUAL_WIDTH_INTEGER(test_cmp_int_add_sub, AUTOTEST_ROUNDS);
		ITERATE_BIGINT_AND_EQUAL_WIDTH_INTEGER(test_cmp_int_mult, AUTOTEST_ROUNDS);
		ITERATE_BIGINT_AND_EQUAL_WIDTH_INTEGER(test_cmp_int_div_mod, AUTOTEST_ROUNDS);
		ITERATE_BIGINT_AND_EQUAL_WIDTH_INTEGER(test_cmp_int_sign_change, AUTOTEST_ROUNDS);

		ITERATE_BIGINT_AND_NOT_WIDER_INTEGER(test_cmp_int_bitwise, AUTOTEST_ROUNDS);
		ITERATE_BIGINT_AND_NOT_WIDER_INTEGER(test_cmp_int_shift, AUTOTEST_ROUNDS);
		ITERATE_BIGINT_AND_NOT_WIDER_INTEGER(test_cmp_int_relational, AUTOTEST_ROUNDS);
		ITERATE_BIGINT_AND_NOT_WIDER_INTEGER(test_cmp_int_add_sub, AUTOTEST_ROUNDS);
		ITERATE_BIGINT_AND_NOT_WIDER_INTEGER(test_cmp_int_mult, AUTOTEST_ROUNDS);
		ITERATE_BIGINT_AND_NOT_WIDER_INTEGER(test_cmp_int_div_mod, AUTOTEST_ROUNDS);
		ITERATE_BIGINT_AND_NOT_WIDER_INTEGER(test_cmp_int_sign_change, AUTOTEST_ROUNDS);

		test_constexpr_values();

		test_performance_header("Built-in integers");
		test_performance<signed char>(PERFORMANCE_TEST_ROUNDS);
		test_performance<signed short>(PERFORMANCE_TEST_ROUNDS);
		test_performance<signed int>(PERFORMANCE_TEST_ROUNDS);
		test_performance<signed long>(PERFORMANCE_TEST_ROUNDS);
		test_performance<signed long long>(PERFORMANCE_TEST_ROUNDS);

		test_performance_header("BigInt<>");
		test_performance<BigInt<CHAR_BIT * 1u>>(PERFORMANCE_TEST_ROUNDS);
		test_performance<BigInt<CHAR_BIT * 2u>>(PERFORMANCE_TEST_ROUNDS);
		test_performance<BigInt<CHAR_BIT * 3u>>(PERFORMANCE_TEST_ROUNDS);
		test_performance<BigInt<CHAR_BIT * 4u>>(PERFORMANCE_TEST_ROUNDS);
		test_performance<BigInt<CHAR_BIT * 5u>>(PERFORMANCE_TEST_ROUNDS);
		test_performance<BigInt<CHAR_BIT * 6u>>(PERFORMANCE_TEST_ROUNDS);
		test_performance<BigInt<CHAR_BIT * 7u>>(PERFORMANCE_TEST_ROUNDS);
		test_performance<BigInt<CHAR_BIT * 8u>>(PERFORMANCE_TEST_ROUNDS);
		test_performance<BigInt<CHAR_BIT * 10u>>(PERFORMANCE_TEST_ROUNDS);
		test_performance<BigInt<CHAR_BIT * 15u>>(PERFORMANCE_TEST_ROUNDS);
		test_performance<BigInt<CHAR_BIT * 16u>>(PERFORMANCE_TEST_ROUNDS);
		test_performance<BigInt<CHAR_BIT * 24u>>(PERFORMANCE_TEST_ROUNDS);
		test_performance<BigInt<CHAR_BIT * 36u>>(PERFORMANCE_TEST_ROUNDS);
		test_performance<BigInt<CHAR_BIT * 48u>>(PERFORMANCE_TEST_ROUNDS);
		test_performance<BigInt<CHAR_BIT * 256u>>(PERFORMANCE_TEST_ROUNDS);
	}

	template <typename BigType_t, typename Integer>
	void test_load_store_int(size_t rounds) {
		static_assert(sizeof(BigType_t) >= sizeof(Integer), "Size assert");
		RndIntGen<Integer> gen;
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
		const auto int_out = static_cast<Integer>(bint);
		ASSERT(int_out == int_ref);
	}

	template <typename A, typename B>
	void test_load_store_big_int(size_t rounds) {
		static_assert(A::NUMBER_WIDTH_BYTES <= B::NUMBER_WIDTH_BYTES, "Size assert");
		RndIntGen<A> gen_a;

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
		static_assert(A::NUMBER_WIDTH_BYTES <= B::NUMBER_WIDTH_BYTES, "Size assert");
		B b_in(a_in);
		A a_out(b_in);
		ASSERT(a_in == a_out);
		ASSERT(a_out == a_in);

		b_in = B(a_in);
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
	static signed long long  get_long_long()   { return 0; }

	static void test_type_conversion_int_to_bigint() {
		printf("->TestBigInt::test_type_conversion_int_to_bigint()\n");

		// Wider to narrower. Explicit conversion.
		BiChar_t bi_char(get_long());
		BiShort_t bi_short(get_long());
		BiInt_t bi_int(get_long());
		BiLong_t bi_long(get_long());
		BiLongLong_t bi_long_long(get_long_long());

		// Implicit conversion. Narrower to wider.
		bi_char = get_char();
		bi_short = get_char();
		bi_int = get_char();
		bi_int = get_char();
		bi_long_long = get_char();

//		bi_char = get_short(); Is not allowed if sizeof(short) > 1u.
		bi_short = get_short();
		bi_int = get_short();
		bi_long = get_short();
		bi_long_long = get_short();

//		bi_char = get_int(); Is not allowed if sizeof(int) > 1u.
//		bi_short = get_int(); Is not allowed if sizeof(int) > sizeof(short).
		bi_int = get_int();
		bi_long = get_int();
		bi_long_long = get_int();

//		bi_char = get_long(); Is not allowed if sizeof(long) > 1u.
//		bi_short = get_long(); Is not allowed if sizeof(long) > sizeof(short).
//		bi_int = get_long(); Is not allowed if sizeof(long) > sizeof(int).
		bi_long = get_long();
		bi_long_long = get_long();

//		bi_char = get_long_long(); Is not allowed if sizeof(long long) > 1u.
//		bi_short = get_long_long(); Is not allowed if sizeof(long long) > sizeof(short).
//		bi_int = get_long_long(); Is not allowed if sizeof(long long) > sizeof(int).
//		bi_long = get_long_long(); Is not allowed if sizeof(long long) > sizeof(long).
		bi_long_long = get_long_long();

		// Explicit conversion.
		bi_char = BiChar_t(get_short());
		bi_char = BiChar_t(get_int());
		bi_char = BiChar_t(get_long());
		bi_char = BiChar_t(get_long_long());
		bi_short = BiShort_t(get_int());
		bi_short = BiShort_t(get_long());
		bi_short = BiShort_t(get_long_long());
		bi_int = BiInt_t(get_long());
		bi_int = BiInt_t(get_long_long());
		bi_long = BiLong_t(get_long_long());

		bi_char = static_cast<BiChar_t>(get_short());
		bi_char = static_cast<BiChar_t>(get_int());
		bi_char = static_cast<BiChar_t>(get_long());
		bi_char = static_cast<BiChar_t>(get_long_long());
		bi_short = static_cast<BiShort_t>(get_int());
		bi_short = static_cast<BiShort_t>(get_long());
		bi_short = static_cast<BiShort_t>(get_long_long());
		bi_int = static_cast<BiInt_t>(get_long());
		bi_int = static_cast<BiInt_t>(get_long_long());
		bi_long = static_cast<BiLong_t>(get_long_long());

	}

	template <typename BigType_t, typename Integer>
	void test_cmp_int_min_max(size_t) {
		static_assert(BigType_t::NUMBER_WIDTH_BYTES == sizeof(Integer), "Size assert");
		const auto bmin = BigType_t::min();
		const auto bmax = BigType_t::max();
		const auto imin = static_cast<Integer>(bmin);
		const auto imax = static_cast<Integer>(bmax);
		ASSERT(imin == std::numeric_limits<Integer>::min());
		ASSERT(imax == std::numeric_limits<Integer>::max());
	}

	template <typename BigType_t, typename Integer>
	void test_cmp_int_bitwise(size_t rounds) {
		static_assert(BigType_t::NUMBER_WIDTH_BYTES >= sizeof(Integer), "Size assert");
		RndIntGen<Integer> gen;
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

			// X vx X
			test_equal<BigType_t, Integer>((bx | bx), (ix | ix));
			test_equal<BigType_t, Integer>((bx | ix), (ix | ix));
			test_equal<BigType_t, Integer>((ix | bx), (ix | ix));

			test_equal<BigType_t, Integer>((bx & bx), (ix & ix));
			test_equal<BigType_t, Integer>((bx & ix), (ix & ix));
			test_equal<BigType_t, Integer>((ix & bx), (ix & ix));

			test_equal<BigType_t, Integer>((bx ^ bx), (ix ^ ix));
			test_equal<BigType_t, Integer>((bx ^ ix), (ix ^ ix));
			test_equal<BigType_t, Integer>((ix ^ bx), (ix ^ ix));

			// X vx Y
			test_equal<BigType_t, Integer>((bx & by), (ix & iy));
			test_equal<BigType_t, Integer>((by & bx), (iy & ix));
			test_equal<BigType_t, Integer>((bx & iy), (ix & iy));
			test_equal<BigType_t, Integer>((ix & by), (ix & iy));

			test_equal<BigType_t, Integer>((bx | by), (ix | iy));
			test_equal<BigType_t, Integer>((by | bx), (iy | ix));
			test_equal<BigType_t, Integer>((bx | iy), (ix | iy));
			test_equal<BigType_t, Integer>((ix | by), (ix | iy));

			test_equal<BigType_t, Integer>((bx ^ by), (ix ^ iy));
			test_equal<BigType_t, Integer>((by ^ bx), (iy ^ ix));
			test_equal<BigType_t, Integer>((bx ^ iy), (ix ^ iy));
			test_equal<BigType_t, Integer>((ix ^ by), (ix ^ iy));

			// X vx X
			bop = bx;
			iop = ix;
			bop |= bop;
			iop |= iop;
			test_equal<BigType_t, Integer>(bop, iop);

			bop = bx;
			iop = ix;
			bop &= bop;
			iop &= iop;
			test_equal<BigType_t, Integer>(bop, iop);

			bop = bx;
			iop = ix;
			bop ^= bop;
			iop ^= iop;
			test_equal<BigType_t, Integer>(bop, iop);

			// X vx Y
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
	void test_cmp_int_shift(size_t rounds) {
		static_assert(BigType_t::NUMBER_WIDTH_BYTES >= sizeof(Integer), "Size assert");
		RndIntGen<Integer> gen;
		RndIntGen<size_t> gen_shift;
		BigType_t bx;
		BigType_t bop;
		while(rounds--) {
			const Integer ix = gen.next();
			Integer iop;
			const size_t shift = (gen_shift.next() % (sizeof(Integer) * CHAR_BIT - 1u));
			bx = ix;
			test_equal<BigType_t, Integer>(bx, ix);
			test_equal<BigType_t, Integer>(bx << shift, ix << shift);

			bop = bx;
			iop = ix;
			bop <<= shift;
			iop <<= shift;
			test_equal<BigType_t, Integer>(bop, iop);

			test_equal<BigType_t, Integer>(bx << BigType_t::NUMBER_WIDTH_BITS, 0);
			test_equal<BigType_t, Integer>(bx.shr(BigType_t::NUMBER_WIDTH_BITS), 0);

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
	void test_cmp_int_relational(size_t rounds) {
		static_assert(BigType_t::NUMBER_WIDTH_BYTES >= sizeof(Integer), "Size assert");
		RndIntGen<Integer> gen;
		BigType_t bx;
		BigType_t by;
		while(rounds--) {
			const Integer ix = gen.next();
			const Integer iy = gen.next();
			bx = ix;
			by = iy;
			test_equal<BigType_t, Integer>(bx, ix);
			test_equal<BigType_t, Integer>(by, iy);

			// X vs X
			ASSERT(bx == bx);
			ASSERT(bx <= bx);
			ASSERT(bx >= bx);
			ASSERT(not (bx != bx));
			ASSERT(not (bx > bx));
			ASSERT(not (bx < bx));

			// X vs Y
			ASSERT((bx == by) == (ix == iy));
			ASSERT((by == bx) == (iy == ix));
			ASSERT((bx == iy) == (ix == iy));
			ASSERT((ix == by) == (ix == iy));

			ASSERT((bx != by) == (ix != iy));
			ASSERT((by != bx) == (iy != ix));
			ASSERT((bx != iy) == (ix != iy));
			ASSERT((ix != by) == (ix != iy));

			ASSERT((bx > by) == (ix > iy));
			ASSERT((by > bx) == (iy > ix));
			ASSERT((bx > iy) == (ix > iy));
			ASSERT((ix > by) == (ix > iy));

			ASSERT((bx < by) == (ix < iy));
			ASSERT((by < bx) == (iy < ix));
			ASSERT((bx < iy) == (ix < iy));
			ASSERT((ix < by) == (ix < iy));

			ASSERT((bx >= by) == (ix >= iy));
			ASSERT((by >= bx) == (iy >= ix));
			ASSERT((bx >= iy) == (ix >= iy));
			ASSERT((ix >= by) == (ix >= iy));

			ASSERT((bx <= by) == (ix <= iy));
			ASSERT((by <= bx) == (iy <= ix));
			ASSERT((bx <= iy) == (ix <= iy));
			ASSERT((ix <= by) == (ix <= iy));

		}
	}

	template <typename BigType_t, typename Integer>
	void test_cmp_int_add_sub(size_t rounds) {
		static_assert(BigType_t::NUMBER_WIDTH_BYTES >= sizeof(Integer), "Size assert");

		test_equal(BigType_t(-1), Integer(-1));
		test_equal(BigType_t(0), Integer(0));
		test_equal(BigType_t(1), Integer(1));

		test_equal(BigType_t::max() + Integer(1), BigType_t::min());
		test_equal(BigType_t::min() - Integer(1), BigType_t::max());
		test_equal(++BigType_t::max(), BigType_t::min());
		test_equal(--BigType_t::min(), BigType_t::max());

		RndIntGen<Integer> gen;
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

			// inc
			bop = bx;
			iop = ix;
			test_equal<BigType_t, Integer>(++bop, ++iop);
			test_equal<BigType_t, Integer>(bop, iop);

			bop = bx;
			iop = ix;
			test_equal<BigType_t, Integer>(bop++, iop++);
			test_equal<BigType_t, Integer>(bop, iop);

			// dec
			bop = bx;
			iop = ix;
			test_equal<BigType_t, Integer>(--bop, --iop);
			test_equal<BigType_t, Integer>(bop, iop);

			bop = bx;
			iop = ix;
			test_equal<BigType_t, Integer>(bop--, iop--);
			test_equal<BigType_t, Integer>(bop, iop);

			// X vs X
			// add
			test_equal<BigType_t, Integer>(bx + bx, ix + ix);
			test_equal<BigType_t, Integer>(bx + ix, ix + ix);
			test_equal<BigType_t, Integer>(ix + bx, ix + ix);

			bop = bx;
			iop = ix;
			bop += bop;
			iop += iop;
			test_equal<BigType_t, Integer>(bop, iop);

			// sub
			test_equal<BigType_t, Integer>(bx - bx, ix - ix);
			test_equal<BigType_t, Integer>(bx - ix, ix - ix);
			test_equal<BigType_t, Integer>(ix - bx, ix - ix);

			bop = bx;
			iop = ix;
			bop -= bop;
			iop -= iop;
			test_equal<BigType_t, Integer>(bop, iop);

			// X vs Y
			// add
			test_equal<BigType_t, Integer>(bx + by, ix + iy);
			test_equal<BigType_t, Integer>(by + bx, iy + ix);
			test_equal<BigType_t, Integer>(bx + iy, ix + iy);
			test_equal<BigType_t, Integer>(ix + by, ix + iy);

			test_equal<BigType_t, Integer>(bx + bx, ix + ix);
			test_equal<BigType_t, Integer>(by + by, iy + iy);

			bop = bx;
			iop = ix;
			bop += by;
			iop += iy;
			test_equal<BigType_t, Integer>(bop, iop);

			// sub
			test_equal<BigType_t, Integer>(bx - by, ix - iy);
			test_equal<BigType_t, Integer>(by - bx, iy - ix);
			test_equal<BigType_t, Integer>(bx - iy, ix - iy);
			test_equal<BigType_t, Integer>(ix - by, ix - iy);

			test_equal<BigType_t, Integer>(bx - bx, ix - ix);
			test_equal<BigType_t, Integer>(by - by, iy - iy);

			bop = bx;
			iop = ix;
			bop -= by;
			iop -= iy;
			test_equal<BigType_t, Integer>(bop, iop);

		}
	}

	template <typename BigType_t, typename Integer>
	void test_cmp_int_mult(size_t rounds) {
		static_assert(BigType_t::NUMBER_WIDTH_BYTES >= sizeof(Integer), "Size assert");
		RndIntGen<Integer> gen;
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

			test_equal<BigType_t, Integer>(bx * BigType_t(0), Integer(0));
			test_equal<BigType_t, Integer>(by * BigType_t(0), Integer(0));
			test_equal<BigType_t, Integer>(bx * BigType_t(1), ix);
			test_equal<BigType_t, Integer>(by * BigType_t(1), iy);
			test_equal<BigType_t, Integer>(bx * BigType_t(-1), -ix);
			test_equal<BigType_t, Integer>(by * BigType_t(-1), -iy);

			test_equal<BigType_t, Integer>(bx, ix);
			test_equal<BigType_t, Integer>(by, iy);

			// X vs X
			test_equal<BigType_t, Integer>(bx * bx, ix * ix);
			test_equal<BigType_t, Integer>(bx * ix, ix * ix);
			test_equal<BigType_t, Integer>(ix * bx, ix * ix);

			// X vs Y
			test_equal<BigType_t, Integer>(bx * by, ix * iy);
			test_equal<BigType_t, Integer>(by * bx, iy * ix);
			test_equal<BigType_t, Integer>(bx * iy, ix * iy);
			test_equal<BigType_t, Integer>(ix * by, ix * iy);

			test_equal<BigType_t, Integer>(bx * bx, ix * ix);
			test_equal<BigType_t, Integer>(by * by, iy * iy);

			bop = bx;
			iop = ix;
			bop *= by;
			iop *= iy;
			test_equal<BigType_t, Integer>(bop, iop);
		}
	}

	template <typename BigType_t, typename Integer>
	void test_cmp_int_div_mod(size_t rounds) {
		static_assert(BigType_t::NUMBER_WIDTH_BYTES >= sizeof(Integer), "Size assert");
		RndIntGen<Integer> gen;
		RndIntGen<Integer, (sizeof(Integer) / 2u + 1u)> gen_sqrt;

		BigType_t bx;
		BigType_t by;
		BigType_t bop;
		while(rounds--) {
			const Integer ix = gen.next();
			Integer iy = gen_sqrt.next();
			while(iy == 0) {
				iy = gen_sqrt.next();
			}
			Integer iop;
			bx = ix;
			by = iy;
//			printf("ix=%s / iy=%s 0x%X / 0x%x\n", std::to_string(ix).c_str(), std::to_string(iy).c_str(), ix, iy);
			test_equal<BigType_t, Integer>(bx / BigType_t(1), ix);
			test_equal<BigType_t, Integer>(by / BigType_t(1), iy);
			test_equal<BigType_t, Integer>(bx / BigType_t(-1), -ix);
			test_equal<BigType_t, Integer>(by / BigType_t(-1), -iy);
			test_equal<BigType_t, Integer>(by / by, Integer(1));
			test_equal<BigType_t, Integer>((-by) / (-by), Integer(1));

			test_equal<BigType_t, Integer>(bx, ix);
			test_equal<BigType_t, Integer>(by, iy);

			// Y vs Y
			test_equal<BigType_t, Integer>(by / by, iy / iy);
			test_equal<BigType_t, Integer>(by / iy, iy / iy);
			test_equal<BigType_t, Integer>(iy / by, iy / iy);

			// X vs Y
			test_equal<BigType_t, Integer>(bx / by, ix / iy);
			test_equal<BigType_t, Integer>(bx / iy, ix / iy);
			test_equal<BigType_t, Integer>(ix / by, ix / iy);
			test_equal<BigType_t, Integer>(by / by, iy / iy);

			test_equal<BigType_t, Integer>(bx % by, ix % iy);
			test_equal<BigType_t, Integer>(bx % iy, ix % iy);
			test_equal<BigType_t, Integer>(ix % by, ix % iy);
			test_equal<BigType_t, Integer>(by % by, iy % iy);

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
	void test_cmp_int_sign_change(size_t rounds) {
		static_assert(BigType_t::NUMBER_WIDTH_BYTES >= sizeof(Integer), "Size assert");

		RndIntGen<Integer> gen;
		BigType_t bx;
		while(rounds--) {
			const Integer ix = gen.next();
			bx = ix;
			test_equal<BigType_t, Integer>(bx, ix);
			test_equal<BigType_t, Integer>(bx.abs(), std::abs(ix));
			test_equal<BigType_t, Integer>(-bx, -ix);
		}
	}

	template <typename BigType_t, typename Integer, typename std::enable_if<(sizeof(BigType_t) == sizeof(Integer)), int>::type = 0>
	static void test_equal(const BigType_t& big_int, const Integer& int_ref) {
		const auto int_out = static_cast<Integer>(big_int);
		ASSERT(int_out == int_ref);
		ASSERT(big_int == int_ref);
		ASSERT(big_int == int_out);
		ASSERT(int_ref == big_int);
		ASSERT(int_out == big_int);
	}

	template <typename BigType_t, typename Integer, typename std::enable_if<(sizeof(BigType_t) > sizeof(Integer)), int>::type = 0>
	static void test_equal(const BigType_t& big_int, const Integer& int_ref) {
		const auto int_out = static_cast<Integer>(big_int);
		ASSERT(int_out == int_ref);
	}

	void test_constexpr_values() {
#if __cplusplus >= 201402L
		printf("->TestBigInt::test_constexpr_values()\n");
		static constexpr BiChar_t BI_CHAR = 'A';
		static constexpr BiShort_t BI_SHORT = short(-1);
		static constexpr BiInt_t BI_INT = 0x7F;
		static constexpr BiLong_t BI_LONG = 0x0ABC;

		static_assert((BI_CHAR + BiChar_t(1)) == 'B', "_");
		static_assert((BI_CHAR | 'A') == 'A', "_");
		static_assert((BI_CHAR & 'A') == 'A', "_");
		static_assert((BI_CHAR ^ 'A') == BiChar_t(0), "_");
		static_assert(BI_SHORT + BiShort_t(1) == BiShort_t(0), "_");
		static_assert((BI_INT - 1) == 0x7E, "_");
		static_assert((BI_INT * 16) == 0x7F0, "_");
		static_assert((BI_INT / 16) == 0x7, "_");
		static_assert((BI_INT ^ 0x7F) == 0, "_");
		static_assert((BI_INT | 0x80) == 0xFF, "_");
		static_assert((BI_INT & 0x80) == 0, "_");
		static_assert((BI_LONG >> 4u) == 0xAB, "_");
		static_assert((BI_LONG << 4u) == 0xABC0, "_");
#endif
	}

	// -----------------------------------------------------------------------
	// Performance test.
	// -----------------------------------------------------------------------
	void test_performance_header(const char* name) {
		printf("\n\n| %-98s |\n", name);
		printf("| Width bits |            Addition |        Substraction |      Multiplication |            Division |\n");
	}

	template <typename BigType_t>
	void test_performance(size_t rounds) {
		srand(0);
		double delay_add_ns, delay_sub_ns, delay_mult_ns, delay_div_ns;
		size_t result = 0;
		result += test_performance_add_sub<BigType_t>(rounds, delay_add_ns, delay_sub_ns);
		result += test_performance_mult<BigType_t>(rounds, delay_mult_ns);
		result += test_performance_div<BigType_t>(rounds, delay_div_ns);
		printf("| %10zu | %16.4f ns | %16.4f ns | %16.4f ns | %16.4f ns |", sizeof(BigType_t) * CHAR_BIT, delay_add_ns, delay_sub_ns, delay_mult_ns, delay_div_ns);
		printf("    checksum [%zu]\n", result);
	}

	template <typename BigType_t>
	size_t test_performance_add_sub(const size_t rounds, double& delay_add_ns, double& delay_sub_ns) {
		size_t result = 0;

		RndIntGen<BigType_t> gen;

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

		std::chrono::steady_clock::time_point clk_before = std::chrono::steady_clock::now();
		for(size_t idx = 0; idx < rounds; ++idx) {
			x[idx] += y[idx];
		}
		const auto delay_add = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now() - clk_before);
		delay_add_ns = static_cast<double>(delay_add.count()) / rounds;

		clk_before = std::chrono::steady_clock::now();
		for(size_t idx = 0; idx < rounds; ++idx) {
			x[idx] -= y[idx];
		}
		const auto delay_sub = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now() - clk_before);
		delay_sub_ns = static_cast<double>(delay_sub.count()) / rounds;

		for(size_t idx = 0; idx < rounds; ++idx) {
			result += (x[idx] < BigType_t(0));
		}

		return result;
	}

	template <typename BigType_t>
	size_t test_performance_mult(const size_t rounds, double& delay_mult_ns) {
		size_t result = 0;
		RndIntGen<BigType_t> gen;

		std::vector<BigType_t> x_ref;
		std::vector<BigType_t> x;
		std::vector<BigType_t> y;

		x_ref.resize(rounds);
		x.resize(rounds);
		y.resize(rounds);

		for(size_t idx = 0; idx < rounds; ++idx) {
			x[idx] = gen.next();
			do {
				y[idx] = (gen.next() >> (sizeof(BigType_t) * CHAR_BIT / 2u));
			} while(y[idx] == BigType_t(0));
			x_ref[idx] = x[idx];
		}

		const std::chrono::steady_clock::time_point clk_before = std::chrono::steady_clock::now();
		for(size_t idx = 0; idx < rounds; ++idx) {
			x[idx] *= y[idx];
		}
		const auto delay_mult = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now() - clk_before);
		delay_mult_ns = static_cast<double>(delay_mult.count()) / rounds;

		for(size_t idx = 0; idx < rounds; ++idx) {
			result += (x[idx] < BigType_t(0));
		}

		return result;
	}

	template <typename BigType_t>
	size_t test_performance_div(const size_t rounds, double& delay_div_ns) {
		size_t result = 0;

		RndIntGen<BigType_t> gen;

		std::vector<BigType_t> x_ref;
		std::vector<BigType_t> x;
		std::vector<BigType_t> y;

		x_ref.resize(rounds);
		x.resize(rounds);
		y.resize(rounds);

		for(size_t idx = 0; idx < rounds; ++idx) {
			x[idx] = gen.next();
			do {
				y[idx] = (gen.next() >> (sizeof(BigType_t) * CHAR_BIT / 2u));
			} while(y[idx] == BigType_t(0));
			x_ref[idx] = x[idx];
		}

		const std::chrono::steady_clock::time_point clk_before = std::chrono::steady_clock::now();
		for(size_t idx = 0; idx < rounds; ++idx) {
			x[idx] /= y[idx];
		}
		const auto delay_div = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now() - clk_before);
		delay_div_ns = static_cast<double>(delay_div.count()) / rounds;

		for(size_t idx = 0; idx < rounds; ++idx) {
			result += (x[idx] < BigType_t(0));
		}

		return result;
	}

};
