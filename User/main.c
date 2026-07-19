#define PICO_CLOCK_AJDUST_PERI_CLOCK_WITH_SYS_CLOCK 1
//#define PICO_STDIO_UART 1
//#define PICO_DEFAULT_UART_BAUD_RATE 115200U
//#define PICO_DEFAULT_UART_BAUD_RATE 230400U
#define PICO_DEFAULT_UART_BAUD_RATE 460800U
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

#if defined(RTE_CMSIS_View_EventRecorder) && defined(RTE_CMSIS_Compiler_STDOUT_Event_Recorder)
#   include <EventRecorder.h>
#   include "EventRecorderConf.h"
#endif

#include "hardware/pio.h"
#include "ws2812.pio.h" // Automatically generated header from ws2812.pio

#define WS2812_PIN 23  // Map to your target pin
#define IS_RGBW false

// Helper function to send the GRB format package to the PIO FIFO block
static inline void put_pixel(uint32_t pixel_grb) {
    pio_sm_put_blocking(pio0, 0, pixel_grb << 8u);
}

// Convert discrete R, G, B channels into a packed 32-bit integer 
static inline uint32_t urgb_u32(uint8_t r, uint8_t g, uint8_t b) {
    return ((uint32_t)(g) << 16) | ((uint32_t)(r) << 8) | (uint32_t)(b);
}

#define UART_ID uart0
#define UART_TX_PIN 0
#define UART_RX_PIN 1

volatile uint32_t g_ticks;
extern uint32_t __INITIAL_SP;
extern void _stage2_boot(void);

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

int main(void) {	
	system_init();
			
// Initialize PIO block 0, state machine 0
	PIO pio = pio0;
	uint sm = 0;
	uint offset = pio_add_program(pio, &ws2812_program);

	// Run the initialization at the standard 800kHz target speed
	ws2812_program_init(pio, sm, offset, WS2812_PIN, 800000);
	
	while(1) {
			printf("\nT1 %u %u\n", HAL_GetTick(), SystemCoreClock);

			// Example 1: Pure Red
			put_pixel(urgb_u32(255, 0, 0));
			sleep_ms(1000);

			// Example 2: Pure Green
			put_pixel(urgb_u32(0, 255, 0));
			sleep_ms(1000);

			// Example 3: Pure Blue
			put_pixel(urgb_u32(0, 0, 255));
			sleep_ms(1000);
		
			gpio_put(PICO_DEFAULT_LED_PIN, 1); // LED Solid during cached test
			
			printf("T2 %u %u\n", HAL_GetTick(), SystemCoreClock);
			sleep_ms(3000);

			// Blink fast right before it enters slow mode so you know it's switching
			gpio_put(PICO_DEFAULT_LED_PIN, 0); 
			sleep_ms(200);
			gpio_put(PICO_DEFAULT_LED_PIN, 1);
			sleep_ms(200);
			gpio_put(PICO_DEFAULT_LED_PIN, 0); 
	}
}
