#include "DMAC.h" 

void DMAC::do_reset()
{
    state = DMA_IDLE;
    src_Reg = 0;
    tgt_Reg = 0;
    length_Reg = 0;

    count_Reg = 0;
    done = 0;
    dram_done = 0;
}

void DMAC::do_DMAC(){

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
                dram_done.write(0);

            }break;

            case DMA_READ:{

                //cout << "////////////DMA_READ///////////////" << " time : " << sc_time_stamp() << endl; 
                dram_done.write(0);
                state = WAIT_DRAM_RESP;
                transport(0, src_Reg.read() + (count_Reg.read() * 4), data_buffer , 4 );
                count_Reg.write(count_Reg.read() + 1);

            }break;

            case DMA_WRITE:{

                dram_done.write(0);
                if (osram_valid.read())
                {
                    state = WAIT_DRAM_RESP;
                    dma_resp.write(1);
                    data_buffer[0] = osram_data[osram_id.read()].read();
                    transport(1, tgt_Reg.read() + (count_Reg.read() * 4), data_buffer , 4 );
                    count_Reg.write(count_Reg.read() + 1);
                }

            }break;

            case WAIT_DRAM_RESP:{
                
                //if (!osram_valid.read())
                    dma_resp.write(0);
                
                if (done)
                {
                    if (dma_type.read() == DMA_READ)
                    {
                        dram_done.write(1);
                        read_data.write(data_buffer[0]);
                        if (count_Reg.read() < length_Reg.read()) 
                        {
                            state = DMA_READ;
                        }
                        else
                        {
                            state = DMA_IRQ;
                            DMA_irt.write(1);
                        }
                    }
                    else
                    {
                        dram_done.write(1);
                        if (count_Reg.read() < length_Reg.read()) 
                        {
                            state = DMA_WRITE;
                        }
                        else
                        {
                            state = DMA_IRQ;
                            DMA_irt.write(1);
                        } 
                    }
                }

            }break; 

            case DMA_IRQ:{

                dram_done.write(0);
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

void DMAC::transport(bool rw, uint32_t addr, int32_t* data, uint32_t length){

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
    //cout << "SEND_REQ"<<"+++++++" << sc_time_stamp() << "+++++++" << endl;
    tlm::tlm_sync_enum status;
    
    status = socket->nb_transport_fw( *trans, phase, delay );

}

tlm::tlm_sync_enum DMAC::nb_transport_bw ( 
    tlm::tlm_generic_payload& trans,
    tlm::tlm_phase& phase, sc_time& delay)
    {
        
        m_peq.notify( trans, phase, delay );
        
        return tlm::TLM_ACCEPTED;
    
    }

void DMAC::peq_cb(tlm::tlm_generic_payload& trans, const tlm::tlm_phase& phase){

    tlm::tlm_sync_enum status;
    sc_time delay;
    delay = sc_time(0, SC_PS); // Accept delay
    switch (phase) {

        case tlm::END_REQ:
           
            //cout << "END_REQ"<<"++++" << sc_time_stamp() << "++++" << endl;
            
            break;
        case tlm::BEGIN_RESP:{
                request_in_progress = 0;
                end_request_event.notify();
                trans.release();

                tlm::tlm_phase fw_phase = tlm::END_RESP;
                sc_time delay = sc_time(0, SC_PS);
                status = socket->nb_transport_fw( trans, fw_phase, delay );

                //cout << "REC_RESP"<<"+++++++" << sc_time_stamp() << "+++++++" << endl;
                done = 1;
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