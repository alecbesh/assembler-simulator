
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

    //Your code starts here:

    for (int i = 0; i < NUMREGS; ++i) {
        state.reg[i] = 0;
    }
    state.pc = 0;
    int count = 0;


    int halt = 0;
    while (halt != -1) {
        int opcode;
        int regA;
        int regB;
        int destReg;
        int offset;
        printState(&state);
        count++;

        opcode = state.mem[state.pc] >> 22;
        if (opcode == 0) { // add
            regA = (state.mem[state.pc] >> 19) & 0b111;
            regB = (state.mem[state.pc] >> 16) & 0b111;
            destReg = (state.mem[state.pc] & 0b111);
            state.reg[destReg] = state.reg[regA] + state.reg[regB];
            state.pc++;
        }

        else if (opcode == 1) { // nor
            regA = (state.mem[state.pc] >> 19) & 0b111;
            regB = (state.mem[state.pc] >> 16) & 0b111;
            destReg = (state.mem[state.pc] & 0b111);
            state.reg[destReg] = ~(state.reg[regA] | state.reg[regB]);
            state.pc++;
        }

        else if (opcode == 2) { // lw
            regA = (state.mem[state.pc] >> 19) & 0b111;
            regB = (state.mem[state.pc] >> 16) & 0b111;
            offset = (state.mem[state.pc] & 0xFFFF);
            if (offset < -32768 || offset > 32767) offset = convertNum(offset);
            state.reg[regB] = state.mem[offset + state.reg[regA]];
            state.pc++;
        }
        else if (opcode == 3) { // sw
            regA = (state.mem[state.pc] >> 19) & 0b111;
            regB = (state.mem[state.pc] >> 16) & 0b111;
            offset = (state.mem[state.pc] & 0xFFFF);
            if (offset < -32768 || offset > 32767) offset = convertNum(offset);
            if(state.numMemory <= state.mem[offset + state.reg[regA]]) //if we are trying to store something on the stack
	            state.numMemory = state.mem[offset + state.reg[regA]] + 1;
            state.mem[offset + state.reg[regA]] = state.reg[regB]; // is it contents of regB or regB itself?
            state.pc++;
        }
        else if (opcode == 4) { // beq
            regA = (state.mem[state.pc] >> 19) & 0b111;
            regB = (state.mem[state.pc] >> 16) & 0b111;
            offset = (state.mem[state.pc] & 0xFFFF);
            if (offset < -32768 || offset > 32767) offset = convertNum(offset);
            if (state.reg[regA] == state.reg[regB]) {
                state.pc = state.pc + 1 + offset;
            }
            else {
                state.pc++;
            }
        }
        else if (opcode == 5) { // jalr
            regA = (state.mem[state.pc] >> 19) & 0b111;
            regB = (state.mem[state.pc] >> 16) & 0b111;
            state.reg[regB] = state.pc + 1;
            state.pc = state.reg[regA];
        }
        
        else if (opcode == 6) { // halt
            state.pc++;
            halt = -1;
            break;
        }
        else if (opcode == 7) { // noop
            state.pc++;
        }
        else {
            exit(1);
        }
    } 
    printf("machine halted\n");
    printf("total of %d instructions executed\n", count);
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

