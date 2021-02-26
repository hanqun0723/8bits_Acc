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

#ifndef _REG_MUX_H
#define _REG_MUX_H

#include "Acc_cofig.h"
#include "systemc.h"

SC_MODULE(Reg_MUX)
{
    /*********************Input and Output port*********************/


    sc_in<sc_int<DATA_WIDTH> > reg_data_i[REG_NUM];  
    sc_in<bool> reg_mux_sel[REG_NUM];                         
      
    sc_out<sc_int<DATA_WIDTH> > reg_data_o[MAC_NUM];             

    void do_RegMUX();

    SC_CTOR(Reg_MUX)
    {
        SC_METHOD(do_RegMUX);
        for(int i = 0; i < REG_NUM; i++)
            sensitive << reg_data_i[i] << reg_mux_sel[i];
    }

};

#endif