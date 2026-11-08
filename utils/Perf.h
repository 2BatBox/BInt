#pragma once

#include "Tsc.h"

#include <cstdio>

struct Perf {

	Tsc::Counter clk_total{0};
	Tsc::Counter clk_before{0};
	const char* name;

	Perf(const char* _name) : name(_name) {}

	void start() {
		clk_before = Tsc::read();
	}

	void stop() {
		clk_total += Tsc::read() - clk_before;
	}

	void printf_val(const char* name, const double& val) {
		printf("%s=%.2f ", name, val);
	}

	void print(const uint64_t call_cnt) {
		printf("%s : ", name);
		printf_val("CALLS", call_cnt);
		printf_val("CLK_TOTAL", clk_total);
		printf_val("CLK_PER_CALL", clk_total / double(call_cnt));
		printf("\n");
	}

};

