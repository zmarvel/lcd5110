
#include "LCD5110.h"


void LCD5110_sendCommand(LCD5110 *dev, uint8_t cmd) {
  spiAcquireBus(dev->spid);
  spiSelect(dev->spid);
  palClearLine(dev->dcLine);

  //spiPolledExchange(dev->spid, cmd);
  spiSend(dev->spid, 1, &cmd);

  palSetLine(dev->dcLine);
  spiUnselect(dev->spid);
  spiReleaseBus(dev->spid);
}

void LCD5110_sendData(LCD5110 *dev, size_t n, uint8_t *data) {
  spiAcquireBus(dev->spid);
  spiSelect(dev->spid);
  palSetLine(dev->dcLine);

  spiSend(dev->spid, n, data);

  palClearLine(dev->dcLine);
  spiUnselect(dev->spid);
  spiReleaseBus(dev->spid);
}

