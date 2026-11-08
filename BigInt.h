#pragma once

#include <type_traits>
#include <string>
#include <array>
#include <climits>

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

	static_assert(std::is_integral_v<Chunk>, "BigInt: Chunk type MUST BE an integral type!");
	static_assert(std::is_signed_v<Chunk>, "BigInt: Chunk type MUST BE signed!");
	static_assert(WIDTH_CHUNKS > 0, "BigInt: WIDTH_CHUNKS is zero!");
	static_assert(WIDTH_CHUNKS <= (SIZE_MAX / 2u), "BigInt: WIDTH_CHUNKS exceeds the threshold!");

	template <typename C, size_t W>
	friend class BigInt;

public:

	using UChunk = std::make_unsigned_t<Chunk>;

	static constexpr size_t CHUNK_WIDTH_BYTES = sizeof(UChunk);
	static constexpr size_t CHUNK_WIDTH_BITS = CHUNK_WIDTH_BYTES * CHAR_BIT;
	static constexpr size_t HALF_WIDTH_CHUNKS = WIDTH_CHUNKS * 2u;
	static constexpr size_t HALF_CHUNK_WIDTH_BITS = CHUNK_WIDTH_BITS / 2u;
	static constexpr size_t NUMBER_WIDTH_CHUNKS = WIDTH_CHUNKS;
	static constexpr size_t NUMBER_WIDTH_BYTES = CHUNK_WIDTH_BYTES * WIDTH_CHUNKS;
	static constexpr size_t NUMBER_WIDTH_BITS = CHUNK_WIDTH_BITS * WIDTH_CHUNKS;
	
	static constexpr UChunk CHUNK_MASK = ~UChunk(0);
	static constexpr UChunk HALF_CHUNK_LOW_MASK = UChunk(CHUNK_MASK >> HALF_CHUNK_WIDTH_BITS);
	static constexpr UChunk HALF_CHUNK_HIGH_MASK = UChunk(CHUNK_MASK << HALF_CHUNK_WIDTH_BITS);
	static constexpr UChunk MSBIT_MASK = UChunk(CHUNK_MASK << (CHUNK_WIDTH_BITS - 1u));

protected:

	using Number_t = std::array<UChunk, WIDTH_CHUNKS>;
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
	constexpr BigInt(const BigInt<C, W>& rv) : _number{0}{
		load_big_int(rv);
	}

	// From wider BigInt. Explicit conversion is needed.
	template <typename C, size_t W, std::enable_if_t<((sizeof(C) * W) > NUMBER_WIDTH_BYTES), int> = 0>
	constexpr explicit BigInt(const BigInt<C, W>& rv) : _number{0}{
		load_big_int(rv);
	}

	// ----------------------------------------------------------------------------
	// Copying operators.
	// ----------------------------------------------------------------------------
	// From not wider integer.
	template <typename T, std::enable_if_t<(std::is_integral_v<T> && std::is_signed_v<T> && sizeof(T) <= NUMBER_WIDTH_BYTES), int> = 0>
	constexpr BigInt& operator=(const T& init) {
		size_t idx = 0;
		while(idx < WIDTH_CHUNKS) {
			_number[idx++] = 0;
		}
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
		result._number[WIDTH_CHUNKS - 1u] = UChunk(~MSBIT_MASK);
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
	constexpr BigInt operator~() const {
		BigInt result(*this);
		result.bitwise_not();
		return result;
	}

	constexpr BigInt& operator&=(const BigInt& rv) {
		bitwise_and(rv);
		return *this;
	}

	constexpr BigInt operator&(const BigInt& rv) const {
		BigInt result(*this);
		result.bitwise_and(rv);
		return result;
	}

	constexpr BigInt& operator|=(const BigInt& rv) {
		bitwise_or(rv);
		return *this;
	}

	constexpr BigInt operator|(const BigInt& rv) const {
		BigInt result(*this);
		result.bitwise_or(rv);
		return result;
	}

	constexpr BigInt& operator^=(const BigInt& rv) {
		bitwise_xor(rv);
		return *this;
	}

	constexpr BigInt operator^(const BigInt& rv) const {
		BigInt result(*this);
		result.bitwise_xor(rv);
		return result;
	}

	// Shift left.
	constexpr BigInt& operator<<=(const size_t shift) {
		shift_left(shift);
		return *this;
	}

	constexpr BigInt operator<<(const size_t shift) const {
		BigInt result(*this);
		result.shift_left(shift);
		return result;
	}

	// Arithmetic right shift.
	constexpr BigInt& operator>>=(const size_t shift) {
		shift_right(shift);
		return *this;
	}

	constexpr BigInt operator>>(const size_t shift) const {
		BigInt result(*this);
		result.shift_right(shift);
		return result;
	}

	// Logical right shift.
	constexpr BigInt& shr(const size_t shift) {
		shift_right_unsigned(shift);
		return *this;
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

	constexpr BigInt operator%(const BigInt& rv) const {
		BigInt result(*this);
		BigInt result_div;
		result.div(rv, result_div);
		return result;
	}

	constexpr BigInt& operator%=(const BigInt& rv) {
		BigInt result_div;
		div(rv, result_div);
		return *this;
	}

	constexpr bool msbit() const {
		return _number[WIDTH_CHUNKS - 1] & MSBIT_MASK;
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

	template <typename T, std::enable_if_t<(std::is_integral_v<T> && std::is_unsigned_v<T> && (sizeof(T) > sizeof(UChunk))), int> = 0>
	constexpr void load_integer_unsigned(T val) {
		size_t idx = 0;
		while((idx < WIDTH_CHUNKS) && (val != 0)) {
			_number[idx++] = val & CHUNK_MASK;
			val >>= CHUNK_WIDTH_BITS;
		}
	}

	template <typename T, std::enable_if_t<(std::is_integral_v<T> && std::is_unsigned_v<T> && (sizeof(T) <= sizeof(UChunk))), int> = 0>
	constexpr void load_integer_unsigned(const T val) {
		size_t idx = 0;
		_number[idx++] = val;
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

	template <typename T, std::enable_if_t<(std::is_integral_v<T> && std::is_unsigned_v<T> && (sizeof(T) > sizeof(UChunk))), int> = 0>
	constexpr void store_integer_unsigned(T& val) const {
		constexpr size_t CHUNKS_RATIO = sizeof(T) / sizeof(UChunk);
		size_t idx = std::min(WIDTH_CHUNKS, CHUNKS_RATIO) - 1u;
		val = 0;
		while(idx < WIDTH_CHUNKS) {
			val <<= CHUNK_WIDTH_BITS;
			val |= _number[idx--] & CHUNK_MASK;
		}
	}

	template <typename T, std::enable_if_t<(std::is_integral_v<T> && std::is_unsigned_v<T> && (sizeof(T) <= sizeof(UChunk))), int> = 0>
	constexpr void store_integer_unsigned(T& val) const {
		val = _number[0];
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
			bitwise_not();
		}
	}

	template <typename C, size_t W, std::enable_if_t<(sizeof(C) < sizeof(UChunk)), int> = 0>
	constexpr void load_big_int_unsigned(BigInt<C, W> rv) {
		static_assert((sizeof(UChunk) % (sizeof(C))) == 0);
		constexpr size_t CHUNKS_RATIO = sizeof(UChunk) / sizeof(C);

		size_t idx = 0;
		while(idx < WIDTH_CHUNKS) {
			_number[idx++] = 0;
		}

		const size_t src_width_chunks = rv.valuable_chunks();

		size_t src_idx = 0;
		size_t dst_idx = 0;
		while((dst_idx < WIDTH_CHUNKS) && (src_idx < src_width_chunks)) {
			const size_t src_shift = (src_idx % CHUNKS_RATIO) * BigInt<C, W>::CHUNK_WIDTH_BITS;
			UChunk chunk_part = rv._number[src_idx];
			chunk_part = (chunk_part << src_shift) & CHUNK_MASK;
			_number[dst_idx] |= chunk_part;
			++src_idx;
			dst_idx = src_idx / CHUNKS_RATIO;
		}

	}

	template <typename C, size_t W, std::enable_if_t<(sizeof(C) > sizeof(UChunk)), int> = 0>
	constexpr void load_big_int_unsigned(BigInt<C, W> rv) {
		static_assert((sizeof(C) % sizeof(UChunk)) == 0);

		constexpr size_t CHUNKS_RATIO = sizeof(C) / sizeof(UChunk);
		const size_t src_width_chunks = rv.valuable_chunks();

		size_t src_idx = 0;
		size_t dst_idx = 0;
		while((dst_idx < WIDTH_CHUNKS) && (src_idx < src_width_chunks)) {
			const size_t src_shift = (dst_idx % CHUNKS_RATIO) * CHUNK_WIDTH_BITS;
			_number[dst_idx] = (rv._number[src_idx] >> src_shift);
			++dst_idx;
			src_idx = dst_idx / CHUNKS_RATIO;
		}

		while(dst_idx < WIDTH_CHUNKS) {
			_number[dst_idx++] = 0;
		}
	}

	template <typename C, size_t W, std::enable_if_t<(sizeof(C) == sizeof(UChunk)), int> = 0>
	constexpr void load_big_int_unsigned(BigInt<C, W> bi) {
		const size_t width = std::min(W, WIDTH_CHUNKS);

		size_t idx = 0;
		while(idx < width) {
			_number[idx] = bi._number[idx];
			++idx;
		}
		while(idx < WIDTH_CHUNKS) {
			_number[idx++] = 0;
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

	constexpr void shift_left(size_t shift_bits) {
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
	constexpr UChunk add(const BigInt& rv) {
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
					const UChunk chunk_xl = (_number[i] & HALF_CHUNK_LOW_MASK);
					const UChunk chunk_xh = (_number[i] >> HALF_CHUNK_WIDTH_BITS);
					const UChunk chunk_yl = (rv._number[j] & HALF_CHUNK_LOW_MASK);
					const UChunk chunk_yh = (rv._number[j] >> HALF_CHUNK_WIDTH_BITS);

					const UChunk chunk_xl_yl = chunk_xl * chunk_yl;
					const UChunk chunk_xl_yh = chunk_xl * chunk_yh;
					const UChunk chunk_xh_yl = chunk_xh * chunk_yl;

					acc.chunk_add_offset(chunk_xl_yl, offset);
					acc.chunk_add_offset(chunk_xl_yh << HALF_CHUNK_WIDTH_BITS, offset);
					acc.chunk_add_offset(chunk_xh_yl << HALF_CHUNK_WIDTH_BITS, offset);

					if(offset < (WIDTH_CHUNKS - 1u)) {
						const UChunk chunk_xh_yh = chunk_xh * chunk_yh;
						acc.chunk_add_offset(chunk_xl_yh >> HALF_CHUNK_WIDTH_BITS, offset + 1u);
						acc.chunk_add_offset(chunk_xh_yl >> HALF_CHUNK_WIDTH_BITS, offset + 1u);
						acc.chunk_add_offset(chunk_xh_yh, offset + 1u);
					}
				}
			}
		}
		*this = acc;
	}

	constexpr void mult_chunk(const UChunk& chunk) {
		BigInt acc;
		for(size_t idx = 0; idx < WIDTH_CHUNKS; ++idx) {
			const UChunk chunk_xl = (_number[idx] & HALF_CHUNK_LOW_MASK);
			const UChunk chunk_xh = (_number[idx] >> HALF_CHUNK_WIDTH_BITS);
			const UChunk chunk_yl = (chunk & HALF_CHUNK_LOW_MASK);
			const UChunk chunk_yh = (chunk >> HALF_CHUNK_WIDTH_BITS);

			const UChunk chunk_xl_yl = chunk_xl * chunk_yl;
			const UChunk chunk_xl_yh = chunk_xl * chunk_yh;
			const UChunk chunk_xh_yl = chunk_xh * chunk_yl;

			acc.chunk_add_offset(chunk_xl_yl, idx);
			acc.chunk_add_offset(chunk_xl_yh << HALF_CHUNK_WIDTH_BITS, idx);
			acc.chunk_add_offset(chunk_xh_yl << HALF_CHUNK_WIDTH_BITS, idx);

			if(idx < (WIDTH_CHUNKS - 1u)) {
				const UChunk chunk_xh_yh = chunk_xh * chunk_yh;
				acc.chunk_add_offset(chunk_xl_yh >> HALF_CHUNK_WIDTH_BITS, idx + 1u);
				acc.chunk_add_offset(chunk_xh_yl >> HALF_CHUNK_WIDTH_BITS, idx + 1u);
				acc.chunk_add_offset(chunk_xh_yh, idx + 1u);
			}
		}
		*this = acc;
	}

	constexpr void div(const BigInt& rv, BigInt& result_div) {
		const bool mod_msbit = msbit();
		const bool div_msbit = msbit() ^ rv.msbit();
		if(msbit()) {
			sign_flip();
		}
		BigInt denom(rv);
		if(denom.msbit()) {
			denom.sign_flip();
		}
		div_unsigned(denom, result_div);
		if(mod_msbit){
			sign_flip();
		}
		if(div_msbit){
			result_div.sign_flip();
		}
	}

	constexpr void div_unsigned(BigInt denom, BigInt& result_div) {
		BigInt num(*this);
		BigInt result;

		const size_t num_idx_hlf_start = num.most_valuable_half_chunks_idx();
		const size_t denom_idx_hlf_start = denom.most_valuable_half_chunks_idx();
		bool carry = false;

		if(num_idx_hlf_start >= denom_idx_hlf_start) {
			size_t shift_hd = num_idx_hlf_start - denom_idx_hlf_start;
			size_t idx_hlf = num_idx_hlf_start;
			denom.shift_left(shift_hd * HALF_CHUNK_WIDTH_BITS);

			while(shift_hd < HALF_WIDTH_CHUNKS) {
				if(carry) {
					const size_t idx_hlf_prev = idx_hlf + 1u;

					while(num.half_chunk(idx_hlf_prev)) {
						const UChunk div_floor = num.full_chunk(idx_hlf_prev) / (denom.half_chunk(idx_hlf) + 1u);
						BigInt mult_result = denom;
						mult_result.mult_chunk(div_floor);
						result.chunk_add_offset(div_floor, 0);
						num -= mult_result;
					}
					carry = false;
				}

				if(num.half_chunk(idx_hlf)){
					const UChunk num_full_chunk = num.full_chunk(idx_hlf);
					const UChunk denom_full_chunk = denom.full_chunk(idx_hlf);

					if(num_full_chunk > denom_full_chunk) {
						const UChunk div_floor = num_full_chunk / (denom_full_chunk + 1u);
						BigInt mult_result = denom;
						mult_result.mult_chunk(div_floor);
						result.chunk_add_offset(div_floor, 0);
						num -= mult_result;
					}

					while(not num.abs_val_less(denom)) {
						num -= denom;
						result.chunk_add_offset(1u, 0);
					}

					carry = num.half_chunk(idx_hlf);
				}

				if(shift_hd) {
					result.shift_left(HALF_CHUNK_WIDTH_BITS);
					denom.shift_right_unsigned(HALF_CHUNK_WIDTH_BITS);
				}
				--shift_hd;
				--idx_hlf;
			}

		}
		result_div = result;
		*this = num;
	}


	// ----------------------------------------------------------------------------
	// Helpers.
	// ----------------------------------------------------------------------------

	// Half chunk getters.
	constexpr UChunk half_chunk(const size_t idx_hlf) const {
		UChunk result = _number[idx_hlf / 2u];
		return ((idx_hlf % 2u) == 0) ? (result & HALF_CHUNK_LOW_MASK) : (result >> HALF_CHUNK_WIDTH_BITS);
	}

	constexpr UChunk full_chunk(const size_t idx_hlf) const {
		const size_t idx = idx_hlf / 2u;
		UChunk result = _number[idx];
		if((idx_hlf % 2u) == 0) {
			result <<= HALF_CHUNK_WIDTH_BITS;
			if(idx) {
				result |= (_number[idx-1] >> HALF_CHUNK_WIDTH_BITS);
			}
		}
		return result;
	}


	constexpr UChunk chunk_add_offset(const UChunk& ch, size_t idx) {
		_number[idx] += ch;
		UChunk carry = (_number[idx] < ch);
		++idx;

		while(carry && (idx < WIDTH_CHUNKS)) {
			_number[idx] += carry;
			carry = (_number[idx] < carry);
			++idx;
		}
		return carry;
	}

	constexpr size_t valuable_chunks() const {
		size_t result = 0;
		size_t idx = WIDTH_CHUNKS - 1u;
		while(idx < WIDTH_CHUNKS) {
			if(_number[idx]) {
				result = idx + 1u;
				break;
			}
			--idx;
		}
		return result;
	}

	constexpr size_t most_valuable_half_chunks_idx() const {
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
		chunk_add_offset(1, 0);
	}

};
