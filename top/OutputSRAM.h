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

#ifndef _OUTPUTSRAM_H
#define _OUTPUTSRAM_H

#include "Acc_cofig.h"
#include "systemc.h"

SC_MODULE(OutputSRAM)
{
    /*********************Input and Output port*********************/

    sc_in<bool>            clk;
    sc_in<bool>            rst;             //active high

    /*from controller*/
    sc_in<bool>            CS;
    sc_in<sc_uint<2> >     read_write;  	//01:read, 10:write, 11:read and write      
    sc_in<sc_uint<OSRAM_ADDR_LEN> > addr_w;    //write addr
    sc_in<sc_uint<OSRAM_ADDR_LEN> > addr_r;     //read  addr

    /*from PE*/
    sc_in<sc_int<OSRAM_DATA_WIDTH> >      data_i;


    sc_out<sc_int<OSRAM_DATA_WIDTH> >     data_o;

    /**************************Inner data**************************/

    sc_int<OSRAM_DATA_WIDTH>  O_SRAMdata[OSRAM_SIZE];

    void do_reset(); 
    void do_OutputSRAM();

    SC_CTOR(OutputSRAM)
    {
        SC_THREAD(do_OutputSRAM);
        sensitive << clk.pos() << rst.pos();
    }

};

#endif