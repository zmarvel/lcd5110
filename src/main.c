/*
    ChibiOS - Copyright (C) 2006..2018 Giovanni Di Sirio

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
*/

#include "ch.h"
#include "hal.h"
#include "LCD5110.h"


SPIConfig spicfg = {
  .end_cb = NULL,
  //.ssline = LINE_ARD_D10,
  .ssport = GPIOA,
  .sspad = GPIOA_ARD_D10,
  .cr1 = SPI_CR1_BR_Msk,
  .cr2 = 0,
};


/*
 * Green LED blinker thread, times are in milliseconds.
 */
static THD_WORKING_AREA(waThread1, 128);
static THD_FUNCTION(Thread1, arg) {

  (void)arg;
  chRegSetThreadName("blinker");
  while (true) {
    palClearLine(LINE_LED_GREEN);
    chThdSleepMilliseconds(500);
    palSetLine(LINE_LED_GREEN);
    chThdSleepMilliseconds(500);
  }
}

/*
 * Application entry point.
 */
int main(void) {

  /*
   * System initializations.
   * - HAL initialization, this also initializes the configured device drivers
   *   and performs the board-specific initializations.
   * - Kernel initialization, the main() function becomes a thread and the
   *   RTOS is active.
   */
  halInit();
  chSysInit();

  /*
   * Activates the serial driver 2 using the driver default configuration.
   */
  sdStart(&SD2, NULL);

  /*
   * Creates the blinker thread.
   */
  chThdCreateStatic(waThread1, sizeof(waThread1), NORMALPRIO, Thread1, NULL);


  /* SPID1
   * CS:   D10/PA11
   * MOSI: D11/PB5
   * MISO: D12/PB4
   * SCK:  D13/PB3
   *       A1/PA1
   */
  palSetLine(LINE_ARD_D10);
  palSetLineMode(LINE_ARD_D10, PAL_MODE_OUTPUT_PUSHPULL);
  palSetLineMode(LINE_ARD_A1, PAL_MODE_ALTERNATE(5));
  palSetLineMode(LINE_ARD_D11, PAL_MODE_ALTERNATE(5));
  palSetLineMode(LINE_ARD_D12, PAL_MODE_ALTERNATE(5));

  spiStart(&SPID1, &spicfg);
  chThdSleepMilliseconds(3);

  /* DC:  D2
   * RST: D4
   */
  palSetLine(LINE_ARD_D2);
  palSetLineMode(LINE_ARD_D2, PAL_MODE_OUTPUT_PUSHPULL);
  palSetLine(LINE_ARD_D4);
  palSetLineMode(LINE_ARD_D4, PAL_MODE_OUTPUT_PUSHPULL);

  LCD5110 lcd = {
    .spid = &SPID1,
    .dcLine = LINE_ARD_D2,
    .rstLine = LINE_ARD_D4
  };


  LCD5110_sendCommand(&lcd, LCD5110_FUNC_SET |
                      LCD5110_FUNC_ACTIVE |
                      LCD5110_FUNC_ADDR_H |
                      LCD5110_FUNC_MODE_EXT);
  LCD5110_sendCommand(&lcd, 0x90);
  LCD5110_sendCommand(&lcd, 0x20);
  LCD5110_sendCommand(&lcd, 0x0c);
  uint8_t buf = 0x1f;
  LCD5110_sendData(&lcd, 1, &buf);

  /*
   * Normal main() thread activity, in this demo it does nothing except
   * sleeping in a loop and check the button state.
   */
  while (true) {
    chThdSleepMilliseconds(500);
  }
}
