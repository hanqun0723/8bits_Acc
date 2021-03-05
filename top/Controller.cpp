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
    pe_state = READ_ISRAM;
    dma_state = DMA_WEIGHT_CONFIG;

    //TODO - control register
    //give initial value
    input_h_Reg = 12;
    input_w_Reg = 12;
    input_c_Reg = 1;
    f_size_Reg = 3;
    f_num_Reg = 1;
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

    first_load_Reg = 0;
    store_col_index_Reg = 0;
    shift_count_Reg = 0;
    prev_store_Reg = 0;
    write_result_Reg = 0;
    count_Reg = 0;

}

void Controller::do_dmaRead()
{
    //////DATA MOVEMENT///////
    //move weight

    //move input
    switch(dma_state.read())
    {
        case DMA_WEIGHT_CONFIG :{
            
            dma_state = DMA_READ_TO_WSRAM;
            DMA_irtclr.write(0);

            ////////////////////DMA CONFIG(WEIGHT)////////////////////
            switch (f_size_Reg.read())
            {
                // case 1 : 

                // break;

                case 3 : 

                    if (input_c_Reg.read() <= WSRAMbank_element && f_num_Reg.read() <= PE_NUM)
                    {
                        src.write(DRAM_WEIGHT_BASE);
                        length.write((f_size_Reg.read() * f_size_Reg.read() * input_c_Reg.read() * f_num_Reg.read() + 4)/ 4 );
                        DMA_start.write(1);
                    }

                break;

                defualt :
                    cout << "filter size doesn't support" << endl;
                break;
            }
        }break;

        case DMA_READ_TO_WSRAM : {
                
            int wsram_length = f_size_Reg.read() * f_size_Reg.read() * input_c_Reg.read();
            int wsram_store_num = 0;

            if (count_Reg.read() == wsram_length && DMA_irt.read())
            {
                dma_state = DMA_INPUT_CONFIG;
                DMA_irtclr.write(1);
            }

            DMA_start.write(0);
            if (data_valid.read())
            {
                for(int i = 0; i < WSRAM_NUM; i++)
                {
                    if (i == wsram_store_num)
                    {
                        W_CS[i].write(1);
                        W_WEB[i].write(1);
                        W_addr[i].write(count_Reg.read()/9);
                        W_bank_sel[i].write(count_Reg.read() % 9);
                        W_data_i[i].write(read_data.read());
                        if (wsram_length - count_Reg.read() < 4)
                        {
                            W_length[i].write(wsram_length - count_Reg.read());
                            count_Reg.write(wsram_length);
                        }
                        else
                        {
                            W_length[i].write(4);
                            count_Reg.write(count_Reg.read() + 4);
                        }
                    }
                    else
                    {
                        W_CS[i].write(0);
                        W_WEB[i].write(0);
                        W_addr[i].write(0);
                        W_length[i].write(0);
                        W_bank_sel[i].write(0);
                        W_data_i[i].write(0);                        
                    }
                }
            }
        }break;

        case DMA_INPUT_CONFIG : {
            ////////////////////DMA CONFIG(INPUT)////////////////////
            dma_state = DMA_READ_TO_ISRAM;
            
            DMA_irtclr.write(0);
            count_Reg.write(0);
            if (first_load_Reg.read() == 0)
            {
                if (c_Reg.read() == 0) 
                {
                    if ( (input_w_Reg.read() - w_Reg.read()) > TILE_W)  
                    {        
                        tile_w_Reg.write(TILE_W);
                        w_Reg.write(w_Reg.read() + TILE_W);
                        load_tile_width = TILE_W;
                    }
                    else
                    {
                        tile_w_Reg.write(input_w_Reg.read() - w_Reg.read());
                        w_Reg.write(w_Reg.read() + input_w_Reg.read() - w_Reg.read());
                        load_tile_width = input_w_Reg.read() - w_Reg.read();
                    }
                    if ( (input_h_Reg.read() - h_Reg.read()) > TILE_H)
                    {
                        tile_h_Reg.write(TILE_H);
                        h_Reg.write(h_Reg.read() + TILE_H);
                        load_tile_height = TILE_H;
                    }
                    else
                    {
                        tile_h_Reg.write(input_h_Reg.read() - h_Reg.read());
                        h_Reg.write(h_Reg.read() + input_h_Reg.read() - h_Reg.read());
                        load_tile_height = input_h_Reg.read() - h_Reg.read();
                    }
                    src.write(DRAM_INPUT_BASE);
                    length.write(load_tile_width * load_tile_height / 4);  
                    DMA_start.write(1);
                }
            }
                cout << endl << "///////////DMA READ INPUT///////////" << endl;
                cout << "tile_h : " << load_tile_height << endl;
                cout << "tile_w : " << load_tile_width << endl;
                //cout << "//////////////////////////////" << endl;

        }break;

        case DMA_READ_TO_ISRAM : {
            DMA_start.write(0);

            if (DMA_irt.read())
            {
                dma_state = DMA_WEIGHT_CONFIG;
                DMA_irtclr.write(1);
            }
            if (data_valid.read()) 
            {
                for(int i = 0; i < ISRAM_BANK_NUM; i++)
                {
                    if ( i == (count_Reg.read() / (tile_h_Reg.read() / 4)))
                    {
                        I_CS[i].write(1);
                        I_WEB[i].write(1);
                        I_addr[i].write(count_Reg.read() % (tile_h_Reg.read() / 4));
                        I_data_i[i].write(read_data.read());  
                    }
                    else
                    {
                        I_CS[i].write(0);
                        I_WEB[i].write(0);
                        I_addr[i].write(0);           
                        I_data_i[i].write(0);             
                    }
                }    
                //read_data            
                count_Reg.write(count_Reg.read() + 1);
            }
        }break;
    }
}

void Controller::do_startPE()
{
    switch(pe_state.read())
    {
        case READ_ISRAM:{
            pe_state = ISRAM_TO_REG;
            //next cycle can get isram data
            for(int i = 0; i < ISRAM_BANK_NUM; i++)
            {
                I_CS[i].write(1);
                I_WEB[i].write(0);
                I_addr[i].write(ibank_addr_Reg.read());
            }
            //cout << "READ_ISRAM" << endl;
        }break;

        case ISRAM_TO_REG:{
            
            pe_state = PE_START;

            int bus_sel_index = ibank_ctrl_Reg.read() * ISRAM_bytes;
            
            store_col_index.write(0);
            for (int i = 0; i < ISRAM_BANK_NUM * ISRAM_bytes; i++) //selct 3 isram bank output
                if ( (i >= bus_sel_index) && (i < (bus_sel_index + ISRAM_bytes * FILTER_SIZE)))
                    bus_mux_sel[i].write(1);
                else
                    bus_mux_sel[i].write(0);

            for(int i = 0; i < REG_NUM; i++)
                if ( (i % 6)  < ISRAM_bytes)
                    store_reg[i].write(1);
                else   
                    store_reg[i].write(0);

            //Read Weight SRAM data
            for (int i = 0; i < WSRAM_NUM; i++)
            {
                W_CS[i].write(1);
                W_WEB[i].write(0);
                W_addr[i].write(wbank_addr_Reg.read());
            }           

            ibank_addr_Reg.write(ibank_addr_Reg.read() + 1);

        }break;

        case PE_START:{

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
                //cout << "bus_mux_sel[ " << i << "] : " << bus_mux_sel[i] << endl;
            }


            if (shift_count_Reg.read() == (tile_w_Reg.read() - FILTER_SIZE + 1 - 3))
            {
                read_data_state = FSIZE_3_CASE1;
                prev_store_Reg.write(1);
                ibank_ctrl_Reg.write(ibank_ctrl_Reg.read() + 1);
            }
            else if (prev_store_Reg.read() && (read_data_state.read() == FSIZE_3_CASE2))
            {
                read_data_state = FSIZE_3_CASE1;
                prev_store_Reg.write(0);
            }
            else if (read_data_state.read() == FSIZE_3_CASE4)
            {
                read_data_state = FSIZE_3_CASE1;
                //prev_store_Reg.write(0);
            }
            else
            {
                read_data_state.write(read_data_state.read() + 1);
            }

            // if (shift_count_Reg.read() == (tile_w_Reg.read() - f_size_Reg.read()))
            // {
            //     write_result_Reg.write(0)
            //     // ibank_ctrl_Reg.write(ibank_ctrl_Reg.read() + 1);
            //     // cout << "ibankctrl+1" << endl;
            // }

            if ((ibank_ctrl_Reg.read() >= tile_h_Reg.read() - 2) && (ibank_addr_Reg.read() > 0) &&
                 (shift_count_Reg.read() == 0 ))
                write_result_Reg.write(0);
            else if (ibank_ctrl_Reg.read() == 0)
                write_result_Reg.write(1);

            switch(read_data_state.read())
            {
                case FSIZE_3_CASE1 : {

                    //Read input sram data
                    //cout << "tile_W_Reg : " << tile_w_Reg.read() << endl;
                    for(int i = 0; i < ISRAM_BANK_NUM; i++)
                    {
                        I_CS[i].write(1);
                        I_WEB[i].write(0);
                        I_addr[i].write(ibank_addr_Reg.read());
                    }

                    if (ibank_addr_Reg.read() == (tile_w_Reg.read() - 1)/ISRAM_bytes )
                        ibank_addr_Reg.write(0);
                    else
                        ibank_addr_Reg.write(ibank_addr_Reg.read() + 1);
                    // if (ibank_ctrl_Reg.read() == (tile_h_Reg.read() - f_size_Reg.read())  ){
                    //     ibank_ctrl_Reg.write(0);
                    // }

                    for (int i = 0; i < REG_NUM; i++)
                        store_reg[i].write(0);

                    store_col_index.write(2);

                }break;

                case FSIZE_3_CASE2 : 
                    if (prev_store_Reg.read())
                    {
                        store_col_index.write(0);
                        for(int i = 0; i < REG_NUM; i++)
                        {
                            for(int i = 0; i < REG_NUM; i++)
                                if ( (i % 6)  < ISRAM_bytes)
                                    store_reg[i].write(1);
                                else   
                                    store_reg[i].write(0);
                        }
                        shift_count_Reg.write(0);
                    }
                    else
                    {
                        store_col_index.write(2);
                        for(int i = 0; i < REG_NUM; i++)
                        {
                            if ( (i % 6)  < (REG_ROW_NUM - ISRAM_bytes))
                                store_reg[i].write(0);
                            else   
                                store_reg[i].write(1);
                        }
                    }
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

            //write result to OutpuSRAM
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
            else{
                for (int i = 0; i < OSRAM_NUM; i++)
                    O_CS[i].write(0);                
            }

        }break;

    }
}

void Controller::do_Controller()
{
    do_reset();

    while(1)
    {
        wait();
        //cout << sc_time_stamp() << "   STATE : " << state << endl; 
        //cout << "Read data state : " << read_data_state.read() << endl;


        switch(state.read())
        {    
            case ACC_IDLE:{

                if (start.read())
                {
                    cout << "start recieve time : " << sc_time_stamp() << endl; 
                    state = ACC_DMA_READ;
                    //state = ACC_DMA_READ; 
                }

            }break;

            case ACC_DMA_READ:{

                //state = ACC_READ_ISRAM;
                if ( (dma_state.read() == DMA_READ_TO_ISRAM) && DMA_irt.read())
                {
                    state = ACC_START_PE;
                }

                do_dmaRead();
                
            }break;

            case ACC_START_PE:{

                if ( (ibank_ctrl_Reg.read() > 0 && (write_result_Reg.read() == 0)) )
                {
                    state = ACC_FINISH;
                    // cout << "ibank_ctrl_Reg : " << ibank_ctrl_Reg.read() << endl;
                    // cout << "ibank_addr_Reg : " << ibank_addr_Reg.read() << endl;
                }
                do_startPE();

            }break;

            // case ACC_READ_ISRAM:{

                
            //     state = ACC_ISRAM_TO_REG;


            //     //next cycle can get isram data
            //     for(int i = 0; i < ISRAM_BANK_NUM; i++)
            //     {
            //         I_CS[i].write(1);
            //         I_WEB[i].write(0);
            //         I_addr[i].write(ibank_addr_Reg.read());
            //     }

            //     if (prev_store_Reg.read())
            //     {
            //         //select bus mux
            //         for (int i = 0; i < ISRAM_BANK_NUM * ISRAM_bytes; i++)
            //         {
            //             if (i == (ibank_ctrl_Reg.read() * ISRAM_bytes) )
            //             {
            //                 for(int j = 0; j < ISRAM_bytes * FILTER_SIZE; j++)
            //                     bus_mux_sel[(ibank_ctrl_Reg.read() - 1) * ISRAM_bytes + j].write(1);
            //                 i = i + ISRAM_bytes * FILTER_SIZE;
            //             }
            //             else
            //                 bus_mux_sel[i].write(0);
            //             //cout << "bus_mux_sel[ " << i << "] : " << bus_mux_sel[i] << endl;
            //         }

            //         store_col_index.write(2);
            //         for(int i = 0; i < REG_NUM; i++)
            //         {
            //             if ( (i % 6)  < (REG_ROW_NUM - ISRAM_bytes))
            //                 store_reg[i].write(0);
            //             else   
            //                 store_reg[i].write(1);
            //         }                 
            //         prev_store_Reg.write(0);
            //     }

            // }break;

            // case ACC_ISRAM_TO_REG:{

                //state = ACC_START_PE;

                //select bus mux
                // for(int i = 0; i < ISRAM_bytes * FILTER_SIZE; i++)
                //     bus_mux_sel[ibank_ctrl_Reg.read() + i].write(1);
            //     for (int i = 0; i < ISRAM_BANK_NUM * ISRAM_bytes; i++)
            //     {
            //         if (i == (ibank_ctrl_Reg.read() * ISRAM_bytes) )
            //         {
            //             for(int j = 0; j < ISRAM_bytes * FILTER_SIZE; j++)
            //                 bus_mux_sel[ibank_ctrl_Reg.read()  * ISRAM_bytes + j].write(1);
            //             i = i + ISRAM_bytes * FILTER_SIZE;
            //         }
            //         else
            //             bus_mux_sel[i].write(0);
            //         //cout << "bus_mux_sel[ " << i << "] : " << bus_mux_sel[i] << endl;
            //     }

            //     for(int i = 0; i < REG_NUM; i++)
            //     {
            //         if ( (i % 6)  < ISRAM_bytes)
            //             store_reg[i].write(1);
            //         else   
            //             store_reg[i].write(0);
            //     }
            //     store_col_index.write(0);

            //     //Read Weight SRAM data
            //     for (int i = 0; i < WSRAM_NUM; i++)
            //     {
            //         W_CS[i].write(1);
            //         W_WEB[i].write(0);
            //         W_addr[i].write(wbank_addr_Reg.read());
            //     }                
            //     ibank_addr_Reg.write(ibank_addr_Reg.read() + 1);
                
            // }break;

            // case ACC_START_PE:{

            //     /////////////////////////////////////////
            //     cout << "ACC_START_PE" << endl;
                // //Read Reg data
                // for (int i = 0; i < FILTER_SIZE; i++)
                //     for (int j = 0; j < REG_ROW_NUM; j++)
                //         if (j < FILTER_SIZE)
                //             reg_mux_sel[i * REG_ROW_NUM + j].write(1);
                //         else    
                //             reg_mux_sel[i * REG_ROW_NUM + j].write(0);
                // //next cycle left shift
                // for (int i = 0; i < FILTER_SIZE; i++)
                //     shift[i].write(1);
                // shift_count_Reg.write(shift_count_Reg.read() + 1);

                // //select bus mux
                // for (int i = 0; i < ISRAM_BANK_NUM * ISRAM_bytes; i++)
                // {
                //     if (i == (ibank_ctrl_Reg.read() * ISRAM_bytes) )
                //     {
                //         for(int j = 0; j < ISRAM_bytes * FILTER_SIZE; j++)
                //         {
                //             bus_mux_sel[ibank_ctrl_Reg.read() * ISRAM_bytes + j].write(1);
                //         }
                //             i = i + ISRAM_bytes * FILTER_SIZE;
                //     }
                //     else
                //         bus_mux_sel[i].write(0);
                //     //cout << "bus_mux_sel[ " << i << "] : " << bus_mux_sel[i] << endl;
                // }
            
                // //Read next bank data
                // if ( (shift_count_Reg.read() == (tile_w_Reg.read() - f_size_Reg.read()) ))
                // {
                //     if ((ibank_ctrl_Reg.read() == (tile_h_Reg.read() - f_size_Reg.read())))
                //         state.write(ACC_IDLE);
                //     else
                //         state.write(ACC_READ_ISRAM);
                //     read_data_state.write(FSIZE_3_CASE1);
                //     shift_count_Reg.write(0);                
                //     prev_store_Reg.write(0);    
                // }
                // // if ( (shift_count_Reg.read() == (tile_w_Reg.read() - f_size_Reg.read() - 2) ))
                // // {
                // //     if ((ibank_ctrl_Reg.read() == (tile_h_Reg.read() - f_size_Reg.read())))
                // //         state.write(ACC_IDLE);
                // //     else
                // //         state.write(ACC_READ_ISRAM);
                // //     read_data_state.write(FSIZE_3_CASE1);
                // //     shift_count_Reg.write(0);                
                // //     prev_store_Reg.write(1);    
                // // }
                // if (read_data_state.read() == FSIZE_3_CASE4)
                //     read_data_state.write(FSIZE_3_CASE1);
                // else
                //     read_data_state.write(read_data_state.read() + 1);


                // switch(read_data_state.read())
                // {
                //     case FSIZE_3_CASE1 : 
                //         for (int i = 0; i < REG_NUM; i++)
                //             store_reg[i].write(0);
                //         //Read input sram data
                //         for(int i = 0; i < ISRAM_BANK_NUM; i++)
                //         {
                //             I_CS[i].write(1);
                //             I_WEB[i].write(0);
                //             I_addr[i].write(ibank_addr_Reg.read());
                //         }
                //         if (ibank_addr_Reg.read() == (tile_w_Reg.read() - 1)/ISRAM_bytes )
                //         {
                //             ibank_addr_Reg.write(0);
                //             ibank_ctrl_Reg.write(ibank_ctrl_Reg.read() + 1);
                //         }
                //         else
                //             ibank_addr_Reg.write(ibank_addr_Reg.read() + 1);

                //         if (ibank_ctrl_Reg.read() == (tile_h_Reg.read() - f_size_Reg.read())  ){
                //             ibank_ctrl_Reg.write(0);
                //         }

                //         write_result_Reg.write(1);
                //         store_col_index.write(2);

                //     break;

                //     case FSIZE_3_CASE2 : 
                //         for(int i = 0; i < REG_NUM; i++)
                //         {
                //             if ( (i % 6)  < (REG_ROW_NUM - ISRAM_bytes))
                //                 store_reg[i].write(0);
                //             else   
                //                 store_reg[i].write(1);
                //         }
                //         store_col_index.write(2);
                //     break;

                //     case FSIZE_3_CASE3 : 
                //         for (int i = 0; i < REG_NUM; i++)
                //             store_reg[i].write(0);
                //     break;

                //     case FSIZE_3_CASE4 :
                //         for (int i = 0; i < REG_NUM; i++)
                //             store_reg[i].write(0);                    
                //     break;

                //     default:
                //     break;
                // }

                // //write result to OutpuSRAM
                // if (write_result_Reg.read())
                // {
                //     for (int i = 0; i < OSRAM_NUM; i++)
                //     {
                //         O_CS[i].write(1);
                //         read_write[i].write(2);
                //         O_addr_w[i].write(osram_addr_Reg.read());
                //     }

                //     if (osram_addr_Reg.read() > (tile_h_Reg.read() * tile_w_Reg.read()))
                //     {
                //         osram_addr_Reg.write(0);
                //     }
                //     else
                //         osram_addr_Reg.write(osram_addr_Reg.read() + 1);
                // }

            //}break;

            case ACC_FINISH :{
                cout << "ACC_FINISH!!!!!!!" << endl;
            }break;

            defualt:
                state = ACC_IDLE;
            break;

        }
    }
}






