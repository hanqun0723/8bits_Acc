#define DATA_WIDTH          (8)               //INT8
#define BUS_WIDTH           (32)
#define TILE_H              (64)
#define TILE_W              (64)

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
#define OSRAM_SIZE          (64*64)       //64*64*2bytes   
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
#define REG_ROW_NUM               (6)

/******SRAM bus******/
#define MUX_BITS              (8)           //4bytes x 64bank


/******Reg MUX******/
#define REG_MUX_BITS              (8)           //LOG2(256)

/******Controller******/
#define ACC_IDLE           (0)
#define ACC_DMA_READ       (1)
#define ACC_START_PE      (2)
#define ACC_DMA_WRITE     (3)
#define ACC_FINISH        (4)

/******PE******/
#define READ_WSRAM  (0)
#define READ_ISRAM  (1)
#define ISRAM_TO_REG (2)
#define PE_START    (3)


#define FSIZE_3_CASE1  (0)
#define FSIZE_3_CASE2  (1)
#define FSIZE_3_CASE3  (2)
#define FSIZE_3_CASE4  (3)

/******DMAC******/
#define DMA_WEIGHT_CONFIG (0)
#define DMA_READ_TO_WSRAM (1)
#define DMA_INPUT_CONFIG  (2)
#define DMA_READ_TO_ISRAM (3)

#define DMA_OUTPUT_CONFIG (0)
#define DMA_WRITE_TO_DRAM  (1)


/******DRAM BASE******/
#define DRAM_INPUT_BASE  (0x00000000) //64MB size
#define DRAM_WEIGHT_BASE (0x01000000) //64MB size
#define DRAM_OUTPUT_BASE (0x02000000) //128MB size


