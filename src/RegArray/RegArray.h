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

#ifndef _REGARRAY_H
#define _REGARRAY_H

#include "../Acc_cofig.h"
#include "systemc.h"

SC_MODULE(RegArray)
{
    /*********************Input and Output port*********************/

    sc_in<bool>         clk;
    sc_in<bool>         rst;                       //active high
    sc_in<sc_int<DATA_WIDTH> > reg_i[REG_NUM];     //from SRAM BUS         

    sc_in<bool>        store_reg;  
    sc_in<bool>        shift[FILTER_SIZE];                   //Support filter size = 3 , shift each row

    sc_out<sc_int<DATA_WIDTH> > reg_o[REG_NUM];
    /**************************Inner data**************************/
    sc_int<DATA_WIDTH> reg[REG_NUM];

    void do_reset();
    void do_RegArray();

    SC_CTOR(RegArray)
    {
        SC_THREAD(do_RegArray);
        sensitive << clk.pos() << rst.pos();
    }

};

#endif