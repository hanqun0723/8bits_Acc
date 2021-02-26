



//to SRAM_BUS
    sc_out<bool> mux_sel[ISRAM_BANK_NUM * ISRAM_bytes];              //Sel 9 bytes to PE
    sc_out<MUX_BITS> reg_mux_sel[REG_NUM];   