/*
 * Project 1
 * EECS 370 LC-2K Instruction-level simulator
 *
 * Make sure *not* to modify printState or any of the associated functions
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// Machine Definitions
#define NUMMEMORY 65536 /* maximum number of words in memory */
#define NUMREGS 8 /* number of machine registers */

// File
#define MAXLINELENGTH 1000 /* MAXLINELENGTH is the max number of characters we read */

typedef struct stateStruct {
    int pc;
    int mem[NUMMEMORY];
    int reg[NUMREGS];
    int numMemory;
} stateType;

void printState(stateType *);

int convertNum(int);

int main(int argc, char *argv[])
{
    char line[MAXLINELENGTH];
    stateType state;
    FILE *filePtr;

    if (argc != 2) {
        printf("error: usage: %s <machine-code file>\n", argv[0]);
        exit(1);
    }

    filePtr = fopen(argv[1], "r");
    if (filePtr == NULL) {
        printf("error: can't open file %s", argv[1]);
        perror("fopen");
        exit(1);
    }

    /* read the entire machine-code file into memory */
    for (state.numMemory = 0; fgets(line, MAXLINELENGTH, filePtr) != NULL; state.numMemory++) {
        if (sscanf(line, "%d", state.mem+state.numMemory) != 1) {
            printf("error in reading address %d\n", state.numMemory);
            exit(1);
        }
        printf("memory[%d]=%d\n", state.numMemory, state.mem[state.numMemory]);
    }

    //Your code starts here

    //initialize registers to 0
    for(int i = 0; i < NUMREGS; ++i){
        state.reg[i] = 0;
    }

    state.pc = 0;
    int instruct = state.mem[state.pc];
    int numInstructs = 0;

    int halted = 0; // boolean for halting

    while(halted != 1){ // represents halt
        printState(&state);
        int opcode = (instruct >> 22) & 0b111;

        if(opcode == 0){ // add
            int regA = (instruct >> 19) & 0b111;
            int regB = (instruct >> 16) & 0b111;
            int destReg = instruct & 0b111;
            state.reg[destReg] = state.reg[regA] + state.reg[regB];
            ++state.pc;
            instruct = state.mem[state.pc];
        }
        else if(opcode == 1){ // nor
            int regA = (instruct >> 19) & 0b111;
            int regB = (instruct >> 16) & 0b111;
            int destReg = instruct & 0b111;
            state.reg[destReg] = ~(state.reg[regA] | state.reg[regB]);
            ++state.pc;
            instruct = state.mem[state.pc];
        }
        else if(opcode == 2){ // lw
            int regA = (instruct >> 19) & 0b111;
            int regB = (instruct >> 16) & 0b111;
            int offSet = instruct & 0xFFFF;
            offSet = convertNum(offSet);

            state.reg[regB] = state.mem[state.reg[regA] + offSet];
            ++state.pc;
            instruct = state.mem[state.pc];
        }
        else if(opcode == 3){ // sw
            int regA = (instruct >> 19) & 0b111;
            int regB = (instruct >> 16) & 0b111;
            int offSet = instruct & 0xFFFF;
            offSet = convertNum(offSet);

            state.mem[state.reg[regA] + offSet] = state.reg[regB];
            ++state.pc;
            instruct = state.mem[state.pc];
        }
        else if(opcode == 4){ // beq
            int regA = (instruct >> 19) & 0b111;
            int regB = (instruct >> 16) & 0b111;
            int offSet = instruct & 0xFFFF;
            offSet = convertNum(offSet);

            if(state.reg[regA] == state.reg[regB])
                state.pc = state.pc + 1 + offSet;
            else
                ++state.pc;
            instruct = state.mem[state.pc];
        }
        else if(opcode == 5){ // jalr
            int regA = (instruct >> 19) & 0b111;
            int regB = (instruct >> 16) & 0b111;

            state.reg[regB] = state.pc + 1;
            state.pc = state.reg[regA];

            instruct = state.mem[state.pc];
        }
        else if(opcode == 6){ // halt
            // should be found from before but there may be comments
            ++state.pc;
            halted = 1;
        }
        else if(opcode == 7){ // noop
            // do nothing
            // often after halt
            ++state.pc;
            instruct = state.mem[state.pc];
        }
        else{
            printf("Unrecognized opcode\n");
            exit(1);
        }

        ++numInstructs;
    }

    printf("machine halted\n");
    printf("total of %d instructions executed\n", numInstructs);
    printf("final state of machine:\n");
    printState(&state);
    return(0);
}

void printState(stateType *statePtr)
{
    int i;
    printf("\n@@@\nstate:\n");
    printf("\tpc %d\n", statePtr->pc);
    printf("\tmemory:\n");
    for (i=0; i<statePtr->numMemory; i++) {
              printf("\t\tmem[ %d ] %d\n", i, statePtr->mem[i]);
    }
    printf("\tregisters:\n");
    for (i=0; i<NUMREGS; i++) {
              printf("\t\treg[ %d ] %d\n", i, statePtr->reg[i]);
    }
    printf("end state\n");
}

int convertNum(int num)
{
    /* convert a 16-bit number into a 32-bit Linux integer */
    if (num & (1<<15) ) {
        num -= (1<<16);
    }
    return(num);
}

