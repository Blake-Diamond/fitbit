/*
 * lcd.h
 *
 *  Created on: Oct 21, 2015
 *      Author: atlantis
 */

/*
  UTFT.h - Multi-Platform library support for Color TFT LCD Boards
  Copyright (C)2015 Rinky-Dink Electronics, Henning Karlsen. All right reserved
  
  This library is the continuation of my ITDB02_Graph, ITDB02_Graph16
  and RGB_GLCD libraries for Arduino and chipKit. As the number of 
  supported display modules and controllers started to increase I felt 
  it was time to make a single, universal library as it will be much 
  easier to maintain in the future.

  Basic functionality of this library was origianlly based on the 
  demo-code provided by ITead studio (for the ITDB02 modules) and 
  NKC Electronics (for the RGB GLCD module/shield).

  This library supports a number of 8bit, 16bit and serial graphic 
  displays, and will work with both Arduino, chipKit boards and select 
  TI LaunchPads. For a full list of tested display modules and controllers,
  see the document UTFT_Supported_display_modules_&_controllers.pdf.

  When using 8bit and 16bit display modules there are some 
  requirements you must adhere to. These requirements can be found 
  in the document UTFT_Requirements.pdf.
  There are no special requirements when using serial displays.

  You can find the latest version of the library at 
  http://www.RinkyDinkElectronics.com/

  This library is free software; you can redistribute it and/or
  modify it under the terms of the CC BY-NC-SA 3.0 license.
  Please see the included documents for further information.

  Commercial use of this library requires you to buy a license that
  will allow commercial use. This includes using the library,
  modified or not, as a tool to sell products.

  The license applies to all part of the library including the 
  examples and tools supplied with the library.
*/

#ifndef LCD_H_
#define LCD_H_

#include "xparameters.h"
#include "xil_io.h"
#include "xil_types.h"
#include "xspi_l.h"
#include "xil_printf.h"

#define SPI_DC          XPAR_SPI_DC_BASEADDR
#define B_RS            0x00000001

#define SPI_DTR         XPAR_SPI_BASEADDR + XSP_DTR_OFFSET
#define SPI_DRR         XPAR_SPI_BASEADDR + XSP_DRR_OFFSET
#define SPI_IISR        XPAR_SPI_BASEADDR + XSP_IISR_OFFSET
#define SPI_SR          XPAR_SPI_BASEADDR + XSP_SR_OFFSET

#define cbi(reg, bitmask)       Xil_Out32(reg, Xil_In32(reg) & ~(u32)bitmask)
#define sbi(reg, bitmask)       Xil_Out32(reg, Xil_In32(reg) |= (u32)bitmask)
#define swap(type, i, j)        {type t = i; i = j; j = t;}

#define DISP_X_SIZE     239
#define DISP_Y_SIZE     329
#define DISP_BLK_SIZE   30

#define ILI9341_BLACK       0x0000  ///<   0,   0,   0
#define ILI9341_NAVY        0x000F  ///<   0,   0, 123
#define ILI9341_DARKGREEN   0x03E0  ///<   0, 125,   0
#define ILI9341_DARKCYAN    0x03EF  ///<   0, 125, 123
#define ILI9341_MAROON      0x7800  ///< 123,   0,   0
#define ILI9341_PURPLE      0x780F  ///< 123,   0, 123
#define ILI9341_OLIVE       0x7BE0  ///< 123, 125,   0
#define ILI9341_LIGHTGREY   0xC618  ///< 198, 195, 198
#define ILI9341_DARKGREY    0x7BEF  ///< 123, 125, 123
#define ILI9341_BLUE        0x001F  ///<   0,   0, 255
#define ILI9341_GREEN       0x07E0  ///<   0, 255,   0
#define ILI9341_CYAN        0x07FF  ///<   0, 255, 255
#define ILI9341_RED         0xF800  ///< 255,   0,   0
#define ILI9341_MAGENTA     0xF81F  ///< 255,   0, 255
#define ILI9341_YELLOW      0xFFE0  ///< 255, 255,   0
#define ILI9341_WHITE       0xFFFF  ///< 255, 255, 255
#define ILI9341_ORANGE      0xFD20  ///< 255, 165,   0
#define ILI9341_GREENYELLOW 0xAFE5  ///< 173, 255,  41
#define ILI9341_PINK        0xFC18  ///< 255, 130, 198

#define FOREGROUND 			1
#define BACKGROUND			0

#define START_BTN 	1
#define STOP_BTN	2
#define CAL_BTN		0
#define MEASURE_BTN 3

#define BUF_SIZE 25
char lcd_buffer[BUF_SIZE];
char lcd_t_buffer[BUF_SIZE];


struct _current_font
{
    u8* font;
    u8 x_size;
    u8 y_size;
    u8 offset;
    u8 numchars;
};

extern int fch; // Foreground color upper byte
extern int fcl; // Foreground color lower byte
extern int bch; // Background color upper byte
extern int bcl; // Background color lower byte

extern struct _current_font cfont;
extern u8 SmallFont[];
extern u8 BigFont[];
extern u8 SevenSegNumFont[];

u32 LCD_Read(char VL);
void LCD_Write_COM(char VL);  
void LCD_Write_DATA(char VL);
void LCD_Write_DATA16(char VH, char VL);

void initLCD(void);
void setXY(int x1, int y1, int x2, int y2);
void setColor(u8 r, u8 g, u8 b);
void setColorFast( u8 fg, u16 color);
void setColorBg(u8 r, u8 g, u8 b);
void clrXY(void);
void clrScr(void);

void drawHLine(int x, int y, int l);
void fillRect(int x1, int y1, int x2, int y2);

void setFont(u8* font);
void printChar(u8 c, int x, int y);
void lcdPrint(char *st, int x, int y);
void init_buf( char* buf, size_t size);
void display_imu_data(char* buf, size_t size);
void display_steps( char* buf, size_t size);
void display_cal_directions(char* buf, size_t size, int signal);
void display_meas_directions(char* buf, size_t size, int signal);


#endif /* LCD_H_ */
