#include "InputSRAMbank.h"
#include "systemc.h"
#include "../Acc_cofig.h"

int sc_main(int argc, char *argv[]){

    sc_clock clk("clk", 10);

    sc_signal<bool> rst;

    sc_signal<bool> wr_enable;
    sc_signal<bool> CS;
    sc_signal<sc_uint<32> > addr;
    sc_signal<sc_int<32> > data_i;
    sc_signal<sc_int<32> > data_o;

    InputSRAMbank iSRAM_bank("iSRAM_bank");

    iSRAM_bank.clk(clk);
    iSRAM_bank.rst(rst);
    iSRAM_bank.wr_enable(wr_enable);
    iSRAM_bank.CS(CS);
    iSRAM_bank.addr(addr);
    iSRAM_bank.data_i(data_i);
    iSRAM_bank.data_o(data_o);

    sc_trace_file *tf = sc_create_vcd_trace_file("RESULT");

    //trace input signal
    sc_trace(tf, iSRAM_bank.clk       ,"iSRAM_bank.clk");
    sc_trace(tf, iSRAM_bank.rst       ,"iSRAM_bank.rst");
    sc_trace(tf, iSRAM_bank.wr_enable ,"iSRAM_bank.wr_enable");
    sc_trace(tf, iSRAM_bank.CS        ,"iSRAM_bank.CS");
    sc_trace(tf, iSRAM_bank.addr      ,"iSRAM_bank.addr");
    sc_trace(tf, iSRAM_bank.data_i    ,"iSRAM_bank.data_i");
    sc_trace(tf, iSRAM_bank.data_o    ,"iSRAM_bank.data_o");
    sc_trace(tf, iSRAM_bank.data[0]   ,"iSRAM_bank.data[0]");
    sc_trace(tf, iSRAM_bank.data[1]   ,"iSRAM_bank.data[1]");
    sc_trace(tf, iSRAM_bank.data[2]   ,"iSRAM_bank.data[2]");
    sc_trace(tf, iSRAM_bank.data[3]   ,"iSRAM_bank.data[3]");
    sc_trace(tf, iSRAM_bank.data[4]   ,"iSRAM_bank.data[4]");
    sc_trace(tf, iSRAM_bank.data[5]   ,"iSRAM_bank.data[5]");
    sc_trace(tf, iSRAM_bank.data[6]   ,"iSRAM_bank.data[6]");
    sc_trace(tf, iSRAM_bank.data[7]   ,"iSRAM_bank.data[7]");
    sc_trace(tf, iSRAM_bank.data[8]   ,"iSRAM_bank.data[8]");
    sc_trace(tf, iSRAM_bank.data[9]   ,"iSRAM_bank.data[9]");

/***Start Simulation***/

    rst.write(1);
    sc_start(10,SC_NS);
    rst.write(0);


    //Write
    wr_enable.write(1);
    for(int i = 0; i < ISRAMbank_element; i++)
    {
        if(i % 2 == 0)
            CS.write(1);
        else
            CS.write(0);
        
        addr.write(i * 4);
        data_i.write(i + 1);   
        sc_start(10, SC_NS);     
    }

    //Read
    wr_enable.write(0);
    CS.write(1);
    for(int i = 0; i < ISRAMbank_element; i++)
    {
        addr.write(i * 4);   //read from addr 0x0, 0x4,...
        sc_start(10, SC_NS);          
    }   

    CS.write(0);
    sc_start(10, SC_NS); 

    sc_close_vcd_trace_file(tf); 

    return (0);
}