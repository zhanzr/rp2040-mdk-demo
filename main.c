#define PICO_CLOCK_AJDUST_PERI_CLOCK_WITH_SYS_CLOCK 1
//#define PICO_STDIO_UART 1
#define PICO_DEFAULT_UART_BAUD_RATE 460800U
//#define PICO_DEFAULT_UART_BAUD_RATE 921600U

#include "pico/stdlib.h"
#include "pico/time.h"
#include "hardware/structs/sio.h"
#include "hardware/clocks.h"
#include "hardware/pll.h"
#include "system_rp2040.h"
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#include "cmsis_compiler.h"

#include "RTE_Components.h"
#include <assert.h>


#if defined(RTE_CMSIS_View_EventRecorder) && defined(RTE_CMSIS_Compiler_STDOUT_Event_Recorder)
#   include <EventRecorder.h>
#   include "EventRecorderConf.h"
#endif

#define	configSYS_CLOCK_K_HZ	125000

#define	configTICK_RATE_HZ	1000

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

bool timer_callback(struct repeating_timer *t) {
    g_ticks++;
    // You can add other periodic tasks here
    return true;  // Keep the timer running
}

uint32_t HAL_GetTick(void) { return g_ticks; }

void HAL_Delay(uint32_t t) {
  uint32_t d = t + g_ticks;
  while (d > g_ticks) {
    __NOP();
  }
}

//void SystemCoreClockUpdate (void) {
//    SystemCoreClock = clock_get_hz(clk_sys);
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

	struct repeating_timer timer;

	// Add a repeating timer that fires every 1ms
	add_repeating_timer_ms(1000 / configTICK_RATE_HZ, timer_callback, NULL, &timer);
}

int main(void) {
    system_init();
		
		printf("RP2040 Pico, @ %u Hz\n", SystemCoreClock);

    while(1) {
//			gpio_put(PICO_DEFAULT_LED_PIN, 1);
//			sleep_ms(configTICK_RATE_HZ);
//			gpio_put(PICO_DEFAULT_LED_PIN, 0);
//			sleep_ms(configTICK_RATE_HZ);
			gpio_xor_mask(1u << PICO_DEFAULT_LED_PIN);
			printf("@ %u MHz, %08X %08X %08X\n", SystemCoreClock/1000000, (uint32_t)_stage2_boot, __INITIAL_SP, (uint32_t)&__INITIAL_SP);
			sleep_ms(configTICK_RATE_HZ);
    }
}