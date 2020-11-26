#include <memory/Memory.h>
#include <cpu/CPU.h>
#include <cpu/Registers.h>
#include <cpu/Opcodes.h>

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define MAX_RAM 256 * 256 // 64KB

void printRegisters(CPU* cpu) {

	printf("IP: %08X\n", getRegisiter(cpu, REG_IP));
	printf("R1: %08X\n", getRegisiter(cpu, REG_R1));
	printf("R2: %08X\n", getRegisiter(cpu, REG_R2));
	printf("R3: %08X\n", getRegisiter(cpu, REG_R3));
	printf("R4: %08X\n", getRegisiter(cpu, REG_R4));
	printf("ACC: %08X\n", getRegisiter(cpu, REG_ACC));
	printf("FLAGS: %08X\n", getRegisiter(cpu, REG_FLAGS));

}

void printMemory(CPU* cpu, uint32_t address) {

	for (int i = 0; i < 8 * 4; i++) {
		printf("%02X", cpu->memory[address + i]);
	}
	printf("\n");

}

int main() {

	uint8_t* mem = createMemory(MAX_RAM);

	CPU* cpu = createCPU(mem);

	uint8_t mCode[] = {
		MOV_LIT_REG, 0x03, 0x00, 0x00, 0x00, REG_R1,
		MOV_LIT_REG, 0x02, 0x00, 0x00, 0x00, REG_R2,
		CMP_REG_REG, REG_R1, REG_R2,
		JGE_LIT, 0x00, 0x10, 0x00, 0x00,
		HLT
	};

	//TODO Greater than or equal / Less than or equal

	memcpy(mem, mCode, sizeof(mCode));

	uint8_t jmpCode[] = {
		MOV_LIT_REG, 0xff, 0xff, 0xff, 0xff, REG_R3,
		HLT
	};

	memcpy(mem + 0x1000, jmpCode, sizeof(jmpCode));


	printRegisters(cpu);
	printMemory(cpu, 0xff00);

	while (!(getRegisiter(cpu, REG_FLAGS) & 0x1)) { // Get the HLT Bit of the Flags register
		step(cpu);
		printRegisters(cpu);
		printMemory(cpu, 0xff00);
	}

	free(cpu);
	free(mem);
	
}