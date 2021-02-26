#ifndef DRAM_H
#define DRAM_H

#include "tlm.h"
#include "tlm_utils/simple_target_socket.h"
#include "ps_config.h"
using namespace sc_core;
using namespace sc_dt;
using namespace std;

SC_MODULE(DRAM){

    tlm_utils::simple_target_socket<DRAM> socket;

    uint32_t mem[MEM_SIZE/4]; // 256MB

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
        for(int i=0; i<(MEM_SIZE/4); i++){
            mem[i] = i;
            //printf("mem[%d] : %d ",2 ,mem[2]);           
        }
    } 
};
#endif