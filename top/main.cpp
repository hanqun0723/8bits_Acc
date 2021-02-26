#include "DNNAcc.h"
#include "systemc.h"
#include "Acc_cofig.h"
#include "string.h"
using namespace std;

int sc_main(int argc, char *argv[]){

    sc_clock clk("clk", 10);
    sc_signal<bool> rst;

    // sc_signal<bool>     I_CS[ISRAM_BANK_NUM];
    // sc_signal<bool>     I_WEB[ISRAM_BANK_NUM];
    // sc_signal<sc_uint<ISRAM_ADDR_LEN> >  I_addr[ISRAM_BANK_NUM];
    sc_signal<sc_int<ISRAM_DATA_WIDTH> > I_data_i[ISRAM_BANK_NUM];
    // sc_signal<bool>     bus_mux_sel[ISRAM_BANK_NUM * ISRAM_bytes];
    // sc_signal<bool>        store_reg;  
    // sc_signal<bool>        shift[FILTER_SIZE];  
    //sc_signal<bool>        reg_mux_sel[REG_NUM];   
    // sc_signal<bool>     W_CS[WSRAM_NUM];
    // sc_signal<bool>     W_WEB[WSRAM_NUM];
    // sc_signal<sc_uint<WSRAM_ADDR_LEN> >  W_addr[WSRAM_NUM];
    sc_signal<sc_uint<3> >    W_length[WSRAM_NUM];           
    sc_signal<sc_uint<WSRAM_BANK_BITS> >     W_bank_sel[WSRAM_NUM];        
    sc_signal<sc_int<BUS_WIDTH> > W_data_i[WSRAM_NUM];
    sc_signal<bool>       weight_valid[PE_NUM];
    sc_signal<bool>       last_channel[PE_NUM];
    sc_signal<bool>           add_prev[PE_NUM];         
    sc_signal<sc_int<PSUM_WIDTH> >    prev_psum[PE_NUM];
    // sc_signal<sc_uint<2> >     read_write[OSRAM_NUM];  	
    // sc_signal<bool>            O_WEB[OSRAM_NUM];             
    // sc_signal<sc_uint<OSRAM_ADDR_LEN> > O_addr_wr[OSRAM_NUM];    
    // sc_signal<sc_uint<OSRAM_ADDR_LEN> > O_addr_r[OSRAM_NUM];  
    sc_signal<bool> start;

    DNNAcc dnnacc("dnnacc");

    dnnacc.clk(clk);
    dnnacc.rst(rst);
    for(int i = 0; i < ISRAM_BANK_NUM; i++)
    {
        // dnnacc.I_CS[i](I_CS[i]);
        // dnnacc.I_WEB[i](I_WEB[i]);
        // dnnacc.I_addr[i](I_addr[i]);
        dnnacc.I_data_i[i](I_data_i[i]);
    }
    // for(int i = 0; i < ISRAM_BANK_NUM * ISRAM_bytes; i++)
    //     dnnacc.bus_mux_sel[i](bus_mux_sel[i]); 

    // dnnacc.store_reg(store_reg);
    // for(int i = 0; i < FILTER_SIZE; i++)
    //     dnnacc.shift[i](shift[i]);
    // for(int i = 0; i < REG_NUM; i++)
    //     dnnacc.reg_mux_sel[i](reg_mux_sel[i]);

    for(int i = 0; i < WSRAM_NUM; i++)
    {
        // dnnacc.W_CS[i](W_CS[i]);
        // dnnacc.W_WEB[i](W_WEB[i]);
        // dnnacc.W_addr[i](W_addr[i]);
        dnnacc.W_length[i](W_length[i]);
        dnnacc.W_data_i[i](W_data_i[i]);
        dnnacc.W_bank_sel[i](W_bank_sel[i]);
    }     
    for(int i = 0; i < PE_NUM; i++)
    {
        dnnacc.weight_valid[i](weight_valid[i]);
        dnnacc.last_channel[i](last_channel[i]);
        dnnacc.add_prev[i](add_prev[i]); 
        dnnacc.prev_psum[i](prev_psum[i]);
    }
    // for(int i = 0; i < OSRAM_NUM; i++)
    // {
    //     dnnacc.read_write[i](read_write[i]);
    //     dnnacc.O_WEB[i](O_WEB[i]);
    //     dnnacc.O_addr_wr[i](O_addr_wr[i]);
    //     dnnacc.O_addr_r[i](O_addr_r[i]);
    // }
    dnnacc.start(start);

    sc_trace_file *tf = sc_create_vcd_trace_file("RESULT");


    //add trace signal
    //sc_trace(tf, iSRAM_bank.clk       ,"iSRAM_bank.clk");
    sc_trace(tf, dnnacc.clk, "dnnacc.clk");
    sc_trace(tf, dnnacc.rst, "dnnacc.rst");

    for(int i = 0; i < ISRAM_BANK_NUM; i++)
    {
        sc_trace(tf, dnnacc.I_CS[i], "dnnacc.I_CS("+to_string(i)+")");
        sc_trace(tf, dnnacc.I_WEB[i], "dnnacc.I_WEB("+to_string(i)+")");
        sc_trace(tf, dnnacc.I_addr[i], "dnnacc.I_addr("+to_string(i)+")");
        sc_trace(tf, dnnacc.I_data_i[i], "dnnacc.I_data_i("+to_string(i)+")");
    }

    for(int i = 0; i < ISRAM_BANK_NUM; i++)
    {
        sc_trace(tf, dnnacc.ibank[i].data[0], "dnnacc.ibank("+to_string(i)+").data("+to_string(0)+")");
        sc_trace(tf, dnnacc.ibank[i].data_o, "dnnacc.ibank("+to_string(i)+").data_o");
    }
    sc_trace(tf, dnnacc.controller.state, "dnnacc.controller.state");

    for(int i = 0; i < MAC_NUM; i++)
    {
        sc_trace(tf, dnnacc.pe[0].input[i], "input("+to_string(i)+")");
        sc_trace(tf, dnnacc.pe[0].weight[i], "weight("+to_string(i)+")");
        sc_trace(tf, dnnacc.pe[0].result[i], "result("+to_string(i)+")");
    }
        sc_trace(tf, dnnacc.pe[0].data_out, "pe_data_out");

    sc_trace(tf, dnnacc.osram[0].CS, "osram.osram_cs");
    sc_trace(tf, dnnacc.osram[0].read_write, "osram.osram_read_write");
    sc_trace(tf, dnnacc.osram[0].data_i, "osram.osram_data_i");
    sc_trace(tf, dnnacc.osram[0].O_SRAMdata[0], "osram.o_sramdata[0]");
    for (int i = 0; i < REG_NUM; i++)
    {
        sc_trace(tf, dnnacc.regarray.reg[i], "reg"+to_string(i)+")");
    }
    for (int i = 0; i < ISRAM_BANK_NUM * ISRAM_bytes; i++)
    {
        sc_trace(tf, dnnacc.controller.bus_mux_sel[i], "bus_mux_sel"+to_string(i)+")");
    }    

    for (int i =0; i < REG_NUM; i++)
        sc_trace(tf, dnnacc.controller.store_reg[i], "store_reg"+to_string(i)+")");

    sc_trace(tf, dnnacc.controller.ibank_addr_Reg, "ibank_addr_Reg");
    sc_trace(tf, dnnacc.controller.read_data_state, "read_data_state");
    sc_trace(tf, dnnacc.controller.shift_count_Reg, "shift_count_Reg");
    sc_trace(tf, dnnacc.controller.store_col_index, "store_col_index");
    //Start Simulation
    rst.write(1);
    start.write(0);
    sc_start(10,SC_NS);
    rst.write(0);
    start.write(1);
    sc_start(10,SC_NS);
    start.write(0);

    for(int i = 0; i < ISRAM_BANK_NUM; i++)
    {
        // if(i == 0)
        // {
        //     cout << "------------------------------------------" << endl;
        //     cout << "ISRAM bank[" << i << "]  "  << endl;
        //     for (int j = 0; j < ISRAMbank_element; j++)
        //         cout << "data[" << j << "] : " << dnnacc.ibank[i].data[j] << " ";
        //     cout << endl;
        //     cout << "------------------------------------------" << endl;
        // }
        // I_CS[i].write(1);
        // I_WEB[i].write(1);
        // I_addr[i].write(0);
        // for(int j = 0; j < ISRAM_BANK_NUM; j++)
        //     I_data_i[j].write(j);
    }
   
    // cout << "WEIGHT SRAM bank 0" << endl;
    // for(int i = 0; i < WSRAMbank_element; i++)
    // {
    //     if (i < 9)
    //     {
    //         cout << "weight[" << i << "] : " << dnnacc.wsram[0].wbank[0].weight[i] << " ";
    //     }
    // }
    // cout << endl << "------------------------------------------" << endl;

    for (int i = 0; i < 7; i++)
    {
        sc_start(10,SC_NS);
        cout << "time : " << sc_time_stamp() << endl;;
        cout << "------------------------------------------" << endl;
        for(int i = 0; i < FILTER_SIZE; i++){
            for (int j = 0; j < REG_ROW_NUM; j++){
                cout << "Reg[" << i * REG_ROW_NUM + j << "] : " << dnnacc.regarray.reg[i * REG_ROW_NUM + j] << " ";
            }
            cout << endl;
        }
        cout << "------------------------------------------" << endl;
        for (int i = 0; i < MAC_NUM; i++)
            cout << "PE[" << i << "] : " << dnnacc.pe[0].result[i] << endl;

        // for(int i = 0; i < 4; i++)
        //     cout << "isram16 data : " << dnnacc.sram_bus.data[i + 60] << " ";

        cout << endl;
        cout << "ibank_ctrl : " << dnnacc.controller.ibank_ctrl_Reg << endl;
        cout << "ibank_addr : " << dnnacc.controller.ibank_addr_Reg << endl;
        // cout << "------------------------------------------" << endl;
        // for (int i = 0; i < MAC_NUM; i++)
        //     cout << "total : " << dnnacc.pe[0].data_out << endl;

    }
   
    cout << "------------------------------------------" << endl;
    for (int i = 0; i < 128; i++)
    {
        if (i == 64)
            cout << endl;
        cout << "OSRAM0_data[" << i << "] : " << dnnacc.osram[0].O_SRAMdata[i] << " ";

    }
    cout << endl << "-------------------------------------------" << endl;

    sc_close_vcd_trace_file(tf); 

    return (0);
}