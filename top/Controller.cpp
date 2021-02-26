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

#include "Controller.h"

void Controller::do_reset()
{
    state = ACC_IDLE;
    read_data_state = FSIZE_3_CASE1;

    //TODO - control register
    //give initial value
    input_h_Reg = 5;
    input_w_Reg = 5;
    input_c_Reg = 3;
    f_size_Reg = 3;
    f_num_Reg = 16;
    stride_Reg = 1;

    h_Reg = 0;
    w_Reg = 0;
    c_Reg = 0;

    ibank_ctrl_Reg = 0;
    ibank_addr_Reg = 0;
    wbank_addr_Reg = 0;
    osram_addr_Reg = 0;

    tile_h_Reg = 0;
    tile_w_Reg = 0;

    store_col_index_Reg = 0;
    shift_count_Reg = 0;
    write_result_Reg = 0;

}

void Controller::do_addrControl()
{
    //////DATA MOVEMENT///////
    //move weight

    //move input
    if (c_Reg.read() == 0)
    {
        if ( (input_w_Reg.read() - w_Reg.read()) > TILE_W)
        {        
            tile_w_Reg.write(TILE_W);
            w_Reg.write(w_Reg.read() + TILE_W);
        }
        else
        {
            tile_w_Reg.write(input_w_Reg.read() - w_Reg.read());
            w_Reg.write(w_Reg.read() + input_w_Reg.read() - w_Reg.read());
        }
        if ( (input_h_Reg.read() - h_Reg.read()) > TILE_H)
        {
            tile_h_Reg.write(TILE_H);
            h_Reg.write(h_Reg.read() + TILE_H);
        }
        else
        {
            tile_h_Reg.write(input_h_Reg.read() - h_Reg.read());
            h_Reg.write(h_Reg.read() + input_h_Reg.read() - h_Reg.read());
        }
    }

    // if (c_Reg.read() == (input_c_Reg.read() - 1))
    //     c_Reg.write(0);
    // else
        c_Reg.write(c_Reg.read() + 1);
}

void Controller::do_Controller()
{
    do_reset();

    while(1)
    {
        wait();
        cout << sc_time_stamp() << "   STATE : " << state << endl; 
        cout << "Read data state : " << read_data_state.read() << endl;

        switch(state.read())
        {    
            case ACC_IDLE:

                if (start.read())
                {
                    cout << "start recieve time : " << sc_time_stamp() << endl; 
                    state = DMA_READ; 
                }

            break;

            case DMA_READ:

                state = ACC_READ_ISRAM;

                do_addrControl();
                
            break;

            case ACC_READ_ISRAM:

                state = ACC_ISRAM_TO_REG;


                cout << endl << "///////////DMA READ///////////" << endl;
                cout << "tile_h : " << tile_h_Reg << endl;
                cout << "tile_w : " << tile_w_Reg << endl;
                cout << "//////////////////////////////" << endl;

                //next cycle can get isram data
                for(int i = 0; i < ISRAM_BANK_NUM; i++)
                {
                    I_CS[i].write(1);
                    I_WEB[i].write(0);
                    I_addr[i].write(ibank_addr_Reg.read());
                }

            break;

            case ACC_ISRAM_TO_REG:

                state = ACC_START_PE;

                //select bus mux
                for(int i = 0; i < ISRAM_bytes * FILTER_SIZE; i++)
                    bus_mux_sel[ibank_ctrl_Reg.read() + i].write(1);
                for(int i = 0; i < REG_NUM; i++)
                {
                    if ( (i % 6)  < ISRAM_bytes)
                        store_reg[i].write(1);
                    else   
                        store_reg[i].write(0);
                }
                store_col_index.write(0);

                //Read Weight SRAM data
                for (int i = 0; i < WSRAM_NUM; i++)
                {
                    W_CS[i].write(1);
                    W_WEB[i].write(0);
                    W_addr[i].write(wbank_addr_Reg.read());
                }                
                ibank_addr_Reg.write(ibank_addr_Reg.read() + 1);

            break;

            case ACC_START_PE:

                /////////////////////////////////////////

                //Read Reg data
                for (int i = 0; i < FILTER_SIZE; i++)
                    for (int j = 0; j < REG_ROW_NUM; j++)
                        if (j < FILTER_SIZE)
                            reg_mux_sel[i * REG_ROW_NUM + j].write(1);
                        else    
                            reg_mux_sel[i * REG_ROW_NUM + j].write(0);
                //next cycle left shift
                for (int i = 0; i < FILTER_SIZE; i++)
                    shift[i].write(1);
                shift_count_Reg.write(shift_count_Reg.read() + 1);

                //select bus mux
                
                for (int i = 0; i < ISRAM_BANK_NUM * ISRAM_bytes; i++)
                {
                    if (i == (ibank_ctrl_Reg.read() * ISRAM_bytes) )
                    {
                        for(int j = 0; j < ISRAM_bytes * FILTER_SIZE; j++)
                        {
                            bus_mux_sel[ibank_ctrl_Reg.read() * ISRAM_bytes + j].write(1);
                        }
                            i = i + ISRAM_bytes * FILTER_SIZE;
                    }
                    else
                        bus_mux_sel[i].write(0);
                }

                if ( (shift_count_Reg.read() == (tile_w_Reg.read() - f_size_Reg.read() - 2) ))
                {
                    read_data_state.write(FSIZE_3_CASE1);
                    shift_count_Reg.write(0);
                }
                else if ( (read_data_state.read() == FSIZE_3_CASE4) )
                    read_data_state.write(FSIZE_3_CASE1);
                else
                    read_data_state.write(read_data_state.read() + 1);

                switch(read_data_state.read())
                {
                    case FSIZE_3_CASE1 : 
                        for (int i = 0; i < REG_NUM; i++)
                            store_reg[i].write(0);
                        //Read input sram data
                        for(int i = 0; i < ISRAM_BANK_NUM; i++)
                        {
                            I_CS[i].write(1);
                            I_WEB[i].write(0);
                            I_addr[i].write(ibank_addr_Reg.read());
                        }
                        if (ibank_addr_Reg.read() == (tile_w_Reg.read() - 1)/ISRAM_bytes )
                        {
                            ibank_addr_Reg.write(0);
                            ibank_ctrl_Reg.write(ibank_ctrl_Reg.read() + 1);
                        }
                        else
                            ibank_addr_Reg.write(ibank_addr_Reg.read() + 1);

                        if (ibank_ctrl_Reg.read() == (tile_h_Reg.read() - f_size_Reg.read())  )
                            ibank_ctrl_Reg.write(0);

                        write_result_Reg.write(1);

                        if ( (ibank_addr_Reg.read() != 0) && (shift_count_Reg.read() == 0))
                        {
                            for (int i = 0; i < REG_NUM; i++)
                                store_reg[i].write(0);
                            store_col_index.write(0);   
                        }
                        else if ((ibank_addr_Reg.read() == 0) && (shift_count_Reg.read() == 0))
                        {
                            for (int i = 0; i < REG_NUM; i++)
                            {
                                if ( (i % 6)  < (REG_ROW_NUM - ISRAM_bytes))
                                    store_reg[i].write(0);
                                else   
                                    store_reg[i].write(1);
                            }
                            store_col_index.write(2);
                        }
                    break;

                    case FSIZE_3_CASE2 : 
                        for(int i = 0; i < REG_NUM; i++)
                        {
                            if ( (i % 6)  < (REG_ROW_NUM - ISRAM_bytes))
                                store_reg[i].write(0);
                            else   
                                store_reg[i].write(1);
                        }
                        store_col_index.write(2);
                    break;

                    case FSIZE_3_CASE3 : 
                        for (int i = 0; i < REG_NUM; i++)
                            store_reg[i].write(0);
                    break;

                    case FSIZE_3_CASE4 :
                        for (int i = 0; i < REG_NUM; i++)
                            store_reg[i].write(0);                    
                    break;

                    default:
                    break;
                }

                if (write_result_Reg.read())
                {
                    for (int i = 0; i < OSRAM_NUM; i++)
                    {
                        O_CS[i].write(1);
                        read_write[i].write(2);
                        O_addr_w[i].write(osram_addr_Reg.read());
                    }

                    if (osram_addr_Reg.read() > (tile_h_Reg.read() * tile_w_Reg.read()))
                    {
                        osram_addr_Reg.write(0);
                    }
                    else
                        osram_addr_Reg.write(osram_addr_Reg.read() + 1);
                }

            break;


            defualt:
                state = ACC_IDLE;
            break;

        }
    }
}






