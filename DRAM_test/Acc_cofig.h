#define DATA_WIDTH          (8)               //INT8
#define BUS_WIDTH           (32)

/***Input SRAM***/
#define ISRAMbank_element   (16)              //1 bank = 64bytes(1Bytes*64) = 16words
#define ISRAM_ADDR_LEN      (4)            //LOG2(16)
#define ISRAM_BANK_NUM      (64)
#define ISRAM_DATA_WIDTH    (32)
#define ISRAM_bytes         (4)

/***Weight SRAM***/
#define WSRAMbank_element  (128)    
#define WSRAM_BANK_NUM           (9)
#define WSRAM_BANK_BITS       (4)     
#define WSRAM_NUM          (32)             
#define WSRAM_ADDR_LEN     (32)


/***Output SRAM***/
#define OSRAM_SIZE          (64*64*2)       //64*64*2bytes   
#define OSRAM_NUM           (32)        
#define OSRAM_ADDR_LEN      (13)            //LOG2(64*64*2)
#define OSRAM_DATA_WIDTH        (16)            
#define O_READ                (1)
#define O_WRITE               (2)
#define O_READ_WRITE                (3)

/*******PE*******/
#define MAC_NUM              (9)
#define PE_NUM               (32)
#define PSUM_WIDTH          (16)

/*******Reg Array******/
#define REG_NUM              (3*6)
#define FILTER_SIZE           (3)           //Support maximum filter size
#define ROW_NUM               (6)

/******SRAM bus******/
#define MUX_BITS              (8)           //LOG2(256)


/******Reg MUX******/
#define REG_MUX_BITS              (8)           //LOG2(256)

/******Controller******/
#define ACC_IDLE           (0)
#define ACC_READ_WEIGHT    (1)
#define ACC_READ_DATA      (2)


/******DMAC******/




