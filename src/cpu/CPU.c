#include "CPU.h"
#include "Registers.h"
#include "Opcodes.h"
#include <memory/Memory.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <cpu/Interrupt.h>

void setRegister(CPU* cpu, int regIndex, uint32_t value);

struct cpu_t* createCPU(uint8_t* memory, uint32_t stackPointer) {
	struct cpu_t *cpu = malloc(sizeof(struct cpu_t));

	if (cpu == NULL) {
		return cpu;
	}

	cpu->memory = memory;

	cpu->regisiters = calloc(REG_COUNT, REG_OFFSET); // Each register is 32 Bits, or 4 bytes

	if (cpu->regisiters == NULL) {
		return NULL;
	}

	cpu->interruptTable = buildInterruptTable();

	if (cpu->interruptTable == NULL) {
		return NULL;
	}

	setRegister(cpu, REG_SP, stackPointer);
	setRegister(cpu, REG_FP, stackPointer);

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

void updateFlagsRegisterUnsigned(CPU* cpu, uint32_t val, uint32_t val1, uint32_t val2) {
	if (val == 0) {
		uint32_t flags = getRegisiter(cpu, REG_FLAGS);

		setRegister(cpu, REG_FLAGS, flags | 0x00000010);
	}

	if (val1 < val2) {
		uint32_t flags = getRegisiter(cpu, REG_FLAGS);

		setRegister(cpu, REG_FLAGS, flags | 0x00000100);
	}
}

void push(CPU* cpu, uint32_t value) {
	uint32_t memAddress = getRegisiter(cpu, REG_SP);

	mem_setU32(cpu->memory, memAddress, value);

	setRegister(cpu, REG_SP, getRegisiter(cpu, REG_SP) - 4);
}

uint32_t pop(CPU* cpu) {
	setRegister(cpu, REG_SP, getRegisiter(cpu, REG_SP) + 4);

	uint32_t memAddress = getRegisiter(cpu, REG_SP);

	uint32_t value = mem_getU32(cpu->memory, memAddress);

	return value;
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

		case MOV_REG_PTR_REG: {

				uint8_t reg1 = fetch(cpu);

				//TODO ERROR

				uint8_t reg2 = fetch(cpu);

				uint32_t memAddress = getRegisiter(cpu, reg1);

				uint32_t value = mem_getU32(cpu->memory, memAddress);

				setRegister(cpu, reg2, value);

				break;
			}

		case MOV_REG_PTR_MEM: {

				uint8_t reg1 = fetch(cpu);

				//TODO ERROR

				uint32_t memAddressDest = fetch32(cpu);

				uint32_t memAddress = getRegisiter(cpu, reg1);

				uint32_t value = mem_getU32(cpu->memory, memAddress);

				mem_setU32(cpu->memory, memAddressDest, value);

				break;
			}

		case MOV_REG_PTR_OFF_REG: {

				uint8_t reg1 = fetch(cpu);

				uint32_t offset = fetch32(cpu);

				uint8_t reg2 = fetch(cpu);

				//TODO CHECKS

				uint32_t memAddress = getRegisiter(cpu, reg1);

				uint32_t value = mem_getU32(cpu->memory, memAddress + offset);

				setRegister(cpu, reg2, value);

				break;
			}

		case MOV_REG_PTR_OFF_REG_REG: {

				uint8_t reg1 = fetch(cpu);

				uint8_t offsetReg = fetch(cpu);

				uint32_t offset = getRegisiter(cpu, offsetReg);

				uint8_t reg2 = fetch(cpu);

				//TODO CHECKS

				uint32_t memAddress = getRegisiter(cpu, reg1);

				uint32_t value = mem_getU32(cpu->memory, memAddress + offset);

				setRegister(cpu, reg2, value);

				break;
			}

		case MOV_REG_REG_PTR: {

				uint8_t reg = fetch(cpu);

				//TODO

				uint32_t val = getRegisiter(cpu, reg);

				uint8_t regPtr = fetch(cpu);

				uint32_t memAddress = getRegisiter(cpu, regPtr);

				mem_setU32(cpu->memory, memAddress, val);

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

				// if val1 is not 0 and val2 is not 0 but val is, overflow must have occurred.
				if (val1 != 0 && val2 != 0 && val == 0) {
					uint32_t flags = getRegisiter(cpu, REG_FLAGS);

					setRegister(cpu, REG_FLAGS, flags | 0x00000100);
				}

				break;
			}

		case ADD_REG_LIT: {

				uint8_t reg1 = fetch(cpu);

				uint32_t val2 = fetch32(cpu);

				//TODO CHECKS

				uint32_t val1 = getRegisiter(cpu, reg1);

				uint32_t val = val1 + val2;

				setRegister(cpu, REG_ACC, val);

				if (!val) {
					uint32_t flags = getRegisiter(cpu, REG_FLAGS);

					setRegister(cpu, REG_FLAGS, flags | 0x00000010);
				}

				if (val1 != 0 && val2 != 0 && val == 0) {
					uint32_t flags = getRegisiter(cpu, REG_FLAGS);

					setRegister(cpu, REG_FLAGS, flags | 0x00000100);
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

				updateFlagsRegisterUnsigned(cpu, val, val1, val2);

				break;
			}

		case SUB_REG_LIT: {

				uint8_t reg1 = fetch(cpu);

				uint32_t val2 = fetch32(cpu);

				//TODO CHECKS

				uint32_t val1 = getRegisiter(cpu, reg1);

				uint32_t val = val1 - val2;

				setRegister(cpu, REG_ACC, val);

				updateFlagsRegisterUnsigned(cpu, val, val1, val2);

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

		case JL_LIT: {

				uint32_t memAddress = fetch32(cpu);

				if (getRegisiter(cpu, REG_FLAGS) & 0x100) {
					setRegister(cpu, REG_IP, memAddress);
				}

				break;
		}

		case JL_REG: {
				uint8_t reg = fetch(cpu);

				//TODO CHECK

				uint32_t memAddress = getRegisiter(cpu, reg);

				if (getRegisiter(cpu, REG_FLAGS) & 0x100) {
					setRegister(cpu, REG_IP, memAddress);
				}

				break;
			}

		case JG_LIT: {

				uint32_t memAddress = fetch32(cpu);

				uint32_t flags = getRegisiter(cpu, REG_FLAGS);

				if (!(flags & 0x10) && !(flags & 0x100)) {
					setRegister(cpu, REG_IP, memAddress);
				}

				break;

			}

		case JG_REG: {

				uint8_t reg = fetch(cpu);

				//TODO CHECK

				uint32_t memAddress = getRegisiter(cpu, reg);

				uint32_t flags = getRegisiter(cpu, REG_FLAGS);

				if (!(flags & 0x10) && !(flags & 0x100)) {
					setRegister(cpu, REG_IP, memAddress);
				}

				break;

			}

		case JLE_LIT: {

				uint32_t memAddress = fetch32(cpu);

				uint32_t flags = getRegisiter(cpu, REG_FLAGS);

				if (flags & 0x10 || flags & 0x100) {
					setRegister(cpu, REG_IP, memAddress);
				}

				break;
			}

		case JLE_REG: {

				uint8_t reg = fetch(cpu);

				//TODO CHECK

				uint32_t memAddress = getRegisiter(cpu, reg);

				uint32_t flags = getRegisiter(cpu, REG_FLAGS);

				if (flags & 0x10 || flags & 0x100) {
					setRegister(cpu, REG_IP, memAddress);
				}

				break;
			}

		case JGE_LIT: {

				uint32_t memAddress = fetch32(cpu);

				if (!(getRegisiter(cpu, REG_FLAGS) & 0x100)) {
					setRegister(cpu, REG_IP, memAddress);
				}

				break;
			}

		case JGE_REG: {

				uint8_t reg = fetch(cpu);

				//TODO CHECK

				uint32_t memAddress = getRegisiter(cpu, reg);

				if (!(getRegisiter(cpu, REG_FLAGS) & 0x100)) {
					setRegister(cpu, REG_IP, memAddress);
				}

				break;
			}

		case CMP_REG_REG: { // Subtraction without changing the ACC register

				uint8_t reg1 = fetch(cpu);

				uint8_t reg2 = fetch(cpu);

				//TODO CHECKS

				uint32_t val1 = getRegisiter(cpu, reg1);
				uint32_t val2 = getRegisiter(cpu, reg2);

				uint32_t val = val1 - val2;

				updateFlagsRegisterUnsigned(cpu, val, val1, val2);

				break;
			}

		case CMP_REG_LIT: {

				uint8_t reg = fetch(cpu);

				uint32_t val2 = fetch32(cpu);

				//TODO CHECKS

				uint32_t val1 = getRegisiter(cpu, reg);

				uint32_t val = val1 - val2;

				updateFlagsRegisterUnsigned(cpu, val, val1, val2);

				break;
			}

		case OR_REG_REG: {
			uint8_t reg1 = fetch(cpu);

			uint8_t reg2 = fetch(cpu);

			//TODO CHECKS

			uint32_t val1 = getRegisiter(cpu, reg1);
			uint32_t val2 = getRegisiter(cpu, reg2);

			uint32_t val = val1 | val2;

			setRegister(cpu, REG_ACC, val);

			break;
		}

		case OR_REG_LIT: {
				uint8_t reg1 = fetch(cpu);

				uint32_t val2 = fetch32(cpu);

				//TODO CHECKS

				uint32_t val1 = getRegisiter(cpu, reg1);

				uint32_t val = val1 | val2;

				setRegister(cpu, REG_ACC, val);

				break;
			}

		case AND_REG_REG: {
				uint8_t reg1 = fetch(cpu);

				uint8_t reg2 = fetch(cpu);

				//TODO CHECKS

				uint32_t val1 = getRegisiter(cpu, reg1);
				uint32_t val2 = getRegisiter(cpu, reg2);

				uint32_t val = val1 & val2;

				setRegister(cpu, REG_ACC, val);

				break;
			}

		case AND_REG_LIT: {
				uint8_t reg1 = fetch(cpu);

				uint32_t val2 = fetch32(cpu);

				//TODO CHECKS

				uint32_t val1 = getRegisiter(cpu, reg1);

				uint32_t val = val1 & val2;

				setRegister(cpu, REG_ACC, val);

				break;
			}

		case XOR_REG_REG: {
				uint8_t reg1 = fetch(cpu);

				uint8_t reg2 = fetch(cpu);

				//TODO CHECKS

				uint32_t val1 = getRegisiter(cpu, reg1);
				uint32_t val2 = getRegisiter(cpu, reg2);

				uint32_t val = val1 ^ val2;

				setRegister(cpu, REG_ACC, val);

				break;
			}

		case XOR_REG_LIT: {
				uint8_t reg1 = fetch(cpu);

				uint32_t val2 = fetch32(cpu);

				//TODO CHECKS

				uint32_t val1 = getRegisiter(cpu, reg1);

				uint32_t val = val1 ^ val2;

				setRegister(cpu, REG_ACC, val);

				break;
			}

		case LSH_REG_REG: {
				uint8_t reg1 = fetch(cpu);

				uint8_t reg2 = fetch(cpu);

				//TODO CHECKS

				uint32_t val1 = getRegisiter(cpu, reg1);
				uint32_t val2 = getRegisiter(cpu, reg2);

				uint32_t val = val1 << val2;

				setRegister(cpu, REG_ACC, val);

				break;
			}

		case LSH_REG_LIT: {
				uint8_t reg1 = fetch(cpu);

				uint32_t val2 = fetch32(cpu);

				//TODO CHECKS

				uint32_t val1 = getRegisiter(cpu, reg1);

				uint32_t val = val1 << val2;

				setRegister(cpu, REG_ACC, val);

				break;
			}

		case RSH_REG_REG: {
				uint8_t reg1 = fetch(cpu);

				uint8_t reg2 = fetch(cpu);

				//TODO CHECKS

				uint32_t val1 = getRegisiter(cpu, reg1);
				uint32_t val2 = getRegisiter(cpu, reg2);

				uint32_t val = val1 >> val2;

				setRegister(cpu, REG_ACC, val);

				break;
			}

		case RSH_REG_LIT: {
				uint8_t reg1 = fetch(cpu);

				uint32_t val2 = fetch32(cpu);

				//TODO CHECKS

				uint32_t val1 = getRegisiter(cpu, reg1);

				uint32_t val = val1 >> val2;

				setRegister(cpu, REG_ACC, val);

				break;
			}

		case NOT_REG: {
				uint8_t reg = fetch(cpu);

				//TODO CHECKS

				uint32_t val = getRegisiter(cpu, reg);

				setRegister(cpu, REG_ACC, ~val);

				break;
			}

		case INC_REG: {

				uint8_t reg = fetch(cpu);

				//TODO CHECK

				uint32_t value = getRegisiter(cpu, reg);

				setRegister(cpu, reg, value + 1);

				if (!(value + 1)) {
					uint32_t flags = getRegisiter(cpu, REG_FLAGS);

					setRegister(cpu, REG_FLAGS, flags | 0x00000010);
				}

				break;
			}

		case DEC_REG: {

				uint8_t reg = fetch(cpu);

				//TODO CHECK

				uint32_t value = getRegisiter(cpu, reg);

				setRegister(cpu, reg, value - 1);

				if (!(value - 1)) {
					uint32_t flags = getRegisiter(cpu, REG_FLAGS);

					setRegister(cpu, REG_FLAGS, flags | 0x00000010);
				}


				break;
			}

		case PSH_REG: {

				uint8_t reg = fetch(cpu);

				//TODO CHECK

				uint32_t value = getRegisiter(cpu, reg);

				push(cpu, value);

				break;
			}

		case PSH_LIT: {

				uint32_t value = fetch32(cpu);

				push(cpu, value);

				break;
			}

		case POP_REG: {

				uint8_t reg = fetch(cpu);

				//TODO CHECK

				uint32_t value = pop(cpu);

				setRegister(cpu, reg, value);

				break;
			}

		case CAL_LIT: {

				uint32_t memAddress = fetch32(cpu);

				uint32_t argCount = mem_getU32(cpu->memory, getRegisiter(cpu, REG_SP) + 4);

				push(cpu, getRegisiter(cpu, REG_R1));
				push(cpu, getRegisiter(cpu, REG_R2));
				push(cpu, getRegisiter(cpu, REG_R3));
				push(cpu, getRegisiter(cpu, REG_R4));
				push(cpu, getRegisiter(cpu, REG_R5));
				push(cpu, getRegisiter(cpu, REG_R6));
				push(cpu, getRegisiter(cpu, REG_R7));
				push(cpu, getRegisiter(cpu, REG_R8));

				push(cpu, getRegisiter(cpu, REG_IP));

				push(cpu, (argCount + 9) * 4 + 4); // Frame size

				setRegister(cpu, REG_FP, getRegisiter(cpu, REG_SP));

				setRegister(cpu, REG_IP, memAddress);

				break;
			}

		case CAL_REG: {

				uint8_t reg = fetch(cpu);

				//TODO CHECK

				uint32_t memAddress = getRegisiter(cpu, reg);

				uint32_t argCount = mem_getU32(cpu->memory, getRegisiter(cpu, REG_SP) + 4);

				push(cpu, getRegisiter(cpu, REG_R1));
				push(cpu, getRegisiter(cpu, REG_R2));
				push(cpu, getRegisiter(cpu, REG_R3));
				push(cpu, getRegisiter(cpu, REG_R4));
				push(cpu, getRegisiter(cpu, REG_R5));
				push(cpu, getRegisiter(cpu, REG_R6));
				push(cpu, getRegisiter(cpu, REG_R7));
				push(cpu, getRegisiter(cpu, REG_R8));

				push(cpu, getRegisiter(cpu, REG_IP));

				push(cpu, (argCount + 9) * 4 + 4); // Frame size

				setRegister(cpu, REG_FP, getRegisiter(cpu, REG_SP) - 4);

				setRegister(cpu, REG_IP, memAddress);

				break;
			}

		case RET: {
			
				setRegister(cpu, REG_SP, getRegisiter(cpu, REG_FP));

				uint32_t frameSize = pop(cpu);

				uint32_t ip = pop(cpu);

				setRegister(cpu, REG_R8, pop(cpu));
				setRegister(cpu, REG_R7, pop(cpu));
				setRegister(cpu, REG_R6, pop(cpu));
				setRegister(cpu, REG_R5, pop(cpu));
				setRegister(cpu, REG_R4, pop(cpu));
				setRegister(cpu, REG_R3, pop(cpu));
				setRegister(cpu, REG_R2, pop(cpu));
				setRegister(cpu, REG_R1, pop(cpu));

				setRegister(cpu, REG_FP, getRegisiter(cpu, REG_FP) + frameSize);

				setRegister(cpu, REG_IP, ip);

				break;
			}

		case INT_LIT: {

				uint8_t intr = fetch(cpu);

				interrupt func = cpu->interruptTable[intr];

				func(cpu);

				break;
			}

		case INT_REG: {

				uint8_t reg = fetch(cpu);

				uint32_t intr = getRegisiter(cpu, reg);

				interrupt func = cpu->interruptTable[intr];

				func(cpu);

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