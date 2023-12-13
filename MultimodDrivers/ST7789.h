#ifndef ST7789_H
#define ST7789_H

#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/ssi.h"
#include "driverlib/sysctl.h"
#include "driverlib/rom.h"

// Let's define some constants used in the code:
#define SSI0_CLK GPIO_PIN_2
#define SSI0_RX GPIO_PIN_4
#define SSI0_TX GPIO_PIN_5

#define ST_CMD_DELAY      0x80    // special signifier for command lists

#define ST77XX_NOP        0x00
#define ST77XX_SWRESET    0x01
#define ST77XX_RDDID      0x04
#define ST77XX_RDDST      0x09

#define ST77XX_SLPIN      0x10
#define ST77XX_SLPOUT     0x11
#define ST77XX_PTLON      0x12
#define ST77XX_NORON      0x13

#define ST77XX_INVOFF     0x20
#define ST77XX_INVON      0x21
#define ST77XX_DISPOFF    0x28
#define ST77XX_DISPON     0x29
#define ST77XX_CASET      0x2A
#define ST77XX_RASET      0x2B
#define ST77XX_RAMWR      0x2C
#define ST77XX_RAMRD      0x2E

#define ST77XX_PTLAR      0x30
#define ST77XX_COLMOD     0x3A
#define ST77XX_MADCTL     0x36

#define ST77XX_MADCTL_MY  0x80
#define ST77XX_MADCTL_MX  0x40
#define ST77XX_MADCTL_MV  0x20
#define ST77XX_MADCTL_ML  0x10
#define ST77XX_MADCTL_RGB 0x00

#define ST77XX_RDID1      0xDA
#define ST77XX_RDID2      0xDB
#define ST77XX_RDID3      0xDC
#define ST77XX_RDID4      0xDD

// Some ready-made 16-bit ('565') color settings:
#define   ST7789_BLACK   0x0000
#define   ST7789_BLUE    0x001F
#define   ST7789_RED     0xF800
#define   ST7789_GREEN   0x07E0
#define   ST7789_CYAN    0x07FF
#define   ST7789_MAGENTA 0xF81F
#define   ST7789_YELLOW  0xFFE0
#define   ST7789_WHITE   0xFFFF

#define TFT_CS      GPIO_PIN_4  // Chip select line
#define TFT_DC      GPIO_PIN_3  // Data/command select line
#define TFT_CS_PORT GPIO_PORTF_BASE
#define TFT_DC_PORT GPIO_PORTF_BASE

#define delay_ms(MS) SysCtlDelay((SysCtlClockGet()/(3*1000))*MS+1)
#define output_high(PIN) GPIOPinWrite(TFT_CS_PORT, PIN, PIN)
#define output_low(PIN) GPIOPinWrite(TFT_CS_PORT, PIN, 0)
#define output_drive(PIN) GPIOPinTypeGPIOOutput(TFT_CS_PORT, PIN)


uint8_t
  _width,     ///< Display width as modified by current rotation
  _height,    ///< Display height as modified by current rotation
  _xstart,    ///< Internal framebuffer X offset
  _ystart,    ///< Internal framebuffer Y offset
  _colstart,  ///< Some displays need this changed to offset
  _rowstart,  ///< Some displays need this changed to offset
  rotation;   ///< Display rotation (0 thru 3)

// Function Prototypes

void init_SSI0(void);
void SPI_XFER(uint32_t base, uint8_t data);
void tft_init(void);
void drawPixel(uint8_t x, uint8_t y, uint16_t color);
void drawHLine(uint8_t x, uint8_t y, uint8_t w, uint16_t color);
void drawVLine(uint8_t x, uint8_t y, uint8_t h, uint16_t color);
void fillRect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint16_t color);
void fillScreen(uint16_t color);
void setRotation(uint8_t m);
void invertDisplay(bool i);
void pushColor(uint16_t color);
void startWrite(void);
void endWrite(void);
void displayInit(const uint8_t *addr);
void writeCommand(uint8_t cmd);
void setAddrWindow(uint8_t x, uint8_t y, uint8_t w, uint8_t h);

#endif // ST7789_H
