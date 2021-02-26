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

#include "PE.h"

void PE::do_reset()
{
    //Reset Register
    for(int i = 0; i < MAC_NUM; i++)
    {
        weight_Reg[i] = 0;
        result_Reg[i] = 0;
    }

}

// void PE::do_reg()
// {
//     do_reset();

//     while(1)
//     {
//         wait();

//         //if(weight_valid.read())
//         //{
//         //     for(int i = 0; i < MAC_NUM; i++)
//         //         weight_Reg[i] = weight[i];            
//         // //}

//         // for(int i = 0; i < MAC_NUM; i++)
//         //     result_Reg[i] = result[i];           
//     }
// }

void PE::do_Mult()
{
    for(int i = 0; i < MAC_NUM; i++)
        result[i] = input[i].read() * weight[i].read();
}

void PE::do_reg()
{
    while(1)
    {
        wait();

        for (int i = 0; i < MAC_NUM; i++)
            result_Reg[i].write(result[i]);
    }
}

void PE::do_Accum()
{
    psum = 0;
    for(int i = 0; i < MAC_NUM; i++)
        psum = psum + result_Reg[i].read();

    // if(add_prev.read())
    //     //psum = psum + prev_psum.read();

    // if(last_channel.read())
    // {
    //     //Relu
    //     if( (psum /*+ bias.read()*/) <= 0)
    //         data_out.write(0);
    //     else
    //         data_out.write(sc_int<16> (psum /*+ bias.read()*/) );           //need Dequantize
    // }
    // else
    // {
       data_out.write(psum);
    //}
}
