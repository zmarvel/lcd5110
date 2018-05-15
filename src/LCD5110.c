
#include <string.h>
#include "LCD5110.h"
#include "pins.h"


void LCD5110_sendCommand(uint8_t cmd) {
  spiAcquireBus(&SPID1);
  palClearLine(LCD5110_LINE_DC);
  spiSelect(&SPID1);

  spiPolledExchange(&SPID1, cmd);

  spiUnselect(&SPID1);
  spiReleaseBus(&SPID1);
}

void LCD5110_sendData(size_t n, uint8_t *data) {
  spiAcquireBus(&SPID1);
  palSetLine(LCD5110_LINE_DC);
  spiSelect(&SPID1);

  spiSend(&SPID1, n, data);

  spiUnselect(&SPID1);
  spiReleaseBus(&SPID1);
}

void LCD5110_setFunction(LCD5110PowerMode pd,
                         LCD5110AddressMode v, LCD5110InstrMode h) {
  uint8_t func = (pd << 2) | (v << 1) | h;
  LCD5110_sendCommand(LCD5110_FUNC_SET | func);
}

void LCD5110_setVOP(uint32_t vop) {
  vop &= 0x7f;
  LCD5110_sendCommand(LCD5110_VOP | vop);
}

void LCD5110_setDisplayMode(LCD5110DisplayMode mode) {
  LCD5110_sendCommand(LCD5110_DISPLAY_CNTL | mode);
}

void LCD5110_setAddressMode(LCD5110AddressMode mode) {
  LCD5110_sendCommand(LCD5110_FUNC_SET | mode);
}

void LCD5110_setRow(uint32_t row) {
  if (row > 40)
    row = 40;

  /* Rows are addressed by "bank", or groups of 8 vertical bytes. */
  row >>= 3;
  LCD5110_sendCommand(LCD5110_Y_ADDR | row);
}

void LCD5110_setColumn(uint32_t col) {
  if (col > 83)
    col = 83;
  LCD5110_sendCommand(LCD5110_X_ADDR | col);
}




/* data should contain width*(height/8) bytes */

void LCD5110_draw(Rect box, uint8_t *data) {
  /* We can take advantage of address auto-increment if box is screen width */
  if (box.width == LCD5110_WIDTH) {
    LCD5110_setRow(box.y);
    LCD5110_setColumn(box.x);
    LCD5110_sendData(box.width*box.height/8, data);
    return;
  }

  for (int i = 0, bank = box.y;
       bank < box.y + box.height;
       i += box.width, bank += 8) {
    LCD5110_setRow(bank);
    LCD5110_setColumn(box.x);

    LCD5110_sendData(box.width, &data[i]);
  }
}

const uint8_t rectRow[84] = {
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff
};
void LCD5110_drawRect(Rect box) {
  /* We can take advantage of address auto-increment if box is screen width */
  if (box.width == LCD5110_WIDTH) {
    LCD5110_setRow(box.y);
    LCD5110_setColumn(box.x);
    LCD5110_sendData(box.width*box.height/8, (uint8_t *)rectRow);
    return;
  }

  for (int i = 0, bank = box.y;
       bank < box.y + box.height;
       i += box.width, bank += 8) {
    LCD5110_setRow(bank);
    LCD5110_setColumn(box.x);

    LCD5110_sendData(box.width, (uint8_t *)rectRow);
  }
}

void LCD5110_blank(void) {
  uint8_t row[84];
  memset(row, 0, 84);

  LCD5110_setRow(0);
  LCD5110_setColumn(0);
  for (int bank = 0; bank < 6; bank++) {
    LCD5110_sendData(84, row);
  }
}


