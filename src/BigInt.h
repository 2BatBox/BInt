#pragma once

#include <type_traits>
#include <string>
#include <array>

static uint64_t _tmp_call_cnt = 0;
static uint64_t _tmp_carry_mult_cnt = 0;
static uint64_t _tmp_mult_cnt = 0;
static uint64_t _tmp_sub_cnt = 0;

/**
 * @tparam Chunk Any signed integral type.
 * @tparam WIDTH_CHUNKS
 *
 * The BigInt is a signed, fixed-size, static stored integer arithmetic template.
 * The integer value is stored in two's complement code as an array of chunks (or limbs if you prefer GMP terminology).
 *
 * |     chunk_0     |     chunk_1     |     chunk_2     |    ...    |     chunk_n     |
 *          |                                                                |
 *  The least significant chunk.                                    The most significant chunk.
 *
 *  The chunk structure.
 *
 *  |           High half chunk.                 |                  Low half chunk.                   |
 *  |     bit_n     |    ...    |    bit_(n/2)   |    bit_((n/2) - 1)   |     ...     |     bit_0     |
 *          |                                                                                |
 *  The most significant bit.                                                  The least significant bit.
 *
 */
template <typename Chunk, size_t WIDTH_CHUNKS>
class BigInt {

	static_assert(std::is_integral_v<Chunk>, "BigInt Chunk type MUST BE an integral type!");
	static_assert(std::is_signed_v<Chunk>, "BigInt Chunk type MUST BE signed!");
	static_assert(WIDTH_CHUNKS > 0, "BigInt WIDTH_CHUNKS is zero!");
	static_assert(WIDTH_CHUNKS <= (SIZE_MAX / 2u), "BigInt WIDTH_CHUNKS is oversized!");

	template <typename C, size_t W>
	friend class BigInt;

public:

	using UnsignedChunk = std::make_unsigned_t<Chunk>;

	static constexpr size_t CHUNK_WIDTH_BYTES = sizeof(UnsignedChunk);
	static constexpr size_t CHUNK_WIDTH_BITS = CHUNK_WIDTH_BYTES * 8u;
	static constexpr size_t HALF_WIDTH_CHUNKS = WIDTH_CHUNKS * 2u;
	static constexpr size_t HALF_CHUNK_WIDTH_BITS = CHUNK_WIDTH_BITS / 2u;
	static constexpr size_t NUMBER_WIDTH_BYTES = CHUNK_WIDTH_BYTES * WIDTH_CHUNKS;
	static constexpr size_t NUMBER_WIDTH_BITS = CHUNK_WIDTH_BITS * WIDTH_CHUNKS;
	
	static constexpr UnsignedChunk CHUNK_MASK = ~UnsignedChunk(0);
	static constexpr UnsignedChunk CHUNK_MAX = ~UnsignedChunk(0);
	static constexpr UnsignedChunk CHUNK_HALF = CHUNK_MAX / 2u;
	static constexpr UnsignedChunk HALF_CHUNK_LOW_MASK = UnsignedChunk(CHUNK_MASK >> HALF_CHUNK_WIDTH_BITS);
	static constexpr UnsignedChunk HALF_CHUNK_HIGH_MASK = UnsignedChunk(CHUNK_MASK << HALF_CHUNK_WIDTH_BITS);
	static constexpr UnsignedChunk MSBIT_MASK = UnsignedChunk(CHUNK_MASK << (CHUNK_WIDTH_BITS - 1u));

protected:

	using Number_t = std::array<UnsignedChunk, WIDTH_CHUNKS>;
	Number_t _number;

public:

	// ----------------------------------------------------------------------------
	// Public constructors.
	// ----------------------------------------------------------------------------
	constexpr BigInt() : _number{0} {}

	constexpr BigInt(const BigInt& rv) = default;
	constexpr BigInt& operator=(const BigInt& rv) = default;
	BigInt(BigInt&& rv) = default;
	BigInt& operator=(BigInt&& rv) = default;

	// From not wider integer. Implicit conversion.
	template <typename T, std::enable_if_t<(std::is_integral_v<T> && std::is_signed_v<T> && sizeof(T) <= NUMBER_WIDTH_BYTES), int> = 0>
	constexpr BigInt(const T& init) : _number{0} {
		load_integer(init);
	}

	// From wider integer. Explicit conversion is needed.
	template <typename T, std::enable_if_t<(std::is_integral_v<T> && std::is_signed_v<T> && sizeof(T) > NUMBER_WIDTH_BYTES), int> = 0>
	constexpr explicit BigInt(const T& init) : _number{0} {
		load_integer(init);
	}

	// From not wider BigInt. Implicit conversion.
	template <typename C, size_t W, std::enable_if_t<((sizeof(C) * W) <= NUMBER_WIDTH_BYTES), int> = 0>
	constexpr BigInt(const BigInt<C, W>& rv) {
		load_big_int(rv);
	}

	// From wider BigInt. Explicit conversion is needed.
	template <typename C, size_t W, std::enable_if_t<((sizeof(C) * W) > NUMBER_WIDTH_BYTES), int> = 0>
	constexpr explicit BigInt(const BigInt<C, W>& rv) {
		load_big_int(rv);
	}

	// From string.
	constexpr explicit BigInt(std::string_view str) : _number{0} {
//		load_string(str);
	}

	// ----------------------------------------------------------------------------
	// Copying operators.
	// ----------------------------------------------------------------------------
	// From not wider integer.
	template <typename T, std::enable_if_t<(std::is_integral_v<T> && std::is_signed_v<T> && sizeof(T) <= NUMBER_WIDTH_BYTES), int> = 0>
	constexpr BigInt& operator=(const T& init) {
		load_integer(init);
		return *this;
	}

	// From not wider BigInt.
	template <typename C, size_t W, std::enable_if_t<((sizeof(C) * W) <= NUMBER_WIDTH_BYTES), int> = 0>
	constexpr BigInt& operator=(const BigInt<C, W>& rv) {
		load_big_int(rv);
		return *this;
	}

	// ----------------------------------------------------------------------------
	// Bounds and limits.
	// ----------------------------------------------------------------------------

	// The maximum value the BigInt can store.
	static constexpr BigInt max() {
		BigInt result;
		result.bitwise_not();
		result._number[WIDTH_CHUNKS - 1u] = UnsignedChunk(~MSBIT_MASK);
		return result;
	}

	// The minimum value the FixPoint can store.
	static constexpr BigInt min() {
		BigInt result;
		result._number[WIDTH_CHUNKS - 1u] = MSBIT_MASK;
		return result;
	}

	// ----------------------------------------------------------------------------
	// Type conversion.
	// ----------------------------------------------------------------------------
	template <typename T, std::enable_if_t<(std::is_integral_v<T> && std::is_signed_v<T> && sizeof(T) >= NUMBER_WIDTH_BYTES), int> = 0>
	constexpr operator T() const {
		T result;
		store_integer(result);
		return result;
	}

	template <typename T, std::enable_if_t<(std::is_integral_v<T> && std::is_signed_v<T> && sizeof(T) < NUMBER_WIDTH_BYTES), int> = 0>
	constexpr explicit operator T() const {
		T result;
		store_integer(result);
		return result;
	}

	// ----------------------------------------------------------------------------
	// Bitwise operators.
	// ----------------------------------------------------------------------------

	// not
	constexpr BigInt operator~() const {
		BigInt result(*this);
		result.bitwise_not();
		return result;
	}

	// and
	constexpr BigInt& operator&=(const BigInt& rv) {
		bitwise_and(rv);
		return *this;
	}

	constexpr BigInt operator&(const BigInt& rv) const {
		BigInt result(*this);
		result.bitwise_and(rv);
		return result;
	}

	// or
	constexpr BigInt& operator|=(const BigInt& rv) {
		bitwise_or(rv);
		return *this;
	}

	constexpr BigInt operator|(const BigInt& rv) const {
		BigInt result(*this);
		result.bitwise_or(rv);
		return result;
	}

	// xor
	constexpr BigInt& operator^=(const BigInt& rv) {
		bitwise_xor(rv);
		return *this;
	}

	constexpr BigInt operator^(const BigInt& rv) const {
		BigInt result(*this);
		result.bitwise_xor(rv);
		return result;
	}

	// Shift left
	constexpr BigInt& operator<<=(const size_t shift) {
		shift_left_unsigned(shift);
		return *this;
	}

	constexpr BigInt operator<<(const size_t shift) const {
		BigInt result(*this);
		result.shift_left_unsigned(shift);
		return result;
	}

	// Shift right
	constexpr BigInt& operator>>=(const size_t shift) {
		shift_right(shift);
		return *this;
	}

	constexpr BigInt operator>>(const size_t shift) const {
		BigInt result(*this);
		result.shift_right(shift);
		return result;
	}

	// ----------------------------------------------------------------------------
	// Relational operators.
	// ----------------------------------------------------------------------------
	constexpr bool operator==(const BigInt& rv) const {
		const size_t idx = last_diff_chunk(rv);
		return _number[idx] == rv._number[idx];
	}

	constexpr bool operator!=(const BigInt& rv) const {
		const size_t idx = last_diff_chunk(rv);
		return _number[idx] != rv._number[idx];
	}

	constexpr bool operator>(const BigInt& rv) const {
		return (msbit() == rv.msbit()) ? abs_val_greater(rv) : (not msbit());
	}

	constexpr bool operator<(const BigInt& rv) const {
		return (msbit() == rv.msbit()) ? abs_val_less(rv) : msbit();
	}

	constexpr bool operator>=(const BigInt& rv) const {
		return not operator<(rv);
	}

	constexpr bool operator<=(const BigInt& rv) const {
		return not operator>(rv);
	}

	// ----------------------------------------------------------------------------
	// Arithmetic operators.
	// ----------------------------------------------------------------------------

	// Sign change operations.
	constexpr BigInt abs() const {
		BigInt result(*this);
		if(msbit()) {
			result.sign_flip();
		}
		return result;
	}

	constexpr BigInt operator-() const {
		BigInt result(*this);
		result.sign_flip();
		return result;
	}

	// Addition.
	constexpr BigInt operator+(const BigInt& rv) const {
		BigInt result(*this);
		result.add(rv);
		return result;
	}

	constexpr BigInt& operator+=(const BigInt& rv) {
		add(rv);
		return *this;
	}

	// Subtraction.
	constexpr BigInt operator-(const BigInt& rv) const {
		BigInt result(*this);
		result.add(-rv);
		return result;
	}

	constexpr BigInt& operator-=(const BigInt& rv) {
		add(-rv);
		return *this;
	}

	// Multiplication.
	constexpr BigInt operator*(const BigInt& rv) const {
		BigInt result(*this);
		result.mult(rv);
		return result;
	}

	constexpr BigInt& operator*=(const BigInt& rv) {
		mult(rv);
		return *this;
	}

	// Division.
	constexpr BigInt operator/(const BigInt& rv) const {
		BigInt result(*this);
		BigInt result_div;
		result.div(rv, result_div);
		return result_div;
	}

	constexpr BigInt& operator/=(const BigInt& rv) {
		BigInt result_div;
		div(rv, result_div);
		*this = result_div;
		return *this;
	}

	// TODO:
	constexpr BigInt operator%(const BigInt& rv) const {
		BigInt result(*this);
		return result;
	}

	constexpr BigInt& operator%=(const BigInt& rv) {
		return *this;
	}

private:

	// Load integers.
	template <typename T, std::enable_if_t<(std::is_integral_v<T> && std::is_signed_v<T>), int> = 0>
	constexpr void load_integer(T val) {
		if(val < 0) {
			val = ~val;
			load_integer_unsigned(static_cast<std::make_unsigned_t<T>>(val));
			bitwise_not();
		} else {
			load_integer_unsigned(static_cast<std::make_unsigned_t<T>>(val));
		}
	}

	template <typename T, std::enable_if_t<(std::is_integral_v<T> && std::is_unsigned_v<T> && (sizeof(T) > sizeof(UnsignedChunk))), int> = 0>
	constexpr void load_integer_unsigned(T val) {
		size_t idx = 0;
		while((idx < WIDTH_CHUNKS) && (val != 0)) {
			_number[idx++] = val & CHUNK_MASK;
			val >>= (sizeof(UnsignedChunk) * 8u);
		}
		while(idx < WIDTH_CHUNKS) {
			_number[idx++] = 0;
		}
	}

	template <typename T, std::enable_if_t<(std::is_integral_v<T> && std::is_unsigned_v<T> && (sizeof(T) <= sizeof(UnsignedChunk))), int> = 0>
	constexpr void load_integer_unsigned(const T val) {
		size_t idx = 0;
		_number[idx++] = val;

		while(idx < WIDTH_CHUNKS) {
			_number[idx++] = 0;
		}
	}

	// Store integers.
	template <typename T, std::enable_if_t<(std::is_integral_v<T> && std::is_signed_v<T>), int> = 0>
	constexpr void store_integer(T& val) const {
		if(msbit()) {
			BigInt tmp(*this);
			tmp.bitwise_not();
			tmp.store_integer_unsigned(reinterpret_cast<std::make_unsigned_t<T>&>(val));
			val = ~val;
		} else {
			store_integer_unsigned(reinterpret_cast<std::make_unsigned_t<T>&>(val));
		}
	}

	template <typename T, std::enable_if_t<(std::is_integral_v<T> && std::is_unsigned_v<T> && (sizeof(T) > sizeof(UnsignedChunk))), int> = 0>
	constexpr void store_integer_unsigned(T& val) const {
		val = 0;
		size_t idx = WIDTH_CHUNKS - 1u;
		while((idx < WIDTH_CHUNKS) && (_number[idx] == 0)) {
			--idx;
		}

		while(idx < WIDTH_CHUNKS) {
			val <<= sizeof(UnsignedChunk) * 8u;
			val |= _number[idx--] & CHUNK_MASK;
		}
	}

	template <typename T, std::enable_if_t<(std::is_integral_v<T> && std::is_unsigned_v<T> && (sizeof(T) <= sizeof(UnsignedChunk))), int> = 0>
	constexpr void store_integer_unsigned(T& val) const {
		val = 0;
		size_t idx = WIDTH_CHUNKS - 1u;
		while((idx < WIDTH_CHUNKS) && (_number[idx] == 0)) {
			--idx;
		}

		if(idx < WIDTH_CHUNKS) {
			val = _number[idx--];
		}
	}


	// Load BigInt.
	template <typename C, size_t W>
	constexpr void load_big_int(BigInt<C, W> bi) {
		const bool msb = bi.msbit();
		if(msb) {
			bi.bitwise_not();
		}
		load_big_int_unsigned(bi);
		if(msb) {
			bi.bitwise_not();
		}
	}

	template <typename C, size_t W, std::enable_if_t<(sizeof(C) < sizeof(Chunk)), int> = 0>
	constexpr void load_big_int_unsigned(BigInt<C, W> bi) {
	}

	template <typename C, size_t W, std::enable_if_t<(sizeof(C) > sizeof(Chunk)), int> = 0>
	constexpr void load_big_int_unsigned(BigInt<C, W> bi) {
		size_t idx = 0;
		while((idx < WIDTH_CHUNKS) && (bi != C(0))) {
			_number[idx] = bi[0] & CHUNK_MASK;
			bi.shift_right_unsigned(CHUNK_WIDTH_BITS);
		}
		while(idx < WIDTH_CHUNKS) {
			_number[idx] = 0;
		}
	}

	template <typename C, size_t W, std::enable_if_t<(sizeof(C) == sizeof(Chunk)), int> = 0>
	constexpr void load_big_int_unsigned(BigInt<C, W> bi) {
		const size_t width = std::min(W, WIDTH_CHUNKS);

		size_t idx = 0;
		while(idx < width) {
			_number[idx] = bi._number[idx];
		}
		while(idx < WIDTH_CHUNKS) {
			_number[idx] = 0;
		}
	}
	

	// Bitwise operators.
	constexpr void bitwise_not() {
		for(auto& chunk : _number) {
			chunk = ~chunk;
		}
	}

	constexpr void bitwise_and(const BigInt& rv) {
		for(size_t idx = 0; idx < WIDTH_CHUNKS; ++idx) {
			_number[idx] &= rv._number[idx];
		}
	}

	constexpr void bitwise_or(const BigInt& rv) {
		for(size_t idx = 0; idx < WIDTH_CHUNKS; ++idx) {
			_number[idx] |= rv._number[idx];
		}
	}

	constexpr void bitwise_xor(const BigInt& rv) {
		for(size_t idx = 0; idx < WIDTH_CHUNKS; ++idx) {
			_number[idx] ^= rv._number[idx];
		}
	}

	constexpr void shift_left_unsigned(size_t shift_bits) {
		shift_bits = std::min(shift_bits, NUMBER_WIDTH_BITS);
		const size_t shift_chunks = shift_bits / CHUNK_WIDTH_BITS;
		
		// Shifting chunks.
		size_t dst = WIDTH_CHUNKS - 1u;
		size_t src = dst - shift_chunks;
		while(src < WIDTH_CHUNKS) {
			_number[dst] = _number[src];
			--dst;
			--src;
		}
		while(dst < WIDTH_CHUNKS) {
			_number[dst] = 0;
			--dst;
		}

		// Shifting bits.
		shift_bits = shift_bits % CHUNK_WIDTH_BITS;
		const size_t shift_bits_compl = CHUNK_WIDTH_BITS - shift_bits;

		if(shift_bits) {
			size_t idx = WIDTH_CHUNKS - 1u;
			while(idx) {
				_number[idx] = (_number[idx] << shift_bits) | (_number[idx - 1] >> shift_bits_compl);
				--idx;
			}
			_number[0] = (_number[0] << shift_bits);
		}

	}

	constexpr void shift_right(const size_t shift_bits) {
		if(msbit()) {
			bitwise_not();
			shift_right_unsigned(shift_bits);
			bitwise_not();
		} else {
			shift_right_unsigned(shift_bits);
		}
	}

	constexpr void shift_right_unsigned(size_t shift_bits) {
		shift_bits = std::min(shift_bits, NUMBER_WIDTH_BITS);
		const size_t shift_chunks = shift_bits / CHUNK_WIDTH_BITS;

		// Shifting chunks.
		size_t dst = 0;
		size_t src = shift_chunks;
		while(src < WIDTH_CHUNKS) {
			_number[dst] = _number[src];
			++dst;
			++src;
		}
		while(dst < WIDTH_CHUNKS) {
			_number[dst] = 0;
			++dst;
		}

		// Shifting bits.
		shift_bits = shift_bits % CHUNK_WIDTH_BITS;
		const size_t shift_bits_compl = CHUNK_WIDTH_BITS - shift_bits;

		if(shift_bits) {
			size_t idx = 0;
			while(idx < (WIDTH_CHUNKS - 1u)) {
				_number[idx] = (_number[idx] >> shift_bits) | (_number[idx + 1] << shift_bits_compl);
				++idx;
			}
			_number[idx] = (_number[idx] >> shift_bits);
		}

	}

	// Arithmetic operators.
	constexpr UnsignedChunk add(const BigInt& rv) {
		bool carry = false;
		for(size_t idx = 0; idx < WIDTH_CHUNKS; ++idx) {
			_number[idx] += rv._number[idx];
			const bool hcarry = _number[idx] < rv._number[idx];
			_number[idx] += carry;
			carry = hcarry | (_number[idx] < carry);
		}
		return carry;
	}

	constexpr void mult(const BigInt& rv) {
		BigInt acc;
		for(size_t i = 0; i < WIDTH_CHUNKS; ++i) {
			for(size_t j = 0; j < WIDTH_CHUNKS; ++j) {
				const size_t offset = i + j;
				if(offset < WIDTH_CHUNKS) {
					const UnsignedChunk chunk_xl = (_number[i] & HALF_CHUNK_LOW_MASK);
					const UnsignedChunk chunk_xh = (_number[i] >> HALF_CHUNK_WIDTH_BITS);
					const UnsignedChunk chunk_yl = (rv._number[j] & HALF_CHUNK_LOW_MASK);
					const UnsignedChunk chunk_yh = (rv._number[j] >> HALF_CHUNK_WIDTH_BITS);

					const UnsignedChunk chunk_xl_yl = chunk_xl * chunk_yl;
					const UnsignedChunk chunk_xl_yh = chunk_xl * chunk_yh;
					const UnsignedChunk chunk_xh_yl = chunk_xh * chunk_yl;

					acc.chunk_add_offset(chunk_xl_yl, offset);
					acc.chunk_add_offset(chunk_xl_yh << HALF_CHUNK_WIDTH_BITS, offset);
					acc.chunk_add_offset(chunk_xh_yl << HALF_CHUNK_WIDTH_BITS, offset);

					if(offset < (WIDTH_CHUNKS - 1u)) {
						const UnsignedChunk chunk_xh_yh = chunk_xh * chunk_yh;
						acc.chunk_add_offset(chunk_xl_yh >> HALF_CHUNK_WIDTH_BITS, offset + 1u);
						acc.chunk_add_offset(chunk_xh_yl >> HALF_CHUNK_WIDTH_BITS, offset + 1u);
						acc.chunk_add_offset(chunk_xh_yh, offset + 1u);
					}
				}
			}
		}
		*this = acc;
	}

	constexpr void mult_chunk(const UnsignedChunk& chunk) {
		BigInt acc;
		for(size_t idx = 0; idx < WIDTH_CHUNKS; ++idx) {
			const UnsignedChunk chunk_xl = (_number[idx] & HALF_CHUNK_LOW_MASK);
			const UnsignedChunk chunk_xh = (_number[idx] >> HALF_CHUNK_WIDTH_BITS);
			const UnsignedChunk chunk_yl = (chunk & HALF_CHUNK_LOW_MASK);
			const UnsignedChunk chunk_yh = (chunk >> HALF_CHUNK_WIDTH_BITS);

			const UnsignedChunk chunk_xl_yl = chunk_xl * chunk_yl;
			const UnsignedChunk chunk_xl_yh = chunk_xl * chunk_yh;
			const UnsignedChunk chunk_xh_yl = chunk_xh * chunk_yl;

			acc.chunk_add_offset(chunk_xl_yl, idx);
			acc.chunk_add_offset(chunk_xl_yh << HALF_CHUNK_WIDTH_BITS, idx);
			acc.chunk_add_offset(chunk_xh_yl << HALF_CHUNK_WIDTH_BITS, idx);

			if(idx < (WIDTH_CHUNKS - 1u)) {
				const UnsignedChunk chunk_xh_yh = chunk_xh * chunk_yh;
				acc.chunk_add_offset(chunk_xl_yh >> HALF_CHUNK_WIDTH_BITS, idx + 1u);
				acc.chunk_add_offset(chunk_xh_yl >> HALF_CHUNK_WIDTH_BITS, idx + 1u);
				acc.chunk_add_offset(chunk_xh_yh, idx + 1u);
			}
		}
		*this = acc;
	}

	constexpr void div(const BigInt& rv, BigInt& result_div) {
		const bool is_msbit = msbit() ^ rv.msbit();
		if(msbit()) {
			sign_flip();
		}
		BigInt denom(rv);
		if(denom.msbit()) {
			denom.sign_flip();
		}
		div_unsigned(denom, result_div);
		if(is_msbit){
			result_div.sign_flip();
		}
	}

	constexpr void div_unsigned(BigInt denom, BigInt& result_div) {
		BigInt num(*this);
		BigInt result;

		const size_t num_idx_hd_start = num.most_valuable_half_chunk_idx();
		const size_t denom_idx_hd_start = denom.most_valuable_half_chunk_idx();
		bool carry = false;

//		++_tmp_call_cnt; // TODO:

		if(num_idx_hd_start >= denom_idx_hd_start) {
			size_t shift_hd = num_idx_hd_start - denom_idx_hd_start;
			size_t idx_hd = num_idx_hd_start;
			denom.shift_left_unsigned(shift_hd * HALF_CHUNK_WIDTH_BITS);

			while(shift_hd < HALF_WIDTH_CHUNKS) {

				if(carry) {
					const size_t idx_hd_prev = idx_hd + 1u;

					while(num.half_chunk(idx_hd_prev)) {
						const UnsignedChunk div_floor = num.full_chunk(idx_hd_prev) / (denom.half_chunk(idx_hd) + 1u);

						BigInt mult_result = denom;
						mult_result.mult_chunk(div_floor);
						result.chunk_add(div_floor);
						num -= mult_result;

//						++_tmp_carry_mult_cnt; // TODO:
					}
					carry = false;
				}

				if(num.half_chunk(idx_hd)){
					const UnsignedChunk num_full_chunk = num.full_chunk(idx_hd);
					const UnsignedChunk denom_full_chunk = denom.full_chunk(idx_hd);

					if(num_full_chunk > denom_full_chunk) {
						const UnsignedChunk div_floor = num_full_chunk / (denom_full_chunk + 1u);
						BigInt mult_result = denom;
						mult_result.mult_chunk(div_floor);
						result.chunk_add(div_floor);
						num -= mult_result;

//						++_tmp_mult_cnt; // TODO:
					}

					while(not num.abs_val_less(denom)) {
						num -= denom;
						result.chunk_add(1u);
//						++_tmp_sub_cnt; // TODO:
					}

					carry = num.half_chunk(idx_hd);
				}

				if(shift_hd) {
					result.shift_left_unsigned(HALF_CHUNK_WIDTH_BITS);
					denom.shift_right_unsigned(HALF_CHUNK_WIDTH_BITS);
				}
				--shift_hd;
				--idx_hd;
			}

		}
		result_div = result;
	}

//	static void dump(const char* point, const size_t idx_hd, const BigInt& num, const BigInt& denom, const BigInt& result, bool carry) {
//		printf("[%s] idx_hd=%zu %s\n", point, idx_hd, carry ? "+carry" : "");
//		printf("num    ");
//		hex(num);
//		printf("denom  ");
//		hex(denom);
//		printf("result ");
//		hex(result);
//	}


	// ----------------------------------------------------------------------------
	// Helpers.
	// ----------------------------------------------------------------------------

	// Half chunk getters.
	constexpr UnsignedChunk half_chunk(const size_t idx_hd) const {
		UnsignedChunk result = _number[idx_hd / 2u];
		return ((idx_hd % 2u) == 0) ? (result & HALF_CHUNK_LOW_MASK) : (result >> HALF_CHUNK_WIDTH_BITS);
	}

	constexpr UnsignedChunk full_chunk(const size_t idx_hd) const {
		const size_t idx = idx_hd / 2u;
		UnsignedChunk result = _number[idx];
		if((idx_hd % 2u) == 0) {
			result <<= HALF_CHUNK_WIDTH_BITS;
			if(idx) {
				result |= (_number[idx-1] >> HALF_CHUNK_WIDTH_BITS);
			}
		}
		return result;
	}


	constexpr UnsignedChunk chunk_add(const UnsignedChunk& ch) {
		_number[0] += ch;
		UnsignedChunk carry = (_number[0] < ch);

		size_t idx = 1u;
		while(carry && (idx < WIDTH_CHUNKS)) {
			_number[idx] += carry;
			carry = (_number[idx] < carry);
			++idx;
		}
		return carry;
	}

	constexpr UnsignedChunk chunk_add_offset(const UnsignedChunk& ch, size_t idx) {
		_number[idx] += ch;
		UnsignedChunk carry = (_number[idx] < ch);
		++idx;

		while(carry && (idx < WIDTH_CHUNKS)) {
			_number[idx] += carry;
			carry = (_number[idx] < carry);
			++idx;
		}
		return carry;
	}

	constexpr size_t most_valuable_half_chunk_idx() const {
		size_t result = 0;
		size_t idx = WIDTH_CHUNKS - 1;
		while(idx < WIDTH_CHUNKS) {
			if(_number[idx] & HALF_CHUNK_HIGH_MASK) {
				result = idx * 2u + 1u;
				break;
			}
			
			if(_number[idx] & HALF_CHUNK_LOW_MASK) {
				result = idx * 2u;
				break;
			}
			
			--idx;
		}
		return result;
	}

	constexpr size_t last_diff_chunk(const BigInt& rv) const {
		size_t idx = WIDTH_CHUNKS - 1;
		while(idx && (_number[idx] == rv._number[idx])) {
			--idx;
		}
		return idx;
	}

	constexpr bool abs_val_less(const BigInt& rv) const {
		const size_t idx = last_diff_chunk(rv);
		return _number[idx] < rv._number[idx];
	}

	constexpr bool abs_val_greater(const BigInt& rv) const {
		const size_t idx = last_diff_chunk(rv);
		return _number[idx] > rv._number[idx];
	}

	constexpr void sign_flip() {
		bitwise_not();
		chunk_add(1);
	}

	constexpr bool msbit() const {
		return _number[WIDTH_CHUNKS - 1] & MSBIT_MASK;
	}

	const UnsignedChunk& operator[](const size_t idx) const {
		return _number[idx];
	}

};
