#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#define MEMORY_SPACE 512 
char mem_arr[MEMORY_SPACE + 1][9];
int reg_arr[16];
static int memin_len;
static int pc = 0;
static int cycles = 0;



//defining command as a struct

typedef struct command {
	char inst[9];//contains the line as String
	int opcode;
	int rd;
	int rs;
	int rt;
	int imm;
}Command;


// fuctions declaration
void command_fields(char* command_line, Command* com);
void Execute(Command* com, FILE* ptrace, FILE* pcycles, FILE* pmemout, FILE* pregout);
void Instructions(FILE* ptrace, FILE* pcycles, FILE* pmemout, FILE* pregout);
void Int_to_Hex_Arr(int num, char hexa[9]);
int Hex_char_to_int(char Hex);
int Hexa_Int_2s(char* hexnum);
void FillArray(FILE* memin);
void Trace(Command* com, FILE* ptrace);
void MemOut(FILE* pmemout);
void RegOut(FILE* pregout);




int main(int argc, char* argv[]) {
	FILE* pmemin = fopen(argv[1], "r"), * pmemout = fopen(argv[2], "w"), * pregout = fopen(argv[3], "w"),
		* ptrace = fopen(argv[4], "w"), * pcycles = fopen(argv[5], "w");
	if (pmemin == NULL || pmemout == NULL || pregout == NULL || ptrace == NULL || pcycles == NULL)
	{
		printf("Error opening one or more of the files! \n ");
		exit(1);
	}
	FillArray(pmemin);
	fclose(pmemin);
	Instructions(ptrace, pcycles, pmemout, pregout);
	fprintf(pcycles, "%d", cycles);
	RegOut(pregout);
	MemOut(pmemout);
	fclose(pmemout);
	fclose(pregout);
	fclose(ptrace);
	fclose(pcycles);
	exit(0);
}


void command_fields(char* cline, Command* command) {
	strcpy(command->inst, cline);  //copying the command to temp string
	command->opcode = (int)strtol((char[]) { cline[0], cline[1], 0 }, NULL, 16);// opcode is 2 first elements=8 bits
	command->rd = (int)strtol((char[]) { cline[2], 0 }, NULL, 16);// rd is 1 next element =4 bits
	command->rs = (int)strtol((char[]) { cline[3], 0 }, NULL, 16);// rs is 1 next element =4 bits
	command->rt = (int)strtol((char[]) { cline[4], 0 }, NULL, 16);// rt is 1 next element =4 bits
	command->imm = (int)strtol((char[]) { cline[5], cline[6], cline[7], 0 }, NULL, 16);// imm is 3 last elements=12 bits
	if (command->imm >= 2048) // if msb is 1 then negative num 
		command->imm -= 4096;
	return;
}


void Execute(Command* command, FILE* ptrace, FILE* pcycles, FILE* pmemout, FILE* pregout) {
	Trace(command, ptrace);
	switch (command->opcode) {
	case 0:         //add
		if (command->rd != 0 && command->rd != 1) {
			if (command->rs == 1)
				reg_arr[command->rs] = command->imm;
			if (command->rt == 1)
				reg_arr[command->rt] = command->imm;
			reg_arr[command->rd] = reg_arr[command->rs] + reg_arr[command->rt];
		}
		pc++;
		cycles++;
		break;
	case 1:			//sub
		if (command->rd != 0 && command->rd != 1) {
			if (command->rs == 1)
				reg_arr[command->rs] = command->imm;
			if (command->rt == 1)
				reg_arr[command->rt] = command->imm;
			reg_arr[command->rd] = reg_arr[command->rs] - reg_arr[command->rt];
		}
		pc++;
		cycles++;
		break;
	case 2:			//and
		if (command->rd != 0 && command->rd != 1) {
			if (command->rs == 1)
				reg_arr[command->rs] = command->imm;
			if (command->rt == 1)
				reg_arr[command->rt] = command->imm;
			reg_arr[command->rd] = reg_arr[command->rs] & reg_arr[command->rt];
		}
		pc++;
		cycles++;
		break;
	case 3:			//or
		if (command->rd != 0 && command->rd != 1) {
			if (command->rs == 1)
				reg_arr[command->rs] = command->imm;
			if (command->rt == 1)
				reg_arr[command->rt] = command->imm;
			reg_arr[command->rd] = reg_arr[command->rs] | reg_arr[command->rt];
		}
		pc++;
		cycles++;
		break;
	case 4:			//sll
		if (command->rd != 0 && command->rd != 1) {
			if (command->rs == 1)
				reg_arr[command->rs] = command->imm;
			if (command->rt == 1)
				reg_arr[command->rt] = command->imm;
			reg_arr[command->rd] = reg_arr[command->rs] << reg_arr[command->rt];
		}
		pc++;
		cycles++;
		break;
	case 5:			//sra
		if (command->rd != 0 && command->rd != 1) {
			if (command->rs == 1)
				reg_arr[command->rs] = command->imm;
			if (command->rt == 1)
				reg_arr[command->rt] = command->imm;
			reg_arr[command->rd] = reg_arr[command->rs] >> reg_arr[command->rt];
		}
		pc++;
		cycles++;
		break;
	case 6:			//srl
		if (command->rd != 0 && command->rd != 1) {
			if (command->rs == 1)
				reg_arr[command->rs] = command->imm;
			if (command->rt == 1)
				reg_arr[command->rt] = command->imm;
			reg_arr[command->rd] = reg_arr[command->rs] >> reg_arr[command->rt];
		}
		pc++;
		cycles++;
		break;
	case 7:			//beq
		if (reg_arr[command->rs] == reg_arr[command->rt]) {
			if (command->rd == 1) {
				reg_arr[command->rd] = command->imm;
			}
			pc = reg_arr[command->rd];
		}
		else
			pc++;
		cycles++;
		break;
	case 8:			//bne
		if (reg_arr[command->rs] != reg_arr[command->rt]) {
			if (command->rd == 1) {
				reg_arr[command->rd] = command->imm;
				pc = reg_arr[command->rd];
			}
		}
		else
			pc++;
		cycles++;
		break;
	case 9:			//blt
		if (reg_arr[command->rs] < reg_arr[command->rt]) {
			if (command->rd == 1) {
				reg_arr[command->rd] = command->imm;
				pc = reg_arr[command->rd];
			}
		}
		else
			pc++;
		cycles++;
		break;
	case 10:		//bgt
		if (reg_arr[command->rs] > reg_arr[command->rt]) {
			if (command->rd == 1) {
				reg_arr[command->rd] = command->imm;
				pc = reg_arr[command->rd];
			}
		}
		else
			pc++;
		cycles++;
		break;
	case 11:		//ble
		if (reg_arr[command->rs] <= reg_arr[command->rt]) {
			if (command->rd == 1) {
				reg_arr[command->rd] = command->imm;
				pc = reg_arr[command->rd];
			}
		}
		else
			pc++;
		cycles++;
		break;
	case 12:		//bge
		if (reg_arr[command->rs] >= reg_arr[command->rt]) {
			if (command->rd == 1) {
				reg_arr[command->rd] = command->imm;
				pc = reg_arr[command->rd];
			}
		}
		else
			pc++;
		cycles++;
		break;
	case 13:		//jal
		reg_arr[15] = pc + 1;
		if (command->rd == 1)
			reg_arr[command->rd] = command->imm;
		pc = reg_arr[command->rd];
		cycles++;
		break;
	case 14:		//lw
		if (command->rd != 0 && command->rd != 1) {
			if (command->rs == 1)
				reg_arr[command->rs] = command->imm;
			if (command->rt == 1)
				reg_arr[command->rt] = command->imm;
			reg_arr[command->rd] = Hexa_Int_2s(mem_arr[(reg_arr[command->rs]) + reg_arr[command->rt]]);
		}
		pc++;
		cycles++;
		break;
	case 15:		//sw
		if (command->rd == 1)
			reg_arr[command->rd] = command->imm;
		if (command->rt == 1)
			reg_arr[command->rt] = command->imm;
		if (command->rs == 1)
			reg_arr[command->rs] = command->imm;
		char hexa[9] = "00000000";
		Int_to_Hex_Arr(reg_arr[command->rd], hexa);
		strcpy(mem_arr[(reg_arr[command->rs] + reg_arr[command->rt])], hexa);
		pc++;
		cycles++;
		break;
	case 19:		//halt
		cycles++;
		RegOut(pregout);
		MemOut(pmemout);
		fprintf(pcycles, "%d", cycles);
		fclose(pregout);
		fclose(pmemout);
		fclose(ptrace);
		fclose(pcycles);
		exit(0);
		break;
	}
	return;
}


int Hex_char_to_int(char Hex) { //this function converts a Hex char into its decimal value
	if (Hex >= 97 && Hex <= 122) {
		Hex -= 32;
	}
	if (Hex >= 48 && Hex <= 57) {  //if Hex in range 0-9 ,acorrding to ascii in range 48-57
		return Hex - 48;
	}
	else {
		if (Hex == 'A') {
			return 10;
		}
		if (Hex == 'B') {
			return 11;
		}
		if (Hex == 'C') {
			return 12;
		}
		if (Hex == 'D') {
			return 13;
		}
		if (Hex == 'E') {
			return 14;
		}
		if (Hex == 'F') {
			return 15;
		}
	}
}


void Int_to_Hex_Arr(int num, char hexa[9])
{
	if (num < 0) //if num is negative, add 2^32 to it
		num = num + 4294967296; // num = num + 2^32
	sprintf(hexa, "%08X", num); //set hexa to be num in signed hex
}

int Hexa_Int_2s(char* hexnum) {
	int i, result = 0, len = strlen(hexnum) - 2;
	hexnum = hexnum + 2;
	for (i = 0; i < len; i++)
	{
		result += Hex_char_to_int(hexnum[len - 1 - i]) * (1 << (4 * i)); // change char by char from right to left, and shift it left 4 times
	}
	if ((len <= 8) && (result & (1 << (len * 4 - 1)))) // if len is less than 4 and the msb is 1, we want to sign extend the number
	{
		result |= -1 * (1 << (len * 4)); // or of the number with: 1 from the 3rd char until the msb of result, then zeros till lsb. -1 for the sign.
	}
	return result;
}

void Instructions(FILE* ptrace, FILE* pcycles, FILE* pmemout, FILE* pregout) {
	Command templine = { NULL, NULL, NULL, NULL, NULL, NULL };
	while (pc < memin_len) {
		command_fields(mem_arr[pc], &templine);
		Execute(&templine, ptrace, pcycles, pmemout, pregout);
	}
	return;
}



void FillArray(FILE* memin)
{
	char lne[9];
	int memin_size = 0;
	while (feof(memin) == 0) {
		fscanf(memin, "%8[^\n]\n", lne); //scans the first 8 chars in a line
		strcpy(mem_arr[memin_size], lne); //fills array[i]
		memin_size++;
		memin_len++;
	}
	for (memin_size; memin_size < MEMORY_SPACE; memin_size++) {
		strcpy(mem_arr[memin_size], "00000000");
	}
}


void Trace(Command* com, FILE* ptrace)
{
	char pctrace[9] = { "00000000" };
	char reg_trace[9] = ("00000000");
	char pc_r0[9] = ("00000000");
	int i = 0;
	reg_arr[1] = com->imm;
	Int_to_Hex_Arr(pc, pctrace);
	Int_to_Hex_Arr(reg_arr[1], &reg_trace);
	fprintf(ptrace, "%s %s %s %s ", pctrace, com->inst, pc_r0, reg_trace);
	for (int i = 2; i < 16; i++) {
		Int_to_Hex_Arr(reg_arr[i], &reg_trace);
		fprintf(ptrace, "%s ", reg_trace);
	}
	fseek(ptrace, -1, SEEK_CUR);
	fprintf(ptrace, "\n");
}
void MemOut(FILE* pmemout) {
	for (int i = 0; i <= MEMORY_SPACE; i++)
		fprintf(pmemout, "%s\n", mem_arr[i]);
}
void RegOut(FILE* pregout) {
	for (int i = 2; i < 16; i++)
		fprintf(pregout, "%08X\n", reg_arr[i]);
}