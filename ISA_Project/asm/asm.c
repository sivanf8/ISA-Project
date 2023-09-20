#define _CRT_SECURE_NO_WARNINGS
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <ctype.h>

#define MAX_LINE_LENGTH  500
#define MAX_LABEL_LENGTH 50
#define MEMORY_SPACE 512
#define MAX_INSTRUCTION_LENGTH  32

typedef char line[MAX_LINE_LENGTH + 1];
//Data structure of the labels.
typedef struct {
	char label_name[MAX_LABEL_LENGTH + 1];
	int label_address;
} label;


label labels[MEMORY_SPACE]; //global array to store the labels and their location.
int labels_amount;
char memory[MEMORY_SPACE][9]; //global array to store the memory
int label_counter = 0;

//initialize_memory to zeros
void initialize_memory(char memory[][9]) {
	for (int i = 0; i < MEMORY_SPACE; i++) {
		strcpy(memory[i], "00000000");
	}
}

int enc[3] = { 0 };                //every field is repersent at max 3 bit long  after encoding  to machine lang. for example opcode is 2 bits long,imm is 3 bits ,registers are 1 bit long 
char enchex[4] = { '0','0','0' };  // the top array 'enc' represent the encoding of field in decimal to hexdecimal as int ,the enchex hold the encoding field in hexadecimal as char
char encInstruction[9] = "00000000";              //this array consist the full instruction encoding into its string in machine lang.

//Data structure of instruction, seperate by fields.
typedef struct {
	char* label;
	char* opcode;
	char* rd;
	char* rs;
	char* rt;
	char* immediate;
} line_fields;

//Define line type. 
typedef enum {
	WORD,
	LABEL,
	COMMAND,
	LABEL_COMMAND,
	EMPTY_LINE,
} line_type;

//Decleration of all functions in the program.
line_fields parting_lines_to_fields(char* line);
line_type kind_of_line(line_fields field);
void execute(FILE* input_file, FILE* output_file);
void first_pass(FILE* input_file);
int second_pass(FILE* input_file);
int opcode_string_to_num(char* opcode);
int register_string_to_num(char* reg);
int imm_to_num(char* imm);
void encode_num_to_enc_and_enhex(int* enc, char* enchex, int dec);
void encode_instruction(int opcode, int Rd, int Rs, int Rt, int imm, char* encInstruction);
int Hexa_Int_2s(char* hexnum);
int Hex_char_to_int(char h);
char int_to_hex_char(int num);


// convert opcode to number
int opcode_string_to_num(char* opcode_str) {
	if (opcode_str == NULL) {
		return -1;
	}
	if (strcmp(opcode_str, "add") == 0) return 0;
	if (strcmp(opcode_str, "sub") == 0) return 1;
	if (strcmp(opcode_str, "and") == 0) return 2;
	if (strcmp(opcode_str, "or") == 0) return 3;
	if (strcmp(opcode_str, "sll") == 0) return 4;
	if (strcmp(opcode_str, "sra") == 0) return 5;
	if (strcmp(opcode_str, "srl") == 0) return 6;
	if (strcmp(opcode_str, "beq") == 0) return 7;
	if (strcmp(opcode_str, "bne") == 0) return 8;
	if (strcmp(opcode_str, "blt") == 0) return 9;
	if (strcmp(opcode_str, "bgt") == 0) return 10;
	if (strcmp(opcode_str, "ble") == 0) return 11;
	if (strcmp(opcode_str, "bge") == 0) return 12;
	if (strcmp(opcode_str, "jal") == 0) return 13;
	if (strcmp(opcode_str, "lw") == 0) return 14;
	if (strcmp(opcode_str, "sw") == 0) return 15;
	if (strcmp(opcode_str, "halt") == 0) return 19;
	return -1;
}

//get the register name and return the register number
int register_string_to_num(char* registr_str) {
	if (strcmp(registr_str, "$zero") == 0) return 0;
	if (strcmp(registr_str, "$imm") == 0) return 1;
	if (strcmp(registr_str, "$v0") == 0) return 2;
	if (strcmp(registr_str, "$a0") == 0) return 3;
	if (strcmp(registr_str, "$a1") == 0) return 4;
	if (strcmp(registr_str, "$t0") == 0) return 5;
	if (strcmp(registr_str, "$t1") == 0) return 6;
	if (strcmp(registr_str, "$t2") == 0) return 7;
	if (strcmp(registr_str, "$t3") == 0) return 8;
	if (strcmp(registr_str, "$s0") == 0) return 9;
	if (strcmp(registr_str, "$s1") == 0) return 10;
	if (strcmp(registr_str, "$s2") == 0) return 11;
	if (strcmp(registr_str, "$gp") == 0) return 12;
	if (strcmp(registr_str, "$sp") == 0) return 13;
	if (strcmp(registr_str, "$fp") == 0) return 14;
	if (strcmp(registr_str, "$ra") == 0) return 15;
	return -1;
}


//convert HEX char in range 0-F to int
int Hex_char_to_int(char Hex) { //this function converts a Hex char into its decimal value
	if (Hex >= 97 && Hex <= 122) {
		Hex -= 32;
	}
	if (Hex >= 48 && Hex <= 57) {  //if Hex in range 0-9 ,acorrding to ascii in range 48-57
		return Hex - 48;
	}
	else {
		if (Hex == 'A') return 10;
		if (Hex == 'B') return 11;
		if (Hex == 'C') return 12;
		if (Hex == 'D') return 13;
		if (Hex == 'E') return 14;
		if (Hex == 'F') return 15;
	}
}

//convert int in range 0-15 to hex in char
char int_to_hex_char(int num) { //this function converts int into its hex value as a char 
	if (num >= 0 && num <= 9) {  //if num in range 0-9 , convert it by add 84 and then it will be in range 48-57 in ascii table
		return num + 48;
	}
	else {
		if (num == 10) return 'A';
		if (num == 11) return 'B';
		if (num == 12) return 'C';
		if (num == 13) return 'D';
		if (num == 14) return 'E';
		if (num == 15) return 'F';
	}
}

//convert Hex to int in 2's complement
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

//divide the line to opcode ,rd,rs,rt,imm
line_fields parting_lines_to_fields(char* line) {
	char* token =strtok(line, "#");
	char* endline = line + strlen(line);
	char* location = line;
	line_fields field = { 0 };
	char* firstw = strtok(location, " :\t\n,");
	if (location >= endline) {
		return field;
	}
	if ((firstw != NULL) && opcode_string_to_num(firstw) != -1) {
		field.label = NULL;
		field.opcode = firstw;
	}
	else
	{
		field.label = firstw;
		location += strlen(location) + 1;
		if (location >= endline)
			return field;
		field.opcode = strtok(location, ": \t\n,");
	}

	location += strlen(location) + 1;
	if (location >= endline)
		return field;

	field.rd = strtok(location, " \t\n,");
	location += strlen(location) + 1;
	if (location >= endline)
		return field;

	field.rs = strtok(location, " \t\n,");
	location += strlen(location) + 1;
	if (location >= endline)
		return field;

	field.rt = strtok(location, " \t\n,");
	location += strlen(location) + 1;
	if (location >= endline)
		return field;

	field.immediate = strtok(location, " \t\n,");
	return field;
}

//determines the kind of line 
line_type kind_of_line(line_fields field) {
	if ((field.label != NULL && field.label[0] == '#') || (field.label == NULL && field.opcode == NULL))
		return EMPTY_LINE;
	if (field.label != NULL && field.opcode == NULL)
		return LABEL;
	if (field.label != NULL && field.opcode != NULL && (strcmp(field.label, ".word") != 0))
		return LABEL_COMMAND;
	if (field.label != NULL && strcmp(field.label, ".word") == 0)
		return WORD;
	if (field.label == NULL && field.opcode != NULL)
		return COMMAND;
}


//encode num which represents field to hex 
void encode_num_to_enc_and_enhex(int* enc, char* enchex, int dec) {   //encoding number 'dec' into hex representation for enc array and enchex array
	int i = 0;
	int curr = 2;
	enc[0] = 0; enc[1] = 0; enc[2] = 0; //initialize enc to 0 before using it.
	while (dec != 0) {
		enc[curr] = dec % 16;
		curr--;
		dec /= 16;
	}
	while (enchex[i] != '\0') {
		enchex[i] = int_to_hex_char(enc[i]);
		i += 1;
	}
}

// Encodeing instruction using prev function for each field and put them together as a full line
void encode_instruction(int opcode, int Rd, int Rs, int Rt, int imm, char* encInstruction) { //this func take the convert the full insturcion  into instrucion in machine machine lang.
	encInstruction[9] = "00000000";
	if (opcode != 0 || Rd != 0 || Rt != 0 || Rs != 0) {
		encode_num_to_enc_and_enhex(enc, enchex, opcode);
		encInstruction[0] = enchex[1];
		encInstruction[1] = enchex[2];
		encode_num_to_enc_and_enhex(enc, enchex, Rd);
		encInstruction[2] = enchex[2];
		encode_num_to_enc_and_enhex(enc, enchex, Rs);
		encInstruction[3] = enchex[2];
		encode_num_to_enc_and_enhex(enc, enchex, Rt);
		encInstruction[4] = enchex[2];
		if (imm >= 0) {
			encode_num_to_enc_and_enhex(enc, enchex, imm);
			encInstruction[5] = enchex[0];
			encInstruction[6] = enchex[1];
			encInstruction[7] = enchex[2];
		}
		if (imm < 0) {
			imm = 4096 + imm;
			encode_num_to_enc_and_enhex(enc, enchex, imm);
			encInstruction[5] = enchex[0];
			encInstruction[6] = enchex[1];
			encInstruction[7] = enchex[2];
		}
	}
	else{
		if (imm >= 0) {
			encode_num_to_enc_and_enhex(enc, enchex, imm);
			encInstruction[0] = '0';
			encInstruction[1] = '0';
			encInstruction[2] = '0';
			encInstruction[3] = '0';
			encInstruction[4] = '0';
			encInstruction[5] = enchex[0];
			encInstruction[6] = enchex[1];
			encInstruction[7] = enchex[2];
		}
		if (imm < 0) {
			imm = 4096 + imm;
			encode_num_to_enc_and_enhex(enc, enchex, imm);
			encInstruction[0] = 'F';
			encInstruction[1] = 'F';
			encInstruction[2] = 'F';
			encInstruction[3] = 'F';
			encInstruction[4] = 'F';
			encInstruction[5] = enchex[0];
			encInstruction[6] = enchex[1];
			encInstruction[7] = enchex[2];
		}
	}
}


// convert field immediate to number
int imm_to_num(char* imm) {  // In each instruction there are three options for the immediate field :
	if (imm[0] == '0' && (imm[1] == 'x' || imm[1] == 'X')) // option 1 : imm is Hex number
	{
		return Hexa_Int_2s(imm);
	}                                                                      //option 2 : imm is label
	if ((imm[0] >= 65 && imm[0] <= 90) || (imm[0] >= 97 && imm[0] <= 122)) // if imm starts with a letter This indicated that this is a label .
	{                                                     //checking if imm[0] is letter if its value in the range of letters in Ascii table.
		for (int i = 0; i < labels_amount; i++)
		{
			if (strcmp(imm, labels[i].label_name) == 0) //check which label and return the label location.
				return labels[i].label_address;
		}
	}
	else {                                   //option 3 : A decimal number either positive or negative
		if (imm[0] >= 48 && imm[0] <= 57) {    //positive number : according to Ascii the number nubmers 0-9 are in range 48-57
			int res = 0;
			int len = strlen(imm);
			for (int i = 0; i < len; i++) {
				res = 10 * res + (imm[i] - 48);
			}
			return res;
		}
		else {                                  // negative number
			int res = 0;
			int len = strlen(imm);
			for (int i = 1; i < len; i++) {
				res = 10 * res + (imm[i] - 48);
			}
			res *= -1;
			return res;
		}
	}
}




//the first pass of the array go over the line of the input file and insert label to the array 'labels' 
void first_pass(FILE* input_file) {
	int current_memory_location = 0;
	line temp_line = { 0 };
	while (fgets(temp_line, MAX_LINE_LENGTH, input_file) != NULL) {
		line_fields ins = parting_lines_to_fields(temp_line); //split the line into instruction by fields.
		line_type lt = kind_of_line(ins);
		if (lt == LABEL) //line type is label, insert the label to the lables array
		{
			strcpy(labels[labels_amount].label_name, ins.label);
			labels[labels_amount].label_address = current_memory_location - labels_amount + label_counter;
			labels_amount++;
			current_memory_location++;
		}
		else if (lt == LABEL_COMMAND)
		{
			strcpy(labels[labels_amount].label_name, ins.label);
			labels[labels_amount].label_address = current_memory_location;
			labels_amount++;
			current_memory_location++;
			label_counter++;
		}
		else if (lt == COMMAND)//if the line type is regular we only need to count the lines. 
		{
			current_memory_location++;
		}
	}
}

//go over the file for the second time
int second_pass(FILE* input_file) {
	fseek(input_file, 0, SEEK_SET);
	int current_memory_location = 0;
	int assembly_code_length = 0;    //the actual code of assembly code
	line lne = { 0 };

	while (fgets(lne, MAX_LINE_LENGTH, input_file) != NULL) {
		line_fields ins = parting_lines_to_fields(lne);
		line_type lt = kind_of_line(ins);
		if (lt == WORD) {
			int address;
			int data;
			if (ins.opcode[0] == '0' && (ins.opcode[1] == 'x' || ins.opcode[1] == 'X')) //check if address is hex 
				address = Hexa_Int_2s(ins.opcode);
			else
				address = atoi(ins.opcode);
			if (ins.rd[0] == '0' && (ins.rd[1] == 'x' || ins.rd[1] == 'X')) //check if data is hex
				data = Hexa_Int_2s(ins.rd);
			else
				data = atoi(ins.rd);
			encode_instruction(0, 0, 0, 0, data, encInstruction);
			strcpy(memory[address], encInstruction);
			if (address >= assembly_code_length)
				assembly_code_length = address + 1;
		}
		else if ((lt == COMMAND) || (lt == LABEL_COMMAND)) {
			int opcode = opcode_string_to_num(ins.opcode);
			int rd = register_string_to_num(ins.rd);
			int rs = register_string_to_num(ins.rs);
			int rt = register_string_to_num(ins.rt);
			int imm = imm_to_num(ins.immediate);
			encode_instruction(opcode, rd, rs, rt, imm, encInstruction);
			strcpy(memory[current_memory_location], encInstruction);
			current_memory_location++;
		}
	}
	if (current_memory_location > assembly_code_length) {
		assembly_code_length = current_memory_location;
	}
	return assembly_code_length;
}



// initialize_memory,execute the first and second pass of the input file, then print it to the output file memin  
void execute(FILE* input_file, FILE* output_file) {
	initialize_memory(memory);
	first_pass(input_file);
	int assembly_code_length = second_pass(input_file);
	for (int i = 0; i < assembly_code_length; i++) {
		fprintf(output_file, "%s", memory[i]);//will print to outpot_file the32 bit per register
		fwrite("\n", 1, 1, output_file);// writes \n into the file
	}
}

//open the assembly_code and create the memin file, then call to the Lower Hierarchy function. 
int main(int argc, char* argv[]) {
	FILE* assembly_code = fopen(argv[1], "r"), * memin = fopen(argv[2], "w");
	if (assembly_code == NULL || memin == NULL) {
		printf("Failed to open one of the files, the program will execute with status 1"); //Failed to open one of the files.
		exit(1);
	}
	execute(assembly_code, memin); //read from the input file and write to the output file
	exit(0);
}
