/**
 * Project 2 - Taken from Project 1 
 * Assembler code fragment for LC-2K 
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define MAXLINELENGTH 1000

int readAndParse(FILE *, char *, char *, char *, char *, char *);
int isNumber(char *);

int
main(int argc, char *argv[])
{
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

    //Header: text, data, symbols, reloc
    int t = 0;
    int d = 0;
    int s = 0;
    int r = 0;

    char labels[MAXLINELENGTH][7];
    char syms[MAXLINELENGTH][7]; //symbol section

    int index = 0;

    /* here is an example for how to use readAndParse to read a line from
        inFilePtr */
    while (readAndParse(inFilePtr, label, opcode, arg0, arg1, arg2) ) {
        //Error checking
        if(strlen(label) > 6){
            printf("Label is too long");
            exit(1);
        }
        if(label[0] != '\0'){
            if(label[0] < 'A' || label[0] > 'z' || (label[0] < 'a' && label[0] > 'Z')){
                printf("Label should start with letter");
                exit(1);
            }
        
            for(int i = 0; i < index; ++i){
                if(!strcmp(label, labels[i])){
                    printf("Duplicate Labels");
                    exit(1);
                }
            }
        }
        strcpy(labels[index], label);
        

        int op = 0;
        int fill = 0;
        int l = 0;

        // Check for dupes
        for(int i = 0; i < s; ++i){
            if(!strcmp(syms[i], label)){
                l = 1;
            }
            if(!strcmp(syms[i], arg0)){
                fill = 1;
            }
            if(!strcmp(syms[i], arg2)){
                op = 1;
            }
        }
        // Add to symbols list
        if(!l && label[0] <= 'Z' && label[0] >= 'A'){
            strcpy(syms[s], label);
            ++s;
        }
        if(!op && (!strcmp(opcode, "lw") || !strcmp(opcode, "sw"))){
            if(arg2[0] <= 'Z' && arg2[0] >= 'A'){
                strcpy(syms[s], arg2);
                ++s;
            }
        }
        if(!fill && arg0[0] <= 'Z' && arg0[0] >= 'A'){
            strcpy(syms[s], arg0);
            ++s;
        }

        if(!strcmp(opcode, ".fill")){
            ++d;
            //if not a number must be label
            if(isNumber(arg0) != 1){
                ++r;
            }
        }
        else{
            ++t;
            if((!strcmp(opcode, "lw") || !strcmp(opcode, "sw")) && (isNumber(arg2) != 1)){
                ++r;
            }
        }
        ++index;
        /* reached end of file */
    }

    /* this is how to rewind the file ptr so that you start reading from the
        beginning of the file */
    rewind(inFilePtr);
    
    int index2 = 0;
    //print header
    fprintf(outFilePtr, "%d %d %d %d\n", t,d,s,r);

    // Text and Data
    while (readAndParse(inFilePtr, label, opcode, arg0, arg1, arg2) ) {
        int mcode = 0;
        if(!strcmp(opcode, "add")){
            //do nothing since it is 0 in binary
            int regA = atoi(arg0);
            mcode = (regA << 19) + mcode;
            int regB = atoi(arg1);
            mcode = (regB << 16) + mcode;
            int destReg = atoi(arg2);
            mcode = destReg + mcode;
        }
        else if(!strcmp(opcode, "nor")){
            mcode = (1 << 22) + mcode;

            int regA = atoi(arg0);
            mcode = (regA << 19) + mcode;
            int regB = atoi(arg1);
            mcode = (regB << 16) + mcode;
            int destReg = atoi(arg2);
            mcode = destReg + mcode;
        }
        else if(!strcmp(opcode, "lw")){
            mcode = (2 << 22) + mcode;

            int regA = atoi(arg0);
            mcode = (regA << 19) + mcode;
            int regB = atoi(arg1);
            mcode = (regB << 16) + mcode;
            int offSet;

            if((arg2[0] >= 'a' && arg2[0] <= 'z') || (arg2[0] >= 'A' && arg2[0] <= 'Z')){
                int match = -1;
                for(int i = 0; i < index; ++i){
                    if(!strcmp(arg2, labels[i])){
                        match = i;
                    }
                }

                if(match < 0 && (arg2[0] >= 'a' && arg2[0] <= 'z')){
                    printf("No matching label");
                    exit(1);
                }
                else if(match < 0 && (arg2[0] >= 'A' && arg2[0] <= 'Z')){
                    offSet = 0;
                }
                else{
                    offSet = match;
                }
            }
            else{
                offSet = atoi(arg2);
            }
            
            if(offSet < -32768 || offSet > 32767){
                printf("offsetField does not fit in 16 bits");
                exit(1);
            }
            mcode = (offSet & 0xFFFF) + mcode;
        }
        else if(!strcmp(opcode, "sw")){
            mcode = (3 << 22) + mcode;

            int regA = atoi(arg0);
            mcode = (regA << 19) + mcode;
            int regB = atoi(arg1);
            mcode = (regB << 16) + mcode;
            int offSet;

            if((arg2[0] >= 'a' && arg2[0] <= 'z') || (arg2[0] >= 'A' && arg2[0] <= 'Z')){
                int match = -1;
                for(int i = 0; i < index; ++i){
                    if(!strcmp(arg2, labels[i])){
                        match = i;
                    }
                }

                if(match < 0 && (arg2[0] >= 'a' && arg2[0] <= 'z')){
                    printf("No matching label");
                    exit(1);
                }
                else if(match < 0 && (arg2[0] >= 'A' && arg2[0] <= 'Z')){
                    offSet = 0;
                }
                else{
                    offSet = match;
                }
            }
            else{
                offSet = atoi(arg2);
            }

            if(offSet < -32768 || offSet > 32767){
                printf("offsetField does not fit in 16 bits");
                exit(1);
            }

            mcode = (offSet & 0xFFFF) + mcode;
        }
        else if(!strcmp(opcode, "beq")){
            mcode = (4 << 22) + mcode;

            int regA = atoi(arg0);
            mcode = (regA << 19) + mcode;
            int regB = atoi(arg1);
            mcode = (regB << 16) + mcode;
            int offSet;

            if((arg2[0] >= 'a' && arg2[0] <= 'z') || (arg2[0] >= 'A' && arg2[0] <= 'Z')){
                int match = -1;
                for(int i = 0; i < index; ++i){
                    if(!strcmp(arg2, labels[i])){
                        match = i;
                    }
                }
                if(match < 0){
                    printf("No matching label");
                    exit(1);
                }
                offSet = match;
                offSet = offSet - 1 - index2;
            }
            else{
                offSet = atoi(arg2);
            }

            if(offSet < -32768 || offSet > 32767){
                printf("offsetField does not fit in 16 bits");
                exit(1);
            }

            mcode = (offSet & 0xFFFF) + mcode;
        }
        else if(!strcmp(opcode, "jalr")){
            mcode = (5 << 22) + mcode;

            int regA = atoi(arg0);
            mcode = (regA << 19) + mcode;
            int regB = atoi(arg1);
            mcode = (regB << 16) + mcode;
        }
        else if(!strcmp(opcode, "halt")){
            mcode = (6 << 22) + mcode;
            // thats it
        }
        else if(!strcmp(opcode, "noop")){
            mcode = (7 << 22) + mcode;
            // thats it
        }
        else if(!strcmp(opcode, ".fill")){
            if(isNumber(arg0)){
                mcode = atoi(arg0);
            }
            else{
                int match = -1;
                for(int i = 0; i < index; ++i){
                    if(!strcmp(labels[i], arg0)){
                        match = i;
                    }
                }
                if(match < 0 && arg0[0] <= 'z' && arg0[0] >= 'a'){
                    printf("No matching label");
                    exit(1);
                }
                else if(match < 0 && arg0[0] <= 'Z' && arg0[0] >= 'A'){
                    match = 0;
                }

                mcode = match;
            }
        }
        else{
            printf("Unrecognized opcode");
            exit(1);
        }

        fprintf(outFilePtr, "%d\n", mcode);
        ++index2;
        /* reached end of file */
    }

    //Symbol Table
    for(int scount = 0; scount < s; ++scount){
        int loc = -1;
        for(int i = 0; i < index; ++i){
            if(!strcmp(syms[scount], labels[i]))
                loc = i;
        }

        if(loc == -1){
            int temp = 0;
            fprintf(outFilePtr, "%s U %d\n", syms[scount], temp);
        }
        else{
            if(loc < t){
                fprintf(outFilePtr, "%s T %d\n", syms[scount], loc);
            }
            else{
                int temp = loc - t;
                fprintf(outFilePtr, "%s D %d\n", syms[scount], temp);
            }
        }
    }
    
    rewind(inFilePtr);
    //Relocation Table
    int index4 = 0;

    while (readAndParse(inFilePtr, label, opcode, arg0, arg1, arg2) ){
        if((!strcmp(opcode, "lw") || !strcmp(opcode, "sw")) && (isNumber(arg2) != 1)){
            fprintf(outFilePtr, "%d %s %s\n", index4, opcode, arg2);
        }
        if(!strcmp(opcode, ".fill") && (isNumber(arg0) != 1)){
            int temp = index4 - t;
            fprintf(outFilePtr, "%d %s %s\n", temp, opcode, arg0);
        }
        ++index4;
    } 

    fclose(outFilePtr);
    return(0);
}

/*
 * Read and parse a line of the assembly-language file.  Fields are returned
 * in label, opcode, arg0, arg1, arg2 (these strings must have memory already
 * allocated to them).
 *
 * Return values:
 *     0 if reached end of file
 *     1 if all went well
 *
 * exit(1) if line is too long.
 */
int
readAndParse(FILE *inFilePtr, char *label, char *opcode, char *arg0,
    char *arg1, char *arg2)
{
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

int
isNumber(char *string)
{
    /* return 1 if string is a number */
    int i;
    return( (sscanf(string, "%d", &i)) == 1);
}


