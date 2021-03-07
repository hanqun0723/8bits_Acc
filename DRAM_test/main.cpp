#include "DMAC.h"
#include "DRAM.h"
#include "systemc.h"
#include "Acc_cofig.h"
#include "string.h"

int sc_main(int argc, char *argv[]){


    sc_clock clk("clk", 10);
    sc_signal<bool> rst;

    sc_signal<sc_uint<32> > src;
    sc_signal<sc_uint<32> > tgt;
    sc_signal<sc_uint<32> > length;
    sc_signal<bool> DMA_start; 

    sc_signal<sc_uint<32> > read_data;
    sc_signal<bool>        data_valid;

    // sc_signal<bool> DMA_type;
    // sc_signal<bool> DMA_start; 
    // sc_signal<bool> write_config;

    // sc_signal<sc_uint<32> > f_size;
    // sc_signal<sc_uint<32> > f_channel;
    // sc_signal<sc_uint<32> > f_num;
    // sc_signal<sc_uint<32> > weight_base;    

    // //test only one weight SRAM
    // sc_signal<bool>    W_CS;
    // sc_signal<bool>    W_WEB;
    // sc_signal<sc_uint<WSRAM_ADDR_LEN> >    W_addr;
    // sc_signal<sc_uint<3> >    W_length;            //1bytes ~ 4bytes   
    // sc_signal<sc_int<BUS_WIDTH> >    W_weight;    //32bits weight
    // sc_signal<sc_uint<WSRAM_BANK_BITS> >     W_bank_sel; 

    DMAC *dmac1 = new DMAC("dmac1");
    DRAM *dram1 = new DRAM("dram1");


    dmac1->socket_m.bind(dram1->socket);
    dmac1->clk(clk);
    dmac1->rst(rst);
    dmac1->src(src);
    dmac1->tgt(tgt);
    dmac1->length(length);
    dmac1->DMA_start(DMA_start);
    dmac1->read_data(read_data);
    dmac1->data_valid(data_valid);
    // dmac1->DMA_type(DMA_type);
    // dmac1->DMA_start(DMA_start);
    // dmac1->write_config(write_config);
    // dmac1->f_size(f_size);
    // dmac1->f_channel(f_channel);
    // dmac1->f_num(f_num);
    // dmac1->weight_base(weight_base);
    // dmac1->W_CS(W_CS);
    // dmac1->W_WEB(W_WEB);
    // dmac1->W_addr(W_addr);
    // dmac1->W_length(W_length);
    // dmac1->W_weight(W_weight);
    // dmac1->W_bank_sel(W_bank_sel);

    sc_trace_file *tf = sc_create_vcd_trace_file("RESULT");

    sc_trace(tf, dmac1->clk, "dmac->clk");
    sc_trace(tf, dmac1->rst, "dmac->rst");
    sc_trace(tf, dmac1->state, "dmac->state");
    sc_trace(tf, dmac1->src, "dmac->src");
    sc_trace(tf, dmac1->tgt, "dmac->tgt");
    sc_trace(tf, dmac1->length, "dmac->length");
    sc_trace(tf, dmac1->DMA_start, "dmac->DMA_start");
    sc_trace(tf, dmac1->read_data, "dmac->read_data");
    sc_trace(tf, dmac1->data_valid, "dmac->data_valid");

    sc_trace(tf, dmac1->count_Reg, "dmac->count_Reg");
    sc_trace(tf, dmac1->length_Reg, "dmac->length_Reg");
    // sc_trace(tf, dmac1->clk, "dmac->clk");
    // sc_trace(tf, dmac1->rst, "dmac->rst");
    // sc_trace(tf, dmac1->DMA_start, "dmac->DMA_start");
    // sc_trace(tf, dmac1->state, "dmac->state");
    // sc_trace(tf, dmac1->counter, "dmac->counter");
    // sc_trace(tf, dmac1->size, "dmac->size");


    //Start Simulation
    rst.write(1);
    sc_start(10,SC_NS);
    rst.write(0);
    src.write(0);
    tgt.write(10);
    length.write(2);
    DMA_start.write(1);
    sc_start(10,SC_NS);
    DMA_start.write(0);

    // write_config.write(1);
    // f_size.write(3);
    // f_channel.write(1);
    // f_num.write(1);
    // weight_base.write(0);
    // sc_start(10,SC_NS);
    // write_config.write(0);
    // DMA_start.write(1);
    sc_start(260,SC_NS);
    sc_close_vcd_trace_file(tf); 
}
