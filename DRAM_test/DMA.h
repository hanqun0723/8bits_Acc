#ifndef DMA_H
#define DMA_H

#include "systemc.h"
#include "tlm.h"
#include "tlm_utils/simple_initiator_socket.h"
#include "tlm_utils/simple_target_socket.h"

using namespace sc_core;
using namespace sc_dt;
using namespace std;

//DMA state machine
#define DMA_READ 1
#define DMA_WRITE 2
#define DMA_FINISH 3

SC_MODULE(DMA){

    tlm_utils::simple_initiator_socket<DMA> socket_m;
    tlm_utils::simple_target_socket<DMA> socket_s;

    sc_in<bool> clk;
    sc_in<bool> Clear;
    sc_out<unsigned int> Interrupt;
    //tlm::tlm_analysis_port<unsigned int> Interrupt; 

    sc_uint<32> in_address;
    int in_data;
    sc_signal<sc_uint<32> > DMA_baseaddr;
    //DMA base address
    sc_signal<sc_uint<32> > static_address_register;

    //DMA control registers, externally addressable, addressed as:
    // 0x0  src | 0x4  tgt | 0x8  size | 0xc  R_START
    sc_signal<sc_uint<32> > src, tgt, size;
    sc_signal<bool> start;

    sc_signal<sc_uint<32> >tmp_src, tmp_tgt, tmp_size;
    int data_m;
    int state;

    void dma();
    void b_transport(tlm::tlm_generic_payload&, sc_time&);

    SC_CTOR(DMA) : socket_m("socket_m"), socket_s("socket_s"){
        socket_s.register_b_transport(this, &DMA::b_transport);
        SC_CTHREAD(dma, clk.pos());
        reset_signal_is(Clear, false);
	static_address_register.write(0xa0000000);
    }
};
#endif
