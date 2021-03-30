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

#ifndef _DNNACC_H
#define _DNNACC_H

#include "Acc_cofig.h"
#include "systemc.h"
#include "InputSRAMbank.h"
#include "SRAM_BUS.h"
#include "RegArray.h"
#include "Reg_MUX.h"
#include "WeightSRAM.h"
#include "PE.h"
#include "OutputSRAM.h"
#include "Controller.h"

SC_MODULE(DNNAcc)
{
    /*********************Input and Output port*********************/

    sc_in<bool>         clk;
    sc_in<bool>         rst;

    //DMAC
    sc_out<sc_uint<2> >  dma_type;
    sc_out<sc_uint<32> > src;
    sc_out<sc_uint<32> > tgt;
    sc_out<sc_uint<32> > osram_id;
    sc_out<sc_uint<32> > length;
    sc_out<sc_int<OSRAM_DATA_WIDTH> > osram_data[OSRAM_NUM];
    sc_out<bool> osram_valid;
    sc_in<bool> dma_resp;
    sc_out<bool> DMA_start; 

    sc_in<sc_int<32> > read_data;
    sc_in<bool> dram_done;
    sc_in<bool> DMA_irt;
    sc_out<bool>  DMA_irtclr;

    //InputSRAM
    sc_signal<bool>     I_CS[ISRAM_BANK_NUM];
    sc_signal<bool>     I_WEB[ISRAM_BANK_NUM];
    sc_signal<sc_uint<ISRAM_ADDR_LEN> >  I_addr[ISRAM_BANK_NUM];
    sc_signal<sc_int<ISRAM_DATA_WIDTH> > I_data_i[ISRAM_BANK_NUM];
    sc_signal<sc_int<ISRAM_DATA_WIDTH> > I_data_o[ISRAM_BANK_NUM];    

    //SRAM bus
    //sc_in<bool> mux_sel[ISRAM_BANK_NUM * ISRAM_bytes];
    sc_signal<bool> bus_mux_sel[ISRAM_BANK_NUM * ISRAM_bytes];
    sc_signal<sc_uint<4> > store_col_index;
    //sc_signal<sc_int<DATA_WIDTH> > bus_pe_o[MAC_NUM];               
    sc_signal<sc_int<DATA_WIDTH> > bus_reg_o[REG_NUM];         

    //RegArray
    sc_signal<bool>        store_reg[REG_NUM];  
    sc_signal<bool>        shift[FILTER_SIZE];                   //Support filter size = 3 , shift each row
    sc_signal<sc_int<DATA_WIDTH> > reg_o[REG_NUM];
    sc_signal<sc_int<DATA_WIDTH> > PE_data_i[MAC_NUM];

    sc_signal<bool> reg_mux_sel[REG_NUM];   

    //WeightSRAM
    sc_signal<bool>     W_CS[WSRAM_NUM];
    sc_signal<bool>     W_WEB[WSRAM_NUM];
    sc_signal<sc_uint<WSRAM_ADDR_LEN> >  W_addr[WSRAM_NUM];
    sc_signal<sc_uint<3> >    W_length[WSRAM_NUM];            //1bytes ~ 4bytes   
    sc_signal<sc_uint<WSRAM_BANK_BITS> >     W_bank_sel[WSRAM_NUM];        
    sc_signal<sc_int<BUS_WIDTH> > W_data_i[WSRAM_NUM];
    sc_signal<sc_int<DATA_WIDTH> > W_data_o[WSRAM_NUM][WSRAM_BANK_NUM];  

    //PE
    // sc_in<bool>       weight_valid[PE_NUM];
    // sc_in<bool>       last_channel[PE_NUM];
    sc_signal<bool>           add_prev;         
    //sc_signal<sc_int<PSUM_WIDTH> >    prev_psum[PE_NUM];
    sc_signal<sc_int<PSUM_WIDTH> >   pe_data_out[PE_NUM];

    //OutputSRAM
    sc_signal<bool>    O_CS[OSRAM_NUM];
    sc_signal<sc_uint<2> >     read_write;  	
    // sc_in<bool>            O_WEB[OSRAM_NUM];             
    sc_signal<sc_uint<OSRAM_ADDR_LEN> > O_addr_w[OSRAM_NUM];    
    sc_signal<sc_uint<OSRAM_ADDR_LEN> >     O_addr_r[OSRAM_NUM];   
    sc_signal<sc_int<OSRAM_DATA_WIDTH> >     O_data_i[OSRAM_NUM];  
    sc_signal<sc_int<OSRAM_DATA_WIDTH> >     O_data_o[OSRAM_NUM];    

    //Controller
    sc_in<bool> start;

    /////////////////////////////////////////
    sc_vector<InputSRAMbank> ibank;
    SRAM_BUS sram_bus;
    RegArray regarray;
    Reg_MUX reg_mux;
    sc_vector<WeightSRAM> wsram;
    sc_vector<PE> pe;
    sc_vector<OutputSRAM> osram;
    Controller controller;

    void propagate();

    SC_CTOR(DNNAcc) : ibank("ibank", ISRAM_BANK_NUM),
                      sram_bus("sram_bus"),
                      regarray("regarray"),
                      reg_mux("reg_mux"),
                      wsram("wsram", WSRAM_NUM),
                      pe("pe", PE_NUM),
                      osram("osram", OSRAM_NUM),
                      controller("controller")
    {
        SC_METHOD(propagate);
        for(int i = 0; i < OSRAM_NUM; i++)
            sensitive << O_data_o[i];

        //Input SRAM
        for(int i = 0; i < ISRAM_BANK_NUM; i++)
        {
            ibank[i].clk(clk);
            ibank[i].rst(rst);
            ibank[i].CS(I_CS[i]);
            ibank[i].WEB(I_WEB[i]);
            ibank[i].addr(I_addr[i]);
            ibank[i].data_i(I_data_i[i]);
            ibank[i].data_o(I_data_o[i]);
        }

        //SRAM bus
        for(int i = 0; i < ISRAM_BANK_NUM; i++)
            sram_bus.bus_data_i[i](I_data_o[i]);            
        for(int i = 0; i < ISRAM_BANK_NUM * ISRAM_bytes; i++)
        {
            sram_bus.bus_mux_sel[i](bus_mux_sel[i]);
        }
        sram_bus.store_col_index(store_col_index);
        for(int i = 0; i < REG_NUM; i++)
        {
            //sram_bus.reg_mux_sel[i](bus_mux_sel[i]); 
            sram_bus.bus_reg_o[i](bus_reg_o[i]);
        }
        // for(int i = 0; i < MAC_NUM; i++)
        //     sram_bus.bus_pe_o[i](bus_pe_o[i]);

        //REG ARRAY
        regarray.clk(clk);
        regarray.rst(rst);
        for(int i = 0; i < REG_NUM; i++)
        {
            regarray.reg_i[i](bus_reg_o[i]);
            regarray.store_reg[i](store_reg[i]);
        }

        for(int i = 0; i < FILTER_SIZE; i++)
            regarray.shift[i](shift[i]);
        for(int i = 0; i < REG_NUM; i++)
            regarray.reg_o[i](reg_o[i]);

        for(int i = 0; i < REG_NUM; i++)
        {
            reg_mux.reg_data_i[i](reg_o[i]);
            reg_mux.reg_mux_sel[i](reg_mux_sel[i]);
        }

        for(int i = 0; i < MAC_NUM; i++)
        {
            reg_mux.reg_data_o[i](PE_data_i[i]);
        }

        //Weight SRAM
        for(int i = 0; i < WSRAM_NUM; i++)
        {
            wsram[i].clk(clk);
            wsram[i].rst(rst);
            wsram[i].CS(W_CS[i]);
            wsram[i].WEB(W_WEB[i]);
            wsram[i].addr(W_addr[i]);
            wsram[i].w_length(W_length[i]);
            wsram[i].weight(W_data_i[i]);
            wsram[i].bank_sel(W_bank_sel[i]);
            for(int j = 0; j < WSRAM_BANK_NUM; j++)
                wsram[i].weight_o[j](W_data_o[i][j]);
        }        

        //PE
        for(int i = 0; i < PE_NUM; i++)
        {
            pe[i].clk(clk);
            pe[i].rst(rst);
            for(int j = 0; j  < MAC_NUM; j++)
            {
                pe[i].input[j](PE_data_i[j]);
                pe[i].weight[j](W_data_o[i][j]);
            } 
            // pe[i].weight_valid(weight_valid[i]);
            // pe[i].last_channel(last_channel[i]);
            pe[i].add_prev(add_prev); 
            pe[i].prev_psum(O_data_o[i]);
            pe[i].data_out(pe_data_out[i]);
        }

        //Output SRAM
        for(int i = 0; i < OSRAM_NUM; i++)
        {
            osram[i].clk(clk);
            osram[i].rst(rst);
            osram[i].CS(O_CS[i]);
            osram[i].read_write(read_write);
            osram[i].addr_w(O_addr_w[i]);
            osram[i].addr_r(O_addr_r[i]);
            osram[i].data_i(pe_data_out[i]);
            osram[i].data_o(O_data_o[i]);
            //osram[i].data_o(osram_data[i]);
            // to DMAC
            //osram[i].data_o(osram_data[i]);
            //osram_data[i](O_data_o[i]);
            //O_data_o[i](osram_data[i]);
        }

        //Controller
        controller.clk(clk);
        controller.rst(rst);
        controller.start(start);

        controller.dma_type(dma_type);
        controller.src(src);
        controller.tgt(tgt);
        controller.osram_id(osram_id);
        controller.length(length);
        controller.DMA_start(DMA_start);
        controller.dram_done(dram_done);
        controller.read_data(read_data);
        controller.DMA_irt(DMA_irt);
        controller.DMA_irtclr(DMA_irtclr);

        for(int i = 0; i < ISRAM_BANK_NUM; i++)
        {
            controller.I_CS[i](I_CS[i]);
            controller.I_WEB[i](I_WEB[i]);
            controller.I_addr[i](I_addr[i]);
            controller.I_data_i[i](I_data_i[i]);
        }        
        for(int i = 0; i < ISRAM_BANK_NUM * ISRAM_bytes; i++)
            controller.bus_mux_sel[i](bus_mux_sel[i]); 
        controller.store_col_index(store_col_index);
        for (int i = 0; i < REG_NUM; i++)
            controller.store_reg[i](store_reg[i]);
        for(int i = 0; i < FILTER_SIZE; i++)
            controller.shift[i](shift[i]);  
        for (int i = 0; i < REG_NUM; i++)
            controller.reg_mux_sel[i](reg_mux_sel[i]);
        for(int i = 0; i < WSRAM_NUM; i++)
        {
            controller.W_CS[i](W_CS[i]);
            controller.W_WEB[i](W_WEB[i]);
            controller.W_addr[i](W_addr[i]);    
            controller.W_length[i](W_length[i]);
            controller.W_bank_sel[i](W_bank_sel[i]);
            controller.W_data_i[i](W_data_i[i]);
        }  
        controller.add_prev(add_prev);
        controller.read_write(read_write);
        for(int i = 0; i < OSRAM_NUM; i++)
        {
            controller.O_CS[i](O_CS[i]);
            controller.O_addr_w[i](O_addr_w[i]);
            controller.O_addr_r[i](O_addr_r[i]);
        }      
        controller.osram_valid(osram_valid);
        controller.dma_resp(dma_resp);
    }
};  
#endif