#pragma once

#include <cstdint>

class Tsc {
public:

	using Counter = uint64_t;

	/**
	 * See
	 * Intel® 64 and IA-32 Architectures Optimization Reference Manual
	 * 12.6 DEBUGGING GUIDELINES
	 *
	 * @return TSC counter value.
	 */
	static Counter read() {
#if defined(__x86_64__) || defined(__amd64__)
		Counter low;
		Counter high;
		__asm__ volatile("rdtsc" : "=a"(low), "=d"(high));
		return (high << 32) | low;
#elif defined(__aarch64__)
		Counter virtual_timer_value;
		asm volatile("mrs %0, cntvct_el0" : "=r"(virtual_timer_value));
		return virtual_timer_value;
#endif
	}

};

