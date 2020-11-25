#pragma once
#include <stdint.h>

typedef struct cpu_t {
	uint8_t* memory;

	uint8_t* regisiters;
} CPU;

CPU* createCPU(uint8_t* memory);

void step(CPU* cpu);

uint32_t getRegisiter(CPU* cpu, int regIndex);