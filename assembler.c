
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#define MAXLINELENGTH 1000

int readAndParse(FILE *, char *, char *, char *, char *, char *);
int isNumber(char *);
int toInt(char* c) {
    int i;
    sscanf(c, "%d", &i);
    return i;
}
char * toArray(int number) {
    int n = log10(number) + 1;
    int i;
    char *numberArray = calloc(n, sizeof(char));
    for (i = n-1; i >= 0; --i, number /= 10)
    {
        numberArray[i] = (number % 10) + '0';
    }
    return numberArray;
}
/*
int toBinary(int x) {
    if (x == 0) return 000;
    else if (x == 1) return 001;
    else if (x = 2) return 010;
    else if (x = 3) return 011;
    else if (x = 4) return 100;
    else if (x = 5) return 101;
    else if (x = 6) return 110;
    else if (x = 7) return 111;
    else return 0;
}
*/
unsigned int toBinary(unsigned int k) {
    return (k == 0 || k == 1 ? k : ((k % 2) + 10 * toBinary(k / 2)));
}







int main(int argc, char *argv[]) {
    char *inFileString, *outFileString;
    FILE *inFilePtr, *outFilePtr;
    char label[MAXLINELENGTH], opcode[MAXLINELENGTH], arg0[MAXLINELENGTH],
            arg1[MAXLINELENGTH], arg2[MAXLINELENGTH];

    if (argc != 3) {
        printf("error: usage: %s <assembly-code-file> <machine-code-file>\n",
            argv[0]);
        exit(1);
    }

    inFileString = argv[1];
    outFileString = argv[2];

    inFilePtr = fopen(inFileString, "r");
    if (inFilePtr == NULL) {
        printf("error in opening %s\n", inFileString);
        exit(1);
    }
    outFilePtr = fopen(outFileString, "w");
    if (outFilePtr == NULL) {
        printf("error in opening %s\n", outFileString);
        exit(1);
    }

    /* here is an example for how to use readAndParse to read a line from
        inFilePtr */
    if (! readAndParse(inFilePtr, label, opcode, arg0, arg1, arg2) ) {
        /* reached end of file */
    }

    /* this is how to rewind the file ptr so that you start reading from the
        beginning of the file */
    rewind(inFilePtr);

    /* after doing a readAndParse, you may want to do the following to test the
        opcode */
    if (!strcmp(opcode, "add")) {
        /* do whatever you need to do for opcode "add" */
    }



    // Pass 1
    int index = 0;
    char labels_array[1000][8];
    while (readAndParse(inFilePtr, label, opcode, arg0, arg1, arg2)) {
        if (label[0] != '\0') {
            // check for duplicate label:
            for (int i = 0; i < 1000; ++i) {
                if (!strcmp(label, labels_array[i])) {
                    exit(1);
                }
            }

            strcpy(labels_array[index], label);
        }
        index++;
    }


    
    // Pass 2
    rewind(inFilePtr);
    int lineNumber = 0;
    while (readAndParse(inFilePtr, label, opcode, arg0, arg1, arg2)) {
        int result = 0;
        if (!strcmp(opcode, "add")) {
            result = 0b000;
            result = result << 22;
            int x = toInt(arg0);
            int y = toInt(arg1);
            x = x << 19;
            result = result | x;
            y = y << 16;
            result = result | y;
            int z = toInt(arg2);
            result = result | z;
        }


        else if (!strcmp(opcode, "nor")) {
            result = 0b001;
            result = result << 22;
            int x = toInt(arg0);
            int y = toInt(arg1);
            x = x << 19;
            result = result | x;
            y = y << 16;
            result = result | y;
            int z = toInt(arg2);
            result = result | z;
        }


        else if (!strcmp(opcode, "lw")) {
            result = 0b010;
            result = result << 22;
            int x = toInt(arg0);
            int y = toInt(arg1);
            x = x << 19;
            result = result | x;
            y = y << 16;
            result = result | y;

            if (isNumber(arg2) == 1) {
                // numeric address
                int z = toInt(arg2);
                if (z <= 32767 && z >= -32768) {
                    result |= (0xFFFF & z);
                }
                else {
                    exit(1); // over 16 bits
                }
            }
            else {
                // symbolic address
                int memAddress = -1;
                for (int i = 0; i < 100; ++i) {
                    if (!strcmp(labels_array[i], arg2)) {
                        memAddress = i;
                        break;
                    }
                }
                if (memAddress == -1) exit(1);
                // load arg1 with contents at memAddress + contents of arg0
                result = result | memAddress;
            }
        }

        else if (!strcmp(opcode, "sw")) {
            result = 0b011;
            result = result << 22;
            int x = toInt(arg0);
            int y = toInt(arg1);
            x = x << 19;
            result = result | x;
            y = y << 16;
            result = result | y;

            if (isNumber(arg2) == 1) {
                int z = toInt(arg2);
                if (z <= 32767 && z >= -32768) {
                    result |= (0xFFFF & z);
                }
                else {
                    exit(1); // over 16 bits
                }
            }
            else {
                int memAddress = -1;
                for (int i = 0; i < 100; ++i) {
                    if (!strcmp(labels_array[i], arg2)) {
                        memAddress = i;
                        break;
                    }
                }
                if (memAddress == -1) exit(1);
                result = result | memAddress;
            }
        }


        else if (!strcmp(opcode, "beq")) {
            result = 0b100;
            result = result << 22;
            int x = toInt(arg0);
            int y = toInt(arg1);
            x = x << 19;
            result = result | x;
            y = y << 16;
            result = result | y;
            if (isNumber(arg2) == 1) {
                int z = toInt(arg2);
                if (z <= 32767 && z >= -32768) {
                    result |= (0xFFFF & z);
                }
                else {
                    exit(1); // over 16 bits
                }
            }
            else {
                int memAddress = -1;
                for (int i = 0; i < 100; ++i) {
                    if (!strcmp(labels_array[i], arg2)) {
                        memAddress = i;
                        break;
                    }
                }
                if (memAddress == -1) exit(1);
                memAddress = memAddress - lineNumber - 1;
                if (memAddress <= 32767 && memAddress >= -32768) {
                    result |= (0xFFFF & memAddress);
                }
                else {
                    exit(1); // over 16 bits
                }
            }
        }


        else if (!strcmp(opcode, "jalr")) {
            result = 0b101;
            result = result << 22;
            int x = toInt(arg0);
            int y = toInt(arg1);
            x = x << 19;
            result = result | x;
            y = y << 16;
            result = result | y;
        }
        else if (!strcmp(opcode, "halt")) {
            result = 0b110;
            result = result << 22;
        }
        else if (!strcmp(opcode, "noop")) {
            result = 0b111;
            result = result << 22;
        }



        else if (!strcmp(opcode, ".fill")) {
            if (isNumber(arg0) == 1) {
                result = toInt(arg0);
            }
            else {
                result = -1;
                for (int i = 0; i < 100; ++i) {
                    if (!strcmp(labels_array[i], arg0)) {
                        result = i;
                        break;
                    }
                }
                if (result == -1) exit(1);
            }
        }
        else {
            exit(1);
        }


        fprintf(outFilePtr, "%d", result);
        fprintf(outFilePtr, "\n");
        lineNumber++;
    }



    fclose(outFilePtr);
    return(0);
}

int readAndParse(FILE *inFilePtr, char *label, char *opcode, char *arg0,
    char *arg1, char *arg2) {
    char line[MAXLINELENGTH];
    char *ptr = line;

    /* delete prior values */
    label[0] = opcode[0] = arg0[0] = arg1[0] = arg2[0] = '\0';

    /* read the line from the assembly-language file */
    if (fgets(line, MAXLINELENGTH, inFilePtr) == NULL) {
	/* reached end of file */
        return(0);
    }

    /* check for line too long */
    if (strlen(line) == MAXLINELENGTH-1) {
	printf("error: line too long\n");
	exit(1);
    }

    /* is there a label? */
    ptr = line;
    if (sscanf(ptr, "%[^\t\n ]", label)) {
	/* successfully read label; advance pointer over the label */
        ptr += strlen(label);
    }

    /*
     * Parse the rest of the line.  Would be nice to have real regular
     * expressions, but scanf will suffice.
     */
    sscanf(ptr, "%*[\t\n\r ]%[^\t\n\r ]%*[\t\n\r ]%[^\t\n\r ]%*[\t\n\r ]%[^\t\n\r ]%*[\t\n\r ]%[^\t\n\r ]",
        opcode, arg0, arg1, arg2);
    return(1);
}


int isNumber(char *string) {
    /* return 1 if string is a number */
    int i;
    return( (sscanf(string, "%d", &i)) == 1);
}
