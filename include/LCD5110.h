
#include <stdint.h>
#include "ch.h"
#include "hal.h"

#define LCD5110_FUNC_SET 0x20
/* PD: 0=active, 1=power-down mode */
#define LCD5110_FUNC_PD_MASK (1 << 2)
#define LCD5110_FUNC_ACTIVE 0
#define LCD5110_FUNC_POWERDOWN (1 << 2)
/* 0=horizontal, 1=vertical addressing */
#define LCD5110_FUNC_ADDR_MASK (1 << 1)
#define LCD5110_FUNC_ADDR_H 0
#define LCD5110_FUNC_ADDR_V (1 << 1)
/* 0=basic mode, 1=extended mode */
#define LCD5110_FUNC_MODE (1 << 0)
#define LCD5110_FUNC_MODE_BASIC 0
#define LCD5110_FUNC_MODE_EXT (1 << 0)


/* Basic mode commands */

#define LCD5110_DISPLAY_CNTL 0x08
#define LCD5110_DISPLAY_CNTL_MASK 0x05
#define LCD5110_DISPLAY_BLANK 0x00
#define LCD5110_DISPLAY_NORMAL 0x04
#define LCD5110_DISPLAY_ALL_ON 0x01
#define LCD5110_DISPLAY_INVERSE 0x05


#define LCD5110_Y_ADDR 0x40
#define LCD5110_X_ADDR 0x80


/* Extended mode commands */

/* Temperature coefficient */
#define LCD5110_TEMP_MASK 0x03
#define LCD5110_TEMP (1 << 2)
#define LCD5110_BIAS_MASK 0x07
#define LCD5110_BIAS (1 << 4)

#define LCD5110_VOP (1 << 7)
#define LCD5110_VOP_MASK 0x7f


typedef struct {
  SPIDriver *spid;
  uint32_t dcLine;
  uint32_t rstLine;
} LCD5110;




void LCD5110_sendCommand(LCD5110 *dev, uint8_t cmd);
void LCD5110_sendData(LCD5110 *dev, size_t n, uint8_t *data);
