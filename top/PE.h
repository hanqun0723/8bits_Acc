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

#ifndef PE_H
#define PE_H

#include "Acc_cofig.h"
#include "systemc.h"

SC_MODULE(PE){

    sc_in<bool> clk;
    sc_in<bool> rst;
  
    sc_in<sc_int<8> > input[9];          //from Input SRAM 
    sc_in<sc_int<8> > weight[9];         //from Weight SRAM
    //sc_in<sc_int<8> > bias;
    //sc_in<sc_int<8> > Dequantize; 

    // sc_in<bool>       weight_valid;
    // sc_in<bool>       last_channel;

    sc_in<bool>           add_prev;          //add_prev = 1 means add previos psum
    sc_in<sc_int<16> >    prev_psum;

    sc_out<sc_int<16> >   data_out;

  
    sc_signal<sc_int<8> > weight_Reg[9];  
    sc_signal<sc_int<8> > result_Reg[9];

    sc_int<16> result[9];     //after multiply
    sc_int<16> psum;


    void do_reset();
    void do_reg();

    //2stage pipeline , Mult + Accum
    void do_Mult();
    void do_Accum();


    SC_CTOR(PE)
    {    
        SC_METHOD(do_Mult);
        for (int i = 0; i < MAC_NUM; i++)
          sensitive << input[i] << weight[i];
          
        SC_THREAD(do_reg);
        sensitive << clk.pos() << rst.pos();

        SC_METHOD(do_Accum);
        for (int i = 0; i < 9; i++){
          sensitive << result_Reg[i];
        }
        sensitive << add_prev << prev_psum; 
        
    }

};


#endif