#pragma once

#include <cstdio>

#define ASSERT(exp) { \
	if(!(exp)) { \
		printf(#exp "\n");  \
        printf("The autotest has failed! at line %d!\n", __LINE__);\
		fflush(stdout); \
		fflush(stderr); \
		abort(); \
	} \
};
