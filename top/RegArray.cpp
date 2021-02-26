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

        for(int i = 0; i < FILTER_SIZE; i++)
        {
            if(shift[i])
            {
                cout << "shift[ " << i << "] :" << shift[i] << endl;
                for(int j = 0; j < REG_ROW_NUM - 1; j++)
                {
                    reg[i * REG_ROW_NUM + j] = reg[i * REG_ROW_NUM + j + 1];
                    reg[i * REG_ROW_NUM + j + 1] = 0;
                }
            }
        }

        //cout << "store reg read!!!!!!" << endl;
        for(int i = 0; i < REG_NUM; i++)
        {
            if (store_reg[i].read())
                reg[i] = reg_i[i].read();
        }


        for(int i = 0; i < REG_NUM; i++)
        {
            reg_o[i].write(reg[i]);
        }

        counter = 0;
    }

}