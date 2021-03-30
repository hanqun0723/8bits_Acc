#ifndef _DMAC_H
#define _DMAC_H



#include "tlm_utils/simple_initiator_socket.h"
#include "tlm_utils/peq_with_cb_and_phase.h"
#include "mm.h"
#include "ps_config.h"

//DMA state machine
#define DMA_IDLE 0
#define DMA_READ 1
#define WAIT_DRAM_READ 2
#define DMA_WRITE 3
#define DMA_IRQ   4
#define DMA_FINISH 5

SC_MODULE(DMA){

    tlm_utils::simple_initiator_socket<DMA> socket;

    sc_in<bool> clk;
    sc_in<bool> rst;

    sc_in<sc_uint<2> > dma_type;
    sc_in<sc_uint<32> > src;
    sc_in<sc_uint<32> > tgt;
    sc_in<sc_uint<32> > osram_id;
    sc_in<sc_uint<32> > length;
    sc_in<sc_int<OSRAM_DATA_WIDTH> > osram_data[OSRAM_NUM];
    sc_in<bool> DMA_start;

    sc_out<sc_int<32> > read_data;
    sc_out<bool>        data_valid;

    sc_out<bool>        DMA_irt;
    sc_in<bool>         DMA_irtclr;

    //////////////////////////////////////////////////////
    sc_signal<sc_uint<32> > state;

    //control register
    sc_signal<sc_uint<32> > src_Reg;
    sc_signal<sc_uint<32> > tgt_Reg;
    sc_signal<sc_uint<32> > length_Reg;

    sc_signal<sc_uint<32> > count_Reg;

    int32_t data_buffer[0];
    sc_int<1> dram_done;

    mm   m_mm;
    
    void do_DMAC();
    void do_reset();
    void transport(bool rw,uint32_t addr, uint32_t* data, uint32_t length);
    
    virtual tlm::tlm_sync_enum nb_transport_bw( 
    tlm::tlm_generic_payload& trans,
    tlm::tlm_phase& phase, sc_time& delay );

    void peq_cb(tlm::tlm_generic_payload& trans, const tlm::tlm_phase& phase);

    tlm::tlm_generic_payload* request_in_progress;
    sc_event end_request_event;
    tlm_utils::peq_with_cb_and_phase<DMA> m_peq;


    SC_CTOR(DMA):
        socket("socket")
        ,request_in_progress(0)
        ,m_peq(this , &DMA::peq_cb)
    {
        socket.register_nb_transport_bw(this, &DMA::nb_transport_bw);
        SC_THREAD(do_DMAC);
        sensitive << clk.pos();
    }

};

#endif