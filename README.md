# 8bits_Acc

Source code and Makefile are under /top folder

Usage : 
1. Edit Layer parameter (/conv_testdata/param.h)
2. conv_testdata/$./gen_data.sh
3. top/$ make clean && make

Unfinished:
1. filter size = 3, test Input channel > 128 or Filter number > 32 (04/01~04/02)
2. Dequantize , bias (04/01)
3. different filter size
4. pooling unit
5. DMA padding?

---

2021/03/25~03/31
1. DRAM blocking (no delay) -> nonblocking(with DDR4-2400)
2. finish new ping pong Input SRAM control -> discuss with cheng yo,hsieh
3. solve Input address unalignment (use align_buffer to handle)

Test pass :
Input : 418 418 3 | Weight : 3 3 3 1 | Output : 416 416 1

---

2021/04/01

Test pass :
Yolov3-tiny(layer1) Input : 418 418 3 | Weight : 3 3 3 16 | Output : 416 416 16

Clock time : 3816.495684
Simulation cycle : 11832269
Simulation time : 39401455.770000 ns
Simulation time : 0.039401 s

---

If Input channel > 128 (Filter number <= 32)
  then do all channel of tile -> means you need to change weight multi-times in doing one tile
 
If Filter number > 32 (Input channel <= 128)
  then do 32 filters first, and do the rest  

If (Input channel > 128) && (Filter number > 32)
  Input channel first, then filter





