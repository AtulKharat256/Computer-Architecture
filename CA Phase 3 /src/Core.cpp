#include <cstring>
#include <map>
#include <iostream>
#include <stdlib.h>
#include <iterator>
#include"Core.h"

PipelineRegister IF_DE;
PipelineRegister DE_EX;
PipelineRegister EX_MA;
PipelineRegister MA_WB;

InstructionPacket IF_DE_IP;
InstructionPacket DE_EX_IP;
InstructionPacket EX_MA_IP;
InstructionPacket MA_WB_IP;

FILE* output;

//Function will set all pre requisites for running the processor
void Core::loadMemory() {
    
    //Instruction memory:
    processor.programcode = fopen(test_file, "r+");
    if(processor.programcode == NULL) {
        printf("Error opening input file!\n");
        exit(3); //test file related error
    }
    
    //Storing value of N:
    processor.reg[3] = N;

    //Enabling/disabling pipelining and forwarding:
    processor.pipeline = pipelining;
    processor.forward = forwarding;

    //------------------------------------------------------
    //json file code
    {
    output = fopen("../gui/src/components/filedata.json","w");
    jsonStringAdder('{','\n');
    jsonStringAdder("initialProgramStatus");
    jsonStringAdder(':','{');
        jsonStringAdder("registerFile");
        jsonStringAdder(':','[');
            for (int i=0; i<31; i++)
            {
            jsonStringAdder(processor.reg[i],',');
            }
            jsonStringAdder(processor.reg[31],']');
        jsonStringAdder('}',',');
        jsonStringAdder("memoryFile");
        jsonStringAdder(':','{');
            jsonStringAdder("data");
            jsonStringAdder(':','{');
            jsonStringAdder("0x10000000","0");
            jsonStringAdder('}',',');
            jsonStringAdder(' ','\n');
            jsonStringAdder("text");
            jsonStringAdder(':','{');
            jsonStringAdder("0x0","0");
            jsonStringAdder('}','}');
            jsonStringAdder(' ','\n');
        jsonStringAdder(',','\n');
        jsonStringAdder("pipelineRegFile");
        jsonStringAdder(':','{');
            jsonStringAdder("IF-DE");
            jsonStringAdder(':','{');
            jsonStringAdder("null","null");
            jsonStringAdder('}',',');

            jsonStringAdder("DE-EX");
            jsonStringAdder(':','{');
            jsonStringAdder("null","null");
            jsonStringAdder('}',',');

            jsonStringAdder("EX-MA");
            jsonStringAdder(':','{');
            jsonStringAdder("null","null");
            jsonStringAdder('}',',');

            jsonStringAdder("MA-WB");
            jsonStringAdder(':','{');
            jsonStringAdder("null","null");
            jsonStringAdder('}','\n');
    jsonStringAdder('}',',');
    jsonStringAdder(' ','\n');
    jsonStringAdder("pipeline",pipelining);
    jsonStringAdder(',','\n');
    jsonStringAdder("forward",forwarding);
    jsonStringAdder(',','\n');
    jsonStringAdder("stall",!forwarding);
    jsonStringAdder(',','\n');
    jsonStringAdder("cycle");
    jsonStringAdder(':','[');
    }
    //json file code ends
    //-------------------------------------------------------
}

//Function starts the actual processor
void Core::Run() {
    if(!pipelining){
        while(1) {
            //------------------------------------------------------
            //json file code
            {
            jsonStringAdder('{',' ');
            jsonStringAdder("number",processor.clock_cycle);
            jsonStringAdder(',',' ');
            jsonStringAdder("IFpc", processor.pc);
            jsonStringAdder(',',' ');
            jsonStringAdder("IFhexcode",processor.instruction);
            jsonStringAdder(',',' ');
            jsonStringAdder("memFile");
            jsonStringAdder(':','{');
            std::map<uint32_t, uint8_t>::iterator it = processor.memory.begin();
            while (it != processor.memory.end())
            {
                jsonStringAdder(it->first,it->second);
                jsonStringAdder(',',' ');
                ++it;
            }
            jsonStringAdder("0xffffffff","0x0");
            jsonStringAdder('}',' '); 
            jsonStringAdder(',','\n');  
            jsonStringAdder("registerFile");
            jsonStringAdder(':','[');
            for (int i=0; i<31; i++)
            {
                jsonStringAdder(processor.reg[i],',');
            }
            jsonStringAdder(processor.reg[31],']');
            jsonStringAdder('}',' ');  
            jsonStringAdder(',','\n'); 
            }   
            //json file code ends
            //------------------------------------------------------
            //Run without pipelining:
            processor.clock_cycle++;
            processor.fetch();
            processor.decode();
            processor.execute();
            processor.mem();
            processor.write_back();
            processor.pc = processor.new_pc;
            printf("Clock Cycle %d finished.\n",processor.clock_cycle);
            std::cout<<"--------------------------------------------------\n\n";
        }
    }else{
        while(1) {
            //Run with pipelining:
            processor.clock_cycle++;
            runCycle();
            std::cout<<"--------------------------------------------------\n";
            printf("Clock Cycle %d finished.\n",processor.clock_cycle);
            std::cout<<"--------------------------------------------------\n";
            std::cout<<"--------------------------------------------------\n\n";
        }
    }
}

//Simulates all functionality in one clock cycle in a pipelined processor
//Called inside Run() if Pipelining knob is TRUE
void Core::runCycle() {

    //------------------------CHECK DEPENDENCY------------------------

    //note: giving control dependency priority over data dependency
    
    if (EX_MA.op_code_pl == 23) { //AUIPC
        EX_MA.ALUres_pl = EX_MA.current_pc_pl + EX_MA.immU_pl;
    }

    processor.DataDependency = isDataDependency();
    processor.ControlDependency = isControlDependency();
    
    if (processor.ControlDependency) {

        //There is control dependency,
        //Meaning bubbles must be inserted.
        
        control_hazards++;
        control_bubbles += 6; //niti notes: why 6?
        
        IF_DE.Stall();
        DE_EX.Stall();

        IF_DE.DepFlag_pl = 1;
        DE_EX.DepFlag_pl = 1;
        
        switch (processor.ControlDependency) {
        
            //B type
            case 1:
            if (predict_type == 2) {
                wrong_predicts++;
            }
            break;

            //JAL
            case 2:
            processor.new_pc = EX_MA.current_pc_pl + EX_MA.immJ_pl;
            EX_MA.ALUres_pl = EX_MA.current_pc_pl + 4;
            break;

            //JALR
            case 3:
            processor.new_pc = EX_MA.op1_pl + EX_MA.immI_pl;
            EX_MA.ALUres_pl = EX_MA.current_pc_pl + 4;
            break;

        }
    
    } else if (!forwarding & (processor.DataDependency!=0)) {
        //There is data dependency,
        //And forwarding knob is FALSE
        //Meaning bubbles must be inserted
        data_hazards++;
        data_bubbles += 3;
        IF_DE = DE_EX;
        DE_EX.Stall();
        DE_EX.DepFlag_pl = 2;
    }

    //------------------------------------------------------
    //json file code
        jsonStringAdder('{',' ');
        jsonStringAdder("controlDep",processor.ControlDependency);
        jsonStringAdder(',',' ');
        jsonStringAdder("dataDep",processor.DataDependency);
        jsonStringAdder(',',' ');
    //json file code
    //------------------------------------------------------

    //-----------------------------FORWARDING IMPLEMENTATION----------------------------------
    
    if (forwarding) {

        //SETTING OP1 AND OP2 FOR EX
        DE_EX.op1_pl = processor.reg[DE_EX.rs1_pl];
        DE_EX.op2_pl = processor.reg[DE_EX.rs2_pl];

        //json code: flag to give info whether we're forwarding any data
        bool jsonForwardFlag = false;

        switch (processor.DataDependency) {
            
            case 1: //MA-->EX FORWARDING

            if(EX_MA.rd_pl == DE_EX.rs1_pl){
                DE_EX.op1_pl = EX_MA.ALUres_pl;
                //json code: set forwarding flag to true.
                jsonForwardFlag = true;
                jsonStringAdder("fPath","MA-EX");
                jsonStringAdder(',',' ');
            }else if(DE_EX.op_code_pl == 35){
                processor.reg[EX_MA.rs2_pl] = EX_MA.ALUres_pl;
                //json code: set forwarding flag to true.
                jsonForwardFlag = true;
                jsonStringAdder("fPath","WB-EX");
                jsonStringAdder(',',' ');
            }else if((!(DE_EX.op_code_pl == 19)) & (!(DE_EX.op_code_pl == 3)) & (!(DE_EX.op_code_pl == 35))){
                DE_EX.op2_pl = EX_MA.ALUres_pl;
                //json code: set forwarding flag to true.
                jsonForwardFlag = true;
                jsonStringAdder("fPath","MA-EX");
                jsonStringAdder(',',' ');
            }
            //json code: insert forwarding flag.
            jsonStringAdder("forwarding",jsonForwardFlag);
            jsonStringAdder(',',' ');  
            break;
            
            case 2:
            
            if (DE_EX.op_code_pl == 35) { //WB-->MA FORWARDING
                DE_EX.DepFlag_pl = 3;

                //json code: set forwarding flag to true
                jsonStringAdder("fPath","WB-MA");
                jsonStringAdder(',',' ');
                jsonForwardFlag = true;
            }else{ //LOAD-USE HAZARD
                data_hazards++;
                data_bubbles += 3;
                IF_DE = DE_EX;
                DE_EX.Stall();
                DE_EX.DepFlag_pl = 2;
            }
            //json code: insert forwarding flag.
            jsonStringAdder("forwarding",jsonForwardFlag);
            jsonStringAdder(',',' '); 
            break;

            case 3: //WB-->EX FORWARDING
            
            if(MA_WB.rd_pl == DE_EX.rs1_pl){
                DE_EX.op1_pl = MA_WB.ALUres_pl;
                //json code: set forwarding flag to true.
                jsonForwardFlag = true;
                jsonStringAdder("fPath","WB-EX");
                jsonStringAdder(',',' ');
            }else if(DE_EX.op_code_pl == 35){
                processor.reg[DE_EX.rs2_pl] = MA_WB_IP.LoadData_ip;
                //json code: set forwarding flag to true.
                jsonForwardFlag = true;
                jsonStringAdder("fPath","WB-EX");
                jsonStringAdder(',',' ');
            }else if((!(DE_EX.op_code_pl == 19)) & (!(DE_EX.op_code_pl == 3)) & (!(DE_EX.op_code_pl == 35))){
                DE_EX.op2_pl = MA_WB.ALUres_pl;
                //json code: set forwarding flag to true.
                jsonForwardFlag = true;
                jsonStringAdder("fPath","WB-EX");
                jsonStringAdder(',',' ');
            }
            //json code: insert forwarding flag.
            jsonStringAdder("forwarding",jsonForwardFlag);
            jsonStringAdder(',',' '); 
            break;  
            
            case 4: //WB-->EX FORWARDING (Load Type)

            if(MA_WB.rd_pl == DE_EX.rs1_pl){
                DE_EX.op1_pl = MA_WB.LoadData_pl;
                //json code: set forwarding flag to true.
                jsonForwardFlag = true;
                jsonStringAdder("fPath","WB-EX");
                jsonStringAdder(',',' ');
            }else if((!(DE_EX.op_code_pl == 19)) & (!(DE_EX.op_code_pl == 3)) & (!(DE_EX.op_code_pl == 35))){
                DE_EX.op2_pl = MA_WB.LoadData_pl;
                //json code: set forwarding flag to true.
                jsonForwardFlag = true;
                jsonStringAdder("fPath","WB-EX");
                jsonStringAdder(',',' ');
            }
            //json code: insert forwarding flag.
            jsonStringAdder("forwarding",jsonForwardFlag);
            jsonStringAdder(',',' '); 
            break;
        }

        //WB-->MA FORWARDING IMPLEMENTATION
        if (EX_MA.DepFlag_pl == 3) {
            processor.reg[EX_MA.rs2_pl] = MA_WB_IP.LoadData_ip;
        }
    
    }
    
    //------------------------IMPLEMENTING PREDICTED BRANCH------------------------
    
    //json code: branch predictor flag
    bool jsonBranch = false;
    if(btb.find(IF_DE.current_pc_pl) != btb.end()){
        if(btb[IF_DE.current_pc_pl].first == 1){
            processor.new_pc = btb[IF_DE.current_pc_pl].second;
        }
        //json code: branch predictor used or not?
        jsonBranch = true;
        jsonStringAdder("bPSuccess",btb[IF_DE.current_pc_pl].first);
        jsonStringAdder(',',' ');
    }
    //json code: insert branch predict data.
    jsonStringAdder("branchPredict",jsonBranch);
    jsonStringAdder(',',' ');

    //------------------------UPDATING PC VALUE------------------------
    
    if (!forwarding) {
        if (!processor.DataDependency || processor.ControlDependency) {
            processor.pc = processor.new_pc;
        }
    } else {
        if ( (processor.DataDependency != 2) || (DE_EX.DepFlag_pl == 3) || processor.ControlDependency) {
            processor.pc = processor.new_pc;
        }
    }

    processor.current_pc = processor.pc;
    
    //------------------------START PIPELINE------------------------
    
    //------------------------------------------------------
    //json file code
    {
    jsonStringAdder("number",processor.clock_cycle);
    jsonStringAdder(',',' ');
    jsonStringAdder("IFpc", processor.pc);
    jsonStringAdder(',',' ');
    jsonStringAdder("IFhexcode",processor.instruction);
    jsonStringAdder(',',' ');
    jsonStringAdder("DEpc", IF_DE.current_pc_pl);
    jsonStringAdder(',',' ');
    jsonStringAdder("DEhexcode",IF_DE.instruction_pl);
    jsonStringAdder(',',' ');
    jsonStringAdder("EXpc", DE_EX.current_pc_pl);
    jsonStringAdder(',',' ');
    jsonStringAdder("EXhexcode",DE_EX.instruction_pl);
    jsonStringAdder(',',' ');
    jsonStringAdder("MApc", EX_MA.current_pc_pl);
    jsonStringAdder(',',' ');
    jsonStringAdder("MAhexcode",EX_MA.instruction_pl);
    jsonStringAdder(',',' ');
    jsonStringAdder("WBpc", MA_WB.current_pc_pl);
    jsonStringAdder(',',' ');
    jsonStringAdder("WBhexcode",MA_WB.instruction_pl);
    jsonStringAdder(',',' ');
    jsonStringAdder("memFile");
    jsonStringAdder(':','{');
    std::map<uint32_t, uint8_t>::iterator it = processor.memory.begin();
    while (it != processor.memory.end())
    {
        jsonStringAdder(it->first,it->second);
        jsonStringAdder(',',' ');
        ++it;
    }
    jsonStringAdder("0xffffffff","0x0");
    jsonStringAdder('}',' '); 
    jsonStringAdder(',','\n');  
    jsonStringAdder("registerFile");
    jsonStringAdder(':','[');
    for (int i=0; i<31; i++)
    {
        jsonStringAdder(processor.reg[i],',');
    }
    jsonStringAdder(processor.reg[31],']');
    jsonStringAdder('}',' ');  
    jsonStringAdder(',','\n'); 
    }
    //json file code ends
    //------------------------------------------------------

    processor.fetch();
    WriteToPacket(IF_DE_IP);
    
    IF_DE.ReadFromRegister();
    processor.decode();
    WriteToPacket(DE_EX_IP);
    
    DE_EX.ReadFromRegister();
    processor.execute();
    WriteToPacket(EX_MA_IP);
    
    EX_MA.ReadFromRegister();
    processor.mem();
    WriteToPacket(MA_WB_IP);

    MA_WB.ReadFromRegister();
    processor.write_back();
    
    IF_DE.WriteToRegister(IF_DE_IP);
    DE_EX.WriteToRegister(DE_EX_IP);
    EX_MA.WriteToRegister(EX_MA_IP);
    MA_WB.WriteToRegister(MA_WB_IP);

    //------------------------STATS CALCULATION AND KNOBS------------------------

    if (EX_MA.op_code_pl) {
        instructions++;
        if ( (EX_MA.op_code_pl == 3) | (EX_MA.op_code_pl == 35) ) {
            data_instructions++;
        } else if ( (EX_MA.op_code_pl == 99) | (EX_MA.op_code_pl == 111) | (EX_MA.op_code_pl == 103) ) {
            control_instructions++;
        }
        if ( (EX_MA.op_code_pl != 99) & (EX_MA.op_code_pl != 35) ) {
            ALU_instructions++;
        }
        if (EX_MA.DepFlag_pl == 3) {instructions--;}
    }

    if (register_file) {showRegisters();}

    if (tracing) {
        if (!trace_instruction) {
            std::cout<<"--------------------------------------------------\n";
            std::cout<<"Tracing for clock cycle "<<processor.clock_cycle<<" (Instruction Number "<<instructions<<") :\n\n";
            std::cout<<"       PC OPCODE RD F3 RS1 RS2 F7 IMMI IMMS IMMB IMMU IMMJ LOADTYPE STORETYPE TAKEBRANCH MEMREAD MEMWRITE REGWRITE ALURES MEMADR LOADDATA\n";
            std::cout<<"IF-DE:"; IF_DE.Trace();
            std::cout<<"DE-EX:"; DE_EX.Trace();
            std::cout<<"EX-MA:"; EX_MA.Trace();
            std::cout<<"MA-WB:"; MA_WB.Trace();
        } else {
            if (trace_instruction == instructions) {
                std::cout<<"--------------------------------------------------\n";
                std::cout<<"Tracing for clock cycle "<<processor.clock_cycle<<" (Instruction Number "<<instructions<<") :\n\n";
                std::cout<<"       PC OPCODE RD F3 RS1 RS2 F7 IMMI IMMS IMMB IMMU IMMJ LOADTYPE STORETYPE TAKEBRANCH MEMREAD MEMWRITE REGWRITE ALURES MEMADR LOADDATA\n";
                std::cout<<"IF-DE:"; IF_DE.Trace();
                std::cout<<"DE-EX:"; DE_EX.Trace();
                std::cout<<"EX-MA:"; EX_MA.Trace();
                std::cout<<"MA-WB:"; MA_WB.Trace();
            }
        }
    }
	if (bpu_details) {
		std::cout<<"--------------------------------------------------\n";
		std::cout<<"Branch Prediction Unit Details (Cycle "<<processor.clock_cycle<<"):\n";
		std::cout<<"Current PC: 0x"<<std::hex<<processor.current_pc<<std::dec<<"\n";
		
		std::cout<<"Branch Target Buffer (BTB) Contents:\n";
		for (const auto& entry : btb) {
		    std::cout<<"  PC: 0x"<<std::hex<<entry.first
		            <<" -> Predicted: "<<(entry.second.first?"Taken":"Not Taken")
		            <<", Target: 0x"<<entry.second.second<<std::dec<<"\n";
		}
		
		// Add PHT/BHT details here if implemented
		// std::cout<<"Pattern History Table:\n";
		// ... [PHT printing code] ...
	}
    //------------------------END AND PRINT STATS------------------------
    
    if (MA_WB.op_code_pl == 127) {Stop();}

}

//If knob to print register file's values is TRUE,
//Shows values in register file after every cycle
void showRegisters() {
    std::cout<<"--------------------------------------------------\n";
    std::cout<<"Register values for clock cycle "<<processor.clock_cycle<<" :\n";
    for (int i=0; i<32; i++) {
        std::cout<<"Register "<<i<<" : "<<processor.reg[i]<< "\n";
    }
}

//Exits the simulator when last instruction is received
//Calculates and prints stats and exits program
void Core::Stop() {
    processor.store_memory();
    fclose(processor.programcode);
    printf("Clock Cycle %d finished.\n", processor.clock_cycle);
    std::cout<<"--------------------------------------------------\n";
    std::cout<<"--------------------------------------------------\n\n";
    std::cout<<"Feeding all memory to Data Memory.mc file.\n\n";
    std::cout<<"--------------------------------------------------\n";
    bubbles = data_bubbles + control_bubbles;
    CPI = (float) processor.clock_cycle / (float) instructions;
    std::cout<<"\nSTATS:\n";
    showStats();
    
    //------------------------------------------------------
    //json file code
    {
    jsonStringAdder('{', ' ');
    jsonStringAdder("program","end");
    jsonStringAdder(' ', '}');
    jsonStringAdder(']',',');
    jsonStringAdder("totalCycles",processor.clock_cycle);
    jsonStringAdder(',','\n');
    jsonStringAdder("tInstructions",instructions);
    jsonStringAdder(',','\n');
    jsonStringAdder("cpi",std::to_string(CPI));
    jsonStringAdder(',','\n');
    jsonStringAdder("dataInstructions",data_instructions);
    jsonStringAdder(',','\n');
    jsonStringAdder("aluInstructions",ALU_instructions);
    jsonStringAdder(',','\n');
    jsonStringAdder("controlIns",control_instructions);
    jsonStringAdder(',','\n');
    jsonStringAdder("bubbles",bubbles);
    jsonStringAdder(',','\n');
    jsonStringAdder("dataHazards",data_hazards);
    jsonStringAdder(',','\n');
    jsonStringAdder("controlHazards",control_hazards);
    jsonStringAdder(',','\n');
    jsonStringAdder("branchMispredicts",wrong_predicts);
    jsonStringAdder(',','\n');
    jsonStringAdder("dataBubbles",data_bubbles);
    jsonStringAdder(',','\n');
    jsonStringAdder("controlBubbles",control_bubbles);
    jsonStringAdder('\n','}');
    fclose(output);
    }
    //json file code ends
    //------------------------------------------------------
    
    exit(0); //program run til completion
}

//Shows all stats at end of program
void Core::showStats() {
    std::cout<<" 1--> Total number of cycles                   : "<<processor.clock_cycle<<"\n";
    std::cout<<" 2--> Total instructions executed              : "<<instructions<<"\n";
    std::cout<<" 3--> CPI                                      : "<<CPI<<"\n";
    std::cout<<" 4--> Number of data transfer instructions     : "<<data_instructions<<"\n";
    std::cout<<" 5--> Number of ALU instructions               : "<<ALU_instructions<<"\n";
    std::cout<<" 6--> Number of Control instructions           : "<<control_instructions<<"\n";
    std::cout<<" 7--> Number of stalls in pipeline             : "<<bubbles<<"\n";
    std::cout<<" 8--> Number of data hazards                   : "<<data_hazards<<"\n";
    std::cout<<" 9--> Number of control hazards                : "<<control_hazards<<"\n";
    std::cout<<"10--> Number of branch mispredictions          : "<<wrong_predicts<<"\n";
    std::cout<<"11--> Number of bubbles due to data hazards    : "<<data_bubbles<<"\n";
    std::cout<<"12--> Number of bubbles due to control hazards : "<<control_bubbles<<"\n";
}
