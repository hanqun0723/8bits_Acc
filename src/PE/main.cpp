#include "PE.h"
#include "systemc.h"

int sc_main(int argc, char *argv[]){

    sc_report_handler::set_actions (SC_WARNING, SC_DO_NOTHING);

    sc_clock clk("clk", 10);
    sc_signal<bool> rst;

    sc_signal<sc_int<8> > input[9];          //from Input SRAM 
    sc_signal<sc_int<8> > weight[9];         //from Weight SRAM
    sc_signal<sc_int<8> > bias;
    //sc_signal<sc_int<8> > Dequantize; 

    sc_signal<bool>       weight_valid;
    sc_signal<bool>       last_channel;

    sc_signal<bool>           add_prev;          //add_prev = 1 means add previos psum
    sc_signal<sc_int<16> >    prev_psum;

    sc_signal<sc_int<16> >   data_out;


    PE pe("PE");

    pe.clk(clk);
    pe.rst(rst);


    for(int i = 0; i < 9; i++)
    {
        pe.input[i](input[i]);
        pe.weight[i](weight[i]);
    }

    pe.bias(bias);
    pe.weight_valid(weight_valid);
    pe.last_channel(last_channel);
    pe.add_prev(add_prev);
    pe.prev_psum(prev_psum);
    pe.data_out(data_out);

    sc_trace_file *tf = sc_create_vcd_trace_file("RESULT");


    sc_trace(tf, pe.clk ,"clk");
    sc_trace(tf, pe.rst ,"rst");

    //trace input signal
    sc_trace(tf, pe.input[0] ,"input[0]");
    sc_trace(tf, pe.input[1] ,"input[1]");
    sc_trace(tf, pe.input[2] ,"input[2]");
    sc_trace(tf, pe.input[3] ,"input[3]");
    sc_trace(tf, pe.input[4] ,"input[4]");
    sc_trace(tf, pe.input[5] ,"input[5]");
    sc_trace(tf, pe.input[6] ,"input[6]");
    sc_trace(tf, pe.input[7] ,"input[7]");
    sc_trace(tf, pe.input[8] ,"input[8]");
    
    //trace weight signal
    sc_trace(tf, pe.weight_valid ,"weight_valid");
    sc_trace(tf, pe.weight[0] ,"weight[0]");
    sc_trace(tf, pe.weight[1] ,"weight[1]");
    sc_trace(tf, pe.weight[2] ,"weight[2]");
    sc_trace(tf, pe.weight[3] ,"weight[3]");
    sc_trace(tf, pe.weight[4] ,"weight[4]");
    sc_trace(tf, pe.weight[5] ,"weight[5]");
    sc_trace(tf, pe.weight[6] ,"weight[6]");
    sc_trace(tf, pe.weight[7] ,"weight[7]");
    sc_trace(tf, pe.weight[8] ,"weight[8]");

    sc_trace(tf, pe.weight_Reg[0] ,"weight_Reg[0]");
    sc_trace(tf, pe.weight_Reg[1] ,"weight_Reg[1]");
    sc_trace(tf, pe.weight_Reg[2] ,"weight_Reg[2]");
    sc_trace(tf, pe.weight_Reg[3] ,"weight_Reg[3]");
    sc_trace(tf, pe.weight_Reg[4] ,"weight_Reg[4]");
    sc_trace(tf, pe.weight_Reg[5] ,"weight_Reg[5]");
    sc_trace(tf, pe.weight_Reg[6] ,"weight_Reg[6]");
    sc_trace(tf, pe.weight_Reg[7] ,"weight_Reg[7]");
    sc_trace(tf, pe.weight_Reg[8] ,"weight_Reg[8]");

    sc_trace(tf, pe.result[0] ,"result[0]");
    sc_trace(tf, pe.result[1] ,"result[1]");
    sc_trace(tf, pe.result[2] ,"result[2]");
    sc_trace(tf, pe.result[3] ,"result[3]");
    sc_trace(tf, pe.result[4] ,"result[4]");
    sc_trace(tf, pe.result[5] ,"result[5]");
    sc_trace(tf, pe.result[6] ,"result[6]");
    sc_trace(tf, pe.result[7] ,"result[7]");
    sc_trace(tf, pe.result[8] ,"result[8]");

    sc_trace(tf, pe.result_Reg[0] ,"result_Reg[0]");
    sc_trace(tf, pe.result_Reg[1] ,"result_Reg[1]");
    sc_trace(tf, pe.result_Reg[2] ,"result_Reg[2]");
    sc_trace(tf, pe.result_Reg[3] ,"result_Reg[3]");
    sc_trace(tf, pe.result_Reg[4] ,"result_Reg[4]");
    sc_trace(tf, pe.result_Reg[5] ,"result_Reg[5]");
    sc_trace(tf, pe.result_Reg[6] ,"result_Reg[6]");
    sc_trace(tf, pe.result_Reg[7] ,"result_Reg[7]");
    sc_trace(tf, pe.result_Reg[8] ,"result_Reg[8]");

    sc_trace(tf, pe.data_out ,"data_out");

    rst.write(1);
    sc_start(10,SC_NS);

    rst.write(0);
//given input and weight
    for(int i = 0; i < 9; i++)
        input[i].write(i);

    weight_valid.write(1);
    for(int i = 0; i < 9; i++)
        weight[i].write(i);
    
    sc_start(10,SC_NS);
    weight_valid.write(0);

    sc_start(30,SC_NS);
    sc_close_vcd_trace_file(tf); 

    return (0);
}