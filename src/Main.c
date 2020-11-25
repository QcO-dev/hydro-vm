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
	printf("ACC: %08X\n", getRegisiter(cpu, REG_ACC));

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
		MOV_LIT_MEM, 0x01, 0x00, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00,
		MOV_LIT_REG, 0x02, 0x00, 0x00, 0x00, REG_R2,
		MOV_MEM_REG, 0x00, 0xff, 0x00, 0x00, REG_R1,
		ADD_REG_REG, REG_R1, REG_R2,
		HLT
	};

	memcpy(mem, mCode, sizeof(mCode));


	printRegisters(cpu);
	printMemory(cpu, 0xff00);
	
	/*for (int i = 0; i < 6; i++) {
		step(cpu);
		printRegisters(cpu);
		printMemory(cpu, 0xff00);
	}*/

	while (!(getRegisiter(cpu, REG_FLAGS) & 0x1)) {
		step(cpu);
		printRegisters(cpu);
		printMemory(cpu, 0xff00);
	}

	free(cpu);
	free(mem);
	
}