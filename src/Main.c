#define _CRT_SECURE_NO_WARNINGS

#include <memory/Memory.h>
#include <cpu/CPU.h>
#include <cpu/Registers.h>
#include <cpu/Opcodes.h>

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

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

typedef struct {
	uint8_t* content;
	unsigned long length;
	char error;
} BFile;

BFile readFile(char* fileName) {
	FILE* fileptr;
	uint8_t* buffer;
	unsigned long filelen;

	fileptr = fopen(fileName, "rb");

	if (fileptr == NULL) {
		fprintf(stderr, "Failed opening %s: %s", fileName, strerror(errno));
		BFile file = { NULL, 0, 1 };
		return file;
	}

	fseek(fileptr, 0, SEEK_END);
	filelen = ftell(fileptr);
	rewind(fileptr);

	buffer = malloc(filelen);

	if (buffer == NULL) {
		fprintf(stderr, "Failed to allocate memory block.\n");
		BFile file = { NULL, 0, 1 };
		return file;
	}

	fread(buffer, filelen, 1, fileptr);
	fclose(fileptr);

	BFile file = {buffer, filelen};

	return file;
}

int main(int argc, char** argv) {

	if (argc != 2) {
		fprintf(stderr, "Expected one argument, got %d.", argc - 1);
		return - 1;
	}

	BFile file = readFile(argv[1]);

	if (file.error) {
		return -1;
	}

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

	memcpy(mem, file.content, file.length);

#ifdef _DEBUG
	printRegisters(cpu);
	printMemory(cpu, 0xffc8, 32);
#endif

	while (!(getRegisiter(cpu, REG_FLAGS) & 0x1)) { // Get the HLT Bit of the Flags register
		step(cpu);
#ifdef _DEBUG
		printRegisters(cpu);
		printMemory(cpu, 0xffc8, 32);
#endif
	}

	uint32_t returnValue = getRegisiter(cpu, REG_ACC);

	free(cpu);
	free(mem);
	
	return returnValue;
}