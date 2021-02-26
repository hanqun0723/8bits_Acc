#include "RegArray.h"

void RegArray::do_reset()
{
    for(int i = 0; i < REG_NUM; i++)
    {
        reg[i] = 0;
        reg_o[i].write(0);
    }
}

void RegArray::do_RegArray(){

    //reset
    do_reset();

    while(1){

        wait();

        if(store_reg.read())
        {
            for(int i = 0; i < REG_NUM; i++)
            {
                reg[i] = reg_i[i].read();
            }
        }

        for(int i = 0; i < FILTER_SIZE; i++)
        {
            if(shift[i])
            {
                for(int j = 0; j < ROW_NUM - 1; j++)
                    reg[i * FILTER_SIZE + j] = reg[i * FILTER_SIZE + j + 1];
            }
        }

    }

}