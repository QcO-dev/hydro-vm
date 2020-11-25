#pragma once
#include <stdint.h>
uint8_t* createMemory(size_t sizeInBytes);

uint16_t mem_getU16(uint8_t* mem, size_t index);

uint32_t mem_getU32(uint8_t* mem, size_t index);

void mem_setU16(uint8_t* mem, size_t index, uint16_t value);

void mem_setU32(uint8_t* mem, size_t index, uint32_t value);