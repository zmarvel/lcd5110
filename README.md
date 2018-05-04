
# LCD5110

Prototyping a driver for Nokia 5110/3310 LCD with PCD8544 LCD controller.

Hardware: STM32L432KC Nucleo-32 development board.

## Pins:

- SPI1:
  - CS: D10/PA11
  - MOSI: D11/PB5, AF 5
  - MISO: D12/PB4, AF 5
  - SCK: A1/PA1, AF 5
- DC: D2/PA12
- RST: D4/PB7

# Building

I used ChibiOS 18.2.0. You can download it from [SourceForge][chibios_sf], or
you can clone it from their [Github][chibios_github] and checkout the
`stable_18.2.x` branch. I symlinked the downloaded folder to `ChibiOS` in the
project root, but you can modify the `CHIBIOS` variable in the Makefile if you
want.

Make sure you have `arm-none-eabi-gcc` to compile.

# Flashing

To load code onto the board and use GDB, I used `openocd` (there are Debian
packages available). You could use the `stlink` [tools][stlink] from texane too.

Using OpenOCD, you can just use `make download` to flash the code and
`make openocd` to start a GDB server. I have included a `.gdbinit` script to
set up GDB appropriately.

[chibios_sf]: https://sourceforge.net/projects/chibios/files/
[chibios_github]: https://github.com/ChibiOS/ChibiOS
[stlink]: https://github.com/texane/stlink
