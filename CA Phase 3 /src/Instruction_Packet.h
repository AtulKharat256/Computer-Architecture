#ifndef PACKET_H_
#define PACKET_H_

#include <stdio.h>
#include <stdlib.h>
#include <string>

#include "RISCV.h"
#include "GUI.h"

//Acts as a mediator before writing to register, just to store the data
struct InstructionPacket{
    
    uint32_t current_pc_ip;
    int instruction_ip[32];

    int op_code_ip;
    int rd_ip;
    int f3_ip;
    int rs1_ip;
    int rs2_ip;
    int f7_ip;
    int immI_ip;
    int immS_ip;
    int immB_ip;
    long immU_ip;
    int immJ_ip;

    int op1_ip;
    int op2_ip;

    int LoadType_ip;
    int StoreType_ip;
    int TakeBranch_ip;
    int MemRead_ip;
    int MemWrite_ip;
    int RegWrite_ip;

    long ALUres_ip;

    long LoadData_ip;
    uint32_t MemAdr_ip;

    int DepFlag_ip;
    
};

extern struct InstructionPacket IF_DE_IP;
extern struct InstructionPacket DE_EX_IP;
extern struct InstructionPacket EX_MA_IP;
extern struct InstructionPacket MA_WB_IP;

void WriteToPacket(struct InstructionPacket &);

#endif