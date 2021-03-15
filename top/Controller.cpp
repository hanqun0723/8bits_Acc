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
    dma_read_state = DMA_WEIGHT_CONFIG;
    dma_write_state = DMA_OUTPUT_CONFIG;

    //TODO - control register
    //give initial value
    input_h_Reg = INPUT_H;
    input_w_Reg = INPUT_W;
    input_c_Reg = INPUT_C;
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
    osram_id_Reg = 0;

    tile_h_Reg = 0;
    tile_w_Reg = 0;

    first_load_Reg = 0;
    store_col_index_Reg = 0;
    shift_count_Reg = 0;
    prev_store_Reg = 0;
    write_result_Reg = 0;
    dma_count_Reg = 0;
    conv_count_Reg = 0;
}

void Controller::do_dmaRead()
{
    //////DATA MOVEMENT///////
    dma_type.write(1);
    switch(dma_read_state.read())
    {
        case DMA_WEIGHT_CONFIG :{
            
            dma_read_state = DMA_READ_TO_WSRAM;
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

            if (dma_count_Reg.read() == wsram_length && DMA_irt.read())
            {
                dma_read_state = DMA_INPUT_CONFIG;
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
                        W_addr[i].write(dma_count_Reg.read()/9);
                        W_bank_sel[i].write(dma_count_Reg.read() % 9);
                        W_data_i[i].write(read_data.read());
                        if (wsram_length - dma_count_Reg.read() < 4)
                        {
                            W_length[i].write(wsram_length - dma_count_Reg.read());
                            dma_count_Reg.write(wsram_length);
                        }
                        else
                        {
                            W_length[i].write(4);
                            dma_count_Reg.write(dma_count_Reg.read() + 4);
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
            dma_read_state = DMA_READ_TO_ISRAM;
            
            DMA_irtclr.write(0);
            dma_count_Reg.write(0);
            if (first_load_Reg.read() == 0)
            {
                if ( (input_w_Reg.read() - w_Reg.read()) > TILE_W)  
                {        
                    tile_w_Reg.write(TILE_W);
                    load_tile_width = TILE_W;
                }
                else
                {
                    tile_w_Reg.write(input_w_Reg.read() - w_Reg.read());
                    load_tile_width = input_w_Reg.read() - w_Reg.read();
                }
                if ( (input_h_Reg.read() - h_Reg.read()) > TILE_H)
                {
                    tile_h_Reg.write(TILE_H);
                    load_tile_height = TILE_H;
                }
                else
                {
                    tile_h_Reg.write(input_h_Reg.read() - h_Reg.read());
                    load_tile_height = input_h_Reg.read() - h_Reg.read();
                }
                src.write(DRAM_INPUT_BASE);
                length.write( (load_tile_width + 3) / 4 * load_tile_height);  
                DMA_start.write(1);
                c_Reg.write(c_Reg.read() + 1);
            }
            else
            {   //Read next bank of input -> 1x64       
                tile_w_Reg.write(TILE_W);
                load_tile_width = TILE_W;
                load_tile_height = 1;
                int offset = (c_Reg.read() * (input_h_Reg.read() * input_w_Reg.read())) + 
                             (h_Reg.read() * input_w_Reg.read()) +
                             (w_Reg.read());
                src.write(DRAM_INPUT_BASE + offset);
                length.write( (load_tile_width + 3) / 4 * load_tile_height);  
                DMA_start.write(1);
                cout << "next_bank" << endl;
                
                if ( (c_Reg.read() < input_c_Reg.read()) && (h_Reg.read() == tile_h_Reg.read() ) )
                {
                    if (c_Reg.read() == (input_c_Reg.read() - 1))
                    {
                        c_Reg.write(0);
                        h_Reg.write(0);
                        w_Reg.write(w_Reg.read() + tile_w_Reg.read());
                    }
                    else
                    {
                        c_Reg.write(c_Reg.read() + 1);
                        h_Reg.write(0);
                    }
                }
                else if (c_Reg.read() == input_c_Reg.read())
                {
                    w_Reg.write(w_Reg.read() + tile_w_Reg.read());
                    c_Reg.write(0);
                }
                else 
                    h_Reg.write(h_Reg.read() + 1);
            }
        
            cout << endl << "//////////Address Control Info////////////" << endl;
            cout << "h_Reg : " << h_Reg.read() << endl;
            cout << "w_Reg : " << w_Reg.read() << endl;
            cout << "c_Reg : " << c_Reg.read() << endl;
            cout << "///////////DMA READ INPUT///////////" << endl;
            cout << "tile_h : " << load_tile_height << endl;
            cout << "tile_w : " << load_tile_width << endl;
            cout << "row_padding : " << ROW_PADD << endl;
            cout << "dma length : " << (load_tile_width + 3)/4  * load_tile_height<< endl;
            cout << "==================================================================" << endl;

        }break;

        case DMA_READ_TO_ISRAM : {

            DMA_start.write(0);

            if (DMA_irt.read())
            {
                dma_read_state = DMA_WEIGHT_CONFIG;
                DMA_irtclr.write(1);
                first_load_Reg.write(1);
                dma_count_Reg.write(0);
            }
            if (data_valid.read()) 
            {
                for(int i = 0; i < ISRAM_BANK_NUM; i++)
                {
                    if (!first_load_Reg.read())
                    {
                        if ( i == (dma_count_Reg.read() / ((tile_w_Reg.read() + 3) / 4)))
                        {
                            I_CS[i].write(1);
                            I_WEB[i].write(1);
                            I_addr[i].write(dma_count_Reg.read() % ((tile_w_Reg.read() + 3) / 4));
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
                    else 
                    {
                        if ( i == h_Reg.read() - 1)
                        {
                            I_CS[i].write(1);
                            I_WEB[i].write(1);
                            I_addr[i].write(dma_count_Reg.read());
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
                }    
                //read_data            
                dma_count_Reg.write(dma_count_Reg.read() + 1);
            }
        }break;

    }
}

void Controller::do_dmaWrite()
{
    dma_type.write(2);
    osram_id.write(0);
    
    switch (dma_write_state.read())
    {
        case DMA_OUTPUT_CONFIG :{
            //cout << "DMA_OUTPUT_CONFIG" << endl;
            dma_write_state = DMA_WRITE_TO_DRAM;
            DMA_irtclr.write(0);
            dma_count_Reg.write(1);
            //osram_id_Reg.write(0);

            tgt.write(DRAM_OUTPUT_BASE);
            length.write(62*62);  
            DMA_start.write(1);

            for (int i = 0; i < OSRAM_NUM; i++)
            {
                O_CS[i] = 1;
                read_write = 1;
                O_addr_r[i] =  dma_count_Reg.read();
            }
        }break;
    
        case DMA_WRITE_TO_DRAM : {

            if (DMA_irt.read())
            {
                dma_read_state = DMA_OUTPUT_CONFIG;
                DMA_irtclr.write(1);
                dma_count_Reg.write(0);
            }
            else
            {
                dma_count_Reg.write(dma_count_Reg.read() + 1);
                for (int i = 0; i < OSRAM_NUM; i++)
                {
                        O_CS[i] = 1;
                        read_write = 1;
                        O_addr_r[i] =  dma_count_Reg.read();
                }
            }
            //cout << "DMA_WRITE_TO_DRAM" << endl;


        }break;
    }

}

void Controller::do_startPE()
{

    out_length = (tile_h_Reg.read() - f_size_Reg.read() + 1) * (tile_w_Reg.read() - f_size_Reg.read() + 1);

    //ping pong Input SRAM control
    if ( (c_Reg.read() != input_c_Reg.read() ) &&  
         (shift_count_Reg.read() == (tile_w_Reg.read() - FILTER_SIZE - 1)) && 
         (h_Reg.read() <= tile_h_Reg.read()))
        dma_read_state.write(DMA_INPUT_CONFIG);
    else if (dma_read_state.read() >= DMA_INPUT_CONFIG)
        do_dmaRead();

    switch(pe_state.read())
    {
        case READ_ISRAM:{
            pe_state = ISRAM_TO_REG;
            //next cycle can get isram data
            for(int i = 0; i < ISRAM_BANK_NUM; i++)
            {
                if ( (dma_read_state.read() != DMA_READ_TO_ISRAM) || 
                   ( (dma_read_state.read() == DMA_READ_TO_ISRAM) && (i != (h_Reg.read() - 1))) )
                {
                    I_CS[i].write(1);
                    I_WEB[i].write(0);
                    I_addr[i].write(0);
                }
            }
            // for(int i = 0; i < ISRAM_BANK_NUM; i++)
            // {
            //     I_CS[i].write(1);
            //     I_WEB[i].write(0);
            //     I_addr[i].write(0);
            // }
            ibank_ctrl_Reg.write(0);
            ibank_addr_Reg.write(0);
            shift_count_Reg.write(0);
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

            //state control
        
            add_prev.write(1);

            //tile done
            if (conv_count_Reg.read() == out_length && !write_result_Reg.read())
            {
                pe_state = READ_ISRAM;
                conv_count_Reg.write(0);
                wbank_addr_Reg.write(wbank_addr_Reg.read() + 1);
            }

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
                    //cout << ISRAM_BANK_NUM * ISRAM_bytes << endl;
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

            //read_data_state control
            if (shift_count_Reg.read() == (tile_w_Reg.read() - FILTER_SIZE + 1 - 3))
            {
                read_data_state = FSIZE_3_CASE1;
                prev_store_Reg.write(1);  
            }
            else if (prev_store_Reg.read() && (read_data_state.read() == FSIZE_3_CASE2) || 
                    (conv_count_Reg.read() == out_length && !write_result_Reg.read()))
            {
                read_data_state = FSIZE_3_CASE1;
                prev_store_Reg.write(0);
            }
            else if (read_data_state.read() == FSIZE_3_CASE4)
                read_data_state = FSIZE_3_CASE1;
            else
                read_data_state.write(read_data_state.read() + 1);

            //Control write output sram
            if (conv_count_Reg.read() == out_length)
            {
                write_result_Reg.write(0);
            }
            else if (read_data_state.read() == FSIZE_3_CASE1 || conv_count_Reg.read()!= 0)
            {
                conv_count_Reg.write(conv_count_Reg.read() + 1);
                write_result_Reg.write(1);
            }
            // cout << "control_state : " << state.read() << endl;
            // cout << "pe_data_state : " << pe_state.read() << endl; 
            // cout << "read_data_state : " << read_data_state.read() << endl;

            for(int i = 0; i < ISRAM_BANK_NUM; i++)
            {
                if ( (dma_read_state.read() != DMA_READ_TO_ISRAM) || 
                   ( (dma_read_state.read() == DMA_READ_TO_ISRAM) && (i != (h_Reg.read() - 1))) )
                {
                    I_CS[i].write(1);
                    I_WEB[i].write(0);
                    I_addr[i].write(ibank_addr_Reg.read());
                }
            }

            switch(read_data_state.read())
            {
                case FSIZE_3_CASE1 : {

                    if (ibank_addr_Reg.read() == (tile_w_Reg.read() - 1)/ISRAM_bytes )
                        ibank_addr_Reg.write(0);
                    else
                        ibank_addr_Reg.write(ibank_addr_Reg.read() + 1);

                    if (prev_store_Reg.read())
                    {
                        if (tile_h_Reg.read() % 4 == 1)
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
                        else 
                        {
                            store_col_index.write(0);
                            for (int i = 0; i < REG_NUM; i++)
                                store_reg[i].write(0);
                        }        
                        if (ibank_ctrl_Reg.read() < (tile_h_Reg.read() - f_size_Reg.read()))
                            ibank_ctrl_Reg.write(ibank_ctrl_Reg.read() + 1);
                        else 
                            ibank_ctrl_Reg.write(0);       
                    }
                    else
                    {
                        store_col_index.write(0);
                        for (int i = 0; i < REG_NUM; i++)
                            store_reg[i].write(0);
                    }                  

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
            if (!write_result_Reg.read())
            {
                for (int i = 0; i < OSRAM_NUM; i++)
                {
                    O_CS[i].write(1);
                    read_write.write(O_READ);
                    osram_addr_Reg.write(0);
                    O_addr_r[i].write(osram_addr_Reg.read());
                }
            }
            else if (write_result_Reg.read())
            {
                for (int i = 0; i < OSRAM_NUM; i++)
                {
                    O_CS[i].write(1);
                    O_addr_w[i].write(osram_addr_Reg.read());
                    O_addr_r[i].write(osram_addr_Reg.read() + 1);
                }
                read_write.write(O_READ_WRITE);
                if (osram_addr_Reg.read() > (tile_h_Reg.read() * tile_w_Reg.read()))
                {
                    osram_addr_Reg.write(0);
                }
                else
                    osram_addr_Reg.write(osram_addr_Reg.read() + 1);
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
                if ( (dma_read_state.read() == DMA_READ_TO_ISRAM) && DMA_irt.read())
                {
                    state = ACC_START_PE;
                }

                do_dmaRead();
                
            }break;

            case ACC_START_PE:{

                out_length = (tile_h_Reg.read() - f_size_Reg.read() + 1) * (tile_w_Reg.read() - f_size_Reg.read() + 1);

                //wait for next tile move to ISRAM
                if ( (c_Reg.read() == 0) && (h_Reg.read() == input_h_Reg.read()) && DMA_irt.read())
                    state = ACC_DMA_WRITE;
                // if ( (c_Reg.read() == input_c_Reg.read()) &&  (osram_addr_Reg.read() == out_length))
                //     state = ACC_DMA_WRITE;

                do_startPE();

            }break;

            case ACC_DMA_WRITE: {
                
                if (DMA_irt.read() && (dma_write_state.read() != DMA_OUTPUT_CONFIG) )
                {
                    if ( (c_Reg.read() == INPUT_C) && (h_Reg.read() == INPUT_H) && (w_Reg.read() == INPUT_W) )
                        state = ACC_FINISH;
                    else
                        state = ACC_DMA_READ;
                }
                else
                    do_dmaWrite();

            }break;

            case ACC_FINISH :{
                cout << "ACC_FINISH!!!!!!!" << endl;
            }break;

            defualt:
                state = ACC_IDLE;
            break;

        }
    }
}






