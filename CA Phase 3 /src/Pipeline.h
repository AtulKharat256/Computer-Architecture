#ifndef PIPELINE_H_
#define PIPELINE_H_

#include <cstring>
#include <map>
#include <iostream>
#include <stdlib.h>

#include "Instruction_Packet.h"
#include "RISCV.h"
#include "GUI.h"

class PipelineRegister{
public:

    //IF-DE Requirement
    uint32_t current_pc_pl;
    int instruction_pl[32];

    //DE-MA Requirement
    int op_code_pl;
    int rd_pl;
    int f3_pl;
    int rs1_pl;
    int rs2_pl;
    int f7_pl;
    int immI_pl;
    int immS_pl;
    int immB_pl;
    long immU_pl;
    int immJ_pl;

    int op1_pl;
    int op2_pl;

    //Control
    int LoadType_pl;
    int StoreType_pl;
    int TakeBranch_pl;
    int MemRead_pl;
    int MemWrite_pl;
    int RegWrite_pl;

    //MA-EX Requirement
    long ALUres_pl;

    //MA-RW Requirement
    long LoadData_pl;
    uint32_t MemAdr_pl;    
    
    //constructor function
    PipelineRegister(){
        
        current_pc_pl = 0;
        for (int i=0 ; i<32 ; i++) {
            instruction_pl[i] = 0;
        }

        op_code_pl = 0;
        rd_pl = 0;
        f3_pl = 0;
        rs1_pl = 0;
        rs2_pl = 0;
        f7_pl = 0;
        immI_pl = 0;
        immS_pl = 0;
        immB_pl = 0;
        immU_pl = 0;
        immJ_pl = 0;

        op1_pl = 0;
        op2_pl = 0;

        LoadType_pl = 0;
        StoreType_pl = 0;
        TakeBranch_pl = 0;
        MemRead_pl = 0;
        MemWrite_pl = 0;
        RegWrite_pl = 0;

        ALUres_pl = 0;

        LoadData_pl = 0;
        MemAdr_pl = 0;

    }

    //For carrying information through the pipelines:
    void WriteToRegister(struct InstructionPacket &);
    void ReadFromRegister();

    //For checking dependencies and inserting bubbles:
    int DepFlag_pl;
    int isConflict();
    void Stall();

    //For tracing values in Pipeline Registers:
    void Trace();

};

//For checking Dependencies between two different Pipeline Registers:
int isDataDependency();
int isControlDependency();

//For implementing branch prediction:
extern std::map<uint32_t, std::pair<bool, uint32_t> > btb;
int checkPredict();
extern int predict_type;

extern PipelineRegister IF_DE;
extern PipelineRegister DE_EX;
extern PipelineRegister EX_MA;
extern PipelineRegister MA_WB;

extern RISCV processor;

#endif