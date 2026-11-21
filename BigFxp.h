#pragma once

#include <cstdint>
#include <limits>
#include <type_traits>
#include <string>

#include "BigInt.h"

namespace bint {

template <size_t WIDTH_BITS, size_t FRAC_DIGITS>
struct BigFxp {

	static constexpr double LOG_BASE_2_10 = 3.321928095; // Rounded to the ceil.
	static constexpr size_t NUMBER_DEC_DIGITS_FLOOR = (WIDTH_BITS - 1u) / LOG_BASE_2_10;
	static constexpr size_t INT_DEC_DIGITS_FLOOR = NUMBER_DEC_DIGITS_FLOOR - FRAC_DIGITS;

	static constexpr size_t FRAC_PART_WIDTH_BITS = LOG_BASE_2_10 * FRAC_DIGITS + 1;
	static constexpr size_t FRAC_PART_WIDTH_BYTES = FRAC_PART_WIDTH_BITS / CHAR_BIT;

	static constexpr size_t INT_PART_WIDTH_BITS = WIDTH_BITS - FRAC_PART_WIDTH_BITS;
	static constexpr size_t INT_PART_WIDTH_BITES = INT_PART_WIDTH_BITS / CHAR_BIT;

	static_assert(FRAC_DIGITS > 0, "BigFxp: FRAC_DIGITS must be greater than zero!");
	static_assert(WIDTH_BITS > FRAC_PART_WIDTH_BITS, "BigFxp: WIDTH_BITS is not enought to store +/-1 with given FRAC_DIGITS!");
	static_assert(NUMBER_DEC_DIGITS_FLOOR >= FRAC_DIGITS, "BigFxp: WIDTH_BITS is not enought to store +/-1 with given FRAC_DIGITS!");

	using BigIntValue = BigInt<WIDTH_BITS>;
	using WiderBigIntValue = BigInt<WIDTH_BITS * 2u>;

	static constexpr BigIntValue BASE = BigIntValue(10);
	static constexpr BigIntValue LIMIT_MULT_BASE = BigIntValue(-1).div_unsigned(BASE);

	static constexpr BigIntValue int_value_ten_exp(size_t arg) {
		BigIntValue result(1);
		while(arg--) {
			result.chunk_mult(10u);
		}
		return result;
	}

	static constexpr BigIntValue int_value_ten_exp_ovf(size_t arg) {
		BigIntValue result(INT_MULIPLIER);
		while(arg--) {
			if(result.cmp_unsigned_greater(LIMIT_MULT_BASE)) {
				result.clear();
				break;
			}
			result.chunk_mult(10u);
		}
		return result;
	}

	static constexpr BigIntValue INT_MULIPLIER = int_value_ten_exp(FRAC_DIGITS);
	static constexpr WiderBigIntValue INT_MULIPLIER_WIDER = WiderBigIntValue(INT_MULIPLIER);
	static constexpr WiderBigIntValue INT_HALF_MULIPLIER = INT_MULIPLIER_WIDER >> 1u;

	BigIntValue raw_value;

	// ----------------------------------------------------------------------------
	// Public constructors.
	// ----------------------------------------------------------------------------
	constexpr BigFxp() = default;

	constexpr BigFxp(const BigFxp& rv) : raw_value(rv.raw_value) {}

	constexpr BigFxp(const BigIntValue& base, int) : raw_value(base) {}

	constexpr explicit BigFxp(const char* str) {
		load_str(str);
	}

	template <typename T, typename std::enable_if<(std::is_integral<T>::value && std::is_signed<T>::value && sizeof(T) <= INT_PART_WIDTH_BITES), int>::type = 0>
	constexpr BigFxp(const T& init) : raw_value(BigIntValue(init) * INT_MULIPLIER) {}

	template <typename T, typename std::enable_if<(std::is_integral<T>::value && std::is_signed<T>::value && sizeof(T) > INT_PART_WIDTH_BITES), int>::type = 0>
	constexpr explicit BigFxp(const T& init) : raw_value(BigIntValue(init) * INT_MULIPLIER) {}

	// ----------------------------------------------------------------------------
	// Assign operators.
	// ----------------------------------------------------------------------------
	constexpr BigFxp& operator=(const BigFxp& rv) = default;

	template <typename T, typename std::enable_if<(std::is_integral<T>::value && std::is_signed<T>::value && sizeof(T) <= INT_PART_WIDTH_BITES), int>::type = 0>
	constexpr BigFxp& operator=(const T& init) {
		raw_value = BigIntValue(init) * INT_MULIPLIER;
		return *this;
	}

	// ----------------------------------------------------------------------------
	// Bounds and limits.
	// ----------------------------------------------------------------------------

	// Fractional part in digits.
	static constexpr size_t precision() {
		return FRAC_DIGITS;
	}

	// Smallest possible change.
	static constexpr BigFxp delta() {
		return BigFxp(BigIntValue(1), 0);
	}

	// The maximum value the BigFxp can store.
	static constexpr BigFxp max() {
		return BigFxp(BigIntValue::max(), 0);
	}

	// The minimum value the BigFxp can store.
	static constexpr BigFxp min() {
		return BigFxp(BigIntValue::min(), 0);
	}

	// Round the value to the ceil.
	constexpr BigFxp round_ceil(const ssize_t digits) const {
		return (digits > 0) ? round_ceil_frac(static_cast<size_t>(digits)) : round_ceil_int(static_cast<size_t>(-digits));
	}

	// Round the value to the floor.
	constexpr BigFxp round_floor(const ssize_t digits) const {
		return (digits > 0) ? round_floor_frac(static_cast<size_t>(digits)) : round_floor_int(static_cast<size_t>(-digits));
	}

	// Round the value to the nearest.
	constexpr BigFxp round(const ssize_t digits) const {
		return (digits > 0) ? round_frac(static_cast<size_t>(digits)) : round_int(static_cast<size_t>(-digits));
	}

	// Drop the digits.
	constexpr BigFxp round_drop(const size_t digits) const {
		return (digits > 0) ? round_drop_frac(static_cast<size_t>(digits)) : round_drop_int(static_cast<size_t>(-digits));
	}

	// ----------------------------------------------------------------------------
	// Type conversion.
	// ----------------------------------------------------------------------------
	template <typename T, typename std::enable_if<(std::is_integral<T>::value && std::is_signed<T>::value), int>::type = 0>
	constexpr explicit operator T() const {
		return static_cast<T>(raw_value / INT_MULIPLIER);
	}

	// ----------------------------------------------------------------------------
	// Relational operators.
	// ----------------------------------------------------------------------------
	constexpr bool operator==(const BigFxp& rv) const { return raw_value == rv.raw_value; }
	constexpr bool operator!=(const BigFxp& rv) const { return raw_value != rv.raw_value; }
	constexpr bool operator> (const BigFxp& rv) const { return raw_value > rv.raw_value;  }
	constexpr bool operator>=(const BigFxp& rv) const { return raw_value >= rv.raw_value; }
	constexpr bool operator< (const BigFxp& rv) const { return raw_value < rv.raw_value;  }
	constexpr bool operator<=(const BigFxp& rv) const { return raw_value <= rv.raw_value; }

	// ----------------------------------------------------------------------------
	// Arithmetic operators.
	// ----------------------------------------------------------------------------

	// Increment / Decrement
	constexpr BigFxp& operator--() {
		raw_value -= INT_MULIPLIER;
		return *this;
	}

	constexpr BigFxp& operator++() {
		raw_value += INT_MULIPLIER;
		return *this;
	}

	constexpr BigFxp operator--(int) {
		BigFxp result(raw_value, 0);
		raw_value -= INT_MULIPLIER;
		return result;
	}

	constexpr BigFxp operator++(int) {
		BigFxp result(raw_value, 0);
		raw_value += INT_MULIPLIER;
		return result;
	}

	// Sign change operators.
	constexpr BigFxp operator+() const {
		return BigFxp(raw_value, 0);
	}

	constexpr BigFxp operator-() const {
		return BigFxp(-raw_value, 0);
	}

	BigFxp abs() const {
		return BigFxp(raw_value.abs(), 0);
	}

	// Addition.
	constexpr BigFxp& operator+=(const BigFxp& rv) {
		raw_value += rv.raw_value;
		return *this;
	}

	constexpr BigFxp operator+(const BigFxp& rv) const {
		return BigFxp(raw_value + rv.raw_value, 0);
	}

	// Subtraction.
	constexpr BigFxp& operator-=(const BigFxp& rv) {
		raw_value -= rv.raw_value;
		return *this;
	}

	constexpr BigFxp operator-(const BigFxp& rv) const {
		return BigFxp(raw_value - rv.raw_value, 0);
	}


	// Multiplication
	constexpr BigFxp& operator*=(const BigFxp& val) {
		raw_value = mult(*this, val);
		return *this;
	}

	constexpr BigFxp operator*(const BigFxp& val) const {
		return BigFxp(mult(*this, val), 0);
	}


	// Division
	constexpr BigFxp& operator/=(const BigFxp& val) {
		raw_value = div(this->raw_value, val.raw_value);
		return *this;
	}

	constexpr BigFxp operator/(const BigFxp& val) const {
		return BigFxp(div(this->raw_value, val.raw_value), 0);
	}

	size_t load_str(const char* str, const char* const str_end = nullptr) {
		const char* const str_begin = str;
		bool is_neg = false;
		if((*str == '+') || (*str == '-')) {
			is_neg = (*str == '-');
			++str;
		}

		bool overflown = false;

		raw_value.clear();

		// Read integer part.
		const size_t digits_int = load_dec_str_unsigned(str, str_end, overflown);
		str += digits_int;

		// Read fractional part.
		size_t digits_frac = 0;
		if(*str == '.') {
			++str;
			digits_frac = load_dec_str_unsigned(str, str_end, overflown);
			str += digits_frac;
		}

		overflown = overflown || (digits_frac > FRAC_DIGITS);

		const size_t digits = digits_int + digits_frac;

		while(digits_frac < FRAC_DIGITS) {
			overflown = overflown || raw_value.cmp_unsigned_greater(LIMIT_MULT_BASE);
			raw_value.chunk_mult(10u);
			++digits_frac;
		}

		const bool pass = (digits) && (!overflown) && ((!raw_value.msbit()) || (is_neg && (raw_value == BigIntValue::min())));
		if(pass && is_neg) {
			raw_value.sign_flip();
		}
		return pass ? (str - str_begin) : 0;
	}

	std::string store_str() const {
		BigIntValue val(raw_value);
		const bool msbit = val.msbit();
		if(msbit) {
			val.sign_flip();
		}

		std::string result;

		// Write fractionl part.
		size_t digits_frac = FRAC_DIGITS;
		bool skip_zero = true;

		while(digits_frac > 0) {
			const BigInt div_result = val.div_unsigned(BASE);
			const char digit = BigIntDigitHolder::get_digit(val.number[0]);
			skip_zero = skip_zero && (digit == '0');
			if(!skip_zero) {
				result.push_back(digit);
			}
			val = div_result;
			--digits_frac;
		}

		if(!result.empty()) {
			result.push_back('.');
		}

		// Write integer part.
		do {
			const BigInt div_result = val.div_unsigned(BASE);
			result.push_back(BigIntDigitHolder::get_digit(val.number[0]));
			val = div_result;
		} while(val.valuable_chunks());

		if(msbit) {
			result.push_back('-');
		}
		std::reverse(result.begin(), result.end());
		return result;
	}

	// ----------------------------------------------------------------------------
	// Public helpers.
	// ----------------------------------------------------------------------------
	constexpr bool is_zero() const {
		return raw_value.is_zero();
	}

	constexpr bool is_negative() const {
		return raw_value.msbit();
	}

	static constexpr BigFxp frac(const BigIntValue& num, const BigIntValue& denom) {
		return BigFxp(div(num, denom), 0);
	}

protected:

	constexpr size_t load_dec_str_unsigned(const char* str, const char* const str_end, bool& overflown) {
		const char* const str_begin = str;
		while((str != str_end) && (*str >= '0') && (*str <= '9')) {
			const char ch = *str - '0';
			overflown = overflown || raw_value.cmp_unsigned_greater(LIMIT_MULT_BASE);
			raw_value.chunk_mult(10u);
			overflown = overflown || raw_value.chunk_add_offset(ch, 0);
			++str;
		}
		return str - str_begin;
	}

	static constexpr BigIntValue mult(const BigFxp& x, const BigFxp& y) {
		WiderBigIntValue xy_mult(x.raw_value);
		xy_mult.mult(WiderBigIntValue(y.raw_value));
		const WiderBigIntValue int_div(xy_mult.div(INT_MULIPLIER_WIDER));
		const WiderBigIntValue tail_round(xy_mult.div(INT_HALF_MULIPLIER));
		return BigIntValue(int_div + tail_round);
	}

	static constexpr BigIntValue div(const BigIntValue& x, const BigIntValue& y) {
		const WiderBigIntValue y_wide(y);
		const WiderBigIntValue x_mult_ten_exp_wide(WiderBigIntValue(x) * INT_MULIPLIER_WIDER);
		return BigIntValue
		(
			(x.msbit() == y.msbit()) ?
			((x_mult_ten_exp_wide + (y_wide >> 1u)) / y_wide) :
			((x_mult_ten_exp_wide - (y_wide >> 1u)) / y_wide)
		);
	}

	constexpr BigFxp round_ceil_frac(const size_t digits) const {
		BigIntValue result(raw_value);
		if(digits < FRAC_DIGITS) {
			const BigIntValue exp(int_value_ten_exp(FRAC_DIGITS - digits));
			const BigIntValue res_mod(result % exp);
			if(!res_mod.is_zero()) {
				result -= res_mod;
				if(!res_mod.msbit()) {
					result += exp;
				}
			}
		}
		return BigFxp(result, 0);
	}

	constexpr BigFxp round_ceil_int(const size_t digits) const {
		BigIntValue result(raw_value);
		const BigIntValue exp(int_value_ten_exp_ovf(digits));
		if(exp.is_zero()) {
			result.clear();
		} else {
			const BigIntValue res_mod(result % exp);
			if(!res_mod.is_zero()) {
				result -= res_mod;
				if(!res_mod.msbit()) {
					result += exp;
				}
			}
		}
		return BigFxp(result, 0);
	}

	constexpr BigFxp round_floor_frac(const size_t digits) const {
		BigIntValue result(raw_value);
		if(digits < FRAC_DIGITS) {
			const BigIntValue exp(int_value_ten_exp(FRAC_DIGITS - digits));
			const BigIntValue res_mod(result % exp);
			if(!res_mod.is_zero()) {
				result -= res_mod;
				if(res_mod.msbit()) {
					result -= exp;
				}
			}
		}
		return BigFxp(result, 0);
	}

	constexpr BigFxp round_floor_int(const size_t digits) const {
		BigIntValue result(raw_value);
		const BigIntValue exp(int_value_ten_exp_ovf(digits));
		if(exp.is_zero()) {
			result.clear();
		} else {
			const BigIntValue res_mod(result % exp);
			if(!res_mod.is_zero()) {
				result -= res_mod;
				if(res_mod.msbit()) {
					result -= exp;
				}
			}
		}
		return BigFxp(result, 0);
	}

	constexpr BigFxp round_frac(const size_t digits) const {
		BigIntValue result(raw_value);
		if(digits < FRAC_DIGITS) {
			const BigIntValue exp(int_value_ten_exp(FRAC_DIGITS - digits));
			const BigIntValue exp_half(exp >> 1u);
			const BigIntValue res_mod(result % exp);
			if(!res_mod.is_zero()) {
				result -= res_mod;
				if(res_mod > exp_half) {
					result += exp;
				} else if(res_mod < (-exp_half)) {
					result -= exp;
				}
			}
		}
		return BigFxp(result, 0);
	}

	constexpr BigFxp round_int(const size_t digits) const {
		BigIntValue result(raw_value);
		const BigIntValue exp(int_value_ten_exp_ovf(digits));
		if(exp.is_zero()) {
			result.clear();
		} else {
			const BigIntValue exp_half(exp >> 1u);
			const BigIntValue res_mod(result % exp);
			if(!res_mod.is_zero()) {
				result -= res_mod;
				if(res_mod > exp_half) {
					result += exp;
				} else if(res_mod < (-exp_half)) {
					result -= exp;
				}
			}
		}
		return BigFxp(result, 0);
	}

	constexpr BigFxp round_drop_frac(const size_t digits) const {
		BigIntValue result(raw_value);
		if(digits < FRAC_DIGITS) {
			const BigIntValue exp(int_value_ten_exp(FRAC_DIGITS - digits));
			const BigIntValue res_mod(result % exp);
			if(!res_mod.is_zero()) {
				result -= res_mod;
			}
		}
		return BigFxp(result, 0);
	}

	constexpr BigFxp round_drop_int(const size_t digits) const {
		BigIntValue result(raw_value);
		const BigIntValue exp(int_value_ten_exp_ovf(digits));
		if(exp.is_zero()) {
			result.clear();
		} else {
			const BigIntValue res_mod(result % exp);
			if(!res_mod.is_zero()) {
				result -= res_mod;
			}
		}
		return BigFxp(result, 0);
	}

};

// ----------------------------------------------------------------------------
// Relational operators.
// ----------------------------------------------------------------------------
template <size_t W, size_t P, typename T, typename std::enable_if<(std::is_integral<T>::value && std::is_signed<T>::value && sizeof(T) <= BigFxp<W, P>::INT_PART_WIDTH_BITES), int>::type = 0>
constexpr bool operator==(const T& left, const BigFxp<W, P>& right) { return right.operator==(left); }

template <size_t W, size_t P, typename T, typename std::enable_if<(std::is_integral<T>::value && std::is_signed<T>::value && sizeof(T) <= BigFxp<W, P>::INT_PART_WIDTH_BITES), int>::type = 0>
constexpr bool operator!=(const T& left, const BigFxp<W, P>& right) { return right.operator!=(left); }

template <size_t W, size_t P, typename T, typename std::enable_if<(std::is_integral<T>::value && std::is_signed<T>::value && sizeof(T) <= BigFxp<W, P>::INT_PART_WIDTH_BITES), int>::type = 0>
constexpr bool operator>(const T& left, const BigFxp<W, P>& right) { return right.operator<(left); }

template <size_t W, size_t P, typename T, typename std::enable_if<(std::is_integral<T>::value && std::is_signed<T>::value && sizeof(T) <= BigFxp<W, P>::INT_PART_WIDTH_BITES), int>::type = 0>
constexpr bool operator>=(const T& left, const BigFxp<W, P>& right) { return right.operator<=(left); }

template <size_t W, size_t P, typename T, typename std::enable_if<(std::is_integral<T>::value && std::is_signed<T>::value && sizeof(T) <= BigFxp<W, P>::INT_PART_WIDTH_BITES), int>::type = 0>
constexpr bool operator<(const T& left, const BigFxp<W, P>& right) { return right.operator>(left); }

template <size_t W, size_t P, typename T, typename std::enable_if<(std::is_integral<T>::value && std::is_signed<T>::value && sizeof(T) <= BigFxp<W, P>::INT_PART_WIDTH_BITES), int>::type = 0>
constexpr bool operator<=(const T& left, const BigFxp<W, P>& right) { return right.operator>=(left); }

// ----------------------------------------------------------------------------
// Arithmetic operators.
// ----------------------------------------------------------------------------
template <size_t W, size_t P, typename T, typename std::enable_if<(std::is_integral<T>::value && std::is_signed<T>::value && sizeof(T) <= BigFxp<W, P>::INT_PART_WIDTH_BITES), int>::type = 0>
constexpr BigFxp<W, P> operator+(const T& left, BigFxp<W, P> right) { return right.operator+=(left); }

template <size_t W, size_t P, typename T, typename std::enable_if<(std::is_integral<T>::value && std::is_signed<T>::value && sizeof(T) <= BigFxp<W, P>::INT_PART_WIDTH_BITES), int>::type = 0>
constexpr BigFxp<W, P> operator-(const T& left, const BigFxp<W, P>& right) { return BigFxp<W, P>(left).operator-=(right); }

template <size_t W, size_t P, typename T, typename std::enable_if<(std::is_integral<T>::value && std::is_signed<T>::value && sizeof(T) <= BigFxp<W, P>::INT_PART_WIDTH_BITES), int>::type = 0>
constexpr BigFxp<W, P> operator*(const T& left, BigFxp<W, P> right) { return right.operator*=(left); }

template <size_t W, size_t P, typename T, typename std::enable_if<(std::is_integral<T>::value && std::is_signed<T>::value && sizeof(T) <= BigFxp<W, P>::INT_PART_WIDTH_BITES), int>::type = 0>
constexpr BigFxp<W, P> operator/(const T& left, const BigFxp<W, P>& right) { return BigFxp<W, P>(left).operator/=(right); }

template <size_t W, size_t P, typename T, typename std::enable_if<(std::is_integral<T>::value && std::is_signed<T>::value && sizeof(T) <= BigFxp<W, P>::INT_PART_WIDTH_BITES), int>::type = 0>
constexpr BigFxp<W, P> operator%(const T& left, const BigFxp<W, P>& right) { return BigFxp<W, P>(left).operator%=(right); }

} // namespace bint
