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

#include "WeightSRAM.h"


void WeightSRAM::do_WeightSRAM(){


    /***Write***/
    if (CS.read())
    {
        if(WEB.read())          
        {            
                for(int i = 0; i < w_length.read(); i++) 
                {
                    //cout << "bank_sel : " << bank_sel.read() << endl;
                    if((bank_sel.read() + i) >= WSRAM_BANK_NUM)
                    {
                        bank_addr[(bank_sel.read() + i) - WSRAM_BANK_NUM] = addr.read() + 1;
                        weight_i[(bank_sel.read() + i) - WSRAM_BANK_NUM] = weight.read().range( (i * 8 + 7), (i * 8));
                    }
                    else
                    {
                        bank_addr[bank_sel.read() + i] = addr.read();
                        weight_i[bank_sel.read() + i] = weight.read().range(i * 8 + 7, i * 8);
                    }
                }
                //cout << "================================" << endl;
            //}
        }
        /***Read***/
        else                    
        {
            for(int i = 0; i < WSRAM_BANK_NUM; i++)
            {
                bank_addr[i] = addr.read();   //Read data address
            }

        }
    }
    
}