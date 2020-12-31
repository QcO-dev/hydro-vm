#include "Interrupt.h"

#include <stdio.h>
#include <stdlib.h>
#include <cpu/Registers.h>

void int_putc(CPU* cpu) {
	uint32_t val = pop(cpu);

	putc(val, stdout);
}

void int_getc(CPU* cpu) {
	uint32_t c = getc(stdin);

	setRegister(cpu, REG_ACC, c);
}

interrupt* buildInterruptTable() {

	interrupt* table;
	table = malloc(INTERRUPT_COUNT * sizeof(interrupt));

	if (table == NULL) {
		return NULL;
	}

	table[0x01] = int_putc;
	table[0x02] = int_getc;

	return table;
}