#include "Instruction_Packet.h"

RISCV processor;

//Function to read from Processor and write to Instruction Packet
void WriteToPacket(InstructionPacket &A){
    
    A.current_pc_ip = processor.current_pc;
    for (int i=0; i< 32; i++) {
        A.instruction_ip[i] = processor.instruction[i];
    }

    A.op_code_ip = processor.op_code;
    A.rd_ip = processor.rd;
    A.f3_ip = processor.f3;
    A.rs1_ip = processor.rs1;
    A.rs2_ip = processor.rs2;
    A.f7_ip = processor.f7;
    A.immI_ip = processor.immI;
    A.immS_ip = processor.immS;
    A.immB_ip = processor.immB;
    A.immU_ip = processor.immU;
    A.immJ_ip = processor.immJ;

    A.op1_ip = processor.op1;
    A.op2_ip = processor.op2;

    A.LoadType_ip = processor.LoadType;
    A.StoreType_ip = processor.StoreType;
    A.TakeBranch_ip = processor.TakeBranch;
    A.MemRead_ip = processor.MemRead;
    A.MemWrite_ip = processor.MemWrite;
    A.RegWrite_ip = processor.RegWrite;

    A.ALUres_ip = processor.ALUres;

    A.LoadData_ip = processor.LoadData;
    A.MemAdr_ip = processor.MemAdr;

    A.DepFlag_ip = processor.DepFlag;
}
