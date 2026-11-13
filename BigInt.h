#pragma once

#include <cstddef>
#include <cstdint>
#include <type_traits>
#include <climits>

#undef BIG_INT_CONSTEXPR
#if __cplusplus >= 201402L
#define BIG_INT_CONSTEXPR constexpr
#else
#define BIG_INT_CONSTEXPR
#endif

/**
 * @tparam WIDTH_BITS
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
template <size_t WIDTH_BITS>
struct BigInt {

	static_assert(WIDTH_BITS > 0, "BigInt: WIDTH_BITS must be greater than zero!");
	static_assert((WIDTH_BITS % CHAR_BIT) == 0, "BigInt: WIDTH_BITS must be a multiple of CHAR_BIT!");

	template <size_t W>
	friend struct BigInt;

	static constexpr size_t NUMBER_WIDTH_BITS = WIDTH_BITS;
	static constexpr size_t NUMBER_WIDTH_BYTES = NUMBER_WIDTH_BITS / CHAR_BIT;

	using UChunk =
		typename std::conditional<((NUMBER_WIDTH_BYTES % sizeof(unsigned long long)) == 0), unsigned long long,
			typename std::conditional<((NUMBER_WIDTH_BYTES % sizeof(unsigned long)) == 0), unsigned long,
				typename std::conditional<((NUMBER_WIDTH_BYTES % sizeof(unsigned int)) == 0), unsigned int,
					typename std::conditional<((NUMBER_WIDTH_BYTES % sizeof(unsigned short)) == 0), unsigned short,
						typename std::conditional<((NUMBER_WIDTH_BYTES % sizeof(unsigned char)) == 0), unsigned char, void>::type
				    >::type
				>::type
			>::type
		>::type;

	static constexpr size_t CHUNK_WIDTH_BYTES = sizeof(UChunk);
	static constexpr size_t CHUNK_WIDTH_BITS = CHUNK_WIDTH_BYTES * CHAR_BIT;
	static constexpr size_t NUMBER_WIDTH_CHUNKS = NUMBER_WIDTH_BYTES / CHUNK_WIDTH_BYTES;
	static constexpr size_t HALF_WIDTH_CHUNKS = NUMBER_WIDTH_CHUNKS * 2u;
	static constexpr size_t HALF_CHUNK_WIDTH_BITS = CHUNK_WIDTH_BITS / 2u;
	
	static constexpr UChunk CHUNK_MASK = ~UChunk(0);
	static constexpr UChunk HALF_CHUNK_LOW_MASK = UChunk(CHUNK_MASK >> HALF_CHUNK_WIDTH_BITS);
	static constexpr UChunk HALF_CHUNK_HIGH_MASK = UChunk(CHUNK_MASK << HALF_CHUNK_WIDTH_BITS);
	static constexpr UChunk MSBIT_MASK = UChunk(CHUNK_MASK << (CHUNK_WIDTH_BITS - 1u));

	using Number_t = UChunk[NUMBER_WIDTH_CHUNKS];

	Number_t number;

	// ----------------------------------------------------------------------------
	// Constructors.
	// ----------------------------------------------------------------------------
	BIG_INT_CONSTEXPR BigInt() : number{0} {}

	BIG_INT_CONSTEXPR BigInt(const BigInt& rv) = default;
	BIG_INT_CONSTEXPR BigInt& operator=(const BigInt& rv) = default;
	BigInt(BigInt&& rv) = default;
	BigInt& operator=(BigInt&& rv) = default;

	template <typename T, typename std::enable_if<(std::is_integral<T>::value && std::is_signed<T>::value && sizeof(T) <= NUMBER_WIDTH_BYTES), int>::type = 0>
	BIG_INT_CONSTEXPR BigInt(const T& init) : number{0} {
		load_integer(init);
	}

	template <typename T, typename std::enable_if<(std::is_integral<T>::value && std::is_signed<T>::value && sizeof(T) > NUMBER_WIDTH_BYTES), int>::type = 0>
	BIG_INT_CONSTEXPR explicit BigInt(const T& init) : number{0} {
		load_integer(init);
	}

	template <size_t W>
	BIG_INT_CONSTEXPR explicit BigInt(const BigInt<W>& rv) : number{0}{
		load_big_int(rv);
	}

	// ----------------------------------------------------------------------------
	// Copying operators.
	// ----------------------------------------------------------------------------
	// From not wider integer.
	template <typename T, typename std::enable_if<(std::is_integral<T>::value && std::is_signed<T>::value && sizeof(T) <= NUMBER_WIDTH_BYTES), int>::type = 0>
	BIG_INT_CONSTEXPR BigInt& operator=(const T& init) {
		size_t idx = 0;
		while(idx < NUMBER_WIDTH_CHUNKS) {
			number[idx++] = 0;
		}
		load_integer(init);
		return *this;
	}

	// From not wider BigInt.
	template <size_t W, typename std::enable_if<(W <= NUMBER_WIDTH_BITS), int>::type = 0>
	BIG_INT_CONSTEXPR BigInt& operator=(const BigInt<W>& rv) {
		load_big_int(rv);
		return *this;
	}

	// ----------------------------------------------------------------------------
	// Bounds and limits.
	// ----------------------------------------------------------------------------

	// The maximum value the BigInt can store.
	static BIG_INT_CONSTEXPR BigInt max() {
		BigInt result;
		result.bitwise_not();
		result.number[NUMBER_WIDTH_CHUNKS - 1u] = UChunk(~MSBIT_MASK);
		return result;
	}

	// The minimum value the FixPoint can store.
	static BIG_INT_CONSTEXPR BigInt min() {
		BigInt result;
		result.number[NUMBER_WIDTH_CHUNKS - 1u] = MSBIT_MASK;
		return result;
	}

	// ----------------------------------------------------------------------------
	// Type conversion.
	// ----------------------------------------------------------------------------
	template <typename T, typename std::enable_if<(std::is_integral<T>::value && std::is_signed<T>::value), int>::type = 0>
	BIG_INT_CONSTEXPR explicit operator T() const {
		T result;
		store_integer(result);
		return result;
	}

	// ----------------------------------------------------------------------------
	// Bitwise operators.
	// ----------------------------------------------------------------------------
	BIG_INT_CONSTEXPR BigInt operator~() const {
		BigInt result(*this);
		result.bitwise_not();
		return result;
	}

	BIG_INT_CONSTEXPR BigInt& operator&=(const BigInt& rv) {
		bitwise_and(rv);
		return *this;
	}

	BIG_INT_CONSTEXPR BigInt& operator|=(const BigInt& rv) {
		bitwise_or(rv);
		return *this;
	}

	BIG_INT_CONSTEXPR BigInt& operator^=(const BigInt& rv) {
		bitwise_xor(rv);
		return *this;
	}

	BIG_INT_CONSTEXPR BigInt operator&(BigInt rv) const {
		return rv.operator&=(*this);
	}

	BIG_INT_CONSTEXPR BigInt operator|(BigInt rv) const {
		return rv.operator|=(*this);
	}

	BIG_INT_CONSTEXPR BigInt operator^(BigInt rv) const {
		return rv.operator^=(*this);
	}

	// Shift left.
	BIG_INT_CONSTEXPR BigInt& operator<<=(const size_t shift) {
		shift_left(shift);
		return *this;
	}

	BIG_INT_CONSTEXPR BigInt operator<<(const size_t shift) const {
		BigInt result(*this);
		return result.operator<<=(shift);
	}

	// Arithmetic right shift.
	BIG_INT_CONSTEXPR BigInt& operator>>=(const size_t shift) {
		shift_right(shift);
		return *this;
	}

	BIG_INT_CONSTEXPR BigInt operator>>(const size_t shift) const {
		BigInt result(*this);
		return result.operator>>=(shift);
	}

	// Logical right shift.
	BIG_INT_CONSTEXPR BigInt& shr(const size_t shift) {
		shift_right_unsigned(shift);
		return *this;
	}

	// ----------------------------------------------------------------------------
	// Relational functions.
	// ----------------------------------------------------------------------------
	BIG_INT_CONSTEXPR bool operator==(const BigInt& rv) const {
		const size_t idx = last_diff_chunk(rv);
		return number[idx] == rv.number[idx];
	}

	BIG_INT_CONSTEXPR bool operator!=(const BigInt& rv) const {
		return !operator==(rv);
	}

	BIG_INT_CONSTEXPR bool operator<(const BigInt& rv) const {
		return (msbit() == rv.msbit()) ? cmp_unsigned_less(rv) : msbit();
	}

	BIG_INT_CONSTEXPR bool operator>(const BigInt& rv) const {
		return (msbit() == rv.msbit()) ? cmp_unsigned_greater(rv) : (!msbit());
	}

	BIG_INT_CONSTEXPR bool operator<=(const BigInt& rv) const {
		return !operator>(rv);
	}

	BIG_INT_CONSTEXPR bool operator>=(const BigInt& rv) const {
		return !operator<(rv);
	}


	// ----------------------------------------------------------------------------
	// Arithmetic operators.
	// ----------------------------------------------------------------------------

	// Sign change operations.
	BIG_INT_CONSTEXPR BigInt abs() const {
		BigInt result(*this);
		if(msbit()) {
			result.sign_flip();
		}
		return result;
	}

	BIG_INT_CONSTEXPR BigInt operator-() const {
		BigInt result(*this);
		result.sign_flip();
		return result;
	}

	// Increment.
	BIG_INT_CONSTEXPR BigInt& operator++() {
		chunk_add_offset(1u, 0);
		return *this;
	}

	BIG_INT_CONSTEXPR BigInt operator++(int) {
		BigInt result(*this);
		operator++();
		return result;
	}

	// Decrement.
	BIG_INT_CONSTEXPR BigInt& operator--() {
		bitwise_not();
		chunk_add_offset(1u, 0);
		bitwise_not();
		return *this;
	}

	BIG_INT_CONSTEXPR BigInt operator--(int) {
		BigInt result(*this);
		operator--();
		return result;
	}

	// Addition.
	BIG_INT_CONSTEXPR BigInt& operator+=(const BigInt& rv) {
		add(rv);
		return *this;
	}

	BIG_INT_CONSTEXPR BigInt operator+(BigInt rv) const {
		return rv.operator+=(*this);
	}

	// Subtraction.
	BIG_INT_CONSTEXPR BigInt& operator-=(const BigInt& rv) {
		add(-rv);
		return *this;
	}

	BIG_INT_CONSTEXPR BigInt operator-(const BigInt& rv) const {
		BigInt result(*this);
		return result.operator-=(rv);
	}

	// Multiplication.
	BIG_INT_CONSTEXPR BigInt& operator*=(const BigInt& rv) {
		mult(rv);
		return *this;
	}

	BIG_INT_CONSTEXPR BigInt operator*(BigInt rv) const {
		return rv.operator*=(*this);
	}

	// Division.
	BIG_INT_CONSTEXPR BigInt& operator/=(const BigInt& rv) {
		BigInt div_result;
		div(rv, div_result);
		*this = div_result;
		return *this;
	}

	BIG_INT_CONSTEXPR BigInt operator/(const BigInt& rv) const {
		BigInt result(*this);
		return result.operator/=(rv);
	}

	BIG_INT_CONSTEXPR BigInt& operator%=(const BigInt& rv) {
		BigInt div_result;
		div(rv, div_result);
		return *this;
	}

	BIG_INT_CONSTEXPR BigInt operator%(const BigInt& rv) const {
		BigInt result(*this);
		return result.operator%=(rv);
	}

	BIG_INT_CONSTEXPR bool msbit() const {
		return number[NUMBER_WIDTH_CHUNKS - 1u] & MSBIT_MASK;
	}

protected:

	// Load integers.
	template <typename T>
	BIG_INT_CONSTEXPR void load_integer(T val) {
		if(val < 0) {
			val = ~val;
			load_integer_unsigned(static_cast<typename std::make_unsigned<T>::type>(val));
			bitwise_not();
		} else {
			load_integer_unsigned(static_cast<typename std::make_unsigned<T>::type>(val));
		}
	}

	template <typename T, typename std::enable_if<(sizeof(T) > sizeof(UChunk)), int>::type = 0>
	BIG_INT_CONSTEXPR void load_integer_unsigned(T val) {
		size_t idx = 0;
		while((idx < NUMBER_WIDTH_CHUNKS) && (val != 0)) {
			number[idx++] = val & CHUNK_MASK;
			val >>= CHUNK_WIDTH_BITS;
		}
	}

	template <typename T, typename std::enable_if<(sizeof(T) <= sizeof(UChunk)), int>::type = 0>
	BIG_INT_CONSTEXPR void load_integer_unsigned(const T val) {
		size_t idx = 0;
		number[idx++] = val;
	}

	// Store integers.
	template <typename T>
	BIG_INT_CONSTEXPR void store_integer(T& val) const {
		typename std::make_unsigned<T>::type uval;
		if(msbit()) {
			BigInt tmp(*this);
			tmp.bitwise_not();
			tmp.store_integer_unsigned(uval);
			uval = ~uval;
		} else {
			store_integer_unsigned(uval);
		}
		val = static_cast<T>(uval);
	}

	template <typename T, typename std::enable_if<(sizeof(T) > sizeof(UChunk)), int>::type = 0>
	BIG_INT_CONSTEXPR void store_integer_unsigned(T& val) const {
		constexpr size_t CHUNKS_RATIO = sizeof(T) / sizeof(UChunk);
		size_t idx = ((NUMBER_WIDTH_CHUNKS < CHUNKS_RATIO) ? NUMBER_WIDTH_CHUNKS : CHUNKS_RATIO) - 1u;
		val = 0;
		while(idx < NUMBER_WIDTH_CHUNKS) {
			val <<= CHUNK_WIDTH_BITS;
			val |= number[idx--] & CHUNK_MASK;
		}
	}

	template <typename T, typename std::enable_if<(sizeof(T) <= sizeof(UChunk)), int>::type = 0>
	BIG_INT_CONSTEXPR void store_integer_unsigned(T& val) const {
		val = number[0];
	}

	// Load BigInt.
	template <size_t W>
	BIG_INT_CONSTEXPR void load_big_int(BigInt<W> bi) {
		const bool msb = bi.msbit();
		if(msb) {
			bi.bitwise_not();
		}
		load_big_int_unsigned(bi);
		if(msb) {
			bitwise_not();
		}
	}

	template <size_t W, typename std::enable_if<(BigInt<W>::CHUNK_WIDTH_BYTES < CHUNK_WIDTH_BYTES), int>::type = 0>
	BIG_INT_CONSTEXPR void load_big_int_unsigned(BigInt<W> rv) {
		constexpr size_t CHUNKS_RATIO = CHUNK_WIDTH_BYTES / BigInt<W>::CHUNK_WIDTH_BYTES;

		size_t idx = 0;
		while(idx < NUMBER_WIDTH_CHUNKS) {
			number[idx++] = 0;
		}

		const size_t src_width_chunks = rv.valuable_chunks();

		size_t src_idx = 0;
		size_t dst_idx = 0;
		while((dst_idx < NUMBER_WIDTH_CHUNKS) && (src_idx < src_width_chunks)) {
			const size_t src_shift = (src_idx % CHUNKS_RATIO) * BigInt<W>::CHUNK_WIDTH_BITS;
			UChunk chunk_part = rv.number[src_idx];
			chunk_part = (chunk_part << src_shift) & CHUNK_MASK;
			number[dst_idx] |= chunk_part;
			++src_idx;
			dst_idx = src_idx / CHUNKS_RATIO;
		}

	}

	template <size_t W, typename std::enable_if<(BigInt<W>::CHUNK_WIDTH_BYTES > CHUNK_WIDTH_BYTES), int>::type = 0>
	BIG_INT_CONSTEXPR void load_big_int_unsigned(BigInt<W> rv) {
		constexpr size_t CHUNKS_RATIO = BigInt<W>::CHUNK_WIDTH_BYTES / CHUNK_WIDTH_BYTES;
		const size_t src_width_chunks = rv.valuable_chunks();

		size_t src_idx = 0;
		size_t dst_idx = 0;
		while((dst_idx < NUMBER_WIDTH_CHUNKS) && (src_idx < src_width_chunks)) {
			const size_t src_shift = (dst_idx % CHUNKS_RATIO) * CHUNK_WIDTH_BITS;
			number[dst_idx] = (rv.number[src_idx] >> src_shift);
			++dst_idx;
			src_idx = dst_idx / CHUNKS_RATIO;
		}

		while(dst_idx < NUMBER_WIDTH_CHUNKS) {
			number[dst_idx++] = 0;
		}
	}

	template <size_t W, typename std::enable_if<(BigInt<W>::CHUNK_WIDTH_BYTES == CHUNK_WIDTH_BYTES), int>::type = 0>
	BIG_INT_CONSTEXPR void load_big_int_unsigned(BigInt<W> bi) {
		const size_t width = ((BigInt<W>::NUMBER_WIDTH_CHUNKS < NUMBER_WIDTH_CHUNKS) ? BigInt<W>::NUMBER_WIDTH_CHUNKS : NUMBER_WIDTH_CHUNKS);

		size_t idx = 0;
		while(idx < width) {
			number[idx] = bi.number[idx];
			++idx;
		}
		while(idx < NUMBER_WIDTH_CHUNKS) {
			number[idx++] = 0;
		}
	}
	
	// Bitwise operators.
	BIG_INT_CONSTEXPR void bitwise_not() {
		for(size_t idx = 0; idx < NUMBER_WIDTH_CHUNKS; ++idx) {
			number[idx] = ~number[idx];
		}
	}

	BIG_INT_CONSTEXPR void bitwise_and(const BigInt& rv) {
		for(size_t idx = 0; idx < NUMBER_WIDTH_CHUNKS; ++idx) {
			number[idx] &= rv.number[idx];
		}
	}

	BIG_INT_CONSTEXPR void bitwise_or(const BigInt& rv) {
		for(size_t idx = 0; idx < NUMBER_WIDTH_CHUNKS; ++idx) {
			number[idx] |= rv.number[idx];
		}
	}

	BIG_INT_CONSTEXPR void bitwise_xor(const BigInt& rv) {
		for(size_t idx = 0; idx < NUMBER_WIDTH_CHUNKS; ++idx) {
			number[idx] ^= rv.number[idx];
		}
	}

	BIG_INT_CONSTEXPR void shift_left(size_t shift_bits) {
		shift_bits = (shift_bits < WIDTH_BITS) ? shift_bits : WIDTH_BITS;
		const size_t shift_chunks = shift_bits / CHUNK_WIDTH_BITS;
		
		// Shifting chunks.
		size_t dst = NUMBER_WIDTH_CHUNKS - 1u;
		size_t src = dst - shift_chunks;
		while(src < NUMBER_WIDTH_CHUNKS) {
			number[dst] = number[src];
			--dst;
			--src;
		}
		while(dst < NUMBER_WIDTH_CHUNKS) {
			number[dst] = 0;
			--dst;
		}

		// Shifting bits.
		shift_bits = shift_bits % CHUNK_WIDTH_BITS;
		const size_t shift_bits_compl = CHUNK_WIDTH_BITS - shift_bits;

		if(shift_bits) {
			size_t idx = NUMBER_WIDTH_CHUNKS - 1u;
			while(idx) {
				number[idx] = (number[idx] << shift_bits) | (number[idx - 1u] >> shift_bits_compl);
				--idx;
			}
			number[0] = (number[0] << shift_bits);
		}

	}

	BIG_INT_CONSTEXPR void shift_right(const size_t shift_bits) {
		if(msbit()) {
			bitwise_not();
			shift_right_unsigned(shift_bits);
			bitwise_not();
		} else {
			shift_right_unsigned(shift_bits);
		}
	}

	BIG_INT_CONSTEXPR void shift_right_unsigned(size_t shift_bits) {
		shift_bits = (shift_bits < WIDTH_BITS) ? shift_bits : WIDTH_BITS;
		const size_t shift_chunks = shift_bits / CHUNK_WIDTH_BITS;

		// Shifting chunks.
		size_t dst = 0;
		size_t src = shift_chunks;
		while(src < NUMBER_WIDTH_CHUNKS) {
			number[dst] = number[src];
			++dst;
			++src;
		}
		while(dst < NUMBER_WIDTH_CHUNKS) {
			number[dst] = 0;
			++dst;
		}

		// Shifting bits.
		shift_bits = shift_bits % CHUNK_WIDTH_BITS;
		const size_t shift_bits_compl = CHUNK_WIDTH_BITS - shift_bits;

		if(shift_bits) {
			size_t idx = 0;
			while(idx < (NUMBER_WIDTH_CHUNKS - 1u)) {
				number[idx] = (number[idx] >> shift_bits) | (number[idx + 1u] << shift_bits_compl);
				++idx;
			}
			number[idx] = (number[idx] >> shift_bits);
		}

	}

	// Arithmetic operators.
	BIG_INT_CONSTEXPR void add(const BigInt& rv) {
		UChunk carry = 0;
		for(size_t idx = 0; idx < NUMBER_WIDTH_CHUNKS; ++idx) {
			UChunk acc = number[idx];
			acc += rv.number[idx];
			const bool hcarry = acc < rv.number[idx];
			acc += carry;
			carry = hcarry | (acc < carry);
			number[idx] = acc;
		}
	}

	BIG_INT_CONSTEXPR void mult(const BigInt& rv) {
		BigInt acc;
		const size_t val_ch = valuable_chunks();
		const size_t rv_val_ch = rv.valuable_chunks();
		for(size_t i = 0; i < val_ch; ++i) {
			for(size_t j = 0; j < rv_val_ch; ++j) {
				const size_t offset = i + j;
				if(offset < NUMBER_WIDTH_CHUNKS) {
					const UChunk chunk_xl = (number[i] & HALF_CHUNK_LOW_MASK);
					const UChunk chunk_xh = (number[i] >> HALF_CHUNK_WIDTH_BITS);
					const UChunk chunk_yl = (rv.number[j] & HALF_CHUNK_LOW_MASK);
					const UChunk chunk_yh = (rv.number[j] >> HALF_CHUNK_WIDTH_BITS);

					const UChunk chunk_xl_yl = chunk_xl * chunk_yl;
					const UChunk chunk_xl_yh = chunk_xl * chunk_yh;
					const UChunk chunk_xh_yl = chunk_xh * chunk_yl;

					acc.chunk_add_offset(chunk_xl_yl, offset);
					acc.chunk_add_offset(chunk_xl_yh << HALF_CHUNK_WIDTH_BITS, offset);
					acc.chunk_add_offset(chunk_xh_yl << HALF_CHUNK_WIDTH_BITS, offset);

					if(offset < (NUMBER_WIDTH_CHUNKS - 1u)) {
						const UChunk chunk_xh_yh = chunk_xh * chunk_yh;
						const size_t offset_next = offset + 1u;
						acc.chunk_add_offset(chunk_xl_yh >> HALF_CHUNK_WIDTH_BITS, offset_next);
						acc.chunk_add_offset(chunk_xh_yl >> HALF_CHUNK_WIDTH_BITS, offset_next);
						acc.chunk_add_offset(chunk_xh_yh, offset_next);
					}
				}
			}
		}
		*this = acc;
	}

	BIG_INT_CONSTEXPR void mult_chunk(const UChunk& chunk) {
		BigInt acc;
		const size_t val_ch = valuable_chunks();
		for(size_t idx = 0; idx < val_ch; ++idx) {
			const UChunk chunk_xl = (number[idx] & HALF_CHUNK_LOW_MASK);
			const UChunk chunk_xh = (number[idx] >> HALF_CHUNK_WIDTH_BITS);
			const UChunk chunk_yl = (chunk & HALF_CHUNK_LOW_MASK);
			const UChunk chunk_yh = (chunk >> HALF_CHUNK_WIDTH_BITS);

			const UChunk chunk_xl_yl = chunk_xl * chunk_yl;
			const UChunk chunk_xl_yh = chunk_xl * chunk_yh;
			const UChunk chunk_xh_yl = chunk_xh * chunk_yl;

			acc.chunk_add_offset(chunk_xl_yl, idx);
			acc.chunk_add_offset(chunk_xl_yh << HALF_CHUNK_WIDTH_BITS, idx);
			acc.chunk_add_offset(chunk_xh_yl << HALF_CHUNK_WIDTH_BITS, idx);

			if(idx < (NUMBER_WIDTH_CHUNKS - 1u)) {
				const UChunk chunk_xh_yh = chunk_xh * chunk_yh;
				const size_t idx_next = idx + 1u;
				acc.chunk_add_offset(chunk_xl_yh >> HALF_CHUNK_WIDTH_BITS, idx_next);
				acc.chunk_add_offset(chunk_xh_yl >> HALF_CHUNK_WIDTH_BITS, idx_next);
				acc.chunk_add_offset(chunk_xh_yh, idx_next);
			}
		}
		*this = acc;
	}

	BIG_INT_CONSTEXPR void div(const BigInt& rv, BigInt& result_div) {
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

	BIG_INT_CONSTEXPR void div_unsigned(BigInt denom, BigInt& result) {
		const size_t num_idx_hlf_start = most_valuable_half_chunk_idx();
		const size_t denom_idx_hlf_start = denom.most_valuable_half_chunk_idx();
		bool carry = false;

		if((denom_idx_hlf_start == 0) && (denom.number[0] == 0)) {
			// Generating SIGFPE. Division by zero on purpose.
			number[0] /= denom.number[0];
		}

		if(num_idx_hlf_start >= denom_idx_hlf_start) {
			size_t shift_hd = num_idx_hlf_start - denom_idx_hlf_start;
			size_t idx_hlf = num_idx_hlf_start;
			denom.shift_left(shift_hd * HALF_CHUNK_WIDTH_BITS);

			while(shift_hd < HALF_WIDTH_CHUNKS) {
				if(carry) {
					const size_t idx_hlf_next = idx_hlf + 1u;

					while(half_chunk(idx_hlf_next)) {
						const UChunk div_floor = full_chunk(idx_hlf_next) / (denom.half_chunk(idx_hlf) + 1u);
						BigInt mult_result = denom;
						mult_result.mult_chunk(div_floor);
						result.chunk_add_offset(div_floor, 0);
						operator-=(mult_result);
					}
					carry = false;
				}

				if(half_chunk(idx_hlf)){
					const UChunk num_full_chunk = full_chunk(idx_hlf);
					const UChunk denom_full_chunk = denom.full_chunk(idx_hlf);

					if(num_full_chunk > denom_full_chunk) {
						const UChunk div_floor = num_full_chunk / (denom_full_chunk + 1u);
						BigInt mult_result = denom;
						mult_result.mult_chunk(div_floor);
						result.chunk_add_offset(div_floor, 0);
						operator-=(mult_result);
					}

					while(!cmp_unsigned_less(denom)) {
						operator-=(denom);
						result.chunk_add_offset(1u, 0);
					}

					carry = half_chunk(idx_hlf);
				}

				if(shift_hd) {
					result.shift_left(HALF_CHUNK_WIDTH_BITS);
					denom.shift_right_unsigned(HALF_CHUNK_WIDTH_BITS);
				}
				--shift_hd;
				--idx_hlf;
			}

		}
	}


	// ----------------------------------------------------------------------------
	// Helpers.
	// ----------------------------------------------------------------------------

	// Half chunk getters.
	BIG_INT_CONSTEXPR UChunk half_chunk(const size_t idx_hlf) const {
		UChunk result = number[idx_hlf / 2u];
		return ((idx_hlf % 2u) == 0) ? (result & HALF_CHUNK_LOW_MASK) : (result >> HALF_CHUNK_WIDTH_BITS);
	}

	BIG_INT_CONSTEXPR UChunk full_chunk(const size_t idx_hlf) const {
		const size_t idx = idx_hlf / 2u;
		UChunk result = number[idx];
		if((idx_hlf % 2u) == 0) {
			result <<= HALF_CHUNK_WIDTH_BITS;
			if(idx) {
				result |= (number[idx-1u] >> HALF_CHUNK_WIDTH_BITS);
			}
		}
		return result;
	}


	BIG_INT_CONSTEXPR UChunk chunk_add_offset(const UChunk& ch, size_t idx) {
		number[idx] += ch;
		UChunk carry = (number[idx] < ch);
		++idx;

		while(carry && (idx < NUMBER_WIDTH_CHUNKS)) {
			number[idx] += carry;
			carry = (number[idx] < carry);
			++idx;
		}
		return carry;
	}

	BIG_INT_CONSTEXPR size_t valuable_chunks() const {
		size_t result = 0;
		for(size_t idx = 0; idx < NUMBER_WIDTH_CHUNKS; ++idx) {
			if(number[idx]) {
				result = idx + 1u;
			}
		}
		return result;
	}

	BIG_INT_CONSTEXPR size_t most_valuable_half_chunk_idx() const {
		size_t result = 0;
		size_t idx = NUMBER_WIDTH_CHUNKS - 1u;
		while(idx < NUMBER_WIDTH_CHUNKS) {
			if(number[idx] & HALF_CHUNK_HIGH_MASK) {
				result = idx * 2u + 1u;
				break;
			}

			if(number[idx] & HALF_CHUNK_LOW_MASK) {
				result = idx * 2u;
				break;
			}

			--idx;
		}
		return result;
	}

	BIG_INT_CONSTEXPR size_t last_diff_chunk(const BigInt& rv) const {
		size_t idx = NUMBER_WIDTH_CHUNKS - 1u;
		while(idx && (number[idx] == rv.number[idx])) {
			--idx;
		}
		return idx;
	}

	BIG_INT_CONSTEXPR bool cmp_unsigned_less(const BigInt& rv) const {
		const size_t idx = last_diff_chunk(rv);
		return number[idx] < rv.number[idx];
	}

	BIG_INT_CONSTEXPR bool cmp_unsigned_greater(const BigInt& rv) const {
		const size_t idx = last_diff_chunk(rv);
		return number[idx] > rv.number[idx];
	}

	BIG_INT_CONSTEXPR void sign_flip() {
		bitwise_not();
		chunk_add_offset(1u, 0);
	}

};

// ----------------------------------------------------------------------------
// Bitwise operators.
// ----------------------------------------------------------------------------

// BigInt vs Integer.
template <size_t W, typename T, typename std::enable_if<(std::is_integral<T>::value && std::is_signed<T>::value && sizeof(T) <= sizeof(BigInt<W>)), int>::type = 0>
BIG_INT_CONSTEXPR BigInt<W> operator&(BigInt<W> left, const T& right) { return left.operator&(BigInt<W>(right)); }

template <size_t W, typename T, typename std::enable_if<(std::is_integral<T>::value && std::is_signed<T>::value && sizeof(T) <= sizeof(BigInt<W>)), int>::type = 0>
BIG_INT_CONSTEXPR BigInt<W> operator|(BigInt<W> left, const T& right) { return left.operator|(BigInt<W>(right)); }

template <size_t W, typename T, typename std::enable_if<(std::is_integral<T>::value && std::is_signed<T>::value && sizeof(T) <= sizeof(BigInt<W>)), int>::type = 0>
BIG_INT_CONSTEXPR BigInt<W> operator^(BigInt<W> left, const T& right) { return left.operator^(BigInt<W>(right)); }

// Integer vs BigInt.
template <size_t W, typename T, typename std::enable_if<(std::is_integral<T>::value && std::is_signed<T>::value && sizeof(T) <= sizeof(BigInt<W>)), int>::type = 0>
BIG_INT_CONSTEXPR BigInt<W> operator&(const T& left, BigInt<W> right) { return right.operator&(BigInt<W>(left)); }

template <size_t W, typename T, typename std::enable_if<(std::is_integral<T>::value && std::is_signed<T>::value && sizeof(T) <= sizeof(BigInt<W>)), int>::type = 0>
BIG_INT_CONSTEXPR BigInt<W> operator|(const T& left, BigInt<W> right) { return right.operator|(BigInt<W>(left)); }

template <size_t W, typename T, typename std::enable_if<(std::is_integral<T>::value && std::is_signed<T>::value && sizeof(T) <= sizeof(BigInt<W>)), int>::type = 0>
BIG_INT_CONSTEXPR BigInt<W> operator^(const T& left, BigInt<W> right) { return right.operator^(BigInt<W>(left)); }


// ----------------------------------------------------------------------------
// Relational operators.
// ----------------------------------------------------------------------------

// BigInt vs Integer.
template <size_t W, typename T, typename std::enable_if<(std::is_integral<T>::value && std::is_signed<T>::value && sizeof(T) <= sizeof(BigInt<W>)), int>::type = 0>
BIG_INT_CONSTEXPR bool operator==(const BigInt<W>& left, const T& right) { return left.operator==(BigInt<W>(right)); }

template <size_t W, typename T, typename std::enable_if<(std::is_integral<T>::value && std::is_signed<T>::value && sizeof(T) <= sizeof(BigInt<W>)), int>::type = 0>
BIG_INT_CONSTEXPR bool operator!=(const BigInt<W>& left, const T& right) { return left.operator!=(BigInt<W>(right)); }

template <size_t W, typename T, typename std::enable_if<(std::is_integral<T>::value && std::is_signed<T>::value && sizeof(T) <= sizeof(BigInt<W>)), int>::type = 0>
BIG_INT_CONSTEXPR bool operator<(const BigInt<W>& left, const T& right) { return left.operator<(BigInt<W>(right)); }

template <size_t W, typename T, typename std::enable_if<(std::is_integral<T>::value && std::is_signed<T>::value && sizeof(T) <= sizeof(BigInt<W>)), int>::type = 0>
BIG_INT_CONSTEXPR bool operator>(const BigInt<W>& left, const T& right) { return left.operator>(BigInt<W>(right)); }

template <size_t W, typename T, typename std::enable_if<(std::is_integral<T>::value && std::is_signed<T>::value && sizeof(T) <= sizeof(BigInt<W>)), int>::type = 0>
BIG_INT_CONSTEXPR bool operator<=(const BigInt<W>& left, const T& right) { return left.operator<=(BigInt<W>(right)); }

template <size_t W, typename T, typename std::enable_if<(std::is_integral<T>::value && std::is_signed<T>::value && sizeof(T) <= sizeof(BigInt<W>)), int>::type = 0>
BIG_INT_CONSTEXPR bool operator>=(const BigInt<W>& left, const T& right) { return left.operator>=(BigInt<W>(right)); }


// Integer vs BigInt.
template <size_t W, typename T, typename std::enable_if<(std::is_integral<T>::value && std::is_signed<T>::value && sizeof(T) <= sizeof(BigInt<W>)), int>::type = 0>
BIG_INT_CONSTEXPR bool operator==(const T& left, const BigInt<W>& right) { return right.operator==(BigInt<W>(left)); }

template <size_t W, typename T, typename std::enable_if<(std::is_integral<T>::value && std::is_signed<T>::value && sizeof(T) <= sizeof(BigInt<W>)), int>::type = 0>
BIG_INT_CONSTEXPR bool operator!=(const T& left, const BigInt<W>& right) { return right.operator!=(BigInt<W>(left)); }

template <size_t W, typename T, typename std::enable_if<(std::is_integral<T>::value && std::is_signed<T>::value && sizeof(T) <= sizeof(BigInt<W>)), int>::type = 0>
BIG_INT_CONSTEXPR bool operator<(const T& left, const BigInt<W>& right) { return right.operator>(BigInt<W>(left)); }

template <size_t W, typename T, typename std::enable_if<(std::is_integral<T>::value && std::is_signed<T>::value && sizeof(T) <= sizeof(BigInt<W>)), int>::type = 0>
BIG_INT_CONSTEXPR bool operator>(const T& left, const BigInt<W>& right) { return right.operator<(BigInt<W>(left)); }

template <size_t W, typename T, typename std::enable_if<(std::is_integral<T>::value && std::is_signed<T>::value && sizeof(T) <= sizeof(BigInt<W>)), int>::type = 0>
BIG_INT_CONSTEXPR bool operator<=(const T& left, const BigInt<W>& right) { return right.operator>=(BigInt<W>(left)); }

template <size_t W, typename T, typename std::enable_if<(std::is_integral<T>::value && std::is_signed<T>::value && sizeof(T) <= sizeof(BigInt<W>)), int>::type = 0>
BIG_INT_CONSTEXPR bool operator>=(const T& left, const BigInt<W>& right) { return right.operator<=(BigInt<W>(left)); }


// ----------------------------------------------------------------------------
// Arithmetic operators.
// ----------------------------------------------------------------------------

// BigInt vs Integer.
template <size_t W, typename T, typename std::enable_if<(std::is_integral<T>::value && std::is_signed<T>::value && sizeof(T) <= sizeof(BigInt<W>)), int>::type = 0>
BIG_INT_CONSTEXPR BigInt<W> operator+(BigInt<W> left, const T& right) { return left.operator+=(BigInt<W>(right)); }

template <size_t W, typename T, typename std::enable_if<(std::is_integral<T>::value && std::is_signed<T>::value && sizeof(T) <= sizeof(BigInt<W>)), int>::type = 0>
BIG_INT_CONSTEXPR BigInt<W> operator-(BigInt<W> left, const T& right) { return left.operator-=(BigInt<W>(right)); }

template <size_t W, typename T, typename std::enable_if<(std::is_integral<T>::value && std::is_signed<T>::value && sizeof(T) <= sizeof(BigInt<W>)), int>::type = 0>
BIG_INT_CONSTEXPR BigInt<W> operator*(BigInt<W> left, const T& right) { return left.operator*=(BigInt<W>(right)); }

template <size_t W, typename T, typename std::enable_if<(std::is_integral<T>::value && std::is_signed<T>::value && sizeof(T) <= sizeof(BigInt<W>)), int>::type = 0>
BIG_INT_CONSTEXPR BigInt<W> operator/(BigInt<W> left, const T& right) { return left.operator/=(BigInt<W>(right)); }

template <size_t W, typename T, typename std::enable_if<(std::is_integral<T>::value && std::is_signed<T>::value && sizeof(T) <= sizeof(BigInt<W>)), int>::type = 0>
BIG_INT_CONSTEXPR BigInt<W> operator%(BigInt<W> left, const T& right) { return left.operator%=(BigInt<W>(right)); }

// Integer vs BigInt.
template <size_t W, typename T, typename std::enable_if<(std::is_integral<T>::value && std::is_signed<T>::value && sizeof(T) <= sizeof(BigInt<W>)), int>::type = 0>
BIG_INT_CONSTEXPR BigInt<W> operator+(const T& left, BigInt<W> right) { return right.operator+=(BigInt<W>(left)); }

template <size_t W, typename T, typename std::enable_if<(std::is_integral<T>::value && std::is_signed<T>::value && sizeof(T) <= sizeof(BigInt<W>)), int>::type = 0>
BIG_INT_CONSTEXPR BigInt<W> operator-(const T& left, const BigInt<W>& right) { return BigInt<W>(left).operator-=(right); }

template <size_t W, typename T, typename std::enable_if<(std::is_integral<T>::value && std::is_signed<T>::value && sizeof(T) <= sizeof(BigInt<W>)), int>::type = 0>
BIG_INT_CONSTEXPR BigInt<W> operator*(const T& left, BigInt<W> right) { return right.operator*=(BigInt<W>(left)); }

template <size_t W, typename T, typename std::enable_if<(std::is_integral<T>::value && std::is_signed<T>::value && sizeof(T) <= sizeof(BigInt<W>)), int>::type = 0>
BIG_INT_CONSTEXPR BigInt<W> operator/(const T& left, const BigInt<W>& right) { return BigInt<W>(left).operator/=(right); }

template <size_t W, typename T, typename std::enable_if<(std::is_integral<T>::value && std::is_signed<T>::value && sizeof(T) <= sizeof(BigInt<W>)), int>::type = 0>
BIG_INT_CONSTEXPR BigInt<W> operator%(const T& left, const BigInt<W>& right) { return BigInt<W>(left).operator%=(right); }
