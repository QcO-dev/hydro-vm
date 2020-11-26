#include "CPU.h"
#include "Registers.h"
#include "Opcodes.h"
#include <memory/Memory.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

struct cpu_t* createCPU(uint8_t* memory) {
	struct cpu_t *cpu = malloc(sizeof(struct cpu_t));

	if (cpu == NULL) {
		//TODO
		return;
	}

	cpu->memory = memory;

	cpu->regisiters = createMemory(REG_COUNT * 4); // Each register is 32 Bits, or 4 bytes

	for (int i = 0; i < REG_COUNT * 4; i++) {
		cpu->regisiters[i] = 0;
	}

	return cpu;
}

uint32_t getRegisiter(CPU* cpu, int regIndex) {
	return mem_getU32(cpu->regisiters, regIndex * REG_OFFSET);
}

void setRegister(CPU* cpu, int regIndex, uint32_t value) {
	mem_setU32(cpu->regisiters, regIndex * REG_OFFSET, value);
}

uint8_t fetch(CPU *cpu) {
	uint32_t nextAddress = getRegisiter(cpu, REG_IP);

	uint8_t instruction = cpu->memory[nextAddress];

	setRegister(cpu, REG_IP, nextAddress + 1);

	return instruction;
}

uint32_t fetch32(CPU *cpu) {

	uint32_t nextAddress = getRegisiter(cpu, REG_IP);

	uint32_t instruction = mem_getU32(cpu->memory, nextAddress);

	setRegister(cpu, REG_IP, nextAddress + 4);

	return instruction;
}

void execute(CPU* cpu, uint8_t instruction) {

	switch (instruction) {
		case MOV_LIT_REG: {

				uint32_t lit = fetch32(cpu);

				uint8_t reg = fetch(cpu);

				if (reg >= REG_COUNT) {
					//TODO ERROR
					return;
				}

				setRegister(cpu, reg, lit);

				break;
			}

		case MOV_REG_REG: {

				uint8_t reg1 = fetch(cpu);

				uint8_t reg2 = fetch(cpu);

				//TODO ERROR CHECKING

				uint32_t reg1Value = getRegisiter(cpu, reg1);

				setRegister(cpu, reg2, reg1Value);

				break;
			}

		case MOV_REG_MEM: {

				uint8_t reg = fetch(cpu);

				//TODO ERROR

				uint32_t memAddress = fetch32(cpu);

				uint32_t regValue = getRegisiter(cpu, reg);

				mem_setU32(cpu->memory, memAddress, regValue);
				 
				break;
			}

		case MOV_MEM_REG: {

				uint32_t memAddress = fetch32(cpu);

				uint8_t reg = fetch(cpu);

				//TODO ERROR

				uint32_t memValue = mem_getU32(cpu->memory, memAddress);

				setRegister(cpu, reg, memValue);

				break;
			}

		case MOV_LIT_MEM: {

				uint32_t lit = fetch32(cpu);

				uint32_t memAddress = fetch32(cpu);

				mem_setU32(cpu->memory, memAddress, lit);

				break;
			}

		case ADD_REG_REG: {

				uint8_t reg1 = fetch(cpu);

				uint8_t reg2 = fetch(cpu);

				//TODO CHECKS

				uint32_t val1 = getRegisiter(cpu, reg1);
				uint32_t val2 = getRegisiter(cpu, reg2);

				uint32_t val = val1 + val2;

				setRegister(cpu, REG_ACC, val);

				if (!val) {

					uint32_t flags = getRegisiter(cpu, REG_FLAGS);

					setRegister(cpu, REG_FLAGS, flags | 0x00000010);
				}

				break;
			}

		case SUB_REG_REG: {

				uint8_t reg1 = fetch(cpu);

				uint8_t reg2 = fetch(cpu);

				//TODO CHECKS

				uint32_t val1 = getRegisiter(cpu, reg1);
				uint32_t val2 = getRegisiter(cpu, reg2);

				uint32_t val = val1 - val2;

				setRegister(cpu, REG_ACC, val);

				printf("\nVAL: %08X\n", val);

				if (val == 0) {
					uint32_t flags = getRegisiter(cpu, REG_FLAGS);

					setRegister(cpu, REG_FLAGS, flags | 0x00000010);
				}

				break;
			}

		case JMP_LIT: {

				uint32_t memAddress = fetch32(cpu);

				setRegister(cpu, REG_IP, memAddress);

				break;
			}

		case JMP_REG: {

				uint8_t reg = fetch(cpu);

				//TODO CHECK

				uint32_t memAddress = getRegisiter(cpu, reg);

				setRegister(cpu, REG_IP, memAddress);

				break;
			}

		case JNE_LIT: {

				uint32_t memAddress = fetch32(cpu);

				if (!(getRegisiter(cpu, REG_FLAGS) & 0x10)) {
					setRegister(cpu, REG_IP, memAddress);
				}

				break;
			}

		case JNE_REG: {

				uint8_t reg = fetch(cpu);

				//TODO CHECK

				uint32_t memAddress = getRegisiter(cpu, reg);

				if (!(getRegisiter(cpu, REG_FLAGS) & 0x10)) {
					setRegister(cpu, REG_IP, memAddress);
				}

				break;
			}

		case JEQ_LIT: {

				uint32_t memAddress = fetch32(cpu);

				if (getRegisiter(cpu, REG_FLAGS) & 0x10) {
					setRegister(cpu, REG_IP, memAddress);
				}

				break;
			}

		case JEQ_REG: {

				uint8_t reg = fetch(cpu);

				//TODO CHECK

				uint32_t memAddress = getRegisiter(cpu, reg);

				if (getRegisiter(cpu, REG_FLAGS) & 0x10) {
					setRegister(cpu, REG_IP, memAddress);
				}

				break;
			}

		case HLT: {
				uint32_t flags = getRegisiter(cpu, REG_FLAGS);

				setRegister(cpu, REG_FLAGS, flags | 0x00000001);
				break;
			}

	}

}

void step(CPU* cpu) {
	uint8_t instruction = fetch(cpu);
	execute(cpu, instruction);
}