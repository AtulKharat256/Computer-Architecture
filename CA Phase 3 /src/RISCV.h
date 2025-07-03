#ifndef RISCV_H_
#define RISCV_H_

#include <cstring>
#include <map>
#include <iostream>
#include <stdlib.h>
#include <bitset>

//-------------------------------------------------------------------------------------
//json file code starts

#include "GUI.h"

//json file code ends
//-------------------------------------------------------------------------------------

#define ADD_LEN 32
#define MEM_SIZE 0x10000

class RISCV{

public:
    int reg[32];
    uint32_t pc;
    uint32_t new_pc;
    uint32_t current_pc;

    int clock_cycle;
    int instruction[32];
    FILE* programcode;

    //ALU Variables
    int op_code;
    int rd;
    int f3;
    int rs1;
    int rs2;
    int f7;
    int immI;
    int immS;
    int immB;
    long immU;
    int immJ;

    //Control Lines
    int LoadType;
    int StoreType;
    int TakeBranch;
    int MemRead;
    int MemWrite;
    int RegWrite;

    long long op1;
    long long op2;

    uint32_t MemAdr;
    long long LoadData;
    long long ALUres;

    //map<MemAdr, ByteMemory>
    //Since byte is the smallest unit of memory being loaded or stored, 
    //the map used in this program will have 1-byte addressing.
    std::map<uint32_t, uint8_t> memory;


    //constructor function
    RISCV()
    {
        pc = 0x0;
        new_pc = 0x0;
        //setting empty memory:
        memory.clear();
        //setting empty registers:
        for (int i= 0; i < ADD_LEN; i++) {
             reg[i] = 0x0;
        }
        reg[2] = 0x7FFFFFF0; //Stack Pointer
        reg[3] = 0x10000000; //Data Pointer

        clock_cycle = 0;
        for (int i= 0; i < ADD_LEN; i++) {
            instruction[i] = 0x0;
        }
        
        //ALU Variables
        op_code = 0;
        rd = 0;
        f3 = 0;
        rs1 = 0;
        rs2 = 0;
        f7 = 0;
        immI = 0;
        immS = 0;
        immB = 0;
        immU = 0;
        immJ = 0;

        //Control Lines
        LoadType = 0;
        StoreType = 0;
        TakeBranch = 0;
        MemRead = 0;
        MemWrite = 0;
        RegWrite = 0;

        MemAdr = 0;
        LoadData = 0;
        ALUres = 0;
    }
    
    //For pipelining:
    int DataDependency;
    int ControlDependency;
    int DepFlag;

    bool pipeline;
    bool forward;

    void fetch();
    void decode();
    void execute();
    void mem();
    void write_back();

    void reset();
    void store_memory(); 
    void instruction_exit();
};

uint32_t stringtohex(char input[11]);

extern RISCV processor;

#endif