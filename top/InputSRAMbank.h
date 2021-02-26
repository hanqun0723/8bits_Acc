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

#ifndef _INPUTSRAMBANK_H
#define _INPUTSRAMBANK_H

#include "Acc_cofig.h"
#include "systemc.h"

SC_MODULE(InputSRAMbank)
{
    /*********************Input and Output port*********************/

    sc_in<bool>         clk;
    sc_in<bool>         rst;            //active high

    /*from controller*/
    sc_in<bool>         CS;
    sc_in<bool>         WEB;           //write = 1, read = 0
    sc_in<sc_uint<ISRAM_ADDR_LEN> > addr;

    /*from I_buffer or BUS*/
    sc_in<sc_int<ISRAM_DATA_WIDTH> > data_i;
    sc_out<sc_int<ISRAM_DATA_WIDTH> > data_o;


    /**************************Inner data**************************/

    sc_int<ISRAM_DATA_WIDTH> data[ISRAMbank_element];


    void do_InputSRAMbank();
    void do_reset();

    SC_CTOR(InputSRAMbank)
    {
        SC_THREAD(do_InputSRAMbank);
        sensitive << clk.pos() << rst.pos();
    }

};


#endif