#include "common.h"

#include "error.h"
#include "blink.h"
#include "control.h"
#include "interrupt.h"
#include "samples.h"
#include "drivers.h"

int main() //{{{
{
	sd_info sd;
	int8_t ret = 0;

	printf("main: clock_set_crystal()\n");
	clock_set_crystal();
	printf("main: interrupt_init()\n");
	interrupt_init();
	printf("main: drivers_init()\n");
	drivers_init();
	printf("main: samples_init()\n");
	samples_init();

	// setup an LED to blink while running, start with yellow to indicate not ready yet 
	blink_init(1000, LED_YELLOW_bm); 

	// try to initalize storage
	g_control_mode = 0;
	ret = sd_init(&sd);
	printf("main: sd_init() : %d\n", ret);

	// main loop for interrupt bottom halves 
	while (1) 
	{
#ifdef GPIO_MAIN_LOOP
			gpio_toggle(&PORTE, (1<<GPIO_MAIN_LOOP));
#endif

		// turn off the CPU between interrupts 
		set_sleep_mode(SLEEP_SMODE_IDLE_gc); 
		sleep_enable();
		sleep_cpu();
		sleep_disable();

		// general ms timer
		if (interrupt_is_set(INTERRUPT_TIMER_MS))
		{
			blink_ms_timer_update();
			interrupt_clear(INTERRUPT_TIMER_MS);
		}

		// UART receive
		if (interrupt_is_set(INTERRUPT_UART_RX))
		{
			control_got_uart_bytes();
			interrupt_clear(INTERRUPT_UART_RX);
		}

		// ADCB DMA (channel 2)
		if (interrupt_is_set(INTERRUPT_DMA_CH2))
		{
			// put samples on FIFO
			samples_ringbuf_write(g_adcb0, SAMPLES_LEN);
			interrupt_clear(INTERRUPT_DMA_CH2);
		}

		// other ADCB DMA (channel 3, double buffered)
		if (interrupt_is_set(INTERRUPT_DMA_CH3))
		{
			// calibration finished, setup normal measurement operation
			if (!g_control_calibrated)
			{
				// voltage measurment
				ADCB.CH0.MUXCTRL = ADC_CH_MUXPOS_PIN0_gc | ADC_CH_MUXNEG_GND_MODE3_gc; 
				// current measurement
				mux_select(MUX_R);

				g_control_calibrated = 1;
			}

			// put samples on FIFO
			samples_ringbuf_write(g_adcb1, SAMPLES_LEN);
			interrupt_clear(INTERRUPT_DMA_CH3);
		}

		if (g_control_mode == CONTROL_MODE_STREAM)
		{
			samples_uart_write();

#ifdef GPIO_SAMPLE_WRITE_DONE
			gpio_toggle(&PORTE, (1<<GPIO_SAMPLE_WRITE_DONE));
#endif
		}

		if (g_control_mode == CONTROL_MODE_STORE)
		{
			samples_store_write();
		}

		//if (g_control_mode == CONTROL_MODE_READ_FILE && g_control_read_ready)
		//{
		//	if (samples_store_read_next(g_adcb0) > 0)
		//		samples_uart_write(g_adcb0, SAMPLES_LEN);
		//	else
		//		samples_uart_write(NULL, 0);
		//	g_control_read_ready = 0;
		//}
	}

	return 0;
} //}}}
