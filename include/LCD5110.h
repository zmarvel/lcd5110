
#include <stdint.h>
#include "ch.h"
#include "hal.h"

#define LCD5110_WIDTH 84
#define LCD5110_HEIGHT 48

#define LCD5110_FUNC_SET (1 << 5)
#define LCD5110_FUNC_MASK 0x07
/* PD: 0=active, 1=power-down mode */
#define LCD5110_FUNC_PD_MASK (1 << 2)
typedef enum {
  LCD5110_POWER_MODE_ACTIVE = 0,
  LCD5110_POWER_MODE_POWERDOWN = (1 << 2),
} LCD5110PowerMode;
/* 0=horizontal, 1=vertical addressing */
#define LCD5110_FUNC_ADDR_MASK (1 << 1)
typedef enum {
  LCD5110_ADDR_HORIZ = 0,
  LCD5110_ADDR_VERT = (1 << 1),
} LCD5110AddressMode;
/* 0=basic mode, 1=extended mode */
#define LCD5110_FUNC_MODE (1 << 0)
#define LCD5110_FUNC_MODE_MASK (1 << 0)
typedef enum {
  LCD5110_INSTR_MODE_BASIC = 0,
  LCD5110_INSTR_MODE_EXT = (1 << 0),
} LCD5110InstrMode;


/* Basic mode commands */

#define LCD5110_DISPLAY_CNTL (1 << 3)
#define LCD5110_DISPLAY_CNTL_MASK 0x05
typedef enum {
  LCD5110_DISPLAY_MODE_BLANK = 0x00,
  LCD5110_DISPLAY_MODE_NORMAL = 0x04,
  LCD5110_DISPLAY_MODE_ALL_ON = 0x01,
  LCD5110_DISPLAY_MODE_INVERSE = 0x05,
} LCD5110DisplayMode;


#define LCD5110_Y_ADDR (1 << 6)
#define LCD5110_Y_ADDR_MASK 0x07
#define LCD5110_X_ADDR (1 << 7)
#define LCD5110_X_ADDR_MASK 0x3f


/* Extended mode commands */

/* Temperature coefficient */
#define LCD5110_TEMP_MASK 0x03
#define LCD5110_TEMP (1 << 2)

#define LCD5110_BIAS_MASK 0x07
#define LCD5110_BIAS (1 << 4)

#define LCD5110_VOP (1 << 7)
#define LCD5110_VOP_MASK 0x7f


typedef struct {
  uint8_t x;
  uint8_t y;
  uint8_t width;
  uint8_t height;
} Rect;


void LCD5110_sendCommand(uint8_t cmd);
void LCD5110_sendData(size_t n, uint8_t *data);
void LCD5110_setFunction(LCD5110PowerMode pd,
                         LCD5110AddressMode v, LCD5110InstrMode h);
void LCD5110_setVOP(uint32_t vop);
void LCD5110_setDisplayMode(LCD5110DisplayMode mode);
void LCD5110_setAddressMode(LCD5110AddressMode mode);
void LCD5110_setRow(uint32_t row);
void LCD5110_setColumn(uint32_t col);
void LCD5110_draw(Rect box, uint8_t *data);
void LCD5110_blank(void);
