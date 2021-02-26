#include "Reg_MUX.h"

void Reg_MUX::do_RegMUX()
{
    int count = 0;

    for(int i = 0; i < REG_NUM; i++)
    {
        if(reg_mux_sel[i])
        {
            reg_data_o[count].write(reg_data_i[i]);
            //cout << "reg_data_i[" << i << "] : " << reg_data_i[i] << endl;
            count++;
        }
    }
}