/*
 * EECS 370, University of Michigan
 * Project 3: LC-2K Pipeline Simulator
 * Instructions are found in the project spec.
 * Make sure NOT to modify printState or any of the associated functions
**/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Machine Definitions
#define NUMMEMORY 65536 // maximum number of data words in memory
#define NUMREGS 8 // number of machine registers

#define ADD 0
#define NOR 1
#define LW 2
#define SW 3
#define BEQ 4
#define JALR 5 // will not implemented for Project 3 // OK
#define HALT 6
#define NOOP 7

#define NOOPINSTRUCTION (NOOP << 22)

typedef struct IFIDStruct {
	int instr;
	int pcPlus1;
} IFIDType;

typedef struct IDEXStruct {
	int instr;
	int pcPlus1;
	int readRegA;
	int readRegB;
	int offset;
    // data hazards
    int dest;
    int dependency;
} IDEXType;

typedef struct EXMEMStruct {
	int instr;
	int branchTarget;
    int eq;
	int aluResult;
	int readRegB;
    // data hazards
    int dest;
    //int dependency;
} EXMEMType;

typedef struct MEMWBStruct {
	int instr;
	int writeData;
    int dest;
} MEMWBType;

typedef struct WBENDStruct {
	int instr;
	int writeData;
    int dest;
} WBENDType;

typedef struct stateStruct {
	int pc;
	int instrMem[NUMMEMORY];
	int dataMem[NUMMEMORY];
	int reg[NUMREGS];
	int numMemory;
	IFIDType IFID;
	IDEXType IDEX;
	EXMEMType EXMEM;
	MEMWBType MEMWB;
	WBENDType WBEND;
	int cycles; // number of cycles run so far
} stateType;

static inline int opcode(int instruction) {
    return instruction>>22;
}

static inline int field0(int instruction) {
    return (instruction>>19) & 0x7;
}

static inline int field1(int instruction) {
    return (instruction>>16) & 0x7;
}

static inline int field2(int instruction) {
    return instruction & 0xFFFF;
}

// convert a 16-bit number into a 32-bit Linux integer
static inline int convertNum(int num) {
    return num - ( (num & (1<<15)) ? 1<<16 : 0 );
}

void printState(stateType*);
void printInstruction(int);
void readMachineCode(stateType*, char*);

int main(int argc, char *argv[]) {
    stateType state, newState;

    if (argc != 2) {
        printf("error: usage: %s <machine-code file>\n", argv[0]);
        exit(1);
    }

    readMachineCode(&state, argv[1]);

    //initialize all pipes to noops
    state.IFID.instr = NOOPINSTRUCTION;
    state.IDEX.instr = NOOPINSTRUCTION;
    state.IDEX.dest = -1;
    state.EXMEM.instr = NOOPINSTRUCTION;
    state.EXMEM.branchTarget = -1;
    state.MEMWB.instr = NOOPINSTRUCTION;
    state.WBEND.instr = NOOPINSTRUCTION;

    while (opcode(state.MEMWB.instr) != HALT) {
        printState(&state);

        newState = state;
        newState.cycles++;

        /* ---------------------- IF stage --------------------- */
        newState.IFID.instr = state.instrMem[state.pc];
        newState.IFID.pcPlus1 = state.pc + 1;
        newState.pc = state.pc + 1;

        /* ---------------------- ID stage --------------------- */
        newState.IDEX.instr = state.IFID.instr;
        newState.IDEX.pcPlus1 = state.IFID.pcPlus1;
        newState.IDEX.readRegA = state.reg[field0(state.IFID.instr)];
        newState.IDEX.readRegB = state.reg[field1(state.IFID.instr)];

        if(opcode(state.IFID.instr) != ADD && opcode(state.IFID.instr) != NOR)
            newState.IDEX.offset = convertNum(field2(state.IFID.instr));
        
        // keep track of dest register
        if(opcode(state.IFID.instr) == ADD){
            newState.IDEX.dest = field2(state.IFID.instr);
        }
        else if(opcode(state.IFID.instr) == NOR){
            newState.IDEX.dest = field2(state.IFID.instr);
        }
        else if(opcode(state.IFID.instr) == LW){
            newState.IDEX.dest = field1(state.IFID.instr);
        }
        else{
            newState.IDEX.dest = -1;
        }

        // data hazards for LW
        int depend = 0;
        if(opcode(state.IDEX.instr) == LW && (opcode(state.IFID.instr) != NOOP && opcode(state.IFID.instr) != HALT)){
            if(field0(state.IFID.instr) == field1(state.IDEX.instr)){
                depend = 1;
            }
            else if(field1(state.IFID.instr) == field1(state.IDEX.instr)){
                if(opcode(state.IFID.instr) != LW)
                    depend = 1;
            }
        }

        if(depend){
            newState.IDEX.instr = NOOPINSTRUCTION;
            newState.IDEX.dest = -1;
            newState.pc = state.pc;
            newState.IFID.instr = state.IFID.instr;
            newState.IFID.pcPlus1 = state.IFID.pcPlus1;
        }


        /* ---------------------- EX stage --------------------- */
        newState.EXMEM.instr = state.IDEX.instr;

        // BEQ instruction
        if(opcode(state.IDEX.instr) == BEQ){
            int regA = 0;
            int regB = 0;
            if(field0(state.IDEX.instr) == state.EXMEM.dest && (opcode(state.EXMEM.instr) == ADD || opcode(state.EXMEM.instr) == NOR) )
                regA = state.EXMEM.aluResult;
            else if(field0(state.IDEX.instr) == state.MEMWB.dest)
                regA = state.MEMWB.writeData;
            else if(field0(state.IDEX.instr) == state.WBEND.dest)
                regA = state.WBEND.writeData;
            else
                regA = state.IDEX.readRegA;

            if(field1(state.IDEX.instr) == state.EXMEM.dest && (opcode(state.EXMEM.instr) == ADD || opcode(state.EXMEM.instr) == NOR) )
                regB = state.EXMEM.aluResult;
            else if(field1(state.IDEX.instr) == state.MEMWB.dest)
                regB = state.MEMWB.writeData;
            else if(field1(state.IDEX.instr) == state.WBEND.dest)
                regB = state.WBEND.writeData;
            else
                regB = state.IDEX.readRegB;

            newState.EXMEM.branchTarget = state.IDEX.pcPlus1 + state.IDEX.offset;
            if(regA == regB)
                newState.EXMEM.eq = 1;
            else
                newState.EXMEM.eq = 0;
        }
        else
            newState.EXMEM.branchTarget = -1; // set it to untargetable memory for checks

        // ADD instruction
        if(opcode(state.IDEX.instr) == ADD){
            int regA = 0;
            int regB = 0;
            if(field0(state.IDEX.instr) == state.EXMEM.dest && (opcode(state.EXMEM.instr) == ADD || opcode(state.EXMEM.instr) == NOR) )
                regA = state.EXMEM.aluResult;
            else if(field0(state.IDEX.instr) == state.MEMWB.dest)
                regA = state.MEMWB.writeData;
            else if(field0(state.IDEX.instr) == state.WBEND.dest)
                regA = state.WBEND.writeData;
            else
                regA = state.IDEX.readRegA;

            if(field1(state.IDEX.instr) == state.EXMEM.dest && (opcode(state.EXMEM.instr) == ADD || opcode(state.EXMEM.instr) == NOR) )
                regB = state.EXMEM.aluResult;
            else if(field1(state.IDEX.instr) == state.MEMWB.dest)
                regB = state.MEMWB.writeData;
            else if(field1(state.IDEX.instr) == state.WBEND.dest)
                regB = state.WBEND.writeData;
            else
                regB = state.IDEX.readRegB;

            newState.EXMEM.aluResult = regA + regB;
        }

        if(opcode(state.IDEX.instr) == NOR){
            int regA = 0;
            int regB = 0;
            if(field0(state.IDEX.instr) == state.EXMEM.dest && (opcode(state.EXMEM.instr) == ADD || opcode(state.EXMEM.instr) == NOR) )
                regA = state.EXMEM.aluResult;
            else if(field0(state.IDEX.instr) == state.MEMWB.dest)
                regA = state.MEMWB.writeData;
            else if(field0(state.IDEX.instr) == state.WBEND.dest)
                regA = state.WBEND.writeData;
            else
                regA = state.IDEX.readRegA;

            if(field1(state.IDEX.instr) == state.EXMEM.dest && (opcode(state.EXMEM.instr) == ADD || opcode(state.EXMEM.instr) == NOR) )
                regB = state.EXMEM.aluResult;
            else if(field1(state.IDEX.instr) == state.MEMWB.dest)
                regB = state.MEMWB.writeData;
            else if(field1(state.IDEX.instr) == state.WBEND.dest)
                regB = state.WBEND.writeData;
            else
                regB = state.IDEX.readRegB;

            newState.EXMEM.aluResult = ~(regA | regB);
        }

        if(opcode(state.IDEX.instr) == LW){
            int regA = 0;
            if(field0(state.IDEX.instr) == state.EXMEM.dest && (opcode(state.EXMEM.instr) == ADD || opcode(state.EXMEM.instr) == NOR) )
                regA = state.EXMEM.aluResult;
            else if(field0(state.IDEX.instr) == state.MEMWB.dest)
                regA = state.MEMWB.writeData;
            else if(field0(state.IDEX.instr) == state.WBEND.dest)
                regA = state.WBEND.writeData;
            else
                regA = state.IDEX.readRegA;
            newState.EXMEM.aluResult = regA + state.IDEX.offset;
        }

        if(opcode(state.IDEX.instr) == SW){
            int regA = 0;
            if(field0(state.IDEX.instr) == state.EXMEM.dest && (opcode(state.EXMEM.instr) == ADD || opcode(state.EXMEM.instr) == NOR) )
                regA = state.EXMEM.aluResult;
            else if(field0(state.IDEX.instr) == state.MEMWB.dest)
                regA = state.MEMWB.writeData;
            else if(field0(state.IDEX.instr) == state.WBEND.dest)
                regA = state.WBEND.writeData;
            else
                regA = state.IDEX.readRegA;
            newState.EXMEM.aluResult = regA + state.IDEX.offset;

            int regB = 0;
            if(field1(state.IDEX.instr) == state.EXMEM.dest && (opcode(state.EXMEM.instr) == ADD || opcode(state.EXMEM.instr) == NOR) )
                regB = state.EXMEM.aluResult;
            else if(field1(state.IDEX.instr) == state.MEMWB.dest)
                regB = state.MEMWB.writeData;
            else if(field1(state.IDEX.instr) == state.WBEND.dest)
                regB = state.WBEND.writeData;
            else
                regB = state.IDEX.readRegB;

            newState.EXMEM.readRegB = regB; // dependendent
        }
        
        newState.EXMEM.dest = state.IDEX.dest;
        /* --------------------- MEM stage --------------------- */
        newState.MEMWB.instr = state.EXMEM.instr;


        // ADD instruction
        if(opcode(state.EXMEM.instr) == ADD){
            newState.MEMWB.writeData = state.EXMEM.aluResult;
        }
        // NOR instruction
        if(opcode(state.EXMEM.instr) == NOR){
            newState.MEMWB.writeData = state.EXMEM.aluResult;
        }

        // LW instruction
        if(opcode(state.EXMEM.instr) == LW){
            newState.MEMWB.writeData = state.dataMem[state.EXMEM.aluResult];
        }
        if(opcode(state.EXMEM.instr) == SW){
            newState.dataMem[state.EXMEM.aluResult] = state.EXMEM.readRegB;
        }
        if(opcode(state.EXMEM.instr) == BEQ){
           if(state.EXMEM.eq){
               newState.pc = state.EXMEM.branchTarget;
               // set previous instuctions to noops
               newState.IFID.instr = NOOPINSTRUCTION;
               newState.IDEX.instr = NOOPINSTRUCTION;
               newState.IDEX.dest = -1;
               newState.EXMEM.instr = NOOPINSTRUCTION;
           }
           // else nothing
        }

        newState.MEMWB.dest = state.EXMEM.dest;
        /* ---------------------- WB stage --------------------- */
        //newState.MEMWB.instr = (HALT << 22);
        newState.WBEND.instr = state.MEMWB.instr;

        // ADD instruction
        if(opcode(state.MEMWB.instr) == ADD){
            newState.WBEND.writeData = state.MEMWB.writeData;
            newState.reg[field2(state.MEMWB.instr)] = state.MEMWB.writeData;
        }

        // NOR instruction
        if(opcode(state.MEMWB.instr) == NOR){
            newState.WBEND.writeData = state.MEMWB.writeData;
            newState.reg[field2(state.MEMWB.instr)] = state.MEMWB.writeData;
        }

        // LW instruction
        if(opcode(state.MEMWB.instr) == LW){
            newState.WBEND.writeData = state.MEMWB.writeData;
            newState.reg[field1(state.MEMWB.instr)] = state.MEMWB.writeData;
        }

        newState.WBEND.dest = state.MEMWB.dest;
        /* ------------------------ END ------------------------ */
        state = newState; /* this is the last statement before end of the loop. It marks the end 
        of the cycle and updates the current state with the values calculated in this cycle */
    }

    printf("machine halted\n");
    printf("total of %d cycles executed\n", state.cycles);
    printf("final state of machine:\n");
    printState(&state);
}

void printInstruction(int instr) {
    switch (opcode(instr)) {
        case ADD:
            printf("add");
            break;
        case NOR:
            printf("nor");
            break;
        case LW:
            printf("lw");
            break;
        case SW:
            printf("sw");
            break;
        case BEQ:
            printf("beq");
            break;
        case JALR:
            printf("jalr");
            break;
        case HALT:
            printf("halt");
            break;
        case NOOP:
            printf("noop");
            break;
        default:
            printf(".fill %d", instr);
            return;
    }
    printf(" %d %d %d", field0(instr), field1(instr), field2(instr));
}

void printState(stateType *statePtr) {
    printf("\n@@@\n");
    printf("state before cycle %d starts:\n", statePtr->cycles);
    printf("\tpc = %d\n", statePtr->pc);

    printf("\tdata memory:\n");
    for (int i=0; i<statePtr->numMemory; ++i) {
        printf("\t\tdataMem[ %d ] = %d\n", i, statePtr->dataMem[i]);
    }
    printf("\tregisters:\n");
    for (int i=0; i<NUMREGS; ++i) {
        printf("\t\treg[ %d ] = %d\n", i, statePtr->reg[i]);
    }

    // IF/ID
    printf("\tIF/ID pipeline register:\n");
    printf("\t\tinstruction = %d ( ", statePtr->IFID.instr);
    printInstruction(statePtr->IFID.instr);
    printf(" )\n");
    printf("\t\tpcPlus1 = %d", statePtr->IFID.pcPlus1);
    if(opcode(statePtr->IFID.instr) == NOOP){
        printf(" (Don't Care)");
    }
    printf("\n");
    
    // ID/EX
    int idexOp = opcode(statePtr->IDEX.instr);
    printf("\tID/EX pipeline register:\n");
    printf("\t\tinstruction = %d ( ", statePtr->IDEX.instr);
    printInstruction(statePtr->IDEX.instr);
    printf(" )\n");
    printf("\t\tpcPlus1 = %d", statePtr->IDEX.pcPlus1);
    if(idexOp == NOOP){
        printf(" (Don't Care)");
    }
    printf("\n");
    printf("\t\treadRegA = %d", statePtr->IDEX.readRegA);
    if (idexOp >= HALT || idexOp < 0) {
        printf(" (Don't Care)");
    }
    printf("\n");
    printf("\t\treadRegB = %d", statePtr->IDEX.readRegB);
    if(idexOp == LW || idexOp > BEQ || idexOp < 0) {
        printf(" (Don't Care)");
    }
    printf("\n");
    printf("\t\toffset = %d", statePtr->IDEX.offset);
    if (idexOp != LW && idexOp != SW && idexOp != BEQ) {
        printf(" (Don't Care)");
    }
    printf("\n");

    // EX/MEM
    int exmemOp = opcode(statePtr->EXMEM.instr);
    printf("\tEX/MEM pipeline register:\n");
    printf("\t\tinstruction = %d ( ", statePtr->EXMEM.instr);
    printInstruction(statePtr->EXMEM.instr);
    printf(" )\n");
    printf("\t\tbranchTarget %d", statePtr->EXMEM.branchTarget);
    if (exmemOp != BEQ) {
        printf(" (Don't Care)");
    }
    printf("\n");
    printf("\t\teq ? %s", (statePtr->EXMEM.eq ? "True" : "False"));
    if (exmemOp != BEQ) {
        printf(" (Don't Care)");
    }
    printf("\n");
    printf("\t\taluResult = %d", statePtr->EXMEM.aluResult);
    if (exmemOp > SW || exmemOp < 0) {
        printf(" (Don't Care)");
    }
    printf("\n");
    printf("\t\treadRegB = %d", statePtr->EXMEM.readRegB);
    if (exmemOp != SW) {
        printf(" (Don't Care)");
    }
    printf("\n");

    // MEM/WB
	int memwbOp = opcode(statePtr->MEMWB.instr);
    printf("\tMEM/WB pipeline register:\n");
    printf("\t\tinstruction = %d ( ", statePtr->MEMWB.instr);
    printInstruction(statePtr->MEMWB.instr);
    printf(" )\n");
    printf("\t\twriteData = %d", statePtr->MEMWB.writeData);
    if (memwbOp >= SW || memwbOp < 0) {
        printf(" (Don't Care)");
    }
    printf("\n");     

    // WB/END
	int wbendOp = opcode(statePtr->WBEND.instr);
    printf("\tWB/END pipeline register:\n");
    printf("\t\tinstruction = %d ( ", statePtr->WBEND.instr);
    printInstruction(statePtr->WBEND.instr);
    printf(" )\n");
    printf("\t\twriteData = %d", statePtr->WBEND.writeData);
    if (wbendOp >= SW || wbendOp < 0) {
        printf(" (Don't Care)");
    }
    printf("\n");

    printf("end state\n");
}

// File
#define MAXLINELENGTH 1000 // MAXLINELENGTH is the max number of characters we read

void readMachineCode(stateType *state, char* filename) {
    char line[MAXLINELENGTH];
    FILE *filePtr = fopen(filename, "r");
    if (filePtr == NULL) {
        printf("error: can't open file %s", filename);
        exit(1);
    }

    printf("instruction memory:\n");
    for (state->numMemory = 0; fgets(line, MAXLINELENGTH, filePtr) != NULL; ++state->numMemory) {
        if (sscanf(line, "%d", state->instrMem+state->numMemory) != 1) {
            printf("error in reading address %d\n", state->numMemory);
            exit(1);
        }
        printf("\tinstrMem[ %d ] = ", state->numMemory);
        printInstruction(state->dataMem[state->numMemory] = state->instrMem[state->numMemory]);
        printf("\n");
    }
}