#include <iostream>
#include <systemc.h> 
#include "tlm_utils/simple_initiator_socket.h"
#include "tlm_utils/simple_target_socket.h"
#include "tlm.h"
#include "DMA.h"
#include "DRAM.h"


#define SC_INCLUDE_DYNAMIC_PROCESSES

using namespace sc_core;
using namespace sc_dt;
using namespace std;

std::string int2str(int k){
  std::stringstream ss;
  ss<<k;
  return ss.str(); }

int sc_main(int argc, char* argv[])
{
  sc_core::sc_report_handler::set_actions( "/IEEE_Std_1666/deprecated",
                                           sc_core::SC_DO_NOTHING );
  

  DMA *dma; //initiator
  DRAM *dram;   //target

//signal
  sc_clock clk("clk", 10);
  sc_signal<bool>  rst;
  sc_signal<bool> DMA_start;

  dma = new DMA("DMA");
  dram = new DRAM("dram");

  dma->clk(clk);
  dma->rst(rst);
  dma->DMA_start(DMA_start);
  dma->socket.bind(dram->socket);

  sc_trace_file *tf = sc_create_vcd_trace_file("result");
  sc_trace(tf,clock,"clk");

  sc_trace(tf, dma->clk, "dma.clk");
  sc_trace(tf, dma->DMA_start, "dma.DMA_start");
  sc_trace(tf, dma->dram_done, "dma.dram_done");
  sc_trace(tf, dma->data_buffer[0], "dma.data_buffer[0]");
  sc_trace(tf, dma->state, "dma.state");
  sc_trace(tf, dma->src, "dma.src");
  
  rst.write(1);
  DMA_start.write(0);
  sc_start(10,SC_NS);
  rst.write(0);
  DMA_start.write(1);
  sc_start(150,SC_NS);


  sc_close_vcd_trace_file(tf); 

  
  return 0;
}