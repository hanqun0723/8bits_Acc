#include "DMAC.h" 

void DMAC::do_reset()
{
    state = DMA_IDLE;
    src_Reg = 0;
    tgt_Reg = 0;
    length_Reg = 0;

    count_Reg = 0;
    dram_done = 0;
}

void DMA::do_DMAC(){

    do_reset();

    while(1){

        wait();

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

            case DMA_READ:{

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
                    // data_valid.write(1);
                    valid_flag = 1;
                    count_Reg.write(count_Reg.read() + 1);
                }
                else 
                {
                    state.write(DMA_IRQ);            
                    data_valid.write(1);
                    DMA_irt.write(1);       
                }

                // state = WAIT_DRAM_READ;
                // dram_done = 0;
                // transport(0, src, data_buffer , 4 );
                // cout << "trasport src : " << src << endl;
            }break;


            case WAIT_DRAM_READ:{

                if (dram_done)
                {
                    cout << "DRAM_done" << endl;
                    cout << "data_buffer : " << data_buffer[0] << endl;
                    cout << "================" << endl;
                    state = DMA_READ;
                    src = 4;
                }

            }break;

            case DMA_WRITE:{

                //cout << "DMA_WRITE" << endl;
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
                else
                {
                    state.write(DMA_IRQ);            
                    DMA_irt.write(1);
                }

            }break;

            case DMA_IRQ:{

                data_valid.write(0);
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

    }
    
}

void DMA::transport(bool rw, uint32_t addr, uint32_t* data, uint32_t length){

    tlm::tlm_phase phase;
    tlm::tlm_generic_payload* trans;
    tlm::tlm_command cmd;
    sc_time delay;
    if(rw==1){
        cmd = tlm::TLM_WRITE_COMMAND; 
    }
    else{
        cmd = tlm::TLM_READ_COMMAND; 
    }

    trans = m_mm.allocate();
    trans->acquire();

    trans->set_command( cmd );
    trans->set_address( addr );
    trans->set_data_ptr( reinterpret_cast<unsigned char*> (data) );
    trans->set_data_length( length ); 
    trans->set_streaming_width( length ); 
    trans->set_byte_enable_ptr( 0 ); 
    trans->set_dmi_allowed( false ); 
    trans->set_response_status( tlm::TLM_INCOMPLETE_RESPONSE );

    if (request_in_progress){
        wait(end_request_event);          
    }
    request_in_progress = trans;

    phase = tlm::BEGIN_REQ;
    delay = sc_time(0, SC_PS);
    cout << "SEND_REQ"<<"+++++++" << sc_time_stamp() << "+++++++" << endl;
    tlm::tlm_sync_enum status;
    
    status = socket->nb_transport_fw( *trans, phase, delay );

}

tlm::tlm_sync_enum DMA::nb_transport_bw ( 
    tlm::tlm_generic_payload& trans,
    tlm::tlm_phase& phase, sc_time& delay)
    {
        
        m_peq.notify( trans, phase, delay );
        
        return tlm::TLM_ACCEPTED;
    
    }

void DMA::peq_cb(tlm::tlm_generic_payload& trans, const tlm::tlm_phase& phase){

    tlm::tlm_sync_enum status;
    sc_time delay;
    delay = sc_time(0, SC_PS); // Accept delay
    switch (phase) {

        case tlm::END_REQ:
           
            cout << "END_REQ"<<"++++" << sc_time_stamp() << "++++" << endl;
            
            break;
        case tlm::BEGIN_RESP:{
                request_in_progress = 0;
                end_request_event.notify();
                trans.release();

                tlm::tlm_phase fw_phase = tlm::END_RESP;
                sc_time delay = sc_time(0, SC_PS);
                status = socket->nb_transport_fw( trans, fw_phase, delay );

                cout << "REC_RESP"<<"+++++++" << sc_time_stamp() << "+++++++" << endl;
                dram_done = 1;
                //k++;
                //cout << " DMA_RESP" <<k<<endl;
        } break;
        case tlm::BEGIN_REQ :
                SC_REPORT_FATAL("TLM-2", "Illegal transaction phase received by initiator");
            break;
        case tlm::END_RESP:
                SC_REPORT_FATAL("TLM-2", "Illegal transaction phase received by initiator");
            break;

        default:
            break;
               
    }
}