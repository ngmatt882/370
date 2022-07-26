/**
 * Project 2
 * LC-2K Linker
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define MAXSIZE 300
#define MAXLINELENGTH 1000
#define MAXFILES 6

typedef struct FileData FileData;
typedef struct SymbolTableEntry SymbolTableEntry;
typedef struct RelocationTableEntry RelocationTableEntry;
typedef struct CombinedFiles CombinedFiles;

struct SymbolTableEntry {
	char label[7];
	char location;
	int offset;
};

struct RelocationTableEntry {
	int offset;
	char inst[7];
	char label[7];
	int file;
};

struct FileData {
	int textSize;
	int dataSize;
	int symbolTableSize;
	int relocationTableSize;
	int textStartingLine; // in final executable
	int dataStartingLine; // in final executable
	int text[MAXSIZE];
	int data[MAXSIZE];
	SymbolTableEntry symbolTable[MAXSIZE];
	RelocationTableEntry relocTable[MAXSIZE];
};

struct CombinedFiles {
	int text[MAXSIZE];
	int data[MAXSIZE];
	SymbolTableEntry symTable[MAXSIZE];
	RelocationTableEntry relocTable[MAXSIZE];
	int textSize;
	int dataSize;
	int symTableSize;
	int relocTableSize;
};

int main(int argc, char *argv[])
{
	char *inFileString, *outFileString;
	FILE *inFilePtr, *outFilePtr; 
	int i, j;

	if (argc <= 2) {
		printf("error: usage: %s <obj file> ... <output-exe-file>\n",
				argv[0]);
		exit(1);
	}

	outFileString = argv[argc - 1];

	outFilePtr = fopen(outFileString, "w");
	if (outFilePtr == NULL) {
		printf("error in opening %s\n", outFileString);
		exit(1);
	}

	FileData files[MAXFILES];

  // read in all files and combine into a "master" file
	for (i = 0; i < argc - 2; i++) {
		inFileString = argv[i+1];

		inFilePtr = fopen(inFileString, "r");
		printf("opening %s\n", inFileString);

		if (inFilePtr == NULL) {
			printf("error in opening %s\n", inFileString);
			exit(1);
		}

		char line[MAXLINELENGTH];
		int sizeText, sizeData, sizeSymbol, sizeReloc;

		// parse first line of file
		fgets(line, MAXSIZE, inFilePtr);
		sscanf(line, "%d %d %d %d",
				&sizeText, &sizeData, &sizeSymbol, &sizeReloc);

		files[i].textSize = sizeText;
		files[i].dataSize = sizeData;
		files[i].symbolTableSize = sizeSymbol;
		files[i].relocationTableSize = sizeReloc;

		// read in text section
		int instr;
		for (j = 0; j < sizeText; j++) {
			fgets(line, MAXLINELENGTH, inFilePtr);
			instr = atoi(line);
			files[i].text[j] = instr;
		}

		// read in data section
		int data;
		for (j = 0; j < sizeData; j++) {
			fgets(line, MAXLINELENGTH, inFilePtr);
			data = atoi(line);
			files[i].data[j] = data;
		}

		// read in the symbol table
		char label[7];
		char type;
		int addr;
		for (j = 0; j < sizeSymbol; j++) {
			fgets(line, MAXLINELENGTH, inFilePtr);
			sscanf(line, "%s %c %d",
					label, &type, &addr);
			files[i].symbolTable[j].offset = addr;
			strcpy(files[i].symbolTable[j].label, label);
			files[i].symbolTable[j].location = type;
		}

		// read in relocation table
		char opcode[7];
		for (j = 0; j < sizeReloc; j++) {
			fgets(line, MAXLINELENGTH, inFilePtr);
			sscanf(line, "%d %s %s",
					&addr, opcode, label);
			files[i].relocTable[j].offset = addr;
			strcpy(files[i].relocTable[j].inst, opcode);
			strcpy(files[i].relocTable[j].label, label);
			files[i].relocTable[j].file	= i;
		}
		fclose(inFilePtr);
	} // end reading files

	// *** INSERT YOUR CODE BELOW ***
	//    Begin the linking process
	//    Happy coding!!!

	int numFiles = argc - 2;
	CombinedFiles final;

	// set up starting lines
	final.textSize = 0;
	for(int i = 0; i < numFiles; ++i){
		files[i].textStartingLine = final.textSize;
		final.textSize += files[i].textSize;
	}

	final.dataSize = 0;
	for(int i = 0; i < numFiles; ++i){
		files[i].dataStartingLine = final.dataSize + final.textSize;
		final.dataSize += files[i].dataSize;
	}

	//check for duplicate globals
	final.symTableSize = 0;
	int currTextLine = 0;
	int currDataLine = 0;
	SymbolTableEntry undefSyms[MAXSIZE];
	int undefSize = 0;

	for(int i = 0; i < numFiles; ++i){
		for(int j = 0; j < files[i].symbolTableSize; ++j){
			for(int k = 0; k < final.symTableSize; ++k){
				if(files[i].symbolTable[j].location != 'U' && !strcmp(files[i].symbolTable[j].label, final.symTable[k].label)){
					printf("Duplicate Label");
					exit(1);
				}
			}
			// Check if [Stack] is defined in Text or Data
			if(files[i].symbolTable[j].location != 'U' && !strcmp(files[i].symbolTable[j].label, "Stack")){
				printf("Can not define [Stack]");
				exit(1);
			}

			if(files[i].symbolTable[j].location == 'T'){
				final.symTable[final.symTableSize].location = files[i].symbolTable[j].location;
				final.symTable[final.symTableSize].offset = currTextLine + files[i].symbolTable[j].offset;

				strcpy(final.symTable[final.symTableSize].label, files[i].symbolTable[j].label);
				++final.symTableSize;
			}
			else if(files[i].symbolTable[j].location == 'D'){
				final.symTable[final.symTableSize].location = files[i].symbolTable[j].location;
				final.symTable[final.symTableSize].offset = currDataLine + files[i].symbolTable[j].offset;

				strcpy(final.symTable[final.symTableSize].label, files[i].symbolTable[j].label);
				++final.symTableSize;
			}
			else if(files[i].symbolTable[j].location == 'U'){
				undefSyms[undefSize].location = files[i].symbolTable[j].location;
				strcpy(undefSyms[undefSize].label, files[i].symbolTable[j].label);
				++undefSize;
			}
			else{
				printf("Unrecognized Location");
				exit(1);
			}
		}
		currTextLine += files[i].textSize;
		currDataLine += files[i].dataSize;
	}

	// define [Stack] label
	strcpy(final.symTable[final.symTableSize].label, "Stack");
	final.symTable[final.symTableSize].location = 'D';
	final.symTable[final.symTableSize].offset = final.textSize + final.dataSize; // inconsistent location
	++final.symTableSize;

	// check for undefined labels
	for(int i = 0; i < undefSize; ++i){
		int def = 0;
		for(int j = 0; j < final.symTableSize; ++j){
			if(!strcmp(undefSyms[i].label, final.symTable[j].label)){
				def = 1;
			}
		}
		if(!def){
			printf("Undefined label");
			exit(1);
		}
	}
	//Move Text and Data into Combined Files
	int index1 = 0;
	int index2 = 0;
	for(int i = 0; i < numFiles; ++i){
		for(int j = 0; j < files[i].textSize; ++j){
			final.text[index1] = files[i].text[j];
			++index1;
		}
		for(int k = 0; k < files[i].dataSize; ++k){
			final.data[index2] = files[i].data[k];
			++index2;
		}
	}
	// Checking
	if(index1 != final.textSize || index2 != final.dataSize){
		printf("Mismatch Sizes");
		exit(1);
	}

	// Relocate
	for(int i = 0; i < numFiles; ++i){
		for(int j = 0; j < files[i].relocationTableSize; ++j){
			// check if stack
			unsigned short int offset = 0;
			if(!strcmp(files[i].relocTable[j].label, "Stack")){
				offset = final.textSize + final.dataSize;
				if(!strcmp(files[i].relocTable[j].inst, ".fill")){
					// datastartline goes to far
					final.data[files[i].dataStartingLine - final.textSize + files[i].relocTable[j].offset] += offset;
				}
				else{
					final.text[files[i].textStartingLine + files[i].relocTable[j].offset] += offset;
				}
				// move on
				continue;
			}
			// check if global 
			if(files[i].relocTable[j].label[0] <= 'Z' && files[i].relocTable[j].label[0] >= 'A'){
				for(int k = 0; k < final.symTableSize; ++k){
					if(!strcmp(final.symTable[k].label, files[i].relocTable[j].label)){
						if(final.symTable[k].location == 'T'){
							offset = final.symTable[k].offset;
						}
						else if(final.symTable[k].location == 'D'){
							offset = final.symTable[k].offset + final.textSize;
						}
					}
				}
			}
			else{ // if local variable
				//find the original offset value
				unsigned short int ogoffset = 0;
				if(!strcmp(files[i].relocTable[j].inst, ".fill")){
					ogoffset = files[i].data[files[i].relocTable[j].offset];
				}
				else{
					//take bits 15-0
					ogoffset = files[i].text[files[i].relocTable[j].offset] & 0xFFFF;
				}

				// set new offset value
				if(ogoffset < files[i].textSize){
					offset = files[i].textStartingLine + ogoffset;
				}
				else{
					offset = ogoffset - files[i].textSize + files[i].dataStartingLine;
				}
			}
			// actual relocating part
			unsigned short int replace = 0;
			if(!strcmp(files[i].relocTable[j].inst, ".fill")){
				// dataStartingLine goes too far
				final.data[files[i].relocTable[j].offset + files[i].dataStartingLine - final.textSize] = offset;
			}
			else{ // lw and sw instructions
				replace = final.text[files[i].relocTable[j].offset + files[i].textStartingLine] & 0xFFFF;
				final.text[files[i].relocTable[j].offset + files[i].textStartingLine] -= replace;
				final.text[files[i].relocTable[j].offset + files[i].textStartingLine] += offset;
			}
		}
	}

	//print to file
	for(int i = 0; i < final.textSize; ++i){
		fprintf(outFilePtr, "%d\n", final.text[i]);
	}
	for(int j = 0; j < final.dataSize; ++j){
		fprintf(outFilePtr, "%d\n", final.data[j]);
	}

	fclose(outFilePtr);
	return(0);
} // main
