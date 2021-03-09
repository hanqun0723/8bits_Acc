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

#include "OutputSRAM.h"

void OutputSRAM::do_reset()
{
    for(int i = 0; i < OSRAM_SIZE; i++)
    {
        O_SRAMdata[i] = 0;
    }
}

void OutputSRAM::do_OutputSRAM()
{
    //reset
    do_reset();

    while(1)
    {
        wait();

        if (CS.read())
        {
            switch(read_write.read())
            {    
                case O_READ:
                    
                    data_o.write(O_SRAMdata[addr_r.read()]);

                break;

                case O_WRITE:

                    O_SRAMdata[addr_w.read()] =  data_i.read();
                    //cout << "OSRAM_write!!!!!!!!" << endl;

                break;

                case O_READ_WRITE:

                    data_o.write(O_SRAMdata[addr_r.read()]);
                    // data_o.write( (O_SRAMdata[addr_r.read()] << DATA_WIDTH) |
                    //                 O_SRAMdata[addr_r.read() + 1]);
                    O_SRAMdata[addr_w.read()] =  data_i.read();

                break;

                defualt:

                    data_o.write(0);
        
                break;

            }
        }
    }
}