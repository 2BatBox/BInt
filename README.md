# BigInt

`BigInt<WIDTH_BITS>` is a header-only C++ template for **signed, fixed-width integer arithmetic**. Its width must be positive and a multiple of `CHAR_BIT`. BigInt stores values in two's complement form, using an array of unsigned machine words (also called *chunks* or *limbs*). Unlike an arbitrary-precision integer, a `BigInt` has a width fixed at compile time.

Include `BigInt.h` to use it:

```
#include "BigInt.h"
#include <cstdint>

BigInt<8>   bi8(1);
BigInt<24>  bi24(1);  // Three bytes: the width need not be a power of two.
BigInt<64>  bi64(1);
BigInt<512> bi256(1);
```

##  What are the similarities between BigInt and built-in integers?

**The same arithmetic results when the native operation is defined.** Addition, subtraction, multiplication, integer division, remainder, comparisons, and ordinary bitwise operations produce the corresponding mathematical/bitwise results when operands and results are representable and no implementation-dependent behavior is involved. So for any given x, y(not zero) and z(less than 32):

```
std::int32_t x;
std::int32_t y;
size_t z;

BigInt<32> bx(x);
BigInt<32> by(y);

assert((bx + by) == (x + y));
assert((bx - by) == (x - y));
assert((bx * by) == (x * y));
assert((bx / by) == (x / y));
assert((bx % by) == (x % y));
assert((bx & by) == (x & y));
assert((bx | by) == (x | y));
assert((bx ^ by) == (x ^ y));
assert((bx == by) == (x == y));
assert((bx != by) == (x != y));
assert((bx > by) == (x > y));
assert((bx >= by) == (x >= y));
assert((bx < by) == (x < y));
assert((bx <= by) == (x <= y));
assert((~bx) == (~x));
assert((bx << z) == (x << z));
```

**The same width means the same range.** For example

```
static_assert(BigInt<32u>::min() == std::numeric_limits<int32_t>::min());
static_assert(BigInt<32u>::max() == std::numeric_limits<int32_t>::max());
```


## What are the differences between BigInt and built-in integers?

**The width is freely selectable in multiples of `CHAR_BIT`.** Built-in integer types offer only the widths provided by the compiler and platform; `BigInt` supports widths such as 24, 40, 72, 128, or 256 bits on a system with 8-bit bytes.

```
BigInt<24>  rgb_sized(123456);
BigInt<40>  five_bytes(123456);
BigInt<128> wide(123456);
```

**Default initialization always gives zero.** A local built-in integer declared without an initializer may be indeterminate; a default-constructed `BigInt` initializes all its chunks to zero.

```
int native;       // No explicit initialization: do not read its value.
BigInt<128> big;  // Exactly zero.
```

**Right shift is always arithmetic.** The `>>` and `>>=` operators propagate the sign bit for negative numbers. This removes the implementation-dependent behavior of shifting negative signed built-in integers in older C++ standards. Use `shr()` when you specifically need a *logical* right shift, which fills high bits with zeros. Unlike `>>`, `shr()` modifies the object in place.

```
BigInt<16> negative(-5);
BigInt<16> arithmetic = negative >> 1; // -3 (sign bits are retained)
BigInt<16> logical = negative;
logical.shr(1);                        // 32765 (zeros inserted at the top)
```

**Logical shift by argument >= WIDTH_BITS always produces zero.**

```
BigInt<16> negative(-5);
BigInt<16> left = negative << 16; // left == 0.
negative.shr(33); // negative == 0.
```

**There is no implicit conversion back to a built-in integer.** A cast is required. Also, this version's constructors and mixed operators accept **signed integral types only**; unsigned integral types are not supported directly on purpose. Since signed / unsigned relationships (conversion, comparision ant etc.) are heavily platform dependent, BigInt leaves all those aspects out of scope.

```
BigInt<128> big = 123;
std::int64_t native = static_cast<std::int64_t>(big);

// BigInt<128> unsupported = 123u; // Unsigned source: no matching constructor.
```
