#include <iomanip>
#include <iostream>

//====================================================
// Simulation Platform
//====================================================

	#ifndef TIME_SCALE
	#define TIME_SCALE (SC_NS)
	#endif

	#ifndef CLK_CYCLE
	#define CLK_CYCLE (5.0)
	#endif

	// Note: When you set TIME_SCALE as SC_NS, then
	// CLK_CYCLE = (5.0) 200MHz, (2.5) 400MhHz, (1.66) 600MHz, (1.25) 800Mhz, (1.0) 1000MHz, (0.833) 1200Mhz
	// Make sure that CLC_CYCLE is a float type value.

//====================================================
// Memory Subsystem
//====================================================
// DRAM Configuration

#define ADDR_LENGTH (32) 
#define DATA_LENGTH (32) 
#define DATA_BUS_BANDWIDTH (32)        //Configuable

//~~~~~~~~
//  DRAM
//~~~~~~~~
#define MEM_SIZE (0x2000000) //MEM_SIZE (0x2000000)bytes,Configuable, uint32_t mem[MEMSIZE] 


#define ROW_BIT (11)
#define COLUMN_BIT (10)
#define BANK_BIT (8)

#ifdef FAST_SIM
	#define tRP (1)
	#define tRCD (1)
	#define tCL (1)
	#define tRAS (1)
	#define magic_delay (1)
#else  // using DDR4-2400
	#define tRP (15)
	#define tRCD (15)
	#define tCL (15)
	#define tRAS (39)
	#define magic_delay (0.8333) // DRAM bus cycle time (ns) (0.8333)
#endif

//****************
#define DRAM4_ID (4)
#define DRAM4_BASE (0X00000000)
#define DRAM4_SIZE (MEM_SIZE)
