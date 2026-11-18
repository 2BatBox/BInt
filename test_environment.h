#pragma once

#include <cstdio>
#include <cstdlib>

#define ASSERT(exp) { \
	if(!(exp)) { \
		printf(#exp "\n");  \
        printf("The autotest has failed! at line %d!\n", __LINE__);\
		fflush(stdout); \
		fflush(stderr); \
		abort(); \
	} \
};

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