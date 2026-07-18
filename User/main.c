#define PICO_CLOCK_AJDUST_PERI_CLOCK_WITH_SYS_CLOCK 1
//#define PICO_STDIO_UART 1
#define PICO_DEFAULT_UART_BAUD_RATE 115200U
//#define PICO_DEFAULT_UART_BAUD_RATE 460800U
//#define PICO_DEFAULT_UART_BAUD_RATE 921600U

#include "pico/stdlib.h"
#include "pico/time.h"
#include "pico/platform.h"
#include "hardware/structs/sio.h"
#include "hardware/clocks.h"
#include "hardware/pll.h"
#include "hardware/sync.h"
#include "hardware/structs/xip_ctrl.h"
#include "system_rp2040.h"
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#include "cmsis_compiler.h"

#include "RTE_Components.h"
#include <assert.h>

#include "utils.h"
#include "custom_def.h"
#include "core_portme.h"

#if defined(RTE_CMSIS_View_EventRecorder) && defined(RTE_CMSIS_Compiler_STDOUT_Event_Recorder)
#   include <EventRecorder.h>
#   include "EventRecorderConf.h"
#endif

#define UART_ID uart0
#define UART_TX_PIN 0
#define UART_RX_PIN 1

volatile uint32_t g_ticks;
extern uint32_t __INITIAL_SP;
extern void _stage2_boot(void);

// Provides the root anchor symbol that perf_counter needs to attach onto
void SysTick_Handler(void) {
    // Keep blank if you are using hardware microsecond timers, 
    // or handle your standard OS/custom tick increments here.
}

int stdout_putchar (int ch) {
	if (ch == '\n') {
		uart_putc_raw(UART_ID, '\r');
	}
	uart_putc_raw(UART_ID, (char)ch);
  return (ch);
}

//bool timer_callback(struct repeating_timer *t) {
//	g_ticks++;
//	
//	// Keep the timer running
//	return true; 
//}

static void system_init(void) {
//	disable_xip_cache();
	
	set_sys_clock_khz(configSYS_CLOCK_K_HZ, true);
	SystemCoreClockUpdate();
	
	// Initialize UART
	uart_init(UART_ID, PICO_DEFAULT_UART_BAUD_RATE);

	// Set the TX and RX pins to use UART
	gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
	gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);

	// Optionally, enable hardware flow control (if needed)
	// uart_set_hw_flow(UART_ID, false, false);

	// Set data format: 8 bits, 1 stop bit, no parity
	uart_set_format(UART_ID, 8, 1, UART_PARITY_NONE);

	// Enable UART (already enabled by uart_init)
//	uart_set_fifo_enabled(UART_ID, true);

	// Now you can use uart_putc, uart_gets, etc.
//	uart_puts(UART_ID, "UART initialized!\n");
		
#if defined(RTE_CMSIS_View_EventRecorder) && defined(RTE_CMSIS_Compiler_STDOUT_Event_Recorder)
	EventRecorderInitialize(0, 1);
#endif

	gpio_init(PICO_DEFAULT_LED_PIN);
	gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);

	stdio_init_all();


//	struct repeating_timer timer;

	// Add a repeating timer that fires every 1ms
//	add_repeating_timer_ms(1000 / configTICK_RATE_HZ, timer_callback, NULL, &timer);
}

// Define a function pointer prototype matching your dhry_main signature
typedef int (*cm_main_func_t)(void);

int main(void) {
	system_init();
			
	printf("RP2040 %u\n", SystemCoreClock);
	
	// Flash version
	
	printf("Standard flash function pointer address: %p\n", coremark_main);

	// Get the standard address of the benchmark function (typically 0x10xxxxxx)
	uint32_t cached_address = (uint32_t)&coremark_main;

	// Force the base pointer window from 0x10000000 to 0x13000000
	// This retains the original lower offset bits and the essential Thumb bit (bit 0)
	uint32_t uncached_address = (cached_address & 0x0FFFFFFF) | 0x13000000;

	// Cast them back to executable function pointers
	cm_main_func_t run_cached_cm_main   = (cm_main_func_t)cached_address;
	cm_main_func_t run_uncached_cm_main = (cm_main_func_t)uncached_address;

	while(1) {
			// --- TEST 1: CACHED FLASH PERFORMANCE ---
			printf("\n[1/2] Starting CACHED flash run (Target: %08X)...\n", cached_address);
			
			gpio_put(PICO_DEFAULT_LED_PIN, 1); // LED Solid during cached test
			run_cached_cm_main(); 
			
			printf("CACHED run complete. %u\n", SystemCoreClock);
			sleep_ms(3000);

			// --- TEST 2: UNCACHED FLASH PERFORMANCE ---
			printf("\n[2/2] Starting UNCACHED flash run (Target: %08X)...\n", uncached_address);
			printf("Expect this to run significantly slower!\n");
			
			// Blink fast right before it enters slow mode so you know it's switching
			gpio_put(PICO_DEFAULT_LED_PIN, 0); 
			sleep_ms(200);
			gpio_put(PICO_DEFAULT_LED_PIN, 1);
			sleep_ms(200);
			gpio_put(PICO_DEFAULT_LED_PIN, 0); 

			// This executes the exact code out of flash but forces a raw 
			// serial QSPI pin lookup for every loop fetch cycle.
			run_uncached_cm_main(); 
			
			printf("UNCACHED run complete. %u\n", SystemCoreClock);
			sleep_ms(3000);
	}
}
