#include "TestBigInt.h"
#include "TestBigFxp.h"

#include "BigFxp.h"

using namespace bint;

void round(BigFxp<16, 3u> mx, unsigned digits) {
	printf("round_ceil(%s, %u)=%s\n", mx.store_str().c_str(), digits, mx.round_ceil(digits).store_str().c_str());
	printf("round_floor(%s, %u)=%s\n", mx.store_str().c_str(), digits, mx.round_floor(digits).store_str().c_str());
	printf("round(%s, %u)=%s\n", mx.store_str().c_str(), digits, mx.round(digits).store_str().c_str());
	printf("round_drop(%s, %u)=%s\n", mx.store_str().c_str(), digits, mx.round_drop(digits).store_str().c_str());
	printf("\n");
}

int main(int argc, char** argv) {

//	BigFxp<16, 3u> mx("-1.51");
//	BigFxp<16, 3u> my(-mx);
//	round(mx, 0);
//	round(my, 0);

//	TestBigInt test_big_int;
	TestBigFxp test_big_fxp;
	return 0;
}
