/* 
The project is developed as part of Computer Architecture course
Project Name: Functional Simulator for subset of RISCV Processor

Developer's Name: Nachiket Patil
Developer's Email id: 2023csb1136@iitrpr.ac.in
Developer's Name: Atul Kharat
Developer's Email id: 2023csb1105@iitrpr.ac.in
Date: 
*/

/* myRISCVSim.cpp
   Purpose of this file: implementation file for myRISCVSim
*/
#include <bits/stdc++.h>
using namespace std;

#define M 32
void write_data_memory();
static unsigned int X[32]; // 32 resitors 
static unsigned int MEM[4000]; //Instruction memory
static int DMEM[1000000]; //give lui in range of 0x00010 (Data memory)
static unsigned int instruction_word;
static unsigned int operand1;
static unsigned int operand2;
// Global variables
char Type = '0'; 
static bitset<M> inst; // 32 bits instruction 
static unsigned int des_reg; // destination resistor 
static int des_res; //destination result after ALU step
string subtype; // subtype of the instruction
static int imm; // immediate value if a immediate instruction
static int  pc = 0; // PC value 
unsigned int sz = 0; // total number of instructions in the assembly program

// determing the type pf the instruction 
char op_R_type(bitset<7> op)
{
  // R type instructions
  bitset<7> opr("0110011");
  if (op == opr)
    return 'R';
  else
    return '0';
}
char op_I_type(bitset<7> op)
{
  // I type instructions
  bitset<7> opi1("0010011");
  bitset<7> opi2("1100111");
  bitset<7> opi3("0000011");

  if (op == opi1 || op == opi2|| op == opi3)
    return 'I';
  else
    return '0';
}
char op_J_type(bitset<7> op)
{
  // J type instructions
  bitset<7> opj("1101111");
  if (op == opj)
    return 'J';
  else
    return '0';
}
char op_B_type(bitset<7> op)
{
  // B type instructions
  bitset<7> opb("1100011");
  if (op == opb)
    return 'B';
  else
    return '0';
}
char op_S_type(bitset<7> op)
{
  // S type instructions
  bitset<7> ops("0100011");
  if (op == ops)
    return 'S';
  else
    return '0';
}
char op_U_type(bitset<7> op)
{
  // U type instructions
  bitset<7> opu1("0110111");
  bitset<7> opu2("0010111");
  if (op == opu1 || op == opu2)
    return 'U';
  else
    return '0';
}
// end checking
string findTwoscomplement(string str) // for the finding the twos complement as requied in calculation of pc after sign extension of immediate 
{
    int n = str.length(); int i=0;
    // Traverse the string to get first '1' from
    // the last of string
    for (i = n-1 ; i >= 0 ; i--)
        if (str[i] == '1')
            break;
            
    // If there exists no '1' concatenate 1 at the
    // starting of string
    if (i == -1)
        return '1' + str;
    // Continue traversal after the first '1' is
    for (int k = i-1 ; k >= 0; k--)
    {
	// Just flip the values
        if (str[k] == '1') str[k] = '0';
        else  str[k] = '1';
    } return str;
}

//  Selection the subtype of instruction for example addi,jal,beg etc 
//  Function will take func3 , func7 and op as input and return subtype as a string from the type of instruction
string subtype_select(bitset<3> func3, bitset<7> func7,bitset<7> op)
{
  // converting the bitset to string for comparison
  string Func3=func3.to_string(),Func7=func7.to_string(),Op = op.to_string();
  // determing the subtype of the instruction
  switch(Type){
    case 'R':{
      // add, and, or, sll, slt, sra, srl, sub, xor
      if(Func3=="000" && Func7 =="0000000"){
        subtype="add";
      }
      else if(Func3=="111" && Func7 =="0000000"){ subtype= "and";}
      else if(Func3=="110" && Func7 =="0000000"){
        subtype= "or";
      }
      else if(Func3=="001" && Func7 =="0000000"){
        subtype= "sll";
      }
      else if(Func3=="010" && Func7 =="0000000"){
        subtype= "slt";
      }
      else if(Func3=="101" && Func7 =="0100000"){
        subtype= "sra";
      }
      else if(Func3=="101" && Func7 =="0000000"){
        subtype= "srl";
      }
      else if(Func3=="000" && Func7 =="0100000"){
        subtype= "sub";
      }
      else if(Func3=="100" && Func7 =="0000000"){
        subtype= "xor";
      }
      break;
    }
    case 'I':{
      // addi, andi, ori, lb, lh, lw, jalr, slli
      if(Func3=="000"&& Op =="0010011" ){
        subtype="addi";
      }
      else if(Func3=="111" ){
        subtype="andi";
      }
      else if(Func3=="110" ){
        subtype="ori";
      }
      else if(Func3=="000" && Op =="0000011" ){
        subtype="lb";
      }
      else if(Func3=="001"  && Op =="0000011"){
        subtype="lh";
      }
      else if(Func3=="010"  && Op =="0000011" ){
        subtype="lw";
      }
      else if(Func3=="000" && Op =="1100111" ){
        subtype="jalr";
      }
      else if(Func3=="001" && Op =="0010011" ){
        subtype="slli";
      }
      break;
    }
    case 'B':{
      // beq, bne, bge, blt
      if(Func3=="000" ){
        subtype="beq";
      }
      else if(Func3=="001" ){
        subtype="bne";
      }
      else if(Func3=="101" ){
        subtype="bge";
      }
      else if(Func3=="100"  ){
        subtype="blt";
      }
      break;
    }
    case 'J':{
        // jal
        subtype="jal";
      break;
    }
    case 'S':{
      // sb, sw, sh
      if(Func3=="000" ){
        subtype="sb";
      }
      else if(Func3=="001" ){
        subtype="sh";
      }
      else if(Func3=="010" ){
        subtype="sw";
      }
      break;
    }
    case 'U':{
      // auipc, lui
      if(Op =="0010111" ){
        subtype="auipc";
      }
      else if(Op =="0110111" ){
        subtype="lui";
      }
      break;
    }
    default:{
      // if no subtype is found then error
      cout<<"error"<<endl;
    }
  }
  return Func3;
}

// function to load the resisters in the file
void load_resister(){
  FILE *fp;
  unsigned int i;
  // opening the file in write mode
  fp = fopen("register.mc", "w");
  if (fp == NULL)
  {
    printf("Error opening dataout.mc file for writing, "); return;
  }
  // writing the resisters in the file
  for (i = 0; i < 32; i++)
  {
    fprintf(fp, "x%u - %u                                                                            \n", i, X[i]);
  } 
  fclose(fp);
}

// function to load the data memory in the file
void load_Memory(){
  FILE *fp;
  unsigned int i;
  // opening the file in write mode
  fp = fopen("D_Memory.mc", "w");
  if (fp == NULL)
  {
    printf("Error opening dataout.mc file for writing, "); return;
  }
  // writing the data memory in the file
  for (i = 0; i < 1000000; i += 4)
  {
    fprintf(fp, "%u                                                                                   \n", DMEM[i]);
  }
  fclose(fp);
}

//function to exit the program
void swi_exit() {
  //  write_data_memory();
  load_resister();
  load_Memory();
  exit(0);
 }

// before running any set of instructions initializing the values of resistors, Instruction and Data memory
// every resitor to 0 except the stack pointer whick will point to the last index of data memory as for the recurssion to work
void reset_proc()
{
  // initializing the resisters to 0
  for (auto i : X)
  {
    i = 0;
  }
  X[2]=999999;
  // initializing the instruction memory to 0
  for (auto p : MEM)
  {
    p = 0;
  }
  // initializing the data memory to 0
  for (auto q : DMEM)  q = 0;
}

// read and write in file start
int read_word(unsigned int *mem, unsigned int address)
{
  int *data;
  // reading the data from the memory
  data = (int *)(mem + address);
  return *data;
}
void write_word(unsigned int *mem, unsigned int address, unsigned int data)
{
  int *data_p;
  // writing the data to the memory
  data_p = (int *)(mem + address);
  *data_p = data;
  sz++; // Determing how many instructions are there 
}
// read and write ends

// function to write the data memory in the file
void write_data_memory()
{
  FILE *fp;
  unsigned int i;
  // opening the file in write mode
  fp = fopen("data_out.mc", "w");
  if (fp == NULL)
  {
    printf("Error opening dataout.mc file for writing, "); return;
  }
  // writing the data memory in the file
  for (i = 0; i <= 4*sz; i = i + 4){fprintf(fp, "%u %u\n", i, read_word(MEM, i));}
  fclose(fp);
}

// function to load the program memory in the file
void load_program_memory()
{
  FILE *fp;
  unsigned int address, instruction;
  // opening the file in read mode
  fp = fopen("test_case.mc", "r"); //reading from the file 
  if (fp == NULL)
  {
    printf("Error opening input mc file, ");
    exit(1);
  }

  // reading the instructions from the file
  while (fscanf(fp, "%x %x", &address, &instruction) != EOF) //instruction is stored in instruction variable and PC in address variable
  {
    write_word(MEM, address, instruction);
  }
  write_data_memory();
  fclose(fp);
}

// reads from the instruction memory and updates the instruction register
void fetch()
{
  inst = MEM[pc];
  string insth;bitset<32> pac(pc);
  cout<<"Fetch instruction : 0x"<<hex <<inst.to_ulong() <<" from address 0x"<<hex<<pac.to_ulong()<<endl;
  bitset<32> exitcode("00000000000000000000000001110011"); //end when come across 0x00000073 as the instruction
  if(inst == exitcode ){
    swi_exit();
  }

}
// reads the instruction register, reads operand1, operand2 fromo register file, decides the operation to be performed in execute stage
void decode()
{
  cout<<"Decode :"<<endl;
  bitset<7> op;
  bitset<7> func7;
  bitset<3> func3;
  bitset<5> rs1, rs2, rd;

  for (int i = 0; i < 7; i++) //determining the OP Code 
  {
    op[i] = inst[i];
  }
  while (1) //Determining the Type of the instruction using the OP Code 
  {
    if (Type == '0')
    {
      Type = op_R_type(op);
    }
    if (Type == '0')
      Type = op_I_type(op);
    if (Type == '0')
      Type = op_J_type(op);
    if (Type == '0')
      Type = op_B_type(op);
    if (Type == '0')
      Type = op_S_type(op);
    if (Type == '0')
      Type = op_U_type(op);
    break;
  }
  cout<<"Format of instruction : "<< Type<<endl;

  // Determing the Func3 , Func 7 and value of rs1 an rs2 in binary form 
  int j = 0;
  // Determining the Func3 and Func7
  for (int i = 25; i < 32; i++)
  {
    func7[j] = inst[i];
    j++;
  }
  j = 0;
  for (int i = 12; i < 15; i++)
  {
    func3[j] = inst[i]; j++;
  }
  j = 0;
  // Determining the rs1 and rs2
  for (int i = 15; i < 20; i++)
  {
    rs1[j] = inst[i]; j++;
  }
  operand1 = rs1.to_ulong(); //converting the binary string to unsigned decimal to get the registor 1 
  j = 0;
  for (int i = 20; i < 25; i++)
  {
    rs2[j] = inst[i];
    j++;
  }
  operand2 = rs2.to_ulong(); //converting the binary string to unsigned decimal to get the registor 2

  switch (Type)
  {
  case 'R':
  {

    j = 0;
    for (int i = 7; i < 12; i++) // Determining destination registor (rd) in binary form 
    {
      rd[j] = inst[i];
      j++;
    }
    des_reg = rd.to_ulong(); //converting the binary string to unsigned decimal to get the destination resistor 
    cout<<"Operand1 : "<< operand1 <<", " <<"Operand2 : "<< operand2<<", "<<"RD : "<< des_reg<< endl;
    // cout << des_reg << endl; 
    break;
   
  } 

  // Determining the immediate value and destination registor (rd) in binary form
  case 'I':{
    bitset<12> immb;
     j = 0;
      bool isneg = false;
    for (int i = 7; i < 12; i++)
    {
      rd[j] = inst[i];
      j++;
    }
    des_reg = rd.to_ulong();
    j=0;

    // determining the immediate of 12 bits 
    for (int i = 20; i < 32; i++)
    {
      immb[j] = inst[i];
      j++;
    }
    // Sign extension of the immediate 
    if(immb[11]==1){ // if MSB of 12 bits immediate is 1
      isneg = true; // Immediate is negative and should be extended to 32 bits accordingly (arithematically)
    }
    if(immb[11]==1){ 
    string s1= immb.to_string();  
    string s = findTwoscomplement(s1);
    bitset<12> opl(s);
    immb = opl;
    }
    imm = immb.to_ulong();
    if(isneg){
      imm = -1*imm;
    }
    cout<<"immediate : "<< imm<<", " <<"Operand1 : "<< operand1<<", "<<"RD : "<< des_reg<< endl; 
    break;

  }
  
  // Determining the immediate value and destination registor (rd) in binary form
  case 'S':{
    bitset<12> immb;
     j=0;
    for (int i = 7; i < 12; i++)
    {
      immb[j] = inst[i];
      j++;
    }
    for (int i = 25; i < 32; i++)
    {
      immb[j] = inst[i];
      j++;
    }
    // Sign extension of the immediate 
    imm = immb.to_ulong();
    if(immb[11]==1){
      imm = -1*imm;
    }
    cout<<"immediate : "<< imm<<", " <<"Operand1 : "<< operand1<<", "<<"operand2 : "<< operand2<< endl; 
    break;

  }

  // Determining the immediate value and destination registor (rd) in binary form
  case 'B':{
    bitset<13> immb;
    bool isneg = false;
     
     immb[0]=0;
    j=1;
    for (int i = 8; i < 12; i++)
    {
      immb[j] = inst[i];
      j++;
    }
    for (int i = 25; i < 31; i++)
    {
      immb[j] = inst[i];
      j++;
    }
    immb[j]=inst[7];
    j++;
    immb[j]= inst[31];

    // Sign extension of the immediate 
    // if MSB of 12 bits immediate is 1
    // Immediate is negative and should be extended to 32 bits accordingly (arithematically)
    if(immb[12]==1){
       isneg = true;
    }
    if(immb[12]==1){
    string s1= immb.to_string();  
    string s = findTwoscomplement(s1);
    bitset<13> opl(s);
    immb=opl;
    }
    imm = immb.to_ulong();
    if(isneg){
      imm = -1*imm;
    }
    cout<<"immediate : "<< imm<<", " <<"Operand1 : "<< operand1<<", "<<"operand2 : "<< operand2<< endl; 
    break;
  }

  // Determining the immediate value and destination registor (rd) in binary form
  case 'U':{
        j = 0;
    for (int i = 7; i < 12; i++)
    {
      rd[j] = inst[i];
      j++;
    }
    des_reg = rd.to_ulong();
    bitset<20> immb;
    j=0;
    for (int i = 12; i < 32; i++)
    {
      immb[j] = inst[i];
      j++;
    }
    imm = immb.to_ulong();
     // the immediate value in the LUI (Load Upper Immediate) and AUIPC (Add Upper Immediate to PC) instructions in RISC-V cannot be negative.So Sign extension required.
    cout<<"immediate : "<< imm<<", "<<"RD : "<< des_reg<< endl; 
    break;
  }

  // Determining the immediate value and destination registor (rd) in binary form
  case 'J':{
    j=0;
    bitset<21> immb;
     bool isneg = false;
    j=0;
    immb[0]=0;
    j=12;
     for (int i = 12; i < 20; i++)
    {
      immb[j] = inst[i];
      j++;
    }
    immb[11]=inst[20];
    j=1;
     for (int i = 21; i < 31; i++)
    {
      immb[j] = inst[i];
      j++;
    }

    // Sign extension of the immediate 
    // if MSB of 12 bits immediate is 1
    // Immediate is negative and should be extended to 32 bits accordingly (arithematically)
    immb[20]=inst[31];
    if(immb[20]==1){
      isneg = true;
    }
    if(immb[20]==1){
    string s1= immb.to_string();  
    string s = findTwoscomplement(s1);
    bitset<21> opl(s);
    immb = opl;
    }
    imm = immb.to_ulong();
    if(isneg){
      imm = -1*imm;
    }
    j = 0;
    for (int i = 7; i < 12; i++)
    {
      rd[j] = inst[i];
      j++;
    }
    des_reg = rd.to_ulong();
    cout<<"immediate : "<< imm<<", "<<"RD : "<< des_reg<< endl; 
    break;
  }
  default:{
    cout<<"error"<<endl;}
  }
  subtype_select(func3,func7,op);
}
// executes the ALU operation based on ALUop
void execute()
{
  cout<<"Operation is "<<subtype<<endl;
  cout<<"Execute : "<<endl;
  if(Type == 'R'){ //add, and, or, sll, slt, sra, srl, sub, xor
    if(subtype == "add")  {des_res = X[operand1] + X[operand2]; cout<<"Adding "<<X[operand1]<<" and "<<X[operand2]<<endl;}
    else if(subtype == "sub") {des_res = X[operand1] - X[operand2]; cout<<"Subtracting "<<X[operand1]<<" and "<<X[operand2]<<endl;}
    else if(subtype == "and") {des_res = X[operand1] & X[operand2]; cout<<"Bitwise AND "<<X[operand1]<<" and "<<X[operand2]<<endl;}
    else if(subtype == "or")  {des_res = X[operand1] | X[operand2]; cout<<"Bitwise OR "<<X[operand1]<<" and "<<X[operand2]<<endl;}
    else if(subtype == "sll") {des_res = X[operand1] << X[operand2]; cout<<"Shift Left "<<X[operand1]<<" and "<<X[operand2]<<endl;}
    else if(subtype == "slt") {des_res = (X[operand1] < X[operand2])?1:0; cout<<"Set Less Than in "<<X[operand1]<<" and "<<X[operand2]<<endl;}
    else if(subtype == "sra") {des_res = X[operand1] >> X[operand2]; cout<<"Shift Right Arithmatic"<<X[operand1]<<" and "<<X[operand2]<<endl;}
    else if(subtype == "xor") {des_res = X[operand1] ^ X[operand2]; cout<<"Bitwise XOR "<<X[operand1]<<" and "<<X[operand2]<<endl;}
    else if(subtype == "srl") {des_res = X[operand1] >> X[operand2]; cout<<"Shift Right Logical "<<X[operand1]<<" and "<<X[operand2]<<endl;} 
    pc=pc+4;
  }
  else if(Type == 'I'){ //addi, andi, ori, lb, lh, lw, jalr
    if(subtype == "addi") {des_res = X[operand1]+imm; cout<<"Adding "<<operand1<<" and "<<imm<<endl;}
    else if(subtype == "andi") {des_res = X[operand1]&imm; cout<<"Bitwise AND "<<operand1<<" and "<<imm<<endl;}
    else if(subtype == "ori") {des_res = X[operand1]|imm; cout<<"Bitwise OR "<<operand1<<" and "<<imm<<endl;}
    else if(subtype == "lb" || subtype == "lh" || subtype == "lw") {des_res = X[operand1]+imm; cout<<"Calculating net memory address by Adding "<<operand1<<" and "<<imm<<endl;}
    else if(subtype == "jalr") { des_res = pc + 4; pc = X[operand1]+imm; cout<<"Calculating net memory address by adding "<<operand1<<" and "<<imm<<endl;} 
    else if(subtype =="slli"){des_res = X[operand1] << imm ; cout<<"Shift Left "<<operand1<<"and"<<operand2<<endl;}
    if(subtype != "jalr"){pc=pc+4;}
  }
  else if(Type == 'B'){//beq, bne, bge, blt
    if(subtype == "beq"){ if(X[operand1] == X[operand2]) {pc += imm; cout<<"if operand1 and operand2 are equal then Adding "<<pc<<" and "<<imm<<endl;}else {pc += 4; cout<<"if operand1 and operand2 are not equal then Adding "<<pc<<" and "<<4<<endl;}}
    else if(subtype == "bne") {if(X[operand1] != X[operand2]) {pc += imm; cout<<"if operand1 and operand2 are not equal then Adding "<<pc<<" and "<<imm<<endl;}  else {pc += 4;cout<<"if operand1 and operand2 are equal then Adding "<<pc<<" and "<<4<<endl;}}
    else if(subtype == "bge") {if(X[operand1] >= X[operand2]){ pc += imm; cout<<"if operand1 greater than and equal to operand2 then Adding "<<pc<<" and "<<imm<<endl;}  else {pc += 4; cout<<"if operand1 less than operand2 then Adding "<<pc<<" and "<<4<<endl;}}
    else if(subtype == "blt") {if(X[operand1] < X[operand2]) {pc += imm; cout<<"if operand1 less than operand2 then Adding "<<pc<<" and "<<imm<<endl;} else {pc += 4; cout<<"if operand1 greater than and equal to operand2 then Adding "<<pc<<" and "<<4<<endl;}}
  }
  else if(Type == 'J'){//jal
    des_res = pc+4; pc += imm; cout<<"Calculating net memory address by adding "<<operand1<<" and "<<imm<<endl;}
  else if(Type == 'S'){//sb, sw, sh
    des_res = X[operand1]+imm;  pc=pc+4; cout<<"Calculating net memory address by Adding "<<operand1<<" and "<<imm<<endl; }
  else if(Type == 'U'){//auipc, lui
    if(subtype == "auipc"){ des_res = pc + (imm<<12); cout<<"Adding pc and Shifted Imm "<<pc<<" and "<<imm<<endl;}
    else if(subtype == "lui"){ des_res = imm<<12;}
    pc=pc+4;
  }
}
// perform the memory operation
void mem()
{
  //Memory operation is required only in Load and store instructions
  cout<<"Memory :\n";
  if(subtype == "lw"|| subtype == "lh" || subtype == "lb"){
    cout<<"Loading from data memory : "<<des_res<<endl;
    des_res = DMEM[des_res]; // Loading value from memory to resister
  }
  else if(subtype == "sw"|| subtype == "sh" || subtype == "sb"){
    DMEM[des_res] = X[operand2];// Loading value from resistor to memory
    cout<<"Storing "<<X[operand2] <<" into data memory : "<<des_res<<endl;
  }
  else cout<<"No Memory operation "<<endl;
}
// writes the results back to register file
void write_back()
{
  cout<<"WriteBack :"<<endl;
  if(Type != 'S' && Type != 'B'){ X[des_reg] = des_res; cout<<"Storing "<<des_res<<" into "<<des_reg<<endl<<endl;}
  else cout<<"No WriteBack Operation \n\n";
  X[0]=0;
  // Ensuring that X0 will always be 0 and wont get overwritten.
  // cout<< X[des_reg]<<endl;
  Type = '0';
}
// should be called when instruction is swi_exit
void run_riscvsim()
{
  while (1)
  {
    fetch();
    decode();
    execute();
    mem();
    write_back();
  }
}
