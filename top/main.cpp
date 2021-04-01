#include "DRAM.h"
#include "DMAC.h"
#include "DNNAcc.h"
#include "systemc.h"
#include "Acc_cofig.h"
#include "string.h"
#include <time.h>
#include "./conv_testdata/param.h"

using namespace std;

int sc_main(int argc, char *argv[]){

    clock_t start_time, end_time;
    int cycle_count = 0;

    sc_clock clk("clk", 10);
    sc_signal<bool> rst;

    sc_signal<sc_uint<2> >  dma_type;
    sc_signal<sc_uint<32> > src;
    sc_signal<sc_uint<32> > tgt;
    sc_signal<sc_uint<32> > osram_id;
    sc_signal<sc_uint<32> > length;
    sc_signal<sc_int<OSRAM_DATA_WIDTH> > osram_data[OSRAM_NUM];
    sc_signal<bool> osram_valid;
    sc_signal<bool> dma_resp;
    sc_signal<bool> DMA_start; 

    sc_signal<sc_int<32> > read_data;
    sc_signal<bool>        dram_done;
    sc_signal<bool>        DMA_irt;
    sc_signal<bool>        DMA_irtclr;

    // sc_signal<bool>     I_CS[ISRAM_BANK_NUM];
    // sc_signal<bool>     I_WEB[ISRAM_BANK_NUM];
    // sc_signal<sc_uint<ISRAM_ADDR_LEN> >  I_addr[ISRAM_BANK_NUM];
    // sc_signal<sc_int<ISRAM_DATA_WIDTH> > I_data_i[ISRAM_BANK_NUM];
    // sc_signal<bool>     bus_mux_sel[ISRAM_BANK_NUM * ISRAM_bytes];
    // sc_signal<bool>        store_reg;  
    // sc_signal<bool>        shift[FILTER_SIZE];  
    //sc_signal<bool>        reg_mux_sel[REG_NUM];   
    // sc_signal<bool>     W_CS[WSRAM_NUM];
    // sc_signal<bool>     W_WEB[WSRAM_NUM];
    // sc_signal<sc_uint<WSRAM_ADDR_LEN> >  W_addr[WSRAM_NUM];
    // sc_signal<sc_uint<3> >    W_length[WSRAM_NUM];           
    // sc_signal<sc_uint<WSRAM_BANK_BITS> >     W_bank_sel[WSRAM_NUM];        
    // sc_signal<sc_int<BUS_WIDTH> > W_data_i[WSRAM_NUM];
    // sc_signal<bool>       weight_valid[PE_NUM];
    // sc_signal<bool>       last_channel[PE_NUM];
    // sc_signal<bool>           add_prev[PE_NUM];         
    // sc_signal<sc_int<PSUM_WIDTH> >    prev_psum[PE_NUM];
    // sc_signal<sc_uint<2> >     read_write[OSRAM_NUM];  	
    // sc_signal<bool>            O_WEB[OSRAM_NUM];             
    // sc_signal<sc_uint<OSRAM_ADDR_LEN> > O_addr_wr[OSRAM_NUM];    
    // sc_signal<sc_uint<OSRAM_ADDR_LEN> > O_addr_r[OSRAM_NUM];  
    sc_signal<bool> start;


    DMAC *dmac1 = new DMAC("dmac1");
    DRAM *dram1 = new DRAM("dram1");
    DNNAcc dnnacc("dnnacc");

    dmac1->socket.bind(dram1->socket);
    dmac1->clk(clk);
    dmac1->rst(rst);
    dmac1->dma_type(dma_type);
    dmac1->src(src);
    dmac1->tgt(tgt);
    dmac1->osram_id(osram_id);
    dmac1->length(length);
    for(int i = 0; i < OSRAM_NUM; i++)
        dmac1->osram_data[i](osram_data[i]);
    dmac1->osram_valid(osram_valid);
    dmac1->dma_resp(dma_resp);
    dmac1->DMA_start(DMA_start);
    dmac1->read_data(read_data);
    dmac1->dram_done(dram_done);
    dmac1->DMA_irt(DMA_irt);
    dmac1->DMA_irtclr(DMA_irtclr);

    dnnacc.clk(clk);
    dnnacc.rst(rst);
    dnnacc.dma_type(dma_type);
    dnnacc.src(src);
    dnnacc.tgt(tgt);
    dnnacc.osram_id(osram_id);
    dnnacc.length(length);
    for(int i = 0; i < OSRAM_NUM; i++)
        dnnacc.osram_data[i](osram_data[i]);   
    dnnacc.osram_valid(osram_valid);
    dnnacc.dma_resp(dma_resp);
    dnnacc.DMA_start(DMA_start);
    dnnacc.read_data(read_data);
    dnnacc.dram_done(dram_done);
    dnnacc.DMA_irt(DMA_irt);
    dnnacc.DMA_irtclr(DMA_irtclr);
    
    for(int i = 0; i < ISRAM_BANK_NUM; i++)
    {
        // dnnacc.I_CS[i](I_CS[i]);
        // dnnacc.I_WEB[i](I_WEB[i]);
        // dnnacc.I_addr[i](I_addr[i]);
        //dnnacc.I_data_i[i](I_data_i[i]);
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
        // dnnacc.W_length[i](W_length[i]);
        // dnnacc.W_data_i[i](W_data_i[i]);
        // dnnacc.W_bank_sel[i](W_bank_sel[i]);
    }     
    for(int i = 0; i < PE_NUM; i++)
    {
        // dnnacc.weight_valid[i](weight_valid[i]);
        // dnnacc.last_channel[i](last_channel[i]);
        // dnnacc.add_prev[i](add_prev[i]); 
        // dnnacc.prev_psum[i](prev_psum[i]);
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
    for(int i = (DRAM_INPUT_BASE/4); i < (DRAM_INPUT_BASE/4 + 20); i++)
    {
        sc_trace(tf, dram1->mem[i], "dram1.mem("+to_string(i)+")");
    }
    for(int i = (DRAM_OUTPUT_BASE/4); i < (DRAM_OUTPUT_BASE/4 + 30); i++)
    {
        sc_trace(tf, dram1->mem[i], "dram1.mem("+to_string(i)+")");
    }
    sc_trace(tf, dmac1->src, "dmac.src");
    sc_trace(tf, dmac1->tgt, "dmac.tgt");
    sc_trace(tf, dmac1->state, "dmac.state");
    sc_trace(tf, dmac1->length, "dmac.length");
    sc_trace(tf, dmac1->DMA_start, "dmac.DMA_start");
    //sc_trace(tf, dmac1->data_valid, "dmac.data_valid");
    sc_trace(tf, dmac1->read_data, "dmac.read_data");
    sc_trace(tf, dmac1->count_Reg, "dmac.count_Reg");
    sc_trace(tf, dmac1->DMA_irt, "dmac.DMA_irt");
    sc_trace(tf, dmac1->DMA_irtclr, "dmac.DMA_irtclr");
    sc_trace(tf, dmac1->dram_done, "dmac.dram_done");
    sc_trace(tf, dmac1->osram_valid, "dmac.osram_valid");
    sc_trace(tf, dmac1->dma_resp, "dmac.dma_resp");
    sc_trace(tf, dnnacc.clk, "dnnacc.clk");
    sc_trace(tf, dnnacc.rst, "dnnacc.rst");

    for(int i = 0; i < ISRAM_BANK_NUM; i++)
    {
        sc_trace(tf, dnnacc.I_CS[i], "dnnacc.I_CS("+to_string(i)+")");
        sc_trace(tf, dnnacc.I_WEB[i], "dnnacc.I_WEB("+to_string(i)+")");
        sc_trace(tf, dnnacc.I_addr[i], "dnnacc.I_addr("+to_string(i)+")");
        //sc_trace(tf, dnnacc.I_data_i[i], "dnnacc.I_data_i("+to_string(i)+")");
    }

    //Weight
    for(int i = 0; i < 1; i++)
    {
        sc_trace(tf, dnnacc.W_CS[i], "dnnacc.W_CS("+to_string(i)+")");
        sc_trace(tf, dnnacc.W_WEB[i], "dnnacc.W_WEB("+to_string(i)+")");
        sc_trace(tf, dnnacc.W_addr[i], "dnnacc.W_addr("+to_string(i)+")");
        sc_trace(tf, dnnacc.W_length[i], "dnnacc.W_length("+to_string(i)+")");
        sc_trace(tf, dnnacc.W_bank_sel[i], "dnnacc.W_bank_sel("+to_string(i)+")");
        sc_trace(tf, dnnacc.W_data_i[i], "dnnacc.W_data_i("+to_string(i)+")");
    }    

    // for(int i = 0; i < WSRAM_BANK_NUM; i++)
    // {
    //     for(int j = 0; j < WSRAMbank_element; j++)
    //     {
    //         sc_trace(tf, dnnacc.wsram[0].wbank[i].weight[j], "dnnacc.wsram(0).wbank_i("+to_string(i)+").weight("+to_string(j)+")");
    //     }
    // }

    for(int i = 0; i < ISRAM_BANK_NUM; i++)
    {
        sc_trace(tf, dnnacc.ibank[i].data_i, "dnnacc.ibank("+to_string(i)+").data_i("+to_string(0)+")");
        for(int j = 0; j < ISRAMbank_element; j++)
            sc_trace(tf, dnnacc.ibank[i].data[j], "dnnacc.ibank("+to_string(i)+").data("+to_string(j)+")");
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
    sc_trace(tf, dnnacc.osram[0].data_o, "osram.osram_data_o");

    for(int i = 0; i < (64*64); i++)
        sc_trace(tf, dnnacc.osram[0].O_SRAMdata[i], "osram.o_sramdata("+to_string(i)+")");
    sc_trace(tf, dnnacc.osram[0].addr_w, "osram.addr_w");
    sc_trace(tf, dnnacc.osram[0].addr_r, "osram.addr_r");

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

    sc_trace(tf, dnnacc.controller.dma_read_state, "dnnacc.controller.dma_read_state");
    sc_trace(tf, dnnacc.controller.dma_write_state, "dnnacc.controller.dma_write_state");
    //sc_trace(tf, dnnacc.controller.data_valid, "dnnacc.controller.data_valid");
    //sc_trace(tf, dnnacc.controller.read_data, "dnnacc.controller.read_data");
    sc_trace(tf, dnnacc.controller.ibank_addr_Reg, "dnnacc.controller.ibank_addr_Reg");
    sc_trace(tf, dnnacc.controller.ibank_ctrl_Reg, "dnnacc.controller.ibank_ctrl_Reg");
    sc_trace(tf, dnnacc.controller.prev_store_Reg, "dnnacc.controller.prev_store_Reg");
    sc_trace(tf, dnnacc.controller.read_data_state, "dnnacc.controller.read_data_state");
    sc_trace(tf, dnnacc.controller.shift_count_Reg, "dnnacc.controller.shift_count_Reg");
    sc_trace(tf, dnnacc.controller.store_col_index, "dnnacc.controller.store_col_index");
    sc_trace(tf, dnnacc.controller.pe_state, "dnnacc.controller.pe_state");
    sc_trace(tf, dnnacc.controller.length, "dnnacc.controller.length");
    sc_trace(tf, dnnacc.controller.dma_count_Reg, "dnnacc.controller.dma_count_Reg");
    sc_trace(tf, dnnacc.controller.write_result_Reg, "dnnacc.controller.write_result_Reg");
    sc_trace(tf, dnnacc.controller.conv_count_Reg, "dnnacc.controller.conv_count_Reg");
    sc_trace(tf, dnnacc.controller.osram_addr_Reg, "dnnacc.controller.osram_addr_Reg");
    sc_trace(tf, dnnacc.controller.wbank_addr_Reg, "dnnacc.controller.wbank_addr_Reg");
    sc_trace(tf, dnnacc.controller.read_write, "dnnacc.controller.read_write");
    sc_trace(tf, dnnacc.controller.h_Reg, "dnnacc.controller.h_Reg");
    sc_trace(tf, dnnacc.controller.w_Reg, "dnnacc.controller.w_Reg");
    sc_trace(tf, dnnacc.controller.c_Reg, "dnnacc.controller.c_Reg");
    sc_trace(tf, dnnacc.controller.ofmap_h_Reg, "dnnacc.controller.ofmap_h_Reg");
    sc_trace(tf, dnnacc.controller.ofmap_w_Reg, "dnnacc.controller.ofmap_w_Reg");
    sc_trace(tf, dnnacc.controller.ofmap_c_Reg, "dnnacc.controller.ofmap_c_Reg");
    sc_trace(tf, dnnacc.controller.out_h_Reg, "dnnacc.controller.out_h_Reg");
    sc_trace(tf, dnnacc.controller.out_w_Reg, "dnnacc.controller.out_w_Reg");
    sc_trace(tf, dnnacc.controller.out_c_Reg, "dnnacc.controller.out_c_Reg");
    sc_trace(tf, dnnacc.controller.tile_w_Reg, "dnnacc.controller.tile_w_Reg");
    sc_trace(tf, dnnacc.controller.tile_h_Reg, "dnnacc.controller.tile_h_Reg");
    sc_trace(tf, dnnacc.controller.osram_req, "dnnacc.controller.osram_req");
    sc_trace(tf, dnnacc.controller.pe_stall, "dnnacc.controller.pe_stall");
    sc_trace(tf, dnnacc.controller.align_Reg, "dnnacc.controller.align_Reg");
    sc_trace(tf, dnnacc.controller.add_prev, "dnnacc.controller.add_prev");
    sc_trace(tf, dnnacc.controller.row_sel_Reg, "dnnacc.controller.row_sel_Reg");
    for(int i = 0; i < ISRAM_BANK_NUM; i++)
        sc_trace(tf, dnnacc.controller.proc_ctrl_Reg[i], "proc_ctrl_Reg("+to_string(i)+")");
    for(int i = 0; i < 7; i++)
        sc_trace(tf, dnnacc.controller.align_data_Reg[i], "dnnacc.controller.align_data_Reg("+to_string(i)+")");
    //Start Simulation
    rst.write(1);
    start.write(0);
    sc_start(10,SC_NS);

    // cout << "////////////WSRAM0///////////// " << endl;
    // for(int i = 0; i < 10; i++)
    // {
    //     for(int j = 0; j < WSRAM_BANK_NUM; j++)
    //     {
    //         cout << dnnacc.wsram[0].wbank[j].weight[i] << " ";
    //     }
    //     cout << endl;
    // }
    // cout << "/////////////////////////////// " << endl;
 
    start_time = clock();
    rst.write(0);
    start.write(1);
    sc_start(10,SC_NS);
    cycle_count++;
    start.write(0);
    while(dnnacc.controller.state.read() != ACC_FINISH)
    {
        sc_start(10,SC_NS);
        cycle_count++;
    }
    end_time = clock();
    //sc_start(220000,SC_NS);

    // for(int i = 0; i < ISRAM_BANK_NUM; i++)
    // {
    //     // if(i == 0)
    //     // {
    //     //     cout << "------------------------------------------" << endl;
    //     //     cout << "ISRAM bank[" << i << "]  "  << endl;
    //     //     for (int j = 0; j < ISRAMbank_element; j++)
    //     //         cout << "data[" << j << "] : " << dnnacc.ibank[i].data[j] << " ";
    //     //     cout << endl;
    //     //     cout << "------------------------------------------" << endl;
    //     // }
    //     // I_CS[i].write(1);
    //     // I_WEB[i].write(1);
    //     // I_addr[i].write(0);
    //     // for(int j = 0; j < ISRAM_BANK_NUM; j++)
    //     //     I_data_i[j].write(j);
    // }
   
    // // cout << "WEIGHT SRAM bank 0" << endl;
    // // for(int i = 0; i < WSRAMbank_element; i++)
    // // {
    // //     if (i < 9)
    // //     {
    // //         cout << "weight[" << i << "] : " << dnnacc.wsram[0].wbank[0].weight[i] << " ";
    // //     }
    // // }
    // // cout << endl << "------------------------------------------" << endl;

    // for (int i = 0; i < 67; i++)
    // {
    //     sc_start(10,SC_NS);
    //     cout << "time : " << sc_time_stamp() << endl;;
    //     cout << "------------------------------------------" << endl;
    //     for(int i = 0; i < FILTER_SIZE; i++){
    //         for (int j = 0; j < REG_ROW_NUM; j++){
    //             cout << "Reg[" << i * REG_ROW_NUM + j << "] : " << dnnacc.regarray.reg[i * REG_ROW_NUM + j] << " ";
    //         }
    //         cout << endl;
    //     }
    //     cout << "------------------------------------------" << endl;
    //     for (int i = 0; i < MAC_NUM; i++)
    //         cout << "PE[" << i << "] : " << dnnacc.pe[0].result[i] << endl;

    //     // for(int i = 0; i < 4; i++)
    //     //     cout << "isram16 data : " << dnnacc.sram_bus.data[i + 60] << " ";

    //     cout << endl;
    //     cout << "ibank_ctrl : " << dnnacc.controller.ibank_ctrl_Reg << endl;
    //     cout << "ibank_addr : " << dnnacc.controller.ibank_addr_Reg << endl;
    //     // cout << "------------------------------------------" << endl;
    //     // for (int i = 0; i < MAC_NUM; i++)
    //     //     cout << "total : " << dnnacc.pe[0].data_out << endl;

    // }


    //sc_trace(tf,  dnnacc.wsram[0].wbank[0].weight[0], "wbank[0].weight[0]");
    // sc_trace(tf,  dnnacc.wsram[0].wbank[1].weight[0], "dnnacc.wsram[0].wbank[1].weight[0]");
    // sc_trace(tf,  dnnacc.wsram[0].wbank[2].weight[0], "dnnacc.wsram[0].wbank[2].weight[0]");

    cout << "////////////WSRAM0///////////// " << endl;
    for(int i = 0; i < 6; i++)
    {
        for(int j = 0; j < WSRAM_BANK_NUM; j++)
        {
            cout << dnnacc.wsram[0].wbank[j].weight[i] << " ";
        }
        cout << endl;
    }
    cout << "////////////WSRAM1///////////// " << endl;
    for(int i = 0; i < 6; i++)
    {
        for(int j = 0; j < WSRAM_BANK_NUM; j++)
        {
            cout << dnnacc.wsram[1].wbank[j].weight[i] << " ";
        }
        cout << endl;
    }
    // cout << "////////////WSRAM1///////////// " << endl;
    // for(int i = 0; i < 6; i++)
    // {
    //     for(int j = 0; j < WSRAM_BANK_NUM; j++)
    //     {
    //         cout << dnnacc.wsram[1].wbank[j].weight[i] << " ";
    //     }
    //     cout << endl;
    // }

    // cout << "------------------------------------------" << endl;
    // for (int i = 0; i < 100; i++)
    // {
    //     if (i % 10 == 0 && i != 0)
    //         cout << endl;
    //     cout << dnnacc.osram[0].O_SRAMdata[i] << " ";

    // }
    // cout << endl << "-------------------------------------------" << endl;

    cout << "------------------------------------------" << endl;
    //load input 
    int size = OUTPUT_H * OUTPUT_W * OUTPUT_C;
    int *golden = new int[size];
    int err = 0;
    ifstream fin("conv_testdata/output.txt");

    for(int i = 0; i < (OUTPUT_W * OUTPUT_H * OUTPUT_C); i++){
        int temp;
        fin >> temp;
        golden[i] = temp;
        if (golden[i] == (dram1->mem[DRAM_OUTPUT_BASE/4 + i]))
            //cout << "";
            cout << "golden["  << i << "]"  << " : " << golden[i] << " , " << "output : " << dram1->mem[(DRAM_OUTPUT_BASE/4) + i] << " pass " << endl;
        else
        {
            cout << "golden["  << i << "]"  << " : " << golden[i] << " , " << "output : " << dram1->mem[(DRAM_OUTPUT_BASE/4) + i] << " error " << endl;
            err++;
        }
    }
    // for(int i = 0; i < 64; i++){
    //     int temp;
    //     fin >> temp;
    //     golden[i] = temp;
    //     if (golden[i] == dnnacc.osram[0].O_SRAMdata[i])
    //         cout << "";
    //         //cout << "golden["  << i << "]"  << " : " << golden[i] << " , " << "output : " << dnnacc.osram[0].O_SRAMdata[i] << " pass " << endl;
    //     else
    //     {
    //         cout << "golden["  << i << "]"  << " : " << golden[i] << " , " << "output : " << dnnacc.osram[0].O_SRAMdata[i] << " error " << endl;
    //         err++;
    //     }
    //     //cout << "index : " << i << " data : " << golden[i] << endl;
    // }
    fin.close();
    if(err == 0){
        printf("\n");
        printf("\n");
        printf("        ****************************               \n");
        printf("        **                        **       |\__||  \n");
        printf("        **  Congratulations !!    **      / O.O  | \n");
        printf("        **                        **    /_____   | \n");
        printf("        **  Simulation PASS!!     **   /^ ^ ^ \\  |\n");
        printf("        **                        **  |^ ^ ^ ^ |w| \n");
        printf("        ****************************   \\m___m__|_|\n");
        printf("\n");
    }
    else{
        printf("\n");
        printf("\n");
        printf("        ****************************               \n");
        printf("        **                        **       |\__||  \n");
        printf("        **  OOPS!!                **      / >,<  | \n");
        printf("        **                        **    /_____   | \n");
        printf("        **  Simulation Failed!!   **   /^ ^ ^ \\  |\n");
        printf("        **                        **  |^ ^ ^ ^ |w| \n");
        printf("        ****************************   \\m___m__|_|\n");
        printf("         Totally has %d errors                     \n", err); 
        printf("\n");
    }

    printf("///////////////Timing Info///////////////////\n");
    printf("Clock time : %f\n", ((double) (end_time - start_time)) / CLOCKS_PER_SEC);
    printf("Simulation cycle : %d\n", cycle_count);
    printf("/////////////////////////////////////////////\n");

    sc_close_vcd_trace_file(tf); 

    return (0);
}