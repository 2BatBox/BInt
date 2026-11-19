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
	static constexpr size_t FRAC_PART_WIDTH_BITS = LOG_BASE_2_10 * FRAC_DIGITS + 1;
	static constexpr size_t FRAC_PART_WIDTH_BYTES = FRAC_PART_WIDTH_BITS / CHAR_BIT;

	static constexpr size_t INT_PART_WIDTH_BITS = WIDTH_BITS - FRAC_PART_WIDTH_BITS;
	static constexpr size_t INT_PART_WIDTH_BITES = INT_PART_WIDTH_BITS / CHAR_BIT;

	static_assert(FRAC_DIGITS > 0, "BigFxp: FRAC_DIGITS must be greater than zero!");
	static_assert(WIDTH_BITS > FRAC_PART_WIDTH_BITS, "BigFxp: WIDTH_BITS is not enought to store +/-1 with given FRAC_DIGITS!");

	using IntValue = BigInt<WIDTH_BITS>;
	using WiderIntValue = BigInt<WIDTH_BITS * 2u>;

	static constexpr IntValue BASE = IntValue(10);
	static constexpr IntValue LIMIT_MULT_BASE = IntValue(-1).div_unsigned(BASE);

	template <typename T>
	static constexpr T ten_mult(T val, size_t exp) {
		while(exp--) {
			val *= T(10);
		}
		return val;
	}

	template <typename T>
	static constexpr T ten_div(T val, size_t exp) {
		while(exp--) {
			val /= T(10);
		}
		return val;
	}

	static constexpr IntValue INT_MULIPLIER = ten_mult(IntValue(1), FRAC_DIGITS);
	static constexpr WiderIntValue INT_MULIPLIER_WIDER = WiderIntValue(INT_MULIPLIER);
	static constexpr WiderIntValue INT_HALF_MULIPLIER = INT_MULIPLIER_WIDER >> 1u;

private:

	IntValue _int_value;

public:

	// ----------------------------------------------------------------------------
	// Public constructors.
	// ----------------------------------------------------------------------------
	constexpr BigFxp() = default;

	constexpr BigFxp(const BigFxp& rv) : _int_value(rv._int_value) {}

	constexpr BigFxp(const IntValue& base, int) : _int_value(base) {}

	constexpr explicit BigFxp(const char* str) {
		load_str(str);
	}

	template <typename T, typename std::enable_if<(std::is_integral<T>::value && std::is_signed<T>::value && sizeof(T) <= INT_PART_WIDTH_BITES), int>::type = 0>
	constexpr BigFxp(const T& init) : _int_value(IntValue(init) * INT_MULIPLIER) {}

	template <typename T, typename std::enable_if<(std::is_integral<T>::value && std::is_signed<T>::value && sizeof(T) > INT_PART_WIDTH_BITES), int>::type = 0>
	constexpr explicit BigFxp(const T& init) : _int_value(IntValue(init) * INT_MULIPLIER) {}

	template <typename T, typename std::enable_if<(std::is_floating_point<T>::value), int>::type = 0>
	constexpr explicit BigFxp(const T& init) : _int_value(static_cast<signed long long>(ten_mult(init, FRAC_DIGITS))) {}

	// ----------------------------------------------------------------------------
	// Assign operators.
	// ----------------------------------------------------------------------------
	constexpr BigFxp& operator=(const BigFxp& rv) = default;

	template <typename T, typename std::enable_if<(std::is_integral<T>::value && std::is_signed<T>::value && sizeof(T) <= INT_PART_WIDTH_BITES), int>::type = 0>
	constexpr BigFxp& operator=(const T& init) {
		_int_value = IntValue(init) * INT_MULIPLIER;
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
		return BigFxp(IntValue(1), 0);
	}

	// The maximum value the BigFxp can store.
	static constexpr BigFxp max() {
		return BigFxp(IntValue::max(), 0);
	}

	// The minimum value the BigFxp can store.
	static constexpr BigFxp min() {
		return BigFxp(IntValue::min(), 0);
	}

	// Round the value to the ceil.
	constexpr BigFxp round_ceil(const size_t digits) const {
		IntValue result(_int_value);
		if(digits < FRAC_DIGITS) {
			const IntValue exp(ten_mult(IntValue(1), FRAC_DIGITS - digits));
			const IntValue res_mod(result % exp);
			if(!res_mod.is_zero()) {
				result -= res_mod;
				if(!res_mod.msbit()) {
					result += exp;
				}
			}
		}
		return BigFxp(result, 0);
	}

	// Round the value to the floor.
	constexpr BigFxp round_floor(const size_t digits) const {
		IntValue result(_int_value);
		if(digits < FRAC_DIGITS) {
			const IntValue exp(ten_mult(IntValue(1), FRAC_DIGITS - digits));
			const IntValue res_mod(result % exp);
			if(!res_mod.is_zero()) {
				result -= res_mod;
				if(res_mod.msbit()) {
					result -= exp;
				}
			}
		}
		return BigFxp(result, 0);
	}

	// Round the value to the nearest.
	constexpr BigFxp round(const size_t digits) const {
		IntValue result(_int_value);
		if(digits < FRAC_DIGITS) {
			const IntValue exp(ten_mult(IntValue(1), FRAC_DIGITS - digits));
			const IntValue exp_half(exp >> 1u);
			const IntValue res_mod(result % exp);
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

	// Drop the digits.
	constexpr BigFxp round_drop(const size_t digits) const {
		IntValue result(_int_value);
		if(digits < FRAC_DIGITS) {
			const IntValue exp(ten_mult(IntValue(1), FRAC_DIGITS - digits));
			const IntValue res_mod(result % exp);
			if(!res_mod.is_zero()) {
				result -= res_mod;
			}
		}
		return BigFxp(result, 0);
	}

	// ----------------------------------------------------------------------------
	// Type conversion.
	// ----------------------------------------------------------------------------
	template <typename T, typename std::enable_if<(std::is_integral<T>::value && std::is_signed<T>::value), int>::type = 0>
	constexpr explicit operator T() const {
		return static_cast<T>(_int_value / INT_MULIPLIER);
	}

	template <typename T, typename std::enable_if<(std::is_floating_point<T>::value), int>::type = 0>
	constexpr explicit operator T() const {
		const auto llsi = static_cast<signed long long>(_int_value);
		auto result = static_cast<double>(llsi);
		return ten_div(result, FRAC_DIGITS);
	}

	const IntValue& raw_value() const   { return _int_value; }
	IntValue&       raw_value()         { return _int_value; }

	// ----------------------------------------------------------------------------
	// Relational operators.
	// ----------------------------------------------------------------------------
	constexpr bool operator==(const BigFxp& rv) const { return _int_value == rv._int_value; }
	constexpr bool operator!=(const BigFxp& rv) const { return _int_value != rv._int_value; }
	constexpr bool operator> (const BigFxp& rv) const { return _int_value > rv._int_value;  }
	constexpr bool operator>=(const BigFxp& rv) const { return _int_value >= rv._int_value; }
	constexpr bool operator< (const BigFxp& rv) const { return _int_value < rv._int_value;  }
	constexpr bool operator<=(const BigFxp& rv) const { return _int_value <= rv._int_value; }

	// ----------------------------------------------------------------------------
	// Arithmetic operators.
	// ----------------------------------------------------------------------------

	// Increment / Decrement
	constexpr BigFxp& operator--() {
		_int_value -= INT_MULIPLIER;
		return *this;
	}

	constexpr BigFxp& operator++() {
		_int_value += INT_MULIPLIER;
		return *this;
	}

	constexpr BigFxp operator--(int) {
		BigFxp result(_int_value, 0);
		_int_value -= INT_MULIPLIER;
		return result;
	}

	constexpr BigFxp operator++(int) {
		BigFxp result(_int_value, 0);
		_int_value += INT_MULIPLIER;
		return result;
	}

	// Sign change operators.
	constexpr BigFxp operator+() const {
		return BigFxp(_int_value, 0);
	}

	constexpr BigFxp operator-() const {
		return BigFxp(-_int_value, 0);
	}

	BigFxp abs() const {
		return BigFxp(_int_value.abs(), 0);
	}

	// Addition.
	constexpr BigFxp& operator+=(const BigFxp& rv) {
		_int_value += rv._int_value;
		return *this;
	}

	constexpr BigFxp operator+(const BigFxp& rv) const {
		return BigFxp(_int_value + rv._int_value, 0);
	}

	// Subtraction.
	constexpr BigFxp& operator-=(const BigFxp& rv) {
		_int_value -= rv._int_value;
		return *this;
	}

	constexpr BigFxp operator-(const BigFxp& rv) const {
		return BigFxp(_int_value - rv._int_value, 0);
	}


	// Multiplication
	constexpr BigFxp& operator*=(const BigFxp& val) {
		_int_value = mult(*this, val);
		return *this;
	}

	constexpr BigFxp operator*(const BigFxp& val) const {
		return BigFxp(mult(*this, val), 0);
	}


	// Division
	constexpr BigFxp& operator/=(const BigFxp& val) {
		_int_value = div(*this, val);
		return *this;
	}

	constexpr BigFxp operator/(const BigFxp& val) const {
		return BigFxp(div(*this, val), 0);
	}

	size_t load_str(const char* str, const char* const str_end = nullptr) {
		const char* const str_begin = str;
		bool is_neg = false;
		if((*str == '+') || (*str == '-')) {
			is_neg = (*str == '-');
			++str;
		}

		bool overflown = false;

		_int_value.clear();

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
			overflown = overflown || _int_value.cmp_unsigned_greater(LIMIT_MULT_BASE);
			_int_value.chunk_mult(10u);
			++digits_frac;
		}

		const bool pass = (digits) && (!overflown) && ((!_int_value.msbit()) || (is_neg && (_int_value == IntValue::min())));
		if(pass && is_neg) {
			_int_value.sign_flip();
		}
		return pass ? (str - str_begin) : 0;
	}

	std::string store_str() const {
		IntValue val(_int_value);
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

private:

	constexpr size_t load_dec_str_unsigned(const char* str, const char* const str_end, bool& overflown) {
		const char* const str_begin = str;
		while((str != str_end) && (*str >= '0') && (*str <= '9')) {
			const char ch = *str - '0';
			overflown = overflown || _int_value.cmp_unsigned_greater(LIMIT_MULT_BASE);
			_int_value.chunk_mult(10u);
			overflown = overflown || _int_value.chunk_add_offset(ch, 0);
			++str;
		}
		return str - str_begin;
	}

	static constexpr IntValue mult(const BigFxp& x, const BigFxp& y) {
		WiderIntValue xy_mult(x._int_value);
		xy_mult.mult(WiderIntValue(y._int_value));
		const WiderIntValue int_div(xy_mult.div(INT_MULIPLIER_WIDER));
		const WiderIntValue tail_round(xy_mult.div(INT_HALF_MULIPLIER));
		return IntValue(int_div + tail_round);
	}

	static constexpr IntValue div(const BigFxp& x, const BigFxp& y) {
		const WiderIntValue x_raw(x._int_value);
		const WiderIntValue y_raw(y._int_value);
		const WiderIntValue x_raw_mult_ten_exp(x_raw * INT_MULIPLIER_WIDER);
		return IntValue
		(
			(x._int_value.msbit() == y._int_value.msbit()) ?
			((x_raw_mult_ten_exp + (y_raw >> 1u)) / y_raw) :
			((x_raw_mult_ten_exp - (y_raw >> 1u)) / y_raw)
		);
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
