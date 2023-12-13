///////////////////////////////////////////////////////////////////////////
////                                                                   ////
////                              ST7789.c                             ////
////                                                                   ////
////               ST7789 display driver for CCS C compiler            ////
////                                                                   ////
///////////////////////////////////////////////////////////////////////////
////                                                                   ////
////               This is a free software with NO WARRANTY.           ////
////                     https://simple-circuit.com/                   ////
////                                                                   ////
///////////////////////////////////////////////////////////////////////////
/**************************************************************************
  This is a library for several Adafruit displays based on ST77* drivers.

  Works with the Adafruit 1.8" TFT Breakout w/SD card
    ----> http://www.adafruit.com/products/358
  The 1.8" TFT shield
    ----> https://www.adafruit.com/product/802
  The 1.44" TFT breakout
    ----> https://www.adafruit.com/product/2088
  as well as Adafruit raw 1.8" TFT display
    ----> http://www.adafruit.com/products/618

  Check out the links above for our tutorials and wiring diagrams.
  These displays use SPI to communicate, 4 or 5 pins are required to
  interface (RST is optional).

  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.
  MIT license, all text above must be included in any redistribution
 **************************************************************************/



#include "../ST7789.h"


void init_SSI0(void) {
    // Enable the SSI0 peripheral
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_SSI0);

    // Configure the pin muxing for SSI0 functions on port A2, A4, and A5.
    GPIOPinConfigure(GPIO_PA2_SSI0CLK);
    GPIOPinConfigure(GPIO_PA4_SSI0RX);
    GPIOPinConfigure(GPIO_PA5_SSI0TX);

    // Configure the GPIO settings for the SSI pins. This function also gives control of these pins to the SSI hardware.
    GPIOPinTypeSSI(GPIO_PORTA_BASE, SSI0_TX | SSI0_RX | SSI0_CLK);

    // Configure and enable the SSI port for SPI master mode.
    SSIConfigSetExpClk(SSI0_BASE, SysCtlClockGet(), SSI_FRF_MOTO_MODE_2, SSI_MODE_MASTER, 1000000, 8);

    // Enable the SSI0 module.
    SSIEnable(SSI0_BASE);
}


void SPI_XFER(uint32_t base, uint8_t data) {
    SSIDataPut(base, data);
    while(SSIBusy(base));
}


#include <stdint.h>
#ifndef bool
#define bool int1
#endif


#ifndef ST7789_SPI_XFER
#define ST7789_SPI_XFER(x) SPI_XFER(SSI0_BASE, x)
#endif



// SCREEN INITIALIZATION ***************************************************

// Rather than a bazillion writecommand() and writedata() calls, screen
// initialization commands and arguments are organized in these tables
// stored in PROGMEM.  The table may look bulky, but that's mostly the
// formatting -- storage-wise this is hundreds of bytes more compact
// than the equivalent code.  Companion function follows.

#define ST7789_240x240_XSTART 0
#define ST7789_240x240_YSTART 80

const uint8_t
  cmd_240x240[] =  {                // Init commands for 7789 screens
    9,                              //  9 commands in list:
    ST77XX_SWRESET,   ST_CMD_DELAY, //  1: Software reset, no args, w/delay
      150,                          //    150 ms delay
    ST77XX_SLPOUT ,   ST_CMD_DELAY, //  2: Out of sleep mode, no args, w/delay
      255,                          //     255 = 500 ms delay
    ST77XX_COLMOD , 1+ST_CMD_DELAY, //  3: Set color mode, 1 arg + delay:
      0x55,                         //     16-bit color
      10,                           //     10 ms delay
    ST77XX_MADCTL , 1,              //  4: Mem access ctrl (directions), 1 arg:
      0x08,                         //     Row/col addr, bottom-top refresh
    ST77XX_CASET  , 4,              //  5: Column addr set, 4 args, no delay:
      0x00,
      ST7789_240x240_XSTART,        //     XSTART = 0
      (240+ST7789_240x240_XSTART)>>8,
      (240+ST7789_240x240_XSTART)&0xFF,  //     XEND = 240
    ST77XX_RASET  , 4,              //  6: Row addr set, 4 args, no delay:
      0x00,
      ST7789_240x240_YSTART,             //     YSTART = 0
      (240+ST7789_240x240_YSTART)>>8,
      (240+ST7789_240x240_YSTART)&0xFF,  //     YEND = 240
    ST77XX_INVON  ,   ST_CMD_DELAY,  //  7: hack
      10,
    ST77XX_NORON  ,   ST_CMD_DELAY, //  8: Normal display on, no args, w/delay
      10,                           //     10 ms delay
    ST77XX_DISPON ,   ST_CMD_DELAY, //  9: Main screen turn on, no args, delay
    255 };                          //     255 = max (500 ms) delay

//*************************** User Functions ***************************//
void tft_init(void);

void drawPixel(uint8_t x, uint8_t y, uint16_t color);
void drawHLine(uint8_t x, uint8_t y, uint8_t w, uint16_t color);
void drawVLine(uint8_t x, uint8_t y, uint8_t h, uint16_t color);
void fillRect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint16_t color);
void fillScreen(uint16_t color);
void setRotation(uint8_t m);
void invertDisplay(bool i);
void pushColor(uint16_t color);

//************************* Non User Functions *************************//
void startWrite(void);
void endWrite(void);
void displayInit(const uint8_t *addr);
void writeCommand(uint8_t cmd);
void setAddrWindow(uint8_t x, uint8_t y, uint8_t w, uint8_t h);

/**************************************************************************/
/*!
    @brief  Call before issuing command(s) or data to display. Performs
            chip-select (if required). Required
            for all display types; not an SPI-specific function.
*/
/**************************************************************************/
void startWrite(void) {
  #ifdef TFT_CS
    GPIOPinWrite(GPIO_PORTF_BASE, TFT_CS, 0);
  #endif
}

/**************************************************************************/
/*!
    @brief  Call after issuing command(s) or data to display. Performs
            chip-deselect (if required). Required
            for all display types; not an SPI-specific function.
*/
/**************************************************************************/
void endWrite(void) {
  #ifdef TFT_CS
    GPIOPinWrite(GPIO_PORTF_BASE, TFT_CS, TFT_CS);
  #endif
}

/**************************************************************************/
/*!
    @brief  Write a single command byte to the display. Chip-select and
            transaction must have been previously set -- this ONLY sets
            the device to COMMAND mode, issues the byte and then restores
            DATA mode. There is no corresponding explicit writeData()
            function -- just use ST7789_SPI_XFER().
    @param  cmd  8-bit command to write.
*/
/**************************************************************************/
void writeCommand(uint8_t cmd) {
    GPIOPinWrite(GPIO_PORTF_BASE, TFT_DC, 0);
  ST7789_SPI_XFER(cmd);
  GPIOPinWrite(GPIO_PORTF_BASE, TFT_DC, TFT_DC);
}

/**************************************************************************/
/*!
    @brief  Companion code to the initiliazation tables. Reads and issues
            a series of LCD commands stored in const byte array.
    @param  addr  Flash memory array with commands and data to send
*/
/**************************************************************************/
void displayInit(const uint8_t *addr){
  uint8_t  numCommands, numArgs;
  uint16_t ms;
  startWrite();

  numCommands = *addr++;   // Number of commands to follow
  
  while(numCommands--) {                 // For each command...

    writeCommand(*addr++); // Read, issue command
    numArgs  = *addr++;    // Number of args to follow
    ms       = numArgs & ST_CMD_DELAY;   // If hibit set, delay follows args
    numArgs &= ~ST_CMD_DELAY;            // Mask out delay bit
    while(numArgs--) {                   // For each argument...
      ST7789_SPI_XFER(*addr++);   // Read, issue argument
    }

    if(ms) {
      ms = *addr++; // Read post-command delay time (ms)
      if(ms == 255) ms = 500;     // If 255, delay for 500 ms
      delay_ms(ms);
    }
  }
  endWrite();
}

/**************************************************************************/
/*!
    @brief  Initialization code for ST7789 display
*/
/**************************************************************************/
void tft_init(void) {
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, TFT_CS);
    GPIOPinWrite(GPIO_PORTF_BASE, TFT_CS, TFT_CS);
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, TFT_DC);

  init_SSI0();

  displayInit(cmd_240x240);

  _colstart = ST7789_240x240_XSTART;
  _rowstart = ST7789_240x240_YSTART;
  _height   = 240;
  _width    = 240;
  setRotation(3);
}

/**************************************************************************/
/*!
  @brief  SPI displays set an address window rectangle for blitting pixels
  @param  x  Top left corner x coordinate
  @param  y  Top left corner x coordinate
  @param  w  Width of window
  @param  h  Height of window
*/
/**************************************************************************/
void setAddrWindow(uint8_t x, uint8_t y, uint8_t w, uint8_t h) {
  x += _xstart;
  y += _ystart;

  writeCommand(ST77XX_CASET); // Column addr set
  ST7789_SPI_XFER(0);
  ST7789_SPI_XFER(x);
  ST7789_SPI_XFER(0);
  ST7789_SPI_XFER(x+w-1);

  writeCommand(ST77XX_RASET); // Row addr set
  ST7789_SPI_XFER(0);
  ST7789_SPI_XFER(y);
  ST7789_SPI_XFER(0);
  ST7789_SPI_XFER(y+h-1);

  writeCommand(ST77XX_RAMWR); // write to RAM
}

/**************************************************************************/
/*!
    @brief  Set origin of (0,0) and orientation of TFT display
    @param  m  The index for rotation, from 0-3 inclusive
*/
/**************************************************************************/
void setRotation(uint8_t m) {
  uint8_t madctl = 0;

  rotation = m & 3; // can't be higher than 3

  switch (rotation) {
   case 0:
     madctl  = ST77XX_MADCTL_MX | ST77XX_MADCTL_MY | ST77XX_MADCTL_RGB;
     _xstart = _colstart;
     _ystart = _rowstart;
     break;
   case 1:
     madctl  = ST77XX_MADCTL_MY | ST77XX_MADCTL_MV | ST77XX_MADCTL_RGB;
     _xstart = _rowstart;
     _ystart = _colstart;
     break;
  case 2:
     madctl  = ST77XX_MADCTL_RGB;
     _xstart = 0;
     _ystart = 0;
     break;
   case 3:
     madctl  = ST77XX_MADCTL_MX | ST77XX_MADCTL_MV | ST77XX_MADCTL_RGB;
     _xstart = 0;
     _ystart = 0;
     break;
  }
  startWrite();
  writeCommand(ST77XX_MADCTL);
  ST7789_SPI_XFER(madctl);
  endWrite();
}

void drawPixel(uint8_t x, uint8_t y, uint16_t color) {
  if((x < _width) && (y < _height)) {
    startWrite();
    setAddrWindow(x, y, 1, 1);
    ST7789_SPI_XFER(color >> 8);
    ST7789_SPI_XFER(color & 0xFF);
    endWrite();
  }
}

/**************************************************************************/
/*!
   @brief    Draw a perfectly horizontal line (this is often optimized in a subclass!)
    @param    x   Left-most x coordinate
    @param    y   Left-most y coordinate
    @param    w   Width in pixels
   @param    color 16-bit 5-6-5 Color to fill with
*/
/**************************************************************************/
void drawHLine(uint8_t x, uint8_t y, uint8_t w, uint16_t color) {
  if( (x < _width) && (y < _height) && w) {   
    uint8_t hi = color >> 8, lo = color;

    if((x >= _width) || (y >= _height))
      return;
    if((x + w - 1) >= _width)  
      w = _width  - x;
    startWrite();
    setAddrWindow(x, y, w, 1);
    while (w--) {
      ST7789_SPI_XFER(hi);
      ST7789_SPI_XFER(lo);
    }
    endWrite();
  }
}

/**************************************************************************/
/*!
   @brief    Draw a perfectly vertical line (this is often optimized in a subclass!)
    @param    x   Top-most x coordinate
    @param    y   Top-most y coordinate
    @param    h   Height in pixels
   @param    color 16-bit 5-6-5 Color to fill with
*/
/**************************************************************************/
void drawVLine(uint8_t x, uint8_t y, uint8_t h, uint16_t color) {
  if( (x < _width) && (y < _height) && h) {  
    uint8_t hi = color >> 8, lo = color;
    if((y + h - 1) >= _height) 
      h = _height - y;
    startWrite();
    setAddrWindow(x, y, 1, h);
    while (h--) {
      ST7789_SPI_XFER(hi);
      ST7789_SPI_XFER(lo);
    }
    endWrite();
  }
}

/**************************************************************************/
/*!
   @brief    Fill a rectangle completely with one color. Update in subclasses if desired!
    @param    x   Top left corner x coordinate
    @param    y   Top left corner y coordinate
    @param    w   Width in pixels
    @param    h   Height in pixels
   @param    color 16-bit 5-6-5 Color to fill with
*/
/**************************************************************************/
void fillRect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint16_t color) {
  if(w && h) {                            // Nonzero width and height?  
    uint8_t hi = color >> 8, lo = color;
    if((x >= _width) || (y >= _height))
      return;
    if((x + w - 1) >= _width)  
      w = _width  - x;
    if((y + h - 1) >= _height) 
      h = _height - y;
    startWrite();
    setAddrWindow(x, y, w, h);
    uint16_t px = (uint16_t)w * h;
    while (px--) {
      ST7789_SPI_XFER(hi);
      ST7789_SPI_XFER(lo);
    }
    endWrite();
  }
}

/**************************************************************************/
/*!
   @brief    Fill the screen completely with one color. Update in subclasses if desired!
    @param    color 16-bit 5-6-5 Color to fill with
*/
/**************************************************************************/
void fillScreen(uint16_t color) {
    fillRect(0, 0, _width, _height, color);
}

/**************************************************************************/
/*!
    @brief  Invert the colors of the display (if supported by hardware).
            Self-contained, no transaction setup required.
    @param  i  true = inverted display, false = normal display.
*/
/**************************************************************************/
void invertDisplay(bool i) {
    startWrite();
    writeCommand(i ? ST77XX_INVON : ST77XX_INVOFF);
    endWrite();
}

/*!
    @brief  Essentially writePixel() with a transaction around it. I don't
            think this is in use by any of our code anymore (believe it was
            for some older BMP-reading examples), but is kept here in case
            any user code relies on it. Consider it DEPRECATED.
    @param  color  16-bit pixel color in '565' RGB format.
*/
void pushColor(uint16_t color) {
    uint8_t hi = color >> 8, lo = color;
    startWrite();
    ST7789_SPI_XFER(hi);
    ST7789_SPI_XFER(lo);
    endWrite();
}

// end of code.
