#pragma once

#include <cpu/CPU.h>

#define INTERRUPT_COUNT 255

typedef void(*interrupt)(struct cpu_t*);

interrupt* buildInterruptTable();