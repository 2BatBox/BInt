#pragma once

#include "BigInt.h"
#include "hex.h"
#include "jtf/utils/Perf.h"

#include <inttypes.h>
#include <algorithm>
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
				result <<= 8u;
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

		const size_t rounds = 10000;

		ITERATE_BIGINT_AND_EQUAL_WIDTH_INTEGERS(test_load_store_int, rounds);
		ITERATE_BIGINT_AND_NARROWER_INTEGERS(test_load_store_int, rounds);

		ITERATE_BIGINT_AND_EQUAL_WIDTH_BIGINT(test_load_store_big_int, rounds);
		ITERATE_BIGINT_AND_WIDER_BIGINT(test_load_store_big_int, rounds);

		test_type_conversion_bigint_int();
		test_type_conversion_bigint_bigint();

//		ITERATE_EQUAL_WIDTH_INTEGERS(test_min_max, rounds);
//		ITERATE_EQUAL_WIDTH_INTEGERS(test_bitwise, rounds);
//		ITERATE_EQUAL_WIDTH_INTEGERS(test_shift, rounds);
//		ITERATE_EQUAL_WIDTH_INTEGERS(test_relational, rounds);
//		ITERATE_EQUAL_WIDTH_INTEGERS(test_add_sub, rounds);
//		ITERATE_EQUAL_WIDTH_INTEGERS(test_mult, rounds);
//		ITERATE_EQUAL_WIDTH_INTEGERS(test_div, rounds);
//		ITERATE_EQUAL_WIDTH_INTEGERS(test_sign_change, rounds);
//		test_constexpr_values();

//		const size_t perf_rounds = 1000ull*1000ull*1ull;
//		test_performance<BigInt<int64_t, 2>, __int128_t>(perf_rounds);

//		if(int_out != int_ref) {
//			hex(int_out);
//			hex(int_ref);
//			hex(big_int);
//		}

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
	static void func_in_char(signed char) {}
	static void func_in_int(signed int) {}

	static signed char func_out_char()   { return 0; }
	static signed int func_out_int()   { return 0; }

	static void test_type_conversion_bigint_int() {
		printf("->TestBigInt::test_type_conversion_bigint_int()\n");

		BigInt<signed char, 1> bi_0(func_out_char());
		BigInt<signed char, 1> bi_1(func_out_int()); // <- Allowed, explicit convertion.

		bi_0 = func_out_char();
//		bi_1 = func_out_int(); // <- Compilation error. int is wider than BigInt<signed char, 1>.

		// Implicit conversion from BigInt to integer is legal since the BigInt value range is within the integer value range.
		func_in_char(BigInt<signed char, 1>::min());
		func_in_int(BigInt<signed char, 1>::min());
		func_in_int(BigInt<signed int, 1>::min());

		// Explicit conversion must be performed since the integer value range is wider than the value range of BigInt.
		func_in_char(static_cast<signed char>(BigInt<signed char, 2u>::min()));
		func_in_char(static_cast<signed char>(BigInt<signed int, 1u>::min()));
		func_in_int(static_cast<signed int>(BigInt<signed char, 99u>::min()));
		func_in_int(static_cast<signed int>(BigInt<signed int, 2u>::min()));
	}

	static void func_in_bint_char(BigInt<signed char, 3u>) {}
	static void func_in_bint_int(BigInt<signed int, 3u>) {}

	static BigInt<signed char, 3u> func_out_bint_char() { return BigInt<signed char, 3u>(0); }
	static BigInt<signed int, 3u> func_out_bint_int()   { return BigInt<signed int, 3u>(0); }

	static void test_type_conversion_bigint_bigint() {
		printf("->TestBigInt::test_type_conversion_bigint_bigint()\n");

		BigInt<signed char, 3> bi_0(func_out_bint_char());
		BigInt<signed char, 3> bi_1(func_out_bint_int());

		bi_0 = func_out_bint_char();
//		bi_1 = func_out_bint_int(); // <- Compilation error. BigInt<signed int, 1u> is wider than BigInt<signed char, 1u>.

		// Implicit conversion.
		func_in_bint_char(BigInt<signed char, 1u>::min());
		func_in_bint_char(BigInt<signed char, 2u>::min());
		func_in_bint_char(BigInt<signed char, 3u>::min());
		func_in_bint_int(BigInt<signed char, 1>::min());
		func_in_bint_int(BigInt<signed int, 1>::min());

		// Explicit conversion must be performed.
		func_in_bint_char(static_cast<int8_t>(BigInt<int16_t, 1>::min()));
		func_in_i16(static_cast<int8_t>(BigInt<int8_t, 3>::min()));
		func_in_i16(static_cast<int8_t>(BigInt<int16_t, 4>::min()));
		func_in_i32(static_cast<int8_t>(BigInt<int64_t, 2>::min()));
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
		UniformIntGen<size_t> gen_shift(0, (sizeof(Integer) * 8u) - 1u);
		BigType_t bx;
		BigType_t bop;
		while(rounds--) {
			const Integer ix = gen.next();
			Integer iop;
			const size_t shift = gen_shift.next();
			bx = ix;
			test_equal<BigType_t, Integer>(bx, ix);
			test_equal<BigType_t, Integer>(bx << shift, ix << shift);
			test_equal<BigType_t, Integer>(bx >> shift, ix >> shift);

			bop = bx;
			iop = ix;
			bop <<= shift;
			iop <<= shift;
			test_equal<BigType_t, Integer>(bop, iop);

			bop = bx;
			iop = ix;
			bop >>= shift;
			iop >>= shift;
			test_equal<BigType_t, Integer>(bop, iop);
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
	void test_div(size_t rounds) {
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

			bop = bx;
			iop = ix;
			bop /= by;
			iop /= iy;
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

		if(int_out != int_ref) {
			hex(int_out);
			hex(int_ref);
			hex(big_int);
		}
		ASSERT(int_out == int_ref);
		ASSERT(big_int == int_ref);
	}


	// -----------------------------------------------------------------------
	// Performance test.
	// -----------------------------------------------------------------------
	template <typename BigType_t, typename Integer>
	void test_performance(size_t rounds) {
		size_t result = 0;
		result += test_performance_load_store_integer<BigType_t, Integer>(rounds);
		result += test_performance_add_sub<BigType_t, Integer>(rounds);
		result += test_performance_mult<BigType_t, Integer>(rounds);
		result += test_performance_div<BigType_t, Integer>(rounds);

		printf("Performance test finishes with %zu errors.\n", result);
		ASSERT(result == 0);
	}

	template <typename BigType_t, typename Integer>
	size_t test_performance_load_store_integer(const size_t rounds) {
		size_t result = 0;
		jtf::utils::Perf perf_load ("load_integer() ");
		jtf::utils::Perf perf_store("store_integer()");

		const auto min_int = static_cast<Integer>(BigType_t::min());
		const auto max_int = static_cast<Integer>(BigType_t::max());
		UniformIntGen<Integer> gen_half(min_int / 2, max_int / 2);

		std::vector<BigType_t> big;
		std::vector<Integer> input;
		std::vector<Integer> output;

		big.resize(rounds);
		input.resize(rounds);
		output.resize(rounds);

		for(size_t idx = 0; idx < rounds; ++idx) {
			input[idx] = gen_half.next();
		}

		perf_load.start();
		for(size_t idx = 0; idx < rounds; ++idx) {
			big[idx] = input[idx];
		}
		perf_load.stop();

		perf_store.start();
		for(size_t idx = 0; idx < rounds; ++idx) {
			output[idx] = big[idx];
		}
		perf_store.stop();

		for(size_t idx = 0; idx < rounds; ++idx) {
			result += (input[idx] != output[idx]);
		}

		const size_t items = rounds * BigType_t::NUMBER_WIDTH_BYTES;
		perf_load.print(rounds, items);
		perf_store.print(rounds, items);

		return result;
	}

	template <typename BigType_t, typename Integer>
	size_t test_performance_add_sub(const size_t rounds) {
		size_t result = 0;
		jtf::utils::Perf perf_add("add()          ");
		jtf::utils::Perf perf_sub("sub()          ");

		const auto min_int = static_cast<Integer>(BigType_t::min());
		const auto max_int = static_cast<Integer>(BigType_t::max());
		UniformIntGen<Integer> gen_full(min_int / 2, max_int / 2);

		std::vector<Integer> ix;
		std::vector<Integer> iy;
		std::vector<BigType_t> x;
		std::vector<BigType_t> y;

		ix.resize(rounds);
		iy.resize(rounds);
		x.resize(rounds);
		y.resize(rounds);

		for(size_t idx = 0; idx < rounds; ++idx) {
			ix[idx] = gen_full.next();
			iy[idx] = gen_full.next();
			x[idx] = ix[idx];
			y[idx] = iy[idx];
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
			result += (x[idx] != ix[idx]);
		}

		const size_t items = rounds * BigType_t::NUMBER_WIDTH_BYTES;
		perf_add.print(rounds, items);
		perf_sub.print(rounds, items);

		return result;
	}

	template <typename BigType_t, typename Integer>
	size_t test_performance_mult(const size_t rounds) {
		size_t result = 0;
		jtf::utils::Perf perf_mult("mult()         ");

		const auto max_int = static_cast<Integer>(BigType_t::max());
		UniformIntGen<Integer> gen_sqrt(-std::sqrt(max_int), std::sqrt(max_int));

		std::vector<Integer> ix;
		std::vector<Integer> iy;
		std::vector<BigType_t> x;
		std::vector<BigType_t> y;

		ix.resize(rounds);
		iy.resize(rounds);
		x.resize(rounds);
		y.resize(rounds);

		for(size_t idx = 0; idx < rounds; ++idx) {
			ix[idx] = gen_sqrt.next();
			iy[idx] = gen_sqrt.next();
			x[idx] = ix[idx];
			y[idx] = iy[idx];
		}

		perf_mult.start();
		for(size_t idx = 0; idx < rounds; ++idx) {
			x[idx] *= y[idx];
		}
		perf_mult.stop();

		for(size_t idx = 0; idx < rounds; ++idx) {
			result += (x[idx] != Integer(ix[idx] * iy[idx]));
		}

		const size_t items = rounds * BigType_t::NUMBER_WIDTH_BYTES;
		perf_mult.print(rounds, items);

		return result;
	}

	template <typename BigType_t, typename Integer>
	size_t test_performance_div(const size_t rounds) {
		size_t result = 0;
		jtf::utils::Perf perf_div("div()          ");

		const auto min_int = static_cast<Integer>(BigType_t::min());
		const auto max_int = static_cast<Integer>(BigType_t::max());
		UniformIntGen<Integer> gen_full(min_int, max_int);
		UniformIntGen<Integer> gen_sqrt(-std::sqrt(max_int), std::sqrt(max_int));

		std::vector<Integer> ix;
		std::vector<Integer> iy;
		std::vector<BigType_t> x;
		std::vector<BigType_t> y;

		ix.resize(rounds);
		iy.resize(rounds);
		x.resize(rounds);
		y.resize(rounds);

		for(size_t idx = 0; idx < rounds; ++idx) {
			ix[idx] = gen_full.next();
			do {
				iy[idx] = gen_sqrt.next();
			} while(iy[idx] == 0);
			x[idx] = ix[idx];
			y[idx] = iy[idx];
		}

		perf_div.start();
		for(size_t idx = 0; idx < rounds; ++idx) {
			x[idx] /= y[idx];
		}
		perf_div.stop();

		for(size_t idx = 0; idx < rounds; ++idx) {
			result += (x[idx] != Integer(ix[idx] / iy[idx]));
		}

		const size_t items = rounds * BigType_t::NUMBER_WIDTH_BYTES;
		perf_div.print(rounds, items);

		return result;
	}

//	void test_constexpr_values() {
//		printf("->TestBigInt::test_constexpr_values()\n");
//		using TypeNarrow_t = BigInt<signed int, 3>;
//		using TypeWide_t = BigInt<signed int, 5>;
//
//	}


};
