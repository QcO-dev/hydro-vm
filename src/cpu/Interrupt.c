#include "Interrupt.h"

#include <stdio.h>
#include <stdlib.h>

void int_putc(CPU* cpu) {

	uint32_t val = pop(cpu);

	putc(val, stdout);
}

interrupt* buildInterruptTable() {

	interrupt* table;
	table = malloc(INTERRUPT_COUNT * sizeof(interrupt));

	if (table == NULL) {
		return NULL;
	}

	table[0x01] = int_putc;

	return table;
}