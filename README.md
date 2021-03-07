# 8bitts_Acc

Unfinished:
1. DMA move outputSRAM data to DRAM
2. ping pong InputSRAM control
3. test multi-channel , partial sum Control
4. test Input channel > 128 or Filter number > 32 

If Input channel > 128 (Filter number <= 32)
  then do all channel of tile -> means you need to change weight multi-times in doing one tile
 
If Filter number > 32 (Input channel <= 128)
  then do 32 filters first, and do the rest  

If (Input channel > 128) && (Filter number > 32)
  Input channel first, then filter





