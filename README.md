# Breath LED Example

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
-Osize
Execution starts, 2000000 runs through Dhrystone
MicroSecond for one run through Dhrystone[28-11693]:     5.833
Dhrystones per Second:  171453.062
DMIPS/MHz:      0.781

-Omax
MicroSecond for one run through Dhrystone[28-3324]:      1.648
Dhrystones per Second:  606796.125
DMIPS/MHz:      2.763
```

### Flash uncached
ARMCLANG 6.24
```
-Osize
Execution starts, 2000000 runs through Dhrystone
MicroSecond for one run through Dhrystone[15253-242901]:         113.824
Dhrystones per Second:  8785.493
DMIPS/MHz:      0.040

-Omax
MicroSecond for one run through Dhrystone[6884-65796]:   29.456
Dhrystones per Second:  33948.941
DMIPS/MHz:      0.155
```

### SRAM
ARMCLANG 6.24
```
-Omax
MicroSecond for one run through Dhrystone[8677-12165]:   1.744
Dhrystones per Second:  573394.500
DMIPS/MHz:      2.611
```
