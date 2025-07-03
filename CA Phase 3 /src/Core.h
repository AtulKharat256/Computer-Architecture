#ifndef CORE_H_
#define CORE_H_

#include <cstring>
#include <map>
#include <iostream>
#include <stdlib.h>

#include "Pipeline.h"
#include "Instruction_Packet.h"
#include "RISCV.h"
#include "GUI.h"

class Core {

public:

    char *test_file;
    int N;
    
    //KNOBS read from command line:
    bool pipelining;
    bool forwarding;
    bool tracing;
    int trace_instruction;
    bool register_file;
    bool pipeline_register;
    bool bpu_details;

    //Store values of stats:
    int instructions; //stat 2
    float CPI; //stat 3
    int data_instructions; //stat 4
    int ALU_instructions; //stat 5
    int control_instructions; //stat 6
    int bubbles; //stat 7
    int data_hazards; //stat 8
    int control_hazards; //stat 9
    int data_bubbles; //stat 11
    int control_bubbles; //stat 12
    int wrong_predicts; //stat 10

    //Constructor
    Core() {
        
        N = 5;
        
        pipelining = false;
        forwarding = false;
        tracing = false;
        trace_instruction = 0;
        register_file = false;
        pipeline_register = false;
	bpu_details = false;
	
        instructions = 0;
        CPI = 1.00;
        data_instructions = 0;
        ALU_instructions = 0;
        control_instructions = 0;

        bubbles = 0;
        data_hazards = 0;
        control_hazards = 0;
        data_bubbles = 0;
        control_bubbles = 0;
        wrong_predicts = 0;
        
    }

    //Opening test file and defining memory
    void loadMemory();

    //Starting the actual simulator
    void Run();

    //Pipeline Functionality
    void runCycle();
    void Stop();
    void showStats();

};

void showRegisters();

extern Core CPU;

extern PipelineRegister IF_DE;
extern PipelineRegister DE_EX;
extern PipelineRegister EX_MA;
extern PipelineRegister MA_WB;

extern RISCV processor;

#endif
