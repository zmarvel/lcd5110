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

#include <string.h>
#include "ch.h"
#include "hal.h"
#include "pins.h"
#include "LCD5110.h"


SPIConfig spicfg = {
  .end_cb = NULL,
  .ssport = GPIOA,
  .sspad = GPIOA_LCD5110_CS,
  .cr1 = SPI_CR1_BR_1,
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

bool lcdInitialized = false;
static THD_WORKING_AREA(waThread2, 128);
static THD_FUNCTION(Thread2, arg) {

  (void)arg;
  chRegSetThreadName("backlight");
  while (true) {
    if (lcdInitialized) {
      palClearLine(LINE_LCD5110_LIGHT);
      chThdSleepMilliseconds(1);
      palSetLine(LINE_LCD5110_LIGHT);
      chThdSleepMilliseconds(2);
    } else {
      chThdSleepMilliseconds(200);
    }
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
  /* Pin setup done in board.h */

  /* Reset LCD */
  palSetLine(LINE_LCD5110_RST);

  spiStop(&SPID1);
  chThdSleepMilliseconds(1);
  spiStart(&SPID1, &spicfg);
  chThdSleepMilliseconds(3);

  //LCD5110_sendCommand(0x21);
  //LCD5110_sendCommand(0x90);
  //LCD5110_sendCommand(0x20);
  //LCD5110_sendCommand(0x0c);

  LCD5110_setFunction(LCD5110_POWER_MODE_ACTIVE, LCD5110_ADDR_HORIZ,
                      LCD5110_INSTR_MODE_EXT);
  LCD5110_setVOP(16);
  LCD5110_sendCommand(LCD5110_TEMP | 0x03);
  LCD5110_sendCommand(LCD5110_BIAS | 0);
  LCD5110_setFunction(LCD5110_POWER_MODE_ACTIVE, LCD5110_ADDR_HORIZ,
                      LCD5110_INSTR_MODE_BASIC);
  LCD5110_setDisplayMode(LCD5110_DISPLAY_MODE_NORMAL);

  //LCD5110_setRow(0);
  //LCD5110_setColumn(0);
}

#define MAX(a, b) (((a) < (b)) ? (b) : (a))
#define MIN(a, b) (((a) < (b)) ? (a) : (b))

extern const uint8_t font8x8[96][8];
extern const uint8_t font4x8[128][4];
//uint8_t stringBuf[64][8];
//void lcdDrawString(uint8_t x, uint8_t y, char *s) {
//  //uint8_t idx;
//  size_t n = strnlen(s, LCD5110_WIDTH*LCD5110_HEIGHT/(8*8));
//  int row = 0;
//  int width = LCD5110_WIDTH-x-1;
//  size_t maxlinelen = width / 8;
//  for (size_t i = 0; i < n; i += maxlinelen) {
//    int linelen = MIN(maxlinelen, n-i);
//    //idx = font8x8_lut[(uint8_t)s[i]];
//    for (int j = i; j < linelen*8; j++)
//      memcpy(stringBuf[j], font8x8[(uint8_t)s[j]-32], 8);
//
//
//    LCD5110_setRow(y+8*row);
//    LCD5110_setColumn(x);
//    LCD5110_sendData(linelen*8, stringBuf[i]);
//    row++;
//  }
//}

uint8_t stringBuf[64][4];
void lcdDrawString(uint8_t x, uint8_t y, char *s) {
  //uint8_t idx;
  size_t n = strnlen(s, LCD5110_WIDTH*LCD5110_HEIGHT/(4*8));
  int row = 0;
  int width = LCD5110_WIDTH-x;
  size_t maxlinelen = width / 4;
  for (size_t i = 0; i < n; i += maxlinelen) {
    int linelen = MIN(maxlinelen, n-i);
    //idx = font8x8_lut[(uint8_t)s[i]];
    for (int j = i; j < linelen*4; j++)
      memcpy(stringBuf[j], font4x8[(uint8_t)s[j]], 4);


    LCD5110_setRow(y+4*row);
    LCD5110_setColumn(x);
    LCD5110_sendData(linelen*4, stringBuf[i]);
    row++;
  }
}



uint8_t spinner[4][8] = {
  { 0x0f, 0x0f, 0x0f, 0x0f, 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00, 0x0f, 0x0f, 0x0f, 0x0f },
  { 0x00, 0x00, 0x00, 0x00, 0xf0, 0xf0, 0xf0, 0xf0 },
  { 0xf0, 0xf0, 0xf0, 0xf0, 0x00, 0x00, 0x00, 0x00 },
};


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
  chThdCreateStatic(waThread2, sizeof(waThread2), NORMALPRIO, Thread2, NULL);

  lcdInit();
  lcdInitialized = true;

  Rect box = {
    .x = 0,
    .y = 0,
    .width = 8,
    .height = 8
  };
  Rect box2 = {
    .x = 16,
    .y = 16,
    .width = 8,
    .height = 8
  };

  LCD5110_blank();

  lcdDrawString(0, 16, "Hello world");

  /*
   * Normal main() thread activity, in this demo it does nothing except
   * sleeping in a loop and check the button state.
   */
  int i = 0;
  while (true) {
    chThdSleepMilliseconds(500);
    LCD5110_draw(box, spinner[i++]);
    if (i > 3)
      i = 0;

  }
}
