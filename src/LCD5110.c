
#include "LCD5110.h"


void LCD5110_sendCommand(LCD5110Driver *dev, uint8_t cmd) {
  spiAcquireBus(dev->spid);
  spiSelect(dev->spid);
  palClearLine(dev->config->dcLine);

  //spiPolledExchange(dev->spid, cmd);
  spiSend(dev->spid, 1, &cmd);

  palSetLine(dev->config->dcLine);
  spiUnselect(dev->spid);
  spiReleaseBus(dev->spid);
}

void LCD5110_sendData(LCD5110Driver *dev, size_t n, uint8_t *data) {
  spiAcquireBus(dev->spid);
  spiSelect(dev->spid);
  palSetLine(dev->config->dcLine);

  spiSend(dev->spid, n, data);

  palClearLine(dev->config->dcLine);
  spiUnselect(dev->spid);
  spiReleaseBus(dev->spid);
}

void LCD5110_setFunction(LCD5110Driver *dev, LCD5110PowerMode pd,
                         LCD5110AddressMode v, LCD5110InstrMode h) {
  uint8_t func = (pd << 2) | (v << 1) | h;
  LCD5110_sendCommand(dev, LCD5110_FUNC_SET | func);
}

void LCD5110_setVOP(LCD5110Driver *dev, uint32_t vop) {
  vop &= 0x7f;
  LCD5110_sendCommand(dev, LCD5110_VOP | vop);
}

void LCD5110_setDisplayMode(LCD5110Driver *dev, LCD5110DisplayMode mode) {
  LCD5110_sendCommand(dev, LCD5110_DISPLAY_CNTL | mode);
}

void LCD5110_setAddressMode(LCD5110Driver *dev, LCD5110AddressMode mode) {
  LCD5110_sendCommand(dev, LCD5110_DISPLAY_CNTL | mode);
}

void LCD5110_setRow(LCD5110Driver *dev, uint32_t row) {
  if (row > 40)
    row = 40;

  row >>= 3;
  LCD5110_sendCommand(dev, LCD5110_Y_ADDR | row);
}

void LCD5110_setColumn(LCD5110Driver *dev, uint32_t col) {
  if (col > 83)
    col = 83;
  LCD5110_sendCommand(dev, LCD5110_X_ADDR | col);
}


