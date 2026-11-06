#include <cstdio>
#include <cstdint>

#include "hex.h"
#include "BigInt.h"
#include "TestBigInt.h"

int main(int argc, char** argv) {

	using Type_t = BigInt<int8_t, 8>;

//	int64_t ix = 0x0349AC4579EA3E86ll;
//	int64_t iy = 0x000000007B2AFF5Bll;

//	int64_t ix = 0x7DFD3A7C3E40F98Bll;
//	int64_t iy = 0x00000000B22CF3FFll;

	int64_t ix = 0x03ab;
	int64_t iy = 0x0048;


	Type_t bx(ix);
	Type_t by(iy);

	hex(bx);
	hex(by);

	TestBigInt::test_equal<Type_t, int64_t>(bx / by, ix / iy);
	TestBigInt test;


	printf("_tmp_call_cnt=%zu\n", _tmp_call_cnt);
	printf("_tmp_carry_mult_cnt=%zu (%.4f per call)\n", _tmp_carry_mult_cnt, _tmp_carry_mult_cnt /  double(_tmp_call_cnt));
	printf("_tmp_mult_cnt=%zu (%.4f per call)\n", _tmp_mult_cnt, _tmp_mult_cnt /  double(_tmp_call_cnt));
	printf("_tmp_sub_cnt=%zu (%.4f per call)\n", _tmp_sub_cnt, _tmp_sub_cnt /  double(_tmp_call_cnt));

	return 0;
}
