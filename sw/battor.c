#include "common.h"

#include "uart.h"

char g_verb = 0;

int main(int argc, char** argv)
{
        char* tty = DEFAULT_TTY;

        uint16_t gain = GAIN_DEFAULT;
        samples_config sconf;
        eeprom_params* eeparams = &sconf.eeparams;

        // clear sample config
        memset(&sconf, 0, sizeof(sconf));

        fprintf(stderr, "Connecting to BattOr on port /dev/ttyUSB0\n");

        uart_init(tty);

        // always update the rtc time
        if (param_write_rtc() < 0)
        {
                fprintf(stderr, "Error: Failed to update RTC.\n");
                return EXIT_FAILURE;
        }

        // init the battor 
        control(CONTROL_TYPE_INIT, 0, 0, 1);

        // read the BattOr's calibration params from its EEPROM
        if (param_read_eeprom(eeparams) < 0)
        {
                fprintf(stderr, "Error: EEPROM read failure or BattOr not calibrated\n");
                return EXIT_FAILURE;
        }

        // set low gain (default)
        sconf.gain = eeparams->gainL;

        // configuration
        control(CONTROL_TYPE_GAIN_SET, gain, 0, 1);

        sconf.sample_rate = eeparams->uart_sr;
        control(CONTROL_TYPE_START_SAMPLING_UART, 0, 0, 1);
        samples_init(&sconf);
        samples_print_config(&sconf);
        samples_print_loop(&sconf);

        return EXIT_SUCCESS;
}
