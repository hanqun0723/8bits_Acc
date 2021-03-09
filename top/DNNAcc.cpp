#include "DNNAcc.h"

void DNNAcc::propagate(){
    
    for(int i = 0; i < OSRAM_NUM; i++)
        osram_data[i].write(O_data_o[i].read());

}