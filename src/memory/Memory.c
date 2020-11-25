#include "Memory.h"
#include <stdlib.h>

uint8_t* createMemory(size_t sizeInBytes) {
	return malloc(sizeInBytes);
}

uint16_t mem_getU16(uint8_t* mem, size_t index) {
	return mem[index + 1] << 8 | mem[index];
}

uint32_t mem_getU32(uint8_t* mem, size_t index) {
	uint32_t value = mem[index + 0] | mem[index + 1] << 8 | mem[index + 2] << 16 | mem[index + 3] << 24;
	return value;
}

void mem_setU16(uint8_t* mem, size_t index, uint16_t value) {
	mem[index] = (value & 0xff00) >> 8;
	mem[index + 1] = value & 0xff;
}

void mem_setU32(uint8_t* mem, size_t index, uint32_t value) {

	/*uint8_t b1 = (value & 0xff000000) >> 24;
	uint8_t b2 = (value & 0xff0000) >> 16;
	uint8_t b3 = (value & 0xff00) >> 8;
	uint8_t b4 = value & 0xff;*/

	uint8_t b4 = value & 0x000000FF;
	uint8_t b3 = (value >> 8) & 0x000000FF;
	uint8_t b2 = (value >> 16) & 0x000000FF;
	uint8_t b1 = (value >> 24) & 0x000000FF;

	mem[index] = b4;
	mem[index + 1] = b3;
	mem[index + 2] = b2;
	mem[index + 3] = b1;

}