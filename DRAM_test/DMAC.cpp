#include "DMAC.h"

void DMAC::do_reset()
{
    state = DMA_IDLE;
    src_Reg = 0;
    tgt_Reg = 0;
    length_Reg = 0;

    count_Reg = 0;
    // f_size_Reg = 0;
    // f_channel_Reg = 0;
    // f_num_Reg = 0;
    // weight_base_Reg = 0;
    // cur_f_channel = 0;
    // cur_f_num = 0;
    // counter = 0;
}

void DMAC::do_DMAC()
{
    do_reset();

    while(1)
    {
        wait();

        tlm::tlm_generic_payload* trans_m = new tlm::tlm_generic_payload;
        sc_time delay = sc_time(0,SC_NS);

        // if (write_config.read())
        // {
        //     f_size_Reg = f_size.read();
        //     f_channel_Reg = f_channel.read();
        //     f_num_Reg = f_num.read();
        //     weight_base_Reg = weight_base.read();
        // }
        // cout << sc_time_stamp() << " ";
        cout << "DMA_state : " << state << endl;

        //////Finite State Machine//////
        switch (state.read())
        {

            case DMA_IDLE:                
                do_reset();
                if (DMA_start.read())
                {
                    src_Reg.write(src);
                    tgt_Reg.write(tgt);
                    length_Reg.write(length);
                    state = DMA_READ;
                }
                data_valid.write(0);
            break;
            
            case DMA_READ:  //Read to Input SRAM or Weight SRAM
                //Read Input data
                if (count_Reg.read() < length_Reg.read())     //Read DRAM
                {
                    if (count_Reg.read() == length_Reg.read() - 1)
                        state.write(DMA_IDLE);
            
                    trans_m->set_command( tlm::TLM_READ_COMMAND );
                    trans_m->set_address( src_Reg.read() + (count_Reg.read() * 4) );
                    trans_m->set_data_ptr(
                        reinterpret_cast<unsigned char*>(&data_m) );
                    trans_m->set_data_length(4);
                    trans_m->set_streaming_width(4);
                    trans_m->set_byte_enable_ptr(0);
                    trans_m->set_dmi_allowed(false);
                    trans_m->set_response_status(
                        tlm::TLM_INCOMPLETE_RESPONSE );
                    socket_m->b_transport( *trans_m, delay ); 
                    read_data.write(data_m); 
                    data_valid.write(1);
                    cout << sc_time_stamp() << "data : " << data_m << endl;
                    count_Reg.write(count_Reg.read() + 1);
                }

                //Read Weight

                // if (f_size_Reg.read() == 3)    //filter 3x3
                // {
                //     if (cur_f_channel.read() <= 128 && cur_f_num.read() <= 32)
                //     {
                //         size = f_size_Reg.read() * f_size_Reg.read() * (f_channel_Reg.read() - cur_f_channel.read()) * (f_num_Reg.read() - cur_f_num.read());
                //         cout << "counter : " << counter.read() << " size : " << size.read() << endl;
                //         if (counter < size)     //Read DRAM
                //         {
                //             trans_m->set_command( tlm::TLM_READ_COMMAND );
                //             trans_m->set_address( weight_base_Reg.read() + (counter.read() * 4) );
                //             trans_m->set_data_ptr(
                //                 reinterpret_cast<unsigned char*>(&data_m) );
                //             trans_m->set_data_length(4);
                //             trans_m->set_streaming_width(4);
                //             trans_m->set_byte_enable_ptr(0);
                //             trans_m->set_dmi_allowed(false);
                //             trans_m->set_response_status(
                //                 tlm::TLM_INCOMPLETE_RESPONSE );
                //             socket_m->b_transport( *trans_m, delay ); 
                //             data.write(data_m);
                //         }
                //         if (counter.read() > 0)        //Write to SRAM
                //         {
                //             cout << "DMA receive data [" << counter.read()-1 << "] : " << data.read() << endl;
                //             W_CS.write(1);
                //             W_WEB.write(1);
                //             W_addr.write(((counter.read()-4)) / 128);
                //             W_length.write(4);
                //             W_weight.write(data_m);
                //             W_bank_sel.write(((counter.read()-4)) % 128);    
                //             if (counter.read() >= size.read())
                //             {
                //                 weight_base_Reg = /*weight_base_Reg + */size;
                //                 state = DMA_IDLE;
                //             }
                //         }
                //         counter.write(counter.read() + 1);
                //     }
                // }

                //Read feature map
            break;

            case DMA_WRITE:

            break;

            default:
                break;
        }
        printf("////////////////////////////////////////\n");

        
    }








}