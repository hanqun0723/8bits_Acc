#include "InputSRAMbank.h"

void InputSRAMbank::do_reset()
{
    for(int i = 0; i < ISRAMbank_element; i++)
    {
        data[i] = 0;
        //data[i] = 16909060 + i;
    }
}

void InputSRAMbank::do_InputSRAMbank()
{
    //reset
    do_reset();

    while(1){

        wait();

        if(CS.read())
        {
            if(WEB.read())                //write data
            {
                data[addr.read()] = data_i.read();
            }
            else                                //read data
            {
                data_o.write(data[addr.read()]);
            }
        }
        else
        {
            data_o.write(0);
        }

    }

}