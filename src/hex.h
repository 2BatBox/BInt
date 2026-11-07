#pragma once

#include <cstdio>
#include <cstdint>

template <typename T>
void hex(const T& val) {
	auto u8 = reinterpret_cast<const uint8_t*>(&val);
	for(unsigned i = 0; i < sizeof(T); i++) {
		printf("%02X ", u8[i]);
	}

	if(sizeof(T) > 1) {
		printf("  |  ");
		auto u16 = reinterpret_cast<const uint16_t*>(&val);
		for (unsigned i = 0; i < sizeof(T) / 2; ++i) {
			printf("%04X ", u16[i]);
		}
	}

	if(sizeof(T) > 3) {
		printf("  |  ");
		auto u32 = reinterpret_cast<const uint32_t*>(&val);
		for (unsigned i = 0; i < sizeof(T) / 4; ++i) {
			printf("%08X ", u32[i]);
		}
	}

	if(sizeof(T) > 7) {
		printf("  |  ");
		auto u64 = reinterpret_cast<const uint64_t*>(&val);
		for (unsigned i = 0; i < sizeof(T) / 8; ++i) {
			printf("%016lX ", u64[i]);
		}
	}

	printf("\n");
}