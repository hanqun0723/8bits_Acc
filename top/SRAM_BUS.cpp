#include "SRAM_BUS.h"

void SRAM_BUS::do_SRAMBUS()
{
    //int count = 0;

    for(int i = 0; i < ISRAM_BANK_NUM; i++)
    {
        for (int j = 0 ; j < ISRAM_bytes; j++)
        {
            data[ (i + 1) * ISRAM_bytes - j - 1] = bus_data_i[i].read().range( (j + 1) * 8 - 1, j * 8);
        }

    }

    //Output to PE
    // for(int i = 0; ISRAM_BANK_NUM * ISRAM_bytes; i++)
    // {
    //     if(mux_sel[i])
    //     {
    //         bus_pe_o[count].write(data[i]);
    //         count++;
    //     }
    //     else
    //         bus_pe_o[count].write(0);
    // }

    base = store_col_index.read();
    reg_index = base;

    for(int i = 0; i < ISRAM_BANK_NUM * ISRAM_bytes; i++)
    {
        if (bus_mux_sel[i].read())
        {
            
            cout << "reg_index : " << reg_index << "| data[" << i << "] : " << data[i] << endl;
            bus_reg_o[reg_index].write(data[i]);
            reg_index = reg_index + 1;
            
            if (base == 0)
            {
                if ( (reg_index - 4) % 6  == 0)
                    reg_index = reg_index + 2;  
            }
            else if (base == 2)
            {
                if (reg_index % 6 == 0)
                    reg_index = reg_index + 2;                
            }
        }
    }

    //Output to Reg
    // for(int i = 0; i < REG_NUM; i++)
    // {
    //     bus_reg_o[i].write(data[bus_mux_sel[i].read()]);
    // }
}