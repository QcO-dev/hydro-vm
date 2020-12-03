#pragma once
#include <stdint.h>

#include <cpu/Interrupt.h>

typedef struct cpu_t {
	uint8_t* memory;

	void(**interruptTable)(struct cpu_t*);

	uint8_t* regisiters;
} CPU;

CPU* createCPU(uint8_t* memory, uint32_t stackPointer);

void step(CPU* cpu);

uint32_t getRegisiter(CPU* cpu, int regIndex);

uint32_t pop(CPU* cpu);