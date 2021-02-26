#include "DMA.h"

/////////////////   DMA is slave   ///////////////
void DMA::b_transport( tlm::tlm_generic_payload& trans, sc_time& delay ){	
		
    tlm::tlm_command cmd_s = trans.get_command();
    sc_dt::uint64 addr_s = trans.get_address();
    unsigned char* data_s = trans.get_data_ptr();

    if (cmd_s ==tlm::TLM_WRITE_COMMAND) {
        in_address = addr_s;
        in_data = *(reinterpret_cast<int*>(data_s));
    }
    //cout << "in_address : " << in_address << "\n";
    //cout << "in_data : " << in_data << "\n\n";
    wait(10,SC_NS);		
    trans.set_response_status( tlm::TLM_OK_RESPONSE);		
}


void DMA::dma(){

//reset
    //cout << "DMA reset\n\n";
    //in_data = 0;
    src.write(0);
    tgt.write(0);
    size.write(0);
    start.write(0);
    Interrupt.write(0);

    while(1){
        tlm::tlm_generic_payload* trans_m = new tlm::tlm_generic_payload;
        sc_time delay = sc_time(0,SC_NS);

        if(start.read() == 0){
            switch(in_address){
            case 0x0:
                //cout << "dma src write" << "\n";
                //cout << "in_data : " << in_data << "\n\n";
                src.write(in_address);
                tmp_src.write(in_data);
                break;

            case 0x4:
                //cout << "dma tgt write" << "\n";
                //cout << "in_data : " << in_data << "\n\n";
                tgt.write(in_address);
                tmp_tgt.write(in_data);
                break;

            case 0x8:
                //cout << "dma size write" << "\n";
                //cout << "in_data : " << in_data << "\n\n";
                size.write(in_address);
                tmp_size.write(in_data);
                break;

            case 0xC:
                //cout << "dma start write" << "\n";
                //cout << "in_data : " << in_data << "\n\n";
                start.write(in_data);
                break;

            default:
                break;
            }        
        }

        if(start.read() == 1 && tmp_size.read() != 0){
            state = DMA_READ;
            while(state < DMA_FINISH){
                if(state == DMA_READ){
                    trans_m->set_command( tlm::TLM_READ_COMMAND );
                    trans_m->set_address( tmp_src.read() );
                    trans_m->set_data_ptr(
                        reinterpret_cast<unsigned char*>(&data_m) );
                    trans_m->set_data_length(4);
                    trans_m->set_streaming_width(4);
                    trans_m->set_byte_enable_ptr(0);
                    trans_m->set_dmi_allowed(false);
                    trans_m->set_response_status(
                        tlm::TLM_INCOMPLETE_RESPONSE );
                    socket_m->b_transport( *trans_m, delay );
                }
                else if(state == DMA_WRITE){
                    trans_m->set_command( tlm::TLM_WRITE_COMMAND );
                    trans_m->set_address( tmp_tgt.read() );
                    trans_m->set_data_ptr(
                        reinterpret_cast<unsigned char*>(&data_m) );
                    trans_m->set_data_length(4);
                    trans_m->set_streaming_width(4);
                    trans_m->set_byte_enable_ptr(0);
                    trans_m->set_dmi_allowed(false);
                    trans_m->set_response_status(
                        tlm::TLM_INCOMPLETE_RESPONSE );
                    socket_m->b_transport( *trans_m, delay );     
                }
                tmp_size.write(tmp_size.read() - 1);
                state++;
            }
            tmp_src.write(tmp_src.read() + 4);
            tmp_tgt.write(tmp_tgt.read() + 4);
            //cout << tmp_size.read() << "\n";
        }

        if(start.read() == 1 && tmp_size.read() == 0){
            wait(10);
	    Interrupt.write(1);
	    wait(8);
	    Interrupt.write(0);
	}
	wait();
    }
}
