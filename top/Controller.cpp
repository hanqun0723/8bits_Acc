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
    pe_state = READ_WSRAM;
    dma_read_state = DMA_WEIGHT_CONFIG;
    dma_write_state = DMA_OUTPUT_CONFIG;

    //TODO - control register
    //give initial value
    input_h_Reg = INPUT_H;
    input_w_Reg = INPUT_W;
    input_c_Reg = INPUT_C;

    f_size_Reg = F_SIZE;
    f_num_Reg = F_NUM;
    stride_Reg = 1;

    out_h_Reg = 0;
    out_w_Reg = 0;
    out_c_Reg = 0;

    h_Reg = 0;
    w_Reg = 0;
    c_Reg = 0;

    ofmap_h_Reg = 0;
    ofmap_w_Reg = 0;
    ofmap_c_Reg = 0;

    ibank_ctrl_Reg = 0;
    ibank_addr_Reg = 0;
    wbank_addr_Reg = 0;
    osram_addr_Reg = 0;
    osram_id_Reg = 0;
    wsram_id_Reg = 0;

    tile_h_Reg = 0;
    tile_w_Reg = 0;

    for (int i = 0; i < ISRAM_BANK_NUM; i++)
        proc_ctrl_Reg[i] = 0;

    first_load_Reg = 0;
    store_col_index_Reg = 0;
    shift_count_Reg = 0;
    prev_store_Reg = 0;
    write_result_Reg = 0;
    dma_count_Reg = 0;
    conv_count_Reg = 0;
    tile_overlap_Reg = 0;
    align_Reg = 0;
    row_sel_Reg = 0;
    store_count_Reg = 0;
}

void Controller::do_Input_addrCtrl()
{

    if (state.read() == ACC_DMA_WRITE && DMA_irt.read() && dma_count_Reg.read() == out_length )  //tile all channel done
    {
        align_Reg.write(0);
        if (w_Reg.read() + tile_w_Reg.read() == input_w_Reg.read())
        {
            // cout << h_Reg.read() << " " << tile_h_Reg.read() << " " << input_h_Reg.read() << endl;
            // cout << " sdfsdf sd fds" << endl;
            if (h_Reg.read() + tile_h_Reg.read() == input_h_Reg.read())
            {
                w_Reg.write(w_Reg.read() + tile_w_Reg.read());
                h_Reg.write(h_Reg.read() + tile_h_Reg.read());
            }
            else if (h_Reg.read() + tile_h_Reg.read() < input_h_Reg.read())
            {
                w_Reg.write(0);
                h_Reg.write(h_Reg.read() + tile_h_Reg.read() - tile_overlap_Reg.read());
                ofmap_w_Reg.write(0);
                ofmap_h_Reg.write(h_Reg.read() + tile_h_Reg.read() - tile_overlap_Reg.read());
            }
            else
            {
                w_Reg.write(w_Reg.read() + tile_w_Reg.read());
                ofmap_w_Reg.write(w_Reg.read() + tile_w_Reg.read());
            }
        }
        else 
        {
            w_Reg.write(w_Reg.read() + tile_w_Reg.read() - tile_overlap_Reg.read());
            ofmap_w_Reg.write(w_Reg.read() + tile_w_Reg.read() - tile_overlap_Reg.read());
        }


        c_Reg.write(0);
        add_prev.write(0);
    }
    else
    {
        row_sel_Reg.write(row_sel_Reg.read() + 1);

        if (row_sel_Reg.read() + 1 == tile_h_Reg.read())
        {
            c_Reg.write(c_Reg.read() + 1); 
            row_sel_Reg.write(0);
        }
        cout << endl << "////////////////////////////////////////////////" << endl;
        cout << "////////// Read Address Control Info////////////" << endl;
        cout << "////////////////////////////////////////////////" << endl;
        cout << "h_Reg : " << h_Reg.read() << endl;
        cout << "w_Reg : " << w_Reg.read() << endl;
        cout << "c_Reg : " << c_Reg.read() << endl;
        cout << "row_sel_Reg : " << row_sel_Reg.read() << endl;
        cout << "///////////DMA READ INPUT///////////" << endl;
        cout << "tile_h : " << tile_h_Reg.read() << endl;
        cout << "tile_w : " << tile_w_Reg.read() << endl;
        cout << "row_padding : " << ROW_PADD << endl;
        cout << "==================================================================" << endl;
    }
 
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
                        src.write(DRAM_WEIGHT_BASE /*+ wsram_id_Reg.read()*/);
                        length.write((f_size_Reg.read() * f_size_Reg.read() * input_c_Reg.read() /* * f_num_Reg.read() */+ 4)/ 4 );
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
            if (dram_done.read())
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
            
            DMA_irtclr.write(0);
            dma_count_Reg.write(0);
            store_count_Reg.write(0);
            tile_overlap_Reg.write(f_size_Reg.read() - 1);

            //tile width
            if (w_Reg.read() == 0)
            {
                if (input_w_Reg.read() - w_Reg.read() > TILE_W)
                {
                    tile_w_Reg.write(TILE_W);
                    //src.write(DRAM_INPUT_BASE);
                    length.write(TILE_W / 4);
                }
                else
                {
                    tile_w_Reg.write(input_w_Reg.read() - w_Reg.read());
                    //src.write(DRAM_INPUT_BASE);
                    length.write( (input_w_Reg.read() - w_Reg.read() + 3)/4);
                }
            }
            else    //tile overlap control, TODO
            {
                if (input_w_Reg.read() - w_Reg.read() - tile_overlap_Reg.read() > TILE_W)
                {
                    tile_w_Reg.write(TILE_W);
                    //src.write(DRAM_INPUT_BASE);
                    if (w_Reg.read() % 4 != 0)
                    {
                        align_Reg.write(1);
                        length.write(TILE_W / 4 + 1);
                    }
                    else 
                    {
                        align_Reg.write(0);
                        length.write(TILE_W / 4);
                    }
                }
                else
                {
                    tile_w_Reg.write(input_w_Reg.read() - w_Reg.read());
                    //src.write(DRAM_INPUT_BASE);
                    if (w_Reg.read() % 4 != 0)
                    {
                        align_Reg.write(1);
                        length.write( (input_w_Reg.read() - w_Reg.read() + 3)/4 + 1);
                    }
                    else 
                    {
                        align_Reg.write(0);
                        length.write( (input_w_Reg.read() - w_Reg.read() + 3)/4);
                    }
                }
            }

            //tile height
            if (c_Reg.read() == 0)
                if (input_h_Reg.read() - h_Reg.read() > TILE_H)
                    tile_h_Reg.write(TILE_H);
                else
                    tile_h_Reg.write(input_h_Reg.read() - h_Reg.read());
            // else    //tile overlap control, TODO
            //     if (input_h_Reg.read() - h_Reg.read() - tile_overlap_Reg.read() > TILE_H)
            //         tile_h_Reg.write(TILE_H);
            //     else
            //         tile_h_Reg.write(input_h_Reg.read() - h_Reg.read() - tile_overlap_Reg.read());

            src.write(DRAM_INPUT_BASE + c_Reg.read() * INPUT_H * (INPUT_W + ROW_PADD) +
                        (h_Reg.read() + row_sel_Reg.read()) * (INPUT_W + ROW_PADD) + w_Reg.read());

            //DMA stall
            if (proc_ctrl_Reg[row_sel_Reg.read()].read())
            {
                dma_read_state = DMA_INPUT_CONFIG;
                DMA_start.write(0);
            }
            else
            {
                dma_read_state = DMA_READ_TO_ISRAM;
                DMA_start.write(1);
            }
            
        }break;

        case DMA_READ_TO_ISRAM : {

            DMA_start.write(0);

            if (DMA_irt.read())
            {
                if (!align_Reg.read() || 
                    (align_Reg.read() && (store_count_Reg.read() == length.read() - 1)) )
                {
                    dma_read_state = DMA_INPUT_CONFIG;
                    DMA_irtclr.write(1);
                    dma_count_Reg.write(0);
                    proc_ctrl_Reg[row_sel_Reg.read()].write(1);

                    //address control
                    do_Input_addrCtrl();                    
                }
            } 

            int bank = row_sel_Reg.read() % ISRAM_BANK_NUM;
            int shift = w_Reg.read() % ISRAM_bytes;

            if (!align_Reg.read())
            {
                if (dram_done.read())
                {
                    if (!align_Reg.read()) //alignment
                    {
                        I_CS[bank].write(1);
                        I_WEB[bank].write(1);
                        I_addr[bank].write(dma_count_Reg.read() % ((tile_w_Reg.read() + 3) / 4));
                        I_data_i[bank].write(read_data.read());      
                    }   
                    dma_count_Reg.write(dma_count_Reg.read() + 1);
                }   
            }
            else //unalignment, get 4 bytes in align data register
            // first cycle  2 3 _ _ _ _ _
            // second cycle 2 3 4 5 6 7 _
            // third cycle  6 7 8 9 10 11 _
            {
                if ( dram_done.read() || (dma_count_Reg == length.read() && store_count_Reg.read() < length.read() - 1) )
                {
                    if (dram_done.read())
                        dma_count_Reg.write(dma_count_Reg.read() + 1);   

                    if (dma_count_Reg.read() == 0)
                    {
                        for (int i = 0; i < shift; i++)
                            align_data_Reg[shift - i - 1].write(read_data.read().range((i + 1) * DATA_WIDTH - 1,i * DATA_WIDTH));
                    }
                    else if (dma_count_Reg.read() == 1)
                    {
                        for (int i = 0; i < ISRAM_bytes; i++)
                            align_data_Reg[shift + ISRAM_bytes - i - 1].write(read_data.read().range( (i + 1) * DATA_WIDTH - 1, i * DATA_WIDTH));                      
                    }
                    else if (dma_count_Reg.read() >= 2)
                    {
                        sc_int<32> concate = (align_data_Reg[0].read(), align_data_Reg[1].read(), align_data_Reg[2].read(), align_data_Reg[3].read());
                        for (int i = 0; i < ISRAM_bytes; i++)
                            align_data_Reg[i] = align_data_Reg[i + ISRAM_bytes]; 
                        for (int i = 0; i < ISRAM_bytes; i++)
                            align_data_Reg[shift + ISRAM_bytes - i - 1].write(read_data.read().range( (i + 1) * DATA_WIDTH - 1, i * DATA_WIDTH));   
                        I_CS[bank].write(1);
                        I_WEB[bank].write(1);
                        I_addr[bank].write( (dma_count_Reg.read() - 2) % ((tile_w_Reg.read() + 3) / 4));
                        I_data_i[bank].write(concate);            
                        store_count_Reg.write(store_count_Reg.read() + 1);          
                    }
                }
                
            }  
        }break;    
    }
}

void Controller::do_Output_addrCtrl()
{
    if (DMA_irt.read() && dma_count_Reg.read() == out_length )
        row_sel_Reg.write(0);
    else 
    {
        row_sel_Reg.write(row_sel_Reg.read() + 1);
        cout << endl << "////////////////////////////////////////////////" << endl;
        cout << "////////// Write Address Control Info////////////" << endl;
        cout << "////////////////////////////////////////////////" << endl;
        cout << "ofmap_h_Reg : " << ofmap_h_Reg.read() << endl;
        cout << "ofmap_w_Reg : " << ofmap_w_Reg.read() << endl;
        cout << "ofmap_c_Reg : " << ofmap_c_Reg.read() << endl;
        cout << "row_sel_Reg : " << row_sel_Reg.read() << endl;
        cout << "///////////DMA WRITE OUTPUT///////////" << endl;
        cout << "tile_h : " << tile_h_Reg.read() - f_size_Reg.read() + 1 << endl;
        cout << "tile_w : " << tile_w_Reg.read() - f_size_Reg.read() + 1  << endl;
        cout << "==================================================================" << endl;
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
            //if (dma_count_Reg.read() == 0)
            dma_count_Reg.write(dma_count_Reg.read() + 1);
            osram_id_Reg.write(0);

            tgt.write(DRAM_OUTPUT_BASE + (ofmap_c_Reg.read() * out_h_Reg.read() * out_w_Reg.read() +
                    (ofmap_h_Reg.read() + row_sel_Reg.read()) * out_w_Reg.read() + ofmap_w_Reg.read()) * 4);
            length.write(tile_w_Reg.read() - f_size_Reg.read() + 1);  
            DMA_start.write(1);

            for (int i = 0; i < OSRAM_NUM; i++)
            {
                O_CS[i] = 1;
                read_write = 1;
                O_addr_r[i] =  dma_count_Reg.read();
                osram_req = 1;
            }
        }break;
    
        case DMA_WRITE_TO_DRAM : {

            DMA_start.write(0);
            if (DMA_irt.read())
            {
                dma_write_state = DMA_OUTPUT_CONFIG;
                DMA_irtclr.write(1);
                if (dma_count_Reg.read() == out_length)
                    dma_count_Reg.write(0);
                do_Output_addrCtrl();
            }
            else
            {

                if (!dma_resp.read())
                    osram_valid.write(0);

                if (osram_req == 1)
                {
                    osram_req = 0;
                    osram_valid.write(1);
                }
                if (dram_done.read())
                {
                    dma_count_Reg.write(dma_count_Reg.read() + 1);
                    for (int i = 0; i < OSRAM_NUM; i++)
                    {
                            O_CS[i] = 1;
                            read_write = 1;
                            O_addr_r[i] =  dma_count_Reg.read();
                            osram_req = 1;
                            osram_valid.write(0);
                    }
                }

            }
            //cout << "DMA_WRITE_TO_DRAM" << endl;


        }break;
    }

}

/////////////////////////PE Controller//////////////////////////////
void Controller::do_startPE()
{

    //out_length = (tile_h_Reg.read() - f_size_Reg.read() + 1) * (tile_w_Reg.read() - f_size_Reg.read() + 1);

    switch(pe_state.read())
    {
        case READ_WSRAM:{

            pe_state = READ_ISRAM;

            //Read Weight SRAM data to PE weight reg
            for (int i = 0; i < WSRAM_NUM; i++)
            {
                W_CS[i].write(1);
                W_WEB[i].write(0);
                W_addr[i].write(wbank_addr_Reg.read());
            }     

        }break;

        case READ_ISRAM:{
            //check proc_status
            pe_stall = 0;

            for (int i = ibank_ctrl_Reg.read(); i < ibank_ctrl_Reg.read() + f_size_Reg.read(); i++)
                if (proc_ctrl_Reg[i] == 0)
                    pe_stall = 1;

            if (pe_stall)   //wait for DMA read
                pe_state = READ_ISRAM;
            else 
            {
                pe_state = ISRAM_TO_REG;

                //next cycle can get isram data
                for (int i = ibank_ctrl_Reg.read(); i < ibank_ctrl_Reg.read() + f_size_Reg.read(); i++)
                {
                    I_CS[i].write(1);
                    I_WEB[i].write(0);
                    I_addr[i].write(0);
                }
            }

            ibank_addr_Reg.write(0);

            //pratial sum control
            // if (c_Reg.read() > 0 && c_Reg.read() != input_c_Reg.read())
            //     add_prev.write(1);
            // else 
            //     add_prev.write(0);
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

            ibank_addr_Reg.write(ibank_addr_Reg.read() + 1);

        }break;

        case PE_START:{

            //filter size = 3
            if (f_size_Reg.read() == 3)
            {
                //select reg array output 
                for (int i = 0; i < FILTER_SIZE; i++)
                    for (int j = 0; j < REG_ROW_NUM; j++)
                        if (j < FILTER_SIZE)
                            reg_mux_sel[i * REG_ROW_NUM + j].write(1);
                        else    
                            reg_mux_sel[i * REG_ROW_NUM + j].write(0);

                //next cycle reg array left shift
                for (int i = 0; i < FILTER_SIZE; i++)
                    shift[i].write(1);
                shift_count_Reg.write(shift_count_Reg.read() + 1);

                //ISRAM output data
                for (int i = ibank_ctrl_Reg.read(); i < ibank_ctrl_Reg.read() + f_size_Reg.read(); i++)
                {
                    I_CS[i].write(1);
                    I_WEB[i].write(0);
                    I_addr[i].write(ibank_addr_Reg.read());
                }

                //Read data state control
                if ((shift_count_Reg.read() > tile_w_Reg.read() - f_size_Reg.read() + 1))
                    read_data_state = FSIZE_3_CASE1;
                else if (read_data_state.read() < FSIZE_3_CASE4)
                    read_data_state.write(read_data_state.read() + 1);
                else 
                    read_data_state = FSIZE_3_CASE1;

                //Ouput SRAM control 
                if ( shift_count_Reg.read() < tile_w_Reg.read() - 1)
                {
                    if (shift_count_Reg.read() == 0)
                    {
                        for (int i = 0; i < OSRAM_NUM; i++)
                        {
                            O_CS[i].write(1);
                            O_addr_r[i].write(osram_addr_Reg.read());
                        }         
                        read_write.write(O_READ);               
                    }
                    else   //wait for pe generate output data
                    {
                        for (int i = 0; i < OSRAM_NUM; i++)
                        {
                            O_CS[i].write(1);
                            O_addr_w[i].write(osram_addr_Reg.read());
                            O_addr_r[i].write(osram_addr_Reg.read() + 1);
                        }
                        read_write.write(O_READ_WRITE);
                        // if (osram_addr_Reg.read() > (tile_h_Reg.read() * tile_w_Reg.read()))
                        // {
                        //     osram_addr_Reg.write(0);
                        // }
                        // else
                        osram_addr_Reg.write(osram_addr_Reg.read() + 1);
                    }
                }
                else
                {
                    //read_write.write(O_READ);
                    for (int i = 0; i < OSRAM_NUM; i++)
                        O_CS[i].write(0);

                    if (ibank_ctrl_Reg.read() == tile_h_Reg.read() - f_size_Reg.read())
                    {
                        for (int i = ibank_ctrl_Reg.read(); i < ibank_ctrl_Reg.read() + f_size_Reg.read(); i++)
                            proc_ctrl_Reg[i].write(0);
                        ibank_ctrl_Reg.write(0);
                    }
                    else
                    { 
                        proc_ctrl_Reg[ibank_ctrl_Reg.read()].write(0);
                        ibank_ctrl_Reg.write(ibank_ctrl_Reg.read() + 1);   
                    }

                    if (osram_addr_Reg.read() == out_length)
                    {
                        osram_addr_Reg.write(0);
                        wbank_addr_Reg.write(wbank_addr_Reg.read() + 1);
                        pe_state = READ_WSRAM;
                        add_prev.write(1);
                    }
                    else 
                        pe_state = READ_ISRAM;

                    shift_count_Reg.write(0);
                }        

                switch(read_data_state.read())
                {
                    case FSIZE_3_CASE1 : {

                        ibank_addr_Reg.write(ibank_addr_Reg.read() + 1);

                        // store_col_index.write(0);
                        for (int i = 0; i < REG_NUM; i++)
                            store_reg[i].write(0);            

                    }break;

                    case FSIZE_3_CASE2 :

                        store_col_index.write(2);
                        for(int i = 0; i < REG_NUM; i++)
                        {
                            if ( (i % 6)  < (REG_ROW_NUM - ISRAM_bytes))
                                store_reg[i].write(0);
                            else   
                                store_reg[i].write(1);
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

        
        switch(state.read())
        {    
            case ACC_IDLE:{

                if (start.read())
                {
                    cout << "start recieve time : " << sc_time_stamp() << endl; 
                    state = ACC_DMA_READ;
                }

            }break;

            case ACC_DMA_READ:{

                if ( (row_sel_Reg.read() == f_size_Reg.read() || row_sel_Reg.read() == 0 && c_Reg.read() > 0)
                    && DMA_irt.read())   //data ready
                    state = ACC_START_PE;

                do_dmaRead();   //Read weight and input
                
            }break;

            case ACC_START_PE:{

                do_startPE();

                //ping pong Input SRAM control
                if (/*h_Reg.read() != tile_h_Reg.read() &&*/ c_Reg != input_c_Reg.read())
                    do_dmaRead(); 

                //wait for next tile move to ISRAM
                // if ( (c_Reg.read() == 0) && (h_Reg.read() == input_h_Reg.read()) && DMA_irt.read())
                //     state = ACC_DMA_WRITE;

                out_length = (tile_h_Reg.read() - f_size_Reg.read() + 1) * (tile_w_Reg.read() - f_size_Reg.read() + 1);

                if ( (c_Reg.read() == input_c_Reg.read()) &&  (osram_addr_Reg.read() == out_length))
                {
                    state = ACC_DMA_WRITE;
                    wbank_addr_Reg.write(0);
                    dma_count_Reg.write(0);
                    //do_Input_addrCtrl();
                }
            }break;

            case ACC_DMA_WRITE: {
                
                out_h_Reg = INPUT_H - f_size_Reg.read() + 1;
                out_w_Reg = INPUT_W - f_size_Reg.read() + 1;
                out_c_Reg = f_num_Reg.read();

                //out_length = (tile_h_Reg.read() - f_size_Reg.read() + 1) * (tile_w_Reg.read() - f_size_Reg.read() + 1);
                do_dmaWrite();
                if (DMA_irt.read() && dma_count_Reg.read() == out_length )
                {
                    do_Input_addrCtrl();
                    if ( (c_Reg.read() == input_c_Reg.read()) && 
                          (h_Reg.read() + tile_h_Reg.read()== input_h_Reg.read()) &&
                          (w_Reg.read() + tile_w_Reg.read() == input_w_Reg.read()) )
                        state = ACC_FINISH;
                    else 
                    {
                        state = ACC_DMA_READ;
                        do_Output_addrCtrl();
                    }
                }

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






