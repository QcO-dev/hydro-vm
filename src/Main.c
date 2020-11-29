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
	printf("FP: %08X\n", getRegisiter(cpu, REG_FP));

}

void printMemory(CPU* cpu, uint32_t address, int values) {

	for (int i = 0; i < values * 4; i += 4) {
		printf("%08X ", mem_getU32(cpu->memory, address + i));
	}
	printf("\n");

}

int main(int argc, char** argv) {

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
		PSH_LIT, 0x03, 0x00, 0x00, 0x00,
		PSH_LIT, 0x02, 0x00, 0x00, 0x00,
		PSH_LIT, 0x01, 0x00, 0x00, 0x00,

		PSH_LIT, 0x03, 0x00, 0x00, 0x00,
		CAL_LIT, 0x00, 0x30, 0x00, 0x00,
		MOV_REG_REG, REG_ACC, REG_R8,
		HLT
	};

	memcpy(mem, mCode, sizeof(mCode));

	uint8_t subR[] = {	
		MOV_REG_PTR_OFF_REG, REG_FP, 44, 0x00, 0x00, 0x00, REG_R1,
		
		ADD_REG_LIT, REG_FP, 44, 0x00, 0x00, 0x00,
		MOV_REG_REG, REG_ACC, REG_SP,
		
		MOV_LIT_REG, 0x00, 0x00, 0x00, 0x00, REG_R3,

		CMP_REG_LIT, REG_R1, 0x00, 0x00, 0x00, 0x00,
		JEQ_LIT, 0x30, 0x30, 0x00, 0x00,
		POP_REG, REG_R2,
		ADD_REG_REG, REG_R3, REG_R2,
		MOV_REG_REG, REG_ACC, REG_R3,
		SUB_REG_LIT, REG_R1, 0x01, 0x00, 0x00, 0x00,
		MOV_REG_REG, REG_ACC, REG_R1,
		JMP_LIT, 23, 0x30, 0x00, 0x00,
		// End

		MOV_REG_REG, REG_R3, REG_ACC,
		RET
	};

	memcpy(mem + 0x3000, subR, sizeof(subR));

	printRegisters(cpu);
	printMemory(cpu, 0xffc8, 32);

	while (!(getRegisiter(cpu, REG_FLAGS) & 0x1)) { // Get the HLT Bit of the Flags register
		step(cpu);
		printRegisters(cpu);
		printMemory(cpu, 0xffc8, 32);
	}

	free(cpu);
	free(mem);
	
}