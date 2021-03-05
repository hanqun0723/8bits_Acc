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

#include "WeightSRAMbank.h"

void WeightSRAMbank::do_reset(){

    for(int i = 0; i < WSRAMbank_element; i++){
        weight[i] = 0;
    }

    weight_o = 0;
}

void WeightSRAMbank::do_WeightSRAMbank(){

    //reset
    do_reset();

    while(1){

        wait();

        if(CS.read())
        {
            if(WEB.read())                //write data
            {
                weight[addr.read()] = weight_i.read();
            }
            else                                //read data
            {
                weight_o.write(weight[addr.read()]);
            }
        }
        else
        {
            weight_o.write(0);
        }

    }

}