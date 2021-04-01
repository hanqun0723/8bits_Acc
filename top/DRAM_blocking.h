#ifndef DRAM_H
#define DRAM_H

#include "systemc.h"
#include "tlm.h"
#include "tlm_utils/simple_target_socket.h"
#include "ps_config.h"
#include <fstream>
#include "./conv_testdata/param.h"
#include "Acc_cofig.h"

using namespace sc_core;
using namespace sc_dt;
using namespace std;

SC_MODULE(DRAM){

    tlm_utils::simple_target_socket<DRAM> socket;

    int32_t mem[MEM_SIZE/4]; // 256MB
    sc_int<32> tmp;
    
    // sc_out<bool> mem_data_valid;

    // DRAM timing
    int calculate_delay(int addr);
    uint32_t row_hit_miss_latency(int addr);
    int get_row_index(int);
    void access(bool,uint32_t,unsigned int ,uint32_t*);
    int row_index;
    void b_transport(tlm::tlm_generic_payload&, sc_time&);

    SC_CTOR(DRAM) : socket("socket"){
        // Register callback for incoming b_transport interface method call
        socket.register_b_transport(this, &DRAM::b_transport);
        row_index = 0xffffffff; // Initial value
        for (int i = 0; i < MEM_SIZE/4; i++) 
            mem[i] = 0;
        ///////////////Read input data///////////////
        ifstream fin("./conv_testdata/input.txt");

        for (int k = 0; k < INPUT_C; k++)
        {
            for (int j = 0; j < INPUT_H; j++)
            {
                for (int i = 0; i < (INPUT_W + ROW_PADD); i++)
                {
                    //alignment 4
                    if ( (i % 4 < 4) && (i < INPUT_W) )
                        fin >> tmp.( (3 - (i % 4)) * 8 + 7,(3 - (i % 4)) * 8);
                    else 
                        tmp.range( (3 - (i % 4)) * 8 + 7,(3 - (i % 4)) * 8) = 0;
                    
                    if ( ((i + 1) % 4) == 0 /*|| ( (i + 1) == (F_SIZE * F_SIZE * INPUT_C * F_NUM))*/){
                        int bytes_offset = (k * INPUT_H * (INPUT_W + ROW_PADD)) + (j * (INPUT_W + ROW_PADD)) + i;
                        mem[bytes_offset/4] = tmp;
                        tmp = 0;
                    }
                }
            }
        }

        // for(int i = 0; i < (INPUT_H * INPUT_W * INPUT_C); i++)
        //     cout << "mem[" <<  i  << "] : " << mem[i] << endl;
        // for(int i = DRAM_INPUT_BASE; i < (INPUT_H * INPUT_W * INPUT_C); i++){
        //     if (i % 4 < 4)
        //         fin >> tmp.range( (3 - (i % 4)) * 8 + 7,(3 - (i % 4)) * 8);
                
        //     if ( ((i + 1) % 4) == 0 /*|| ( (i + 1) == (F_SIZE * F_SIZE * INPUT_C * F_NUM))*/){
        //         mem[i/4] = tmp;
        //         tmp = 0;
        //     }
        // }
        fin.close();    

        ///////////////Read Weight///////////////
        ifstream fweight("./conv_testdata/weight.txt");

        for(int i = 0; i < (F_SIZE * F_SIZE * INPUT_C * F_NUM); i++){
            if (i % 4 < 4){
                fweight >> tmp.range(i % 4 * 8 + 7,i % 4 * 8);
            }
            if ( (((i + 1) % 4) == 0 ) || ( (i + 1) == (F_SIZE * F_SIZE * INPUT_C * F_NUM))){
                mem[DRAM_WEIGHT_BASE/4  + (i/4)] = tmp;
                tmp = 0;
            }
        }
        fin.close(); 

        // for(int i = DRAM_WEIGHT_BASE/4; i < DRAM_WEIGHT_BASE/4 + 10/*(MEM_SIZE/4)*/; i++)
        //     cout << "mem[" << i << "] : " << mem[i] << endl;          

    } 
};
#endif