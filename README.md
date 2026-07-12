# Breath LED Example

This example is designed for using RP2040 DFP on the Raspberry Pi Pico. 

125 Mhz

UART0 retarget stdout


# Adapted for ARMClang V6.24

Cortex M0+ 125 MHz
## dhrystone

### Flash cached
ARMCLANG 6.24
```
-Ofast
Execution starts, 4000000 runs through Dhrystone
MicroSecond for one run through Dhrystone[23-14064]:     3.510
Dhrystones per Second:  284880.000
DMIPS/MHz:      1.297

-Omax
error
```

### Flash uncached
ARMCLANG 6.24
```
-Ofast
error
```

### SRAM
ARMCLANG 6.24
```
-Omax

```
