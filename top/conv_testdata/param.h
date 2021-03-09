#define INPUT_W (46)
#define INPUT_H (46)
#define INPUT_C (1)

#define F_SIZE (3)
#define F_NUM  (1)

#define OUTPUT_W (INPUT_W-F_SIZE+1)
#define OUTPUT_H (INPUT_H-F_SIZE+1)
#define OUTPUT_C (F_NUM)

//padding for memory alignment
#if (INPUT_W % 4) == 0
    #define ROW_PADD (0)
#else
    #define ROW_PADD (4 - (INPUT_W % 4))
#endif