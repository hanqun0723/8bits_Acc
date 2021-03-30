/*
 *  Copyright (c) 2020.  	Computer Architecture and System Laboratory, CASLab
 *							Dept. of Electrical Engineering & Inst. of Computer and Communication Engineering
 *							National Cheng Kung University, Tainan, Taiwan
 *  All Right Reserved
 *
 *	Written by  Han-Qun Huang (hanqun0723@gmail.com)
 *   
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation.
 * 
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 * 
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _DMAC_H
#define _DMAC_H

#include "Acc_cofig.h"
#include "systemc.h"
#include "tlm.h"
#include "tlm_utils/simple_initiator_socket.h"

using namespace sc_core;
using namespace sc_dt;
using namespace std;

//DMA state machine
#define DMA_IDLE 0
#define DMA_READ 1
#define DMA_WRITE 2
#define DMA_IRQ   3
#define DMA_FINISH 4

SC_MODULE(DMAC)
{
    tlm_utils::simple_initiator_socket<DMAC> socket_m;

    sc_in<bool> clk;
    sc_in<bool> rst;

    // sc_in<bool> mem_data_valid;
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
    //sc_signal<sc_int<32> > data;

    //internel register
    sc_signal<sc_uint<32> > count_Reg;
    int data_m;


    void do_DMAC();
    void do_reset();
    void b_transport(tlm::tlm_generic_payload&, sc_time&);

    //void do_reset();

    SC_CTOR(DMAC):socket_m("socket_m")
    {
        SC_THREAD(do_DMAC);
        sensitive << clk.pos() << rst.pos();
    }

};


#endif