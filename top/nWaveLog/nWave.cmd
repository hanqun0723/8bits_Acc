wvConvertFile -win $_nWave1 -o \
           "/home/hanqun/int8_vp/8bits_Acc/top/RESULT.vcd.fsdb" "RESULT.vcd"
wvOpenFile -win $_nWave1 {/home/hanqun/int8_vp/8bits_Acc/top/RESULT.vcd.fsdb}
nMemSetPreference
wvConvertFile -win $_nWave1 -o \
           "/home/hanqun/int8_vp/8bits_Acc/top/RESULT.vcd.fsdb" \
           "/home/hanqun/int8_vp/8bits_Acc/top/RESULT.vcd"
wvOpenFile -win $_nWave1 {/home/hanqun/int8_vp/8bits_Acc/top/RESULT.vcd.fsdb}
wvExit
