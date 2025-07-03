#include <iostream>
#include <stdlib.h>

#include "Pipeline.h"

//----------------------------------------------CARRYING INFO THROUGH PIPELINE----------------------------------------------

//Function to read from Pipeline Register and write to Processor
void PipelineRegister::ReadFromRegister() {

    processor.DepFlag = DepFlag_pl;
    
    processor.current_pc = current_pc_pl;
    for (int i=0; i< 32; i++) {
        processor.instruction[i] = instruction_pl[i];
    }

    processor.op_code = op_code_pl;
    processor.rd = rd_pl;
    processor.f3 = f3_pl;
    processor.rs1 = rs1_pl;
    processor.rs2 = rs2_pl;
    processor.f7 = f7_pl;
    processor.immI = immI_pl;
    processor.immS = immS_pl;
    processor.immB = immB_pl;
    processor.immU = immU_pl;
    processor.immJ = immJ_pl;

    processor.op1 = op1_pl;
    processor.op2 = op2_pl;

    processor.LoadType = LoadType_pl;
    processor.StoreType = StoreType_pl;
    processor.TakeBranch =  TakeBranch_pl;
    processor.MemRead = MemRead_pl;
    processor.MemWrite = MemWrite_pl;
    processor.RegWrite = RegWrite_pl;

    processor.ALUres = ALUres_pl;

    processor.LoadData = LoadData_pl;
    processor.MemAdr = MemAdr_pl;
}

//Function to read from Instruction Packet and write to Pipeline Register
void PipelineRegister::WriteToRegister(InstructionPacket &A) {

    DepFlag_pl = A.DepFlag_ip;
    
    current_pc_pl = A.current_pc_ip;
    for (int i=0; i< 32; i++) {
        instruction_pl[i] = A.instruction_ip[i];
    }

    op_code_pl = A.op_code_ip;
    rd_pl = A.rd_ip;
    f3_pl = A.f3_ip;
    rs1_pl = A.rs1_ip;
    rs2_pl = A.rs2_ip;
    f7_pl = A.f7_ip;
    immI_pl = A.immI_ip;
    immS_pl = A.immS_ip;
    immB_pl = A.immB_ip;
    immU_pl = A.immU_ip;
    immJ_pl = A.immJ_ip;

    op1_pl = A.op1_ip;
    op2_pl = A.op2_ip;

    LoadType_pl = A.LoadType_ip;
    StoreType_pl = A.StoreType_ip;
    TakeBranch_pl = A.TakeBranch_ip;
    MemRead_pl = A.MemRead_ip;
    MemWrite_pl = A.MemWrite_ip;
    RegWrite_pl = A.RegWrite_ip;

    ALUres_pl = A.ALUres_ip;

    LoadData_pl = A.LoadData_ip;
    MemAdr_pl = A.MemAdr_ip;
}

//----------------------------------------------BRANCH PREDICTION----------------------------------------------

//Data strucutre for BRANCH TARGET BUFFER (BTB)
//btb is map from uint32_t to pair of bool and uint32_t
std::map<uint32_t, std::pair<bool, uint32_t> > btb;

//Function checks if predited value of branch taken / not taken is correct or not
//If not correct, updates the value in BTB
int checkPredict() {

    if(btb.find(EX_MA.current_pc_pl) == btb.end()){
        
        btb[EX_MA.current_pc_pl].first = 1;
        btb[EX_MA.current_pc_pl].second = EX_MA.current_pc_pl + EX_MA.immB_pl;
        if (EX_MA.TakeBranch_pl) {
            processor.new_pc = EX_MA.current_pc_pl + EX_MA.immB_pl;
            return 1;
        }
        return 0; //was not present in btb, added now
    
    } else {
        
        if( EX_MA.TakeBranch_pl && (btb[EX_MA.current_pc_pl].first == 0) ){
            
            btb[EX_MA.current_pc_pl].first = 1;
            processor.new_pc = EX_MA.current_pc_pl + EX_MA.immB_pl;
            return 2; //wrong prediction type 1

        } else if ( (EX_MA.TakeBranch_pl == 0) && (btb[EX_MA.current_pc_pl].first == 1) ){
            
            btb[EX_MA.current_pc_pl].first = 0;
            processor.new_pc = EX_MA.current_pc_pl + 4;
            return 2; //wrong prediction type 2
        
        } else {
            processor.new_pc = processor.pc + 4;
            return 0; //right prediction
        }
    }
}

//Stores returned value of checkPredict() function
int predict_type;

//----------------------------------------------DEPENDENCIES AND BUBBLES----------------------------------------------

//Checks for data conflict between Pipeline Register DE-EX and other Pipeline Registers
int PipelineRegister::isConflict() {

    if ( (DE_EX.op_code_pl == 0) | (op_code_pl == 0)) {
        //registers are still empty
        return 0;
    }    
    
    if ( (DE_EX.op_code_pl == 55) | (DE_EX.op_code_pl == 23) | (DE_EX.op_code_pl == 111) ) {
        //LUI | AUIPC | JAL
        //doesnt read from any register
        return 0;
    }

    bool flag = true;
    if ( (DE_EX.op_code_pl == 19) | (DE_EX.op_code_pl == 3) | (DE_EX.op_code_pl == 103) ) {
        //I type | load type | JALR
        //doesnt read from rs2
        flag = false;
    }

    if ( (op_code_pl == 35) | (op_code_pl == 99) ) {
        //store type | B type
        //doesnt write to any register
        return 0;
    }

    int src1 = DE_EX.rs1_pl;
    int src2;
    int dest = rd_pl;

    if (flag) {
        src2 = DE_EX.rs2_pl;
    } else {
        src2 = 32; // (dest == source2) will always be false
    }

    if ( (dest == src1) | (dest == src2) ) {
        if (op_code_pl == 3) { //Load type
            return 2;
        } else {
            return 1;
        }
    } else {
        return 0;
    }

}

//Calls isConflict() to find where exactly conflict is
//Decides what type of forwarding has to be done
int isDataDependency() {
    if(EX_MA.isConflict()){
        switch (EX_MA.isConflict()){
            case 1:
                return 1;  //Add or AddI type 
            case 2:
                return 2;  //Load Type
            default:
                return 0;  // Unexpected case
        }
    } else if(MA_WB.isConflict()){
        switch (MA_WB.isConflict()){
            case 1:
                return 3; //Add or AddI type
            case 2:
                return 4; //Load Type
            default:
                return 0;
        }
    } else {
        return 0;
    }
}

//Checks if a branch instruction has been executed
//Calls checkPredict() to see if bubbles need to be inserted
int isControlDependency() {
    if (EX_MA.op_code_pl == 99) {
        predict_type = checkPredict();
        if (predict_type) {
            return 1; // B type
        }
    } else if (EX_MA.op_code_pl == 111) {
        return 2; //JAL
    } else if (EX_MA.op_code_pl == 103) {
        return 3; //JALR
    }
    return 0;
}

//Sets all values in pipeline register to 0
//Similar to inserting a bubble
void PipelineRegister::Stall() {
    
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

//----------------------------------------------TRACING----------------------------------------------
void PipelineRegister::Trace() {
    std::cout<<" "<<current_pc_pl;
    std::cout<<"  "<<op_code_pl;
    std::cout<<"  "<<rd_pl;
    std::cout<<"  "<<f3_pl;
    std::cout<<"  "<<rs1_pl;
    std::cout<<"  "<<rs2_pl;
    std::cout<<"  "<<f7_pl;
    std::cout<<"  "<<immI_pl;
    std::cout<<"  "<<immS_pl;
    std::cout<<"  "<<immB_pl;
    std::cout<<"  "<<immU_pl;
    std::cout<<"  "<<immJ_pl;
    std::cout<<"  "<<LoadType_pl;
    std::cout<<"  "<<StoreType_pl;
    std::cout<<"  "<<TakeBranch_pl;
    std::cout<<"  "<<MemRead_pl;
    std::cout<<"  "<<MemWrite_pl;
    std::cout<<"  "<<RegWrite_pl;
    std::cout<<"  "<<ALUres_pl;
    std::cout<<"  "<<MemAdr_pl;
    std::cout<<"  "<<LoadData_pl;
    std::cout<<"\n";
}
