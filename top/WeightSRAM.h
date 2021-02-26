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

#ifndef _WEIGHTSRAM_H
#define _WEIGHTSRAM_H

#include "Acc_cofig.h"
#include "WeightSRAMbank.h"
#include "systemc.h"

SC_MODULE(WeightSRAM){

    /*********************Input and Output port*********************/

    sc_in<bool>         clk;
    sc_in<bool>         rst;             

    sc_in<bool>         CS;    
    sc_in<bool>         WEB;          
           
    sc_in<sc_uint<WSRAM_ADDR_LEN> >    addr;

    sc_in<sc_uint<3> >    w_length;            //1bytes ~ 4bytes   
    sc_in<sc_int<BUS_WIDTH> >    weight;    //32bits weight


    sc_in<sc_uint<WSRAM_BANK_BITS> >     bank_sel;      
    
    sc_out<sc_int<DATA_WIDTH> > weight_o[WSRAM_BANK_NUM];


    /**************************Inner data**************************/
    sc_signal<sc_uint<WSRAM_ADDR_LEN> > bank_addr[WSRAM_BANK_NUM];
    sc_signal<sc_int<DATA_WIDTH> > weight_i[WSRAM_BANK_NUM];


    //WeightSRAMbank wbank[WSRAM_BANK_NUM];
    sc_vector<WeightSRAMbank> wbank;    //Initialize array of instances

    void do_WeightSRAM();
    void do_reset();

	SC_CTOR(WeightSRAM) : wbank("wbank",WSRAM_BANK_NUM)
	{
        
        for(int i = 0; i < WSRAM_BANK_NUM; i++)
        {
            wbank[i].clk(clk);
            wbank[i].rst(rst);
            wbank[i].WEB(WEB);
            wbank[i].CS(CS);
            wbank[i].addr(bank_addr[i]);
            wbank[i].weight_i(weight_i[i]);
            wbank[i].weight_o(weight_o[i]);
        }

        SC_THREAD(do_WeightSRAM);
        sensitive << clk.pos() << rst.pos();

    }

};


#endif