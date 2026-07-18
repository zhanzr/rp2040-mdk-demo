# Example

This example is designed for using RP2040 DFP on the Raspberry Pi Pico. 

125 Mhz

UART0 retarget stdout

## About cached and uncached flash
The hardware maps the external flash to four parallel windows:

0x10000000: Normal XIP access (Cache enabled, Allocating) <- Your standard main runs here

0x11000000: Cacheable, non-allocating (Checks for hits, but updates nothing on a miss)

0x12000000: Non-cacheable, allocating

0x13000000: Bypass Cache completely (Non-cacheable, non-allocating)

# Adapted for ARMClang V6.24

Cortex M0+ 125 MHz
## dhrystone

### Flash cached
ARMCLANG 6.24
```
-Omax
2K performance run parameters for coremark.
CoreMark Size    : 666
Total ticks      : 13463
Total time (secs): 13.463000
Iterations/Sec   : 297.110599
Iterations       : 4000
Compiler version : Clang 20.0.0git
Compiler flags   : -xc -std=c11 --target=arm-arm-none-eabi -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard -c -fno-rtti -funsigned-char -fshort-enums -fshort-wchar
Memory location  : Static
seedcrc          : 0xe9f5
[0]crclist       : 0xe714
[0]crcmatrix     : 0x1fd7
[0]crcstate      : 0x8e3a
[0]crcfinal      : 0x65c5
Correct operation validated. See readme.txt for run and reporting rules.
CoreMark 1.0 : 297.110599 / Clang 20.0.0git -xc -std=c11 --target=arm-arm-none-eabi -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard -c -fno-rtti -funsigned-char -fshort-enums -fshort-wchar / Static
```

### Flash uncached
ARMCLANG 6.24
```
-Omax
2K performance run parameters for coremark.
CoreMark Size    : 666
Total ticks      : 282032
Total time (secs): 282.032000
Iterations/Sec   : 14.182788
Iterations       : 4000
Compiler version : Clang 20.0.0git
Compiler flags   : -xc -std=c11 --target=arm-arm-none-eabi -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard -c -fno-rtti -funsigned-char -fshort-enums -fshort-wchar
Memory location  : Static
seedcrc          : 0xe9f5
[0]crclist       : 0xe714
[0]crcmatrix     : 0x1fd7
[0]crcstate      : 0x8e3a
[0]crcfinal      : 0x65c5
Correct operation validated. See readme.txt for run and reporting rules.
CoreMark 1.0 : 14.182788 / Clang 20.0.0git -xc -std=c11 --target=arm-arm-none-eabi -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard -c -fno-rtti -funsigned-char -fshort-enums -fshort-wchar / Static
```
