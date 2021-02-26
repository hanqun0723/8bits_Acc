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

#ifndef _SRAM_BUS_H
#define _SRAM_BUS_H

#include "Acc_cofig.h"
#include "systemc.h"

SC_MODULE(SRAM_BUS)
{
    /*********************Input and Output port*********************/

    sc_in<sc_int<ISRAM_DATA_WIDTH> > bus_data_i[ISRAM_BANK_NUM];    //from 64 InputSRAM bank
    sc_in<bool> bus_mux_sel[ISRAM_BANK_NUM * ISRAM_bytes];              //Sel 16 bytes to PE
    sc_in<sc_uint<4> > store_col_index;

    //sc_in<sc_uint<MUX_BITS> > reg_mux_sel[REG_NUM];                           

    //sc_out<sc_int<DATA_WIDTH> > bus_pe_o[MAC_NUM];                //Output to PE
    sc_out<sc_int<DATA_WIDTH> > bus_reg_o[REG_NUM];               //Output to data reg

    /**************************Inner data**************************/
    sc_int<DATA_WIDTH> data[ISRAM_BANK_NUM * ISRAM_bytes];
    sc_int<32> base;
    sc_int<32> reg_index;
    
    void do_SRAMBUS();

    SC_CTOR(SRAM_BUS)
    {
        SC_METHOD(do_SRAMBUS);
        for(int i = 0; i < ISRAM_BANK_NUM; i++)
            sensitive << bus_data_i[i];
        for(int i = 0; i < ISRAM_BANK_NUM * ISRAM_bytes; i++)
            sensitive << bus_mux_sel[i];        
        sensitive << store_col_index;

        // for(int i = 0; i < REG_NUM; i++)
        //     sensitive << reg_mux_sel[i];
    }

};

#endif