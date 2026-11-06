#pragma once

#include "BigInt.h"
#include "jtf/utils/Perf.h"

#include <inttypes.h>
#include <algorithm>
#include <random>

#define ASSERT(exp) { \
	if(not (exp)) { \
		printf(#exp "\n"); \
		fflush(stdout); \
		fflush(stderr); \
		abort(); \
	} \
};

//namespace std {
//
//	static std::string to_string(const __int128_t& i128) {
//		std::string result;
//		__int128_t val = i128;
//		const bool is_neg = val < 0;
//		if(val) {
//			result.push_back('0' + std::abs(val % 10));
//			val /= 10;
//		}
//		val = std::abs(val);
//		while(val) {
//			result.push_back('0' + val % 10);
//			val /= 10;
//		}
//		if(is_neg) {
//			result.push_back('-');
//		}
//		std::reverse(result.begin(), result.end());
//		return result;
//	}
//
//};

class TestBigInt {

	template <typename T, std::enable_if_t<(std::is_integral_v<T>), int> = 0>
	struct UniformRanGen {
		std::mt19937_64 rnd;
		std::uniform_int_distribution<T> distr;
		UniformRanGen(const T min = std::numeric_limits<T>::min(), const T max = std::numeric_limits<T>::max()) : rnd(0), distr(min, max) {}
		T next() {
			return distr(rnd);
		}
	};

#define ITERATE_EQUAL_WIDTH_INTEGERS(func, rounds) { \
	printf("->TestBigInt::"#func"()\n"); \
	func<BigInt<int8_t, 1>, int8_t>(rounds); \
	func<BigInt<int8_t, 2>, int16_t>(rounds); \
	func<BigInt<int8_t, 4>, int32_t>(rounds); \
	func<BigInt<int8_t, 8>, int64_t>(rounds); \
	func<BigInt<int8_t, 16>, __int128_t>(rounds); \
	func<BigInt<int16_t, 1>, int16_t>(rounds); \
	func<BigInt<int16_t, 2>, int32_t>(rounds); \
	func<BigInt<int16_t, 4>, int64_t>(rounds); \
	func<BigInt<int16_t, 8>, __int128_t>(rounds); \
	func<BigInt<int32_t, 1>, int32_t>(rounds); \
	func<BigInt<int32_t, 2>, int64_t>(rounds); \
	func<BigInt<int32_t, 4>, __int128_t>(rounds); \
	func<BigInt<int64_t, 1>, int64_t>(rounds); \
	func<BigInt<int64_t, 2>, __int128_t>(rounds); \
}

#define ITERATE_EQUAL_WIDTH_BIG_INT(func, rounds) { \
	printf("->TestBigInt::"#func"()\n"); \
	func<BigInt<int8_t,   1>, BigInt<int8_t,  1>>(rounds); \
	func<BigInt<int8_t,   2>, BigInt<int16_t, 1>>(rounds); \
	func<BigInt<int8_t,   4>, BigInt<int16_t, 2>>(rounds); \
	func<BigInt<int8_t,   6>, BigInt<int16_t, 3>>(rounds); \
	func<BigInt<int8_t,   4>, BigInt<int32_t, 1>>(rounds); \
	func<BigInt<int8_t,   8>, BigInt<int32_t, 2>>(rounds); \
	func<BigInt<int8_t,  12>, BigInt<int32_t, 3>>(rounds); \
	func<BigInt<int8_t,   8>, BigInt<int64_t, 1>>(rounds); \
	func<BigInt<int8_t,  16>, BigInt<int64_t, 2>>(rounds); \
	func<BigInt<int8_t,  24>, BigInt<int64_t, 3>>(rounds); \
	func<BigInt<int16_t,  1>, BigInt<int16_t, 1>>(rounds); \
	func<BigInt<int16_t,  2>, BigInt<int32_t, 1>>(rounds); \
	func<BigInt<int16_t,  4>, BigInt<int32_t, 2>>(rounds); \
	func<BigInt<int16_t,  6>, BigInt<int32_t, 3>>(rounds); \
	func<BigInt<int16_t,  4>, BigInt<int64_t, 1>>(rounds); \
	func<BigInt<int16_t,  8>, BigInt<int64_t, 2>>(rounds); \
	func<BigInt<int16_t, 12>, BigInt<int64_t, 3>>(rounds); \
	func<BigInt<int32_t,  1>, BigInt<int32_t, 1>>(rounds); \
	func<BigInt<int32_t,  2>, BigInt<int64_t, 1>>(rounds); \
	func<BigInt<int32_t,  4>, BigInt<int64_t, 2>>(rounds); \
	func<BigInt<int32_t,  6>, BigInt<int64_t, 3>>(rounds); \
}

public:

	TestBigInt() {

		const size_t rounds = 10000;

		ITERATE_EQUAL_WIDTH_INTEGERS(test_load_store_int, rounds);
//		ITERATE_EQUAL_WIDTH_BIG_INT(test_load_store_big_int, rounds);

		ITERATE_EQUAL_WIDTH_INTEGERS(test_min_max, rounds);
		ITERATE_EQUAL_WIDTH_INTEGERS(test_bitwise, rounds);
		ITERATE_EQUAL_WIDTH_INTEGERS(test_shift, rounds);
		ITERATE_EQUAL_WIDTH_INTEGERS(test_relational, rounds);
		ITERATE_EQUAL_WIDTH_INTEGERS(test_add_sub, rounds);
		ITERATE_EQUAL_WIDTH_INTEGERS(test_mult, rounds);
		ITERATE_EQUAL_WIDTH_INTEGERS(test_div, rounds);
		ITERATE_EQUAL_WIDTH_INTEGERS(test_sign_change, rounds);
//		test_type_conversion();
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
		static_assert(sizeof(BigType_t) == sizeof(Integer));
		UniformRanGen<Integer> gen;
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
	void test_load_store_big_int(size_t) {
		static_assert(A::NUMBER_WIDTH_BYTES == B::NUMBER_WIDTH_BYTES);
//		ASSERT(A::max() == B::max());
		test_load_store_big_int_value<A, B>(A::max());
		test_load_store_big_int_value<A, B>(A::min());
	}

	template <typename A, typename B>
	void test_load_store_big_int_value(const A& in) {
		B bi(in);
		A out(bi);
		ASSERT(out == in);
		ASSERT(bi == in);

		bi = in;
		out = bi;
		ASSERT(out == in);
		ASSERT(bi == in);
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
		UniformRanGen<Integer> gen;
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
		UniformRanGen<Integer> gen;
		UniformRanGen<size_t> gen_shift(0, (sizeof(Integer) * 8u) - 1u);
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
		UniformRanGen<Integer> gen;
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
		UniformRanGen<Integer> gen;
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
		UniformRanGen<Integer> gen;
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
		UniformRanGen<Integer> gen;
		UniformRanGen<Integer> gen_sqrt(-std::sqrt(std::numeric_limits<Integer>::max()), std::sqrt(std::numeric_limits<Integer>::max()));
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
		UniformRanGen<Integer> gen;
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

	// ----------------------------------------------------------------------------
	// Type conversion.
	// ----------------------------------------------------------------------------
	static void dummy_i8(int8_t) {}
	static void dummy_i16(int16_t) {}
	static void dummy_i32(int32_t) {}

	static void test_type_conversion() {
		printf("->TestBigInt::test_type_conversion()\n");

		BigInt<int8_t, 3> bi24_0(static_cast<int8_t>(-1));
		BigInt<int8_t, 3> bi24_1(static_cast<int16_t>(-1));
//		BigInt<int8_t, 3> bi24_1(static_cast<int32_t>(-1)); // <- Compilation error. int32_t is wider.

		bi24_0 = static_cast<int8_t>(-1);
		bi24_1 = static_cast<int16_t>(-1);
//		bi24_1 = static_cast<int32_t>(-1); // <- Compilation error. int32_t is wider.

		// Implicit conversion from BigInt to integer is legal since the BigInt value range is within the integer value range.
		dummy_i8(BigInt<int8_t, 1>::min());
		dummy_i16(BigInt<int8_t, 2>::min());
		dummy_i16(BigInt<int16_t, 1>::min());
		dummy_i32(BigInt<int8_t, 4>::min());
		dummy_i32(BigInt<int16_t, 2>::min());
		dummy_i32(BigInt<int32_t, 1>::min());

		// Explicit conversion must be performed since the integer value range is wider than the value range of BigInt.
		dummy_i8(static_cast<int8_t>(BigInt<int16_t, 1>::min()));
		dummy_i16(static_cast<int8_t>(BigInt<int8_t, 3>::min()));
		dummy_i16(static_cast<int8_t>(BigInt<int16_t, 4>::min()));
		dummy_i32(static_cast<int8_t>(BigInt<int64_t, 2>::min()));
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
		UniformRanGen<Integer> gen_half(min_int / 2, max_int / 2);

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
		UniformRanGen<Integer> gen_full(min_int / 2, max_int / 2);

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
		UniformRanGen<Integer> gen_sqrt(-std::sqrt(max_int), std::sqrt(max_int));

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
		UniformRanGen<Integer> gen_full(min_int, max_int);
		UniformRanGen<Integer> gen_sqrt(-std::sqrt(max_int), std::sqrt(max_int));

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
