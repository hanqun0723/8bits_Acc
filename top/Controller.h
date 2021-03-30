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

#ifndef _CONTROLLER_H
#define _CONTROLLER_H

#include "./conv_testdata/param.h"
#include "Acc_cofig.h"
#include "systemc.h"

SC_MODULE(Controller)
{
    /*********************Input and Output port*********************/

    sc_in<bool>      clk;
    sc_in<bool>      rst;

    sc_in<bool>     start;

    // //sc_in<sc_int<32> > input_w;
    // //sc_in<sc_int<32> > input_h;
    // sc_in<sc_int<32> > filter_size;
    // //sc_in<sc_int<32> > filter_ch;

    // sc_in<sc_int<32> > tile_w;
    // sc_in<sc_int<32> > tile_h;

    //DMAC
    sc_out<sc_uint<2> >  dma_type;
    sc_out<sc_uint<32> > src;
    sc_out<sc_uint<32> > tgt;
    sc_out<sc_uint<32> > osram_id; //select data from output sram
    sc_out<sc_uint<32> > length;
    sc_out<bool> DMA_start; 
    sc_in<bool>      dram_done;
    sc_in<sc_int<32> > read_data;
    sc_in<bool>      DMA_irt;
    sc_out<bool>     DMA_irtclr;

    //Input SRAM
    sc_out<bool>     I_CS[ISRAM_BANK_NUM];
    sc_out<bool>     I_WEB[ISRAM_BANK_NUM];
    sc_out<sc_uint<ISRAM_ADDR_LEN> >  I_addr[ISRAM_BANK_NUM];
    sc_out<sc_int<ISRAM_DATA_WIDTH> > I_data_i[ISRAM_BANK_NUM];

    //SRAM bus
    sc_out<bool> bus_mux_sel[ISRAM_BANK_NUM * ISRAM_bytes];
    sc_out<sc_uint<4> > store_col_index;

    //Reg array
    sc_out<bool>        store_reg[REG_NUM];
    sc_out<bool>        shift[FILTER_SIZE];  

    //Reg mux
    sc_out<bool>        reg_mux_sel[REG_NUM];   

    //Weight SRAM
    sc_out<bool>     W_CS[WSRAM_NUM];
    sc_out<bool>     W_WEB[WSRAM_NUM];
    sc_out<sc_uint<WSRAM_ADDR_LEN> >  W_addr[WSRAM_NUM];
    sc_out<sc_uint<3> >    W_length[WSRAM_NUM];    
    sc_out<sc_uint<WSRAM_BANK_BITS> >     W_bank_sel[WSRAM_NUM];
    sc_out<sc_int<BUS_WIDTH> > W_data_i[WSRAM_NUM];                  

    //PE
    // sc_out<bool>       weight_valid[PE_NUM];
    // sc_out<bool>       last_channel[PE_NUM];
    sc_out<bool>           add_prev;         
    // sc_out<sc_int<PSUM_WIDTH> >    prev_psum[PE_NUM];

    //Output SRAM
    sc_out<bool>    O_CS[OSRAM_NUM];
    sc_out<sc_uint<2> >     read_write;  	
    // sc_out<bool>            O_WEB[OSRAM_NUM];             
    sc_out<sc_uint<OSRAM_ADDR_LEN> > O_addr_w[OSRAM_NUM];    
    sc_out<sc_uint<OSRAM_ADDR_LEN> > O_addr_r[OSRAM_NUM];  
    sc_out<bool> osram_valid;
    sc_in<bool> dma_resp;

    //control register
    sc_signal<sc_uint<32> > input_h_Reg;
    sc_signal<sc_uint<32> > input_w_Reg;
    sc_signal<sc_uint<32> > input_c_Reg;    //same with filter channel
    sc_signal<sc_uint<32> > f_size_Reg;     //filter size
    sc_signal<sc_uint<32> > f_num_Reg;      //number of filter
    sc_signal<sc_uint<32> > stride_Reg;
    
    int out_length;
    sc_int<32> osram_req;
    //address control
    sc_signal<sc_uint<32> > h_Reg;
    sc_signal<sc_uint<32> > w_Reg;
    sc_signal<sc_uint<32> > c_Reg;

    //interal register
    sc_signal<sc_uint<32> > ibank_ctrl_Reg; 
    sc_signal<sc_uint<32> > ibank_addr_Reg;
    sc_signal<sc_uint<32> > wbank_addr_Reg;
    sc_signal<sc_uint<32> > osram_addr_Reg;
    sc_signal<sc_uint<32> > osram_id_Reg;

    sc_signal<sc_uint<32> > tile_h_Reg;
    sc_signal<sc_uint<32> > tile_w_Reg;

    sc_signal<bool> first_load_Reg;
    sc_signal<sc_uint<32> > store_col_index_Reg;
    sc_signal<sc_uint<32> > shift_count_Reg;
    sc_signal<bool> prev_store_Reg;
    sc_signal<bool> write_result_Reg;
    sc_signal<sc_uint<32> > dma_count_Reg;
    sc_signal<sc_uint<32> > conv_count_Reg;
    sc_signal<sc_uint<32> > tile_overlap_Reg;
    
    sc_uint<32> load_tile_width;
    sc_uint<32> load_tile_height;
    //DMAC
    // sc_in<bool>         DMA_done;   //when finish one buffer
    // //sc_in<bool>         DMA_irt;    //all data are transfered
    // sc_out<sc_uint<3> > readType;   //1 : Weight 2 : data;
    // sc_out<sc_uint<3> > DMA_start;
    /**************************Inner data**************************/

    sc_signal<sc_uint<32> > state;

    sc_signal<sc_uint<32> > dma_read_state;
    sc_signal<sc_uint<32> > dma_write_state;
    sc_signal<sc_uint<32> > pe_state;
    sc_signal<sc_uint<32> > read_data_state;


    void do_Controller();
    void do_dmaRead();
    void do_dmaWrite();

    void do_startPE();
    void do_reset();

    SC_CTOR(Controller)
    {
        SC_THREAD(do_Controller);
        sensitive << clk.pos() << rst.pos();
    }

};


#endif