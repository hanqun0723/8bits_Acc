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

        //////Finite State Machine//////
        switch (state.read())
        {

            case DMA_IDLE:{      
                do_reset();
                if (DMA_start.read())
                {
                    if (dma_type.read() == DMA_READ)
                    {
                        src_Reg.write(src);
                        length_Reg.write(length);
                        state = DMA_READ;
                    }
                    else if (dma_type.read() == DMA_WRITE)
                    {
                        tgt_Reg.write(tgt);
                        length_Reg.write(length);
                        state = DMA_WRITE;                        
                    }
                }
                data_valid.write(0);
            }break;
            
            case DMA_READ:{  //Read to Input SRAM or Weight SRAM

                if (count_Reg.read() == length_Reg.read())
                {
                    state.write(DMA_IRQ);            
                    data_valid.write(0);
                    DMA_irt.write(1);
                }

                //Read Input data
                if (count_Reg.read() < length_Reg.read())     //Read DRAM
                {
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
                    count_Reg.write(count_Reg.read() + 1);
                }
            }break;

            case DMA_WRITE:{

                //cout << "DMA_WRITE" << endl;
                
                if (count_Reg.read() == length_Reg.read())
                {
                    state.write(DMA_IRQ);            
                    DMA_irt.write(1);
                }

                if (count_Reg.read() < length_Reg.read())     //Read DRAM
                {
                    data_m = osram_data[osram_id.read()].read();
                    trans_m->set_command( tlm::TLM_WRITE_COMMAND );
                    trans_m->set_address( tgt_Reg.read() + (count_Reg.read() * 4) );
                    trans_m->set_data_ptr(
                        reinterpret_cast<unsigned char*>(&data_m) );
                    trans_m->set_data_length(4);
                    trans_m->set_streaming_width(4);
                    trans_m->set_byte_enable_ptr(0);
                    trans_m->set_dmi_allowed(false);
                    trans_m->set_response_status(
                        tlm::TLM_INCOMPLETE_RESPONSE );
                    socket_m->b_transport( *trans_m, delay ); 
                    count_Reg.write(count_Reg.read() + 1);
                }

            }break;

            case DMA_IRQ:{

                if (DMA_irtclr.read())
                {
                    state = DMA_IDLE;
                    do_reset();
                    DMA_irt.write(0);
                }

            }break;
            default:
                break;
        }
        //printf("////////////////////////////////////////\n");

        
    }








}