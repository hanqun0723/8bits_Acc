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

#ifndef _IU_H
#define _IU_H

#include "systemc.h"
#include "Acc_cofig.h"

/***
    TODO : inheretence ahb slave
***/

SC_MODULE(CU){

    sc_in<bool>         clk;
    sc_in<bool>         rst;            //active high


    /*from controller*/
    sc_in<sc_uint<ISRAM_ADDR_LEN> > ISRAM_addr;
    sc_insc_int<ISRAM_DATA_WIDTH> > ISRAM_data_i;

    sc_signal<bool>         ISRAM_CS[ISRAM_BANK_NUM];
    sc_signal<bool>         ISRAM_WEB[ISRAM_BANK_NUM];        
    sc_signal<sc_int<ISRAM_DATA_WIDTH> > ISRAM_data_o[ISRAM_BANK_NUM];    

    sc_signal<sc_int<OSRAM_DATA_WIDTH> > PE_result[PE_NUM];

    sc_signal<sc_uint<2> > OSRAM_read_write[OSRAM_NUM];
    sc_signal<bool>        OSRAM_WEB[OSRAM_NUM];
    sc_signal<sc_uint<OSRAM_ADDR_LEN> > OSRAM_addr_wr[OSRAM_NUM];
    sc_signal<sc_uint<OSRAM_ADDR_LEN> > OSRAM_addr_r[OSRAM_NUM];
    sc_signal<sc_int<OSRAM_DATA_WIDTH> >     data_o[OSRAM_NUM];


    sc_vector<InputSRAMbank> ibank;
    sc_vector<WeightSRAM> W_SRAM;
    sc_vector<OutputSRAM> O_SRAM;         
    sc_vertor<PE>   PE_array;

    SC_CTOR(CU) : ibank("ibank", ISRAM_BANK_NUM),
                  W_SRAM("W_SRAM", WSRAM_NUM),
                  O_SRAM("O_SRAM", OSRAM_NUM),
                  PE_array("PE_array", PE_NUM)
    {
        
        for(int i = 0; i < ISRAM_BANK_NUM; i++)
        {
            ibank[i].clk(clk);
            ibank[i].rst(rst);
            ibank[i].CS(ISRAM_CS[i]);
            ibank[i].WEB(ISRAM_WEB[i]);
            ibank[i].addr(ISRAM_addr);
            ibank[i].data_i(ISRAM_data_i);
            ibank[i].data_o(ISRAM_data_o[i];
        }

        for(int i = 0; i < WSRAM_NUM; i++)
        {


        }

        for(int i = 0; i < OSRAM_NUM; i++)
        {
            O_SRAM[i].clk(clk);
            O_SRAM[i].rst(rst);
            O_SRAM[i].read_write(OSRAM_read_write[i]);
            O_SRAM[i].WEB(OSRAM_WEB[i]);
            O_SRAM[i].addr_wr(OSRAM_addr_wr[i]);
            O_SRAM[i].addr_r(OSRAM_addr_r[i]);
            O_SRAM[i].result_wr(PE_result[i]);
            O_SRAM[i].data_o(data_o[i]);
        }

        for(int i = 0; i < PE_NUM; i++)
        {
            
        }

    }

};


#endif