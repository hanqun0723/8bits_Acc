#include "InputSRAMbank.h"

void InputSRAMbank::do_reset(){

    for(int i = 0; i < 16; i++){
        data[i] = 0;
    }

    data_o = 0;
}

void InputSRAMbank::do_InputSRAMbank(){

    //reset
    do_reset();

    while(1){

        wait();

        if(CS.read())
        {
            if(wr_enable.read())                //write data
            {
                data[addr.read() >> 2] = data_i.read();
            }
            else                                //read data
            {
                data_o.write(data[addr.read() >> 2]);
            }
        }
        else
        {
            data_o.write(0);
        }

    }

}