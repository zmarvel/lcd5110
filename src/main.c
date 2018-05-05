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
#include "pins.h"
#include "LCD5110.h"


SPIConfig spicfg = {
  .end_cb = NULL,
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

/* SPID1
 * CS:   D10/PA11
 * MOSI: D11/PB5
 * MISO: D12/PB4
 * SCK:  D13/PB3
 *       A1/PA1
 * DC:  D2
 * RST: D4
 */
void lcdInit(void) {
  /* SPI setup */
  palSetLine(LCD5110_LINE_CS);
  palSetLineMode(LCD5110_LINE_CS, PAL_MODE_OUTPUT_PUSHPULL);
  palSetLineMode(LCD5110_LINE_SCK, PAL_MODE_ALTERNATE(5));
  palSetLineMode(LCD5110_LINE_MOSI, PAL_MODE_ALTERNATE(5));
  palSetLineMode(LCD5110_LINE_MISO, PAL_MODE_ALTERNATE(5));

  spiStart(&SPID1, &spicfg);
  chThdSleepMilliseconds(3);

  /* DC pin setup */
  palSetLine(LCD5110_LINE_DC);
  palSetLineMode(LCD5110_LINE_DC, PAL_MODE_OUTPUT_PUSHPULL);

  /* RST pin setup, reset LCD */
  palClearLine(LCD5110_LINE_RST);
  palSetLineMode(LCD5110_LINE_RST, PAL_MODE_OUTPUT_PUSHPULL);
  chThdSleepMilliseconds(3);
  palSetLine(LCD5110_LINE_RST);

  LCD5110_setFunction(LCD5110_POWER_MODE_ACTIVE, LCD5110_ADDR_HORIZ,
                      LCD5110_FUNC_MODE_EXT);
  LCD5110_setVOP(16);
  LCD5110_setFunction(LCD5110_POWER_MODE_ACTIVE, LCD5110_ADDR_HORIZ,
                      LCD5110_FUNC_MODE_BASIC);
  LCD5110_setDisplayMode(LCD5110_DISPLAY_NORMAL);
  LCD5110_setAddressMode(LCD5110_DISPLAY_NORMAL);

  LCD5110_setRow(0);
  LCD5110_setColumn(0);
  
  uint8_t buf[] = {
    0x1f, 0x05, 0x07, 0x00, 0x1f, 0x04, 0x1f, 0x00
  };
  for (int i = 0; i < 8; i++)
    LCD5110_sendData(8, buf);
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

  lcdInit();

  /*
   * Normal main() thread activity, in this demo it does nothing except
   * sleeping in a loop and check the button state.
   */
  while (true) {
    chThdSleepMilliseconds(500);
  }
}
