#include "BigInt.h"

#include <climits>

int main(int argc, char** argv) {
	using BigType_t = BigInt<CHAR_BIT * sizeof(int)>;

	int x = 0;
	int y = 0;
	int result;

	if(argc > 1) {
		result = static_cast<int>(BigType_t(x) / BigType_t(y));
	} else {
		result = x / y;
	}

	return result;
}
