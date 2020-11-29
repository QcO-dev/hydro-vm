#include <memory/Memory.h>
#include <cpu/CPU.h>
#include <cpu/Registers.h>
#include <cpu/Opcodes.h>

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define MAX_RAM 256 * 256
#define STACK_POINTER_START MAX_RAM - 4

void printRegisters(CPU* cpu) {

	printf("IP: %08X\n", getRegisiter(cpu, REG_IP));
	printf("R1: %08X\n", getRegisiter(cpu, REG_R1));
	printf("R2: %08X\n", getRegisiter(cpu, REG_R2));
	printf("R3: %08X\n", getRegisiter(cpu, REG_R3));
	printf("R4: %08X\n", getRegisiter(cpu, REG_R4));
	printf("ACC: %08X\n", getRegisiter(cpu, REG_ACC));
	printf("FLAGS: %08X\n", getRegisiter(cpu, REG_FLAGS));
	printf("SP: %08X\n", getRegisiter(cpu, REG_SP));

}

void printMemory(CPU* cpu, uint32_t address, int values) {

	for (int i = 0; i < values * 4; i += 4) {
		printf("%08X ", mem_getU32(cpu->memory, address + i));
	}
	printf("\n");

}

int main() {

	uint8_t* mem = createMemory(MAX_RAM);

	if (mem == NULL) {
		fprintf(stderr, "Failed to allocate memory block.\n");
		return -1;
	}

	CPU* cpu = createCPU(mem, STACK_POINTER_START);

	if (mem == NULL) {
		fprintf(stderr, "Failed to allocate memory block.\n");
		return -1;
	}

	uint8_t mCode[] = {
		PSH_LIT, 0x00, 0x00, 0x00, 0x00,
		CAL_LIT, 0x00, 0x30, 0x00, 0x00,
		HLT
	};

	memcpy(mem, mCode, sizeof(mCode));

	uint8_t subR[] = {
		MOV_LIT_REG, 0x01, 0x00, 0x00, 0x00, REG_R1,
		MOV_REG_REG, REG_R1, REG_ACC,
		RET
	};

	memcpy(mem + 0x3000, subR, sizeof(subR));

	printRegisters(cpu);
	printMemory(cpu, MAX_RAM - (32 * 4), 32);

	while (!(getRegisiter(cpu, REG_FLAGS) & 0x1)) { // Get the HLT Bit of the Flags register
		step(cpu);
		printRegisters(cpu);
		printMemory(cpu, MAX_RAM - (32 * 4), 32);
	}

	free(cpu);
	free(mem);
	
}