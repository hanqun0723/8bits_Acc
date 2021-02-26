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

#ifndef _WEIGHTSRAMBANK_H
#define _WEIGHTSRAMBANK_H

#include "Acc_cofig.h"
#include "systemc.h"

SC_MODULE(WeightSRAMbank){

    /*********************Input and Output port*********************/

    sc_in<bool>         clk;
    sc_in<bool>         rst;             //active high

    sc_in<bool>         wr_enable;      //write = 1, read = 0
    sc_in<bool>         CS;        

    sc_in<sc_uint<32> > addr;
    sc_in<sc_int<8> > weight_i;


    sc_out<sc_int<8> > weight_o;


    /**************************Inner data**************************/

    sc_int<8> weight[WSRAMbank_element];


    void do_WeightSRAMbank();
    void do_reset();

    SC_CTOR(WeightSRAMbank){

        SC_THREAD(do_WeightSRAMbank);
        sensitive << clk.pos() << rst.pos();

    }

};


#endif