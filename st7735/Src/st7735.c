
#include "stm32f4xx_hal.h"
#include "st7735.h"

#define DELAY 0x80

// based on Adafruit ST7735 library for Arduino
static const uint8_t
  init_cmds1[] = {            // Init for 7735R, part 1 (red or green tab)
    15,                       // 15 commands in list:
    ST7735_SWRESET,   DELAY,  //  1: Software reset, 0 args, w/delay
      150,                    //     150 ms delay
    ST7735_SLPOUT ,   DELAY,  //  2: Out of sleep mode, 0 args, w/delay
      255,                    //     500 ms delay
    ST7735_FRMCTR1, 3      ,  //  3: Frame rate ctrl - normal mode, 3 args:
      0x01, 0x2C, 0x2D,       //     Rate = fosc/(1x2+40) * (LINE+2C+2D)
    ST7735_FRMCTR2, 3      ,  //  4: Frame rate control - idle mode, 3 args:
      0x01, 0x2C, 0x2D,       //     Rate = fosc/(1x2+40) * (LINE+2C+2D)
    ST7735_FRMCTR3, 6      ,  //  5: Frame rate ctrl - partial mode, 6 args:
      0x01, 0x2C, 0x2D,       //     Dot inversion mode
      0x01, 0x2C, 0x2D,       //     Line inversion mode
    ST7735_INVCTR , 1      ,  //  6: Display inversion ctrl, 1 arg, no delay:
      0x07,                   //     No inversion
    ST7735_PWCTR1 , 3      ,  //  7: Power control, 3 args, no delay:
      0xA2,
      0x02,                   //     -4.6V
      0x84,                   //     AUTO mode
    ST7735_PWCTR2 , 1      ,  //  8: Power control, 1 arg, no delay:
      0xC5,                   //     VGH25 = 2.4C VGSEL = -10 VGH = 3 * AVDD
    ST7735_PWCTR3 , 2      ,  //  9: Power control, 2 args, no delay:
      0x0A,                   //     Opamp current small
      0x00,                   //     Boost frequency
    ST7735_PWCTR4 , 2      ,  // 10: Power control, 2 args, no delay:
      0x8A,                   //     BCLK/2, Opamp current small & Medium low
      0x2A,  
    ST7735_PWCTR5 , 2      ,  // 11: Power control, 2 args, no delay:
      0x8A, 0xEE,
    ST7735_VMCTR1 , 1      ,  // 12: Power control, 1 arg, no delay:
      0x0E,
    ST7735_INVOFF , 0      ,  // 13: Don't invert display, no args, no delay
    ST7735_MADCTL , 1      ,  // 14: Memory access control (directions), 1 arg:
      ST7735_ROTATION,        //     row addr/col addr, bottom to top refresh
    ST7735_COLMOD , 1      ,  // 15: set color mode, 1 arg, no delay:
      0x05 },                 //     16-bit color

#if (defined(ST7735_IS_128X128) || defined(ST7735_IS_160X128))
  init_cmds2[] = {            // Init for 7735R, part 2 (1.44" display)
    2,                        //  2 commands in list:
    ST7735_CASET  , 4      ,  //  1: Column addr set, 4 args, no delay:
      0x00, 0x00,             //     XSTART = 0
      0x00, 0x7F,             //     XEND = 127
    ST7735_RASET  , 4      ,  //  2: Row addr set, 4 args, no delay:
      0x00, 0x00,             //     XSTART = 0
      0x00, 0x7F },           //     XEND = 127
#endif // ST7735_IS_128X128

#ifdef ST7735_IS_160X80
  init_cmds2[] = {            // Init for 7735S, part 2 (160x80 display)
    3,                        //  3 commands in list:
    ST7735_CASET  , 4      ,  //  1: Column addr set, 4 args, no delay:
      0x00, 0x00,             //     XSTART = 0
      0x00, 0x4F,             //     XEND = 79
    ST7735_RASET  , 4      ,  //  2: Row addr set, 4 args, no delay:
      0x00, 0x00,             //     XSTART = 0
      0x00, 0x9F ,            //     XEND = 159
    ST7735_INVON, 0 },        //  3: Invert colors
#endif

  init_cmds3[] = {            // Init for 7735R, part 3 (red or green tab)
    4,                        //  4 commands in list:
    ST7735_GMCTRP1, 16      , //  1: Magical unicorn dust, 16 args, no delay:
      0x02, 0x1c, 0x07, 0x12,
      0x37, 0x32, 0x29, 0x2d,
      0x29, 0x25, 0x2B, 0x39,
      0x00, 0x01, 0x03, 0x10,
    ST7735_GMCTRN1, 16      , //  2: Sparkles and rainbows, 16 args, no delay:
      0x03, 0x1d, 0x07, 0x06,
      0x2E, 0x2C, 0x29, 0x2D,
      0x2E, 0x2E, 0x37, 0x3F,
      0x00, 0x00, 0x02, 0x10,
    ST7735_NORON  ,    DELAY, //  3: Normal display on, no args, w/delay
      10,                     //     10 ms delay
    ST7735_DISPON ,    DELAY, //  4: Main screen turn on, no args w/delay
      100 };                  //     100 ms delay


static void ST7735_Select()
{
    HAL_GPIO_WritePin(ST7735_CS_GPIO_Port, ST7735_CS_Pin, GPIO_PIN_RESET);
}

void ST7735_Unselect()
{
    HAL_GPIO_WritePin(ST7735_CS_GPIO_Port, ST7735_CS_Pin, GPIO_PIN_SET);
}

static void ST7735_Reset()
{
    HAL_GPIO_WritePin(ST7735_RES_GPIO_Port, ST7735_RES_Pin, GPIO_PIN_RESET);
    HAL_Delay(5);
    HAL_GPIO_WritePin(ST7735_RES_GPIO_Port, ST7735_RES_Pin, GPIO_PIN_SET);
}

static void ST7735_WriteCommand(uint8_t cmd)
{
    HAL_GPIO_WritePin(ST7735_DC_GPIO_Port, ST7735_DC_Pin, GPIO_PIN_RESET);
    HAL_SPI_Transmit(&ST7735_SPI_PORT, &cmd, sizeof(cmd), HAL_MAX_DELAY);
}

static void ST7735_WriteData(uint8_t* buff, size_t buff_size)
{
    HAL_GPIO_WritePin(ST7735_DC_GPIO_Port, ST7735_DC_Pin, GPIO_PIN_SET);
    HAL_SPI_Transmit(&ST7735_SPI_PORT, buff, buff_size, HAL_MAX_DELAY);
}

static void ST7735_ExecuteCommandList(const uint8_t *addr)
{
    uint8_t numCommands, numArgs;
    uint16_t ms;

    numCommands = *addr++;
    while(numCommands--) {
        uint8_t cmd = *addr++;
        ST7735_WriteCommand(cmd);

        numArgs = *addr++;
        // If high bit set, delay follows args
        ms = numArgs & DELAY;
        numArgs &= ~DELAY;
        if(numArgs) {
            ST7735_WriteData((uint8_t*)addr, numArgs);
            addr += numArgs;
        }

        if(ms) {
            ms = *addr++;
            if(ms == 255) ms = 500;
            HAL_Delay(ms);
        }
    }
}

static void ST7735_SetAddressWindow(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1)
{
    // column address set
    ST7735_WriteCommand(ST7735_CASET);
    uint8_t data[] = { 0x00, x0 + ST7735_XSTART, 0x00, x1 + ST7735_XSTART };
    ST7735_WriteData(data, sizeof(data));

    // row address set
    ST7735_WriteCommand(ST7735_RASET);
    data[1] = y0 + ST7735_YSTART;
    data[3] = y1 + ST7735_YSTART;
    ST7735_WriteData(data, sizeof(data));

    // write to RAM
    ST7735_WriteCommand(ST7735_RAMWR);
}

void ST7735_Init()
{
    ST7735_Select();
    ST7735_Reset();
    ST7735_ExecuteCommandList(init_cmds1);
    ST7735_ExecuteCommandList(init_cmds2);
    ST7735_ExecuteCommandList(init_cmds3);
    ST7735_Unselect();
}

void ST7735_DrawPixel(uint16_t x, uint16_t y, uint16_t color)
{
    if((x >= ST7735_WIDTH) || (y >= ST7735_HEIGHT))
        return;

    ST7735_Select();

    ST7735_SetAddressWindow(x, y, x+1, y+1);
    uint8_t data[] = { color >> 8, color & 0xFF };
    ST7735_WriteData(data, sizeof(data));

    ST7735_Unselect();
}

static void ST7735_WriteChar(uint16_t x, uint16_t y, char ch, FontDef font, uint16_t color, uint16_t bgcolor)
{
    uint32_t i, b, j;

    ST7735_SetAddressWindow(x, y, x+font.width-1, y+font.height-1);

    for(i = 0; i < font.height; i++) {
        b = font.data[(ch - 32) * font.height + i];
        for(j = 0; j < font.width; j++) {
            if((b << j) & 0x8000)  {
                uint8_t data[] = { color >> 8, color & 0xFF };
                ST7735_WriteData(data, sizeof(data));
            } else {
                uint8_t data[] = { bgcolor >> 8, bgcolor & 0xFF };
                ST7735_WriteData(data, sizeof(data));
            }
        }
    }
}

void ST7735_WriteString(uint16_t x, uint16_t y, const char* str, FontDef font, uint16_t color, uint16_t bgcolor)
{
    ST7735_Select();

    while(*str) {
        if(x + font.width >= ST7735_WIDTH) {
            x = 0;
            y += font.height;
            if(y + font.height >= ST7735_HEIGHT) {
                break;
            }

            if(*str == ' ') {
                // skip spaces in the beginning of the new line
                str++;
                continue;
            }
        }

        ST7735_WriteChar(x, y, *str, font, color, bgcolor);
        x += font.width;
        str++;
    }

    ST7735_Unselect();
}


void ST7735_WriteStringWithSelect(uint16_t x, uint16_t y, const char* str, FontDef font, uint8_t select_pos, Color_TypeDef color)
{
    ST7735_Select();

    uint8_t pos_counter = 6;

    while(*str) {
        if (x + font.width >= ST7735_WIDTH) {
            x = 0;
            y += font.height;
            if (y + font.height >= ST7735_HEIGHT) {
                break;
            }

            if(*str == ' ') {
                // skip spaces in the beginning of the new line
                str++;
                continue;
            }
        }
        if (pos_counter == select_pos && (*str >= '0' && *str <= '9'))
        	ST7735_WriteChar(x, y, *str, font, color.selectText, color.selectBackground);
        else
        	ST7735_WriteChar(x, y, *str, font, color.mainText, color.mainBackground);

        if (*str >= '0' && *str <= '9') pos_counter--;

        x += font.width;
        str++;
    }

    ST7735_Unselect();
}

void ST7735_FillRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color)
{
    if((x >= ST7735_WIDTH) || (y >= ST7735_HEIGHT)) return;
    if((x + w - 1) >= ST7735_WIDTH) w = ST7735_WIDTH - x;
    if((y + h - 1) >= ST7735_HEIGHT) h = ST7735_HEIGHT - y;

    ST7735_Select();
    ST7735_SetAddressWindow(x, y, x+w-1, y+h-1);

    uint8_t data[] = { color >> 8, color & 0xFF };
    HAL_GPIO_WritePin(ST7735_DC_GPIO_Port, ST7735_DC_Pin, GPIO_PIN_SET);
    for(y = h; y > 0; y--) {
        for(x = w; x > 0; x--) {
            HAL_SPI_Transmit(&ST7735_SPI_PORT, data, sizeof(data), HAL_MAX_DELAY);
        }
    }
    ST7735_Unselect();
}

void ST7735_FillScreen(uint16_t color)
{
    ST7735_FillRectangle(0, 0, ST7735_WIDTH, ST7735_HEIGHT, color);
}

void ST7735_DrawImage(uint16_t x, uint16_t y, uint16_t w, uint16_t h, const uint16_t* data)
{
    if((x >= ST7735_WIDTH) || (y >= ST7735_HEIGHT)) return;
    if((x + w - 1) >= ST7735_WIDTH) return;
    if((y + h - 1) >= ST7735_HEIGHT) return;

    ST7735_Select();
    ST7735_SetAddressWindow(x, y, x+w-1, y+h-1);
    ST7735_WriteData((uint8_t*)data, sizeof(uint16_t)*w*h);
    ST7735_Unselect();
}

void ST7735_InvertColors(bool invert)
{
    ST7735_Select();
    ST7735_WriteCommand(invert ? ST7735_INVON : ST7735_INVOFF);
    ST7735_Unselect();
}


void ST7735_DrawHLine(uint8_t x, uint8_t x1, uint8_t y, uint16_t color)
{
	ST7735_Select();
	uint16_t len = x1-x;
	uint8_t data[] = {color >> 8, color & 0xFF};
	ST7735_SetAddressWindow(x,y,x1,y);
	for(int i = 0; i < len; i++) {
		ST7735_WriteData(data, sizeof(data));
	}
	ST7735_Unselect();
}

void ST7735_DrawVLine(uint8_t x, uint8_t y, uint8_t y1, uint16_t color)
{
	ST7735_Select();
	uint16_t len = y1-y;
	uint8_t data[] = {color >> 8, color & 0xFF};
	ST7735_SetAddressWindow(x,y,x,y1);
	for(int i = 0;i < len; i++) {
		ST7735_WriteData(data, sizeof(data));
	}
	ST7735_Unselect();
}

void ST7735_DrawLine(int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color)
{
	int16_t dx = x2-x1;
	int16_t dy = y2-y1;
	int16_t dxsym = (dx > 0) ? 1 : -1;
	int16_t dysym = (dy > 0) ? 1 : -1;

	if (dx == 0) {
		(y2 > y1) ? ST7735_DrawVLine(x1, y1, y2, color) : ST7735_DrawVLine(x1, y2, y1, color);
		return;
	}
	if (dy == 0) {
		(x2 > x1) ? ST7735_DrawHLine(x1, x2, y1, color) : ST7735_DrawHLine(x2, x1, y1, color);
		return;
	}

	dx *= dxsym;
	dy *= dysym;
	int16_t dx2 = dx << 1;
	int16_t dy2 = dy << 1;
	int16_t di;

	if (dx >= dy) {
		di = dy2 - dx;
		while (x1 != x2) {
			ST7735_DrawPixel(x1, y1, color);
			x1 += dxsym;
			if (di < 0) {
				di += dy2;
			}
			else {
				di += dy2 - dx2;
				y1 += dysym;
			}
		}
	}
	else {
		di = dx2 - dy;
		while (y1 != y2) {
			ST7735_DrawPixel(x1, y1, color);
			y1 += dysym;
			if (di < 0) {
				di += dx2;
			}
			else {
				di += dx2 - dy2;
				x1 += dxsym;
			}
		}
	}
	ST7735_DrawPixel(x1, y1, color);
}

void ST7735_drawCircleHelper(int16_t x0, int16_t y0, int16_t r, uint8_t cornername, uint16_t color)
{
  int16_t f = 1 - r;
  int16_t ddF_x = 1;
  int16_t ddF_y = -2 * r;
  int16_t x = 0;
  int16_t y = r;

  while (x < y) {
    if (f >= 0) {
      y--;
      ddF_y += 2;
      f += ddF_y;
    }
    x++;
    ddF_x += 2;
    f += ddF_x;
    if (cornername & 0x4) {
    	ST7735_DrawPixel(x0 + x, y0 + y, color);
    	ST7735_DrawPixel(x0 + y, y0 + x, color);
    }
    if (cornername & 0x2) {
    	ST7735_DrawPixel(x0 + x, y0 - y, color);
    	ST7735_DrawPixel(x0 + y, y0 - x, color);
    }
    if (cornername & 0x8) {
    	ST7735_DrawPixel(x0 - y, y0 + x, color);
    	ST7735_DrawPixel(x0 - x, y0 + y, color);
    }
    if (cornername & 0x1) {
    	ST7735_DrawPixel(x0 - y, y0 - x, color);
    	ST7735_DrawPixel(x0 - x, y0 - y, color);
    }
  }
}


void ST7735_drawRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, uint16_t color)
{
  int16_t max_radius = ((w < h) ? w : h) / 2;
  if (r > max_radius)
	  r = max_radius;

  ST7735_DrawHLine(x + r, x + r + w - 2 * r, y, color);         // Top
  ST7735_DrawHLine(x + r, x + r + w - 2 * r, y + h - 1, color); // Bottom
  ST7735_DrawVLine(x, y + r, y + r + h - 2 * r, color);         // Left
  ST7735_DrawVLine(x + w - 1, y + r, y + r + h - 2 * r, color); // Right

  ST7735_drawCircleHelper(x + r, y + r, r, 1, color);
  ST7735_drawCircleHelper(x + w - r - 1, y + r, r, 2, color);
  ST7735_drawCircleHelper(x + w - r - 1, y + h - r - 1, r, 4, color);
  ST7735_drawCircleHelper(x + r, y + h - r - 1, r, 8, color);
}




void SquareIcon(uint8_t x, uint8_t y, uint16_t color)
{
	ST7735_drawRoundRect(x, y, 32, 32, 7, color);
	ST7735_drawRoundRect(x+1, y+1, 30, 30, 5, color);
	ST7735_FillRectangle(x+4, y+6, 2, 12, color);
	ST7735_FillRectangle(x+4, y+6, 11, 2, color);
	ST7735_FillRectangle(x+15, y+6, 2, 20, color);
	ST7735_FillRectangle(x+15, y+24, 12, 2, color);
	ST7735_FillRectangle(x+26, y+14, 2, 12, color);
}

void TriangleIcon(uint8_t x, uint8_t y, uint16_t color)
{
	ST7735_drawRoundRect(x, y, 32, 32, 7, color);
	ST7735_drawRoundRect(x + 1, y + 1, 30, 30, 5, color);
	for (uint16_t a = 0; a < 3; a++) {
		ST7735_DrawLine(x + 9, y + 3 + a, x + 3, y + 14 + a, color);
		ST7735_DrawLine(x + 21, y + 26 + a, x + 9, y + 3 + a, color);
		ST7735_DrawLine(x + 28, y + 15 + a, x + 21, y + 26 + a, color);
	}
}




void SineIcon(uint8_t x, uint8_t y, uint16_t color)
{
	uint16_t Color = ((color & 0xFF00) >> 8) | ((color & 0xFF) << 8);
	uint16_t Sine_Icon_only[676] = {
		0x0000, 0x0000, 0x0000, 0x0000, 0x0000, Color, Color, Color, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,   // 0x0010 (16) pixels
		0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, Color, Color,   // 0x0020 (32) pixels
		Color, Color, Color, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,   // 0x0030 (48) pixels
		0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, Color, Color, 0x0000, Color, Color, Color, 0x0000, 0x0000,   // 0x0040 (64) pixels
		0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,   // 0x0050 (80) pixels
		0x0000, Color, Color, 0x0000, 0x0000, 0x0000, Color, Color, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,   // 0x0060 (96) pixels
		0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, Color, Color, 0x0000, 0x0000, 0x0000,   // 0x0070 (112) pixels
		0x0000, Color, Color, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,   // 0x0080 (128) pixels
		0x0000, 0x0000, 0x0000, 0x0000, Color, Color, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, Color, Color, 0x0000, 0x0000, 0x0000,   // 0x0090 (144) pixels
		0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, Color, Color,   // 0x00A0 (160) pixels
		0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, Color, Color, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,   // 0x00B0 (176) pixels
		0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, Color, Color, Color, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,   // 0x00C0 (192) pixels
		Color, Color, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,   // 0x00D0 (208) pixels
		0x0000, Color, Color, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, Color, Color, Color, 0x0000, 0x0000, 0x0000,   // 0x00E0 (224) pixels
		0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, Color, Color, 0x0000, 0x0000, 0x0000,   // 0x00F0 (240) pixels
		0x0000, 0x0000, 0x0000, 0x0000, 0x0000, Color, Color, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,   // 0x0100 (256) pixels
		0x0000, 0x0000, 0x0000, 0x0000, Color, Color, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, Color,   // 0x0110 (272) pixels
		Color, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, Color, Color,   // 0x0120 (288) pixels
		0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, Color, Color, Color, 0x0000, 0x0000, 0x0000, 0x0000,   // 0x0130 (304) pixels
		0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, Color, Color, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,   // 0x0140 (320) pixels
		0x0000, 0x0000, 0x0000, 0x0000, Color, Color, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,   // 0x0150 (336) pixels
		Color, Color, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, Color, Color,   // 0x0160 (352) pixels
		0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, Color, Color, 0x0000, 0x0000, 0x0000, 0x0000,   // 0x0170 (368) pixels
		0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, Color, Color, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,   // 0x0180 (384) pixels
		0x0000, 0x0000, 0x0000, 0x0000, Color, Color, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,   // 0x0190 (400) pixels
		0x0000, 0x0000, 0x0000, Color, Color, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, Color, Color, Color,   // 0x01A0 (416) pixels
		0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, Color, Color, 0x0000,   // 0x01B0 (432) pixels
		0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, Color, Color, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,   // 0x01C0 (448) pixels
		0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, Color, Color, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,   // 0x01D0 (464) pixels
		0x0000, Color, Color, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,   // 0x01E0 (480) pixels
		0x0000, 0x0000, Color, Color, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, Color, Color, Color, 0x0000, 0x0000, 0x0000,   // 0x01F0 (496) pixels
		0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, Color, Color, Color, 0x0000,   // 0x0200 (512) pixels
		0x0000, 0x0000, 0x0000, 0x0000, Color, Color, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,   // 0x0210 (528) pixels
		0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, Color, Color, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, Color, Color,   // 0x0220 (544) pixels
		0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,   // 0x0230 (560) pixels
		0x0000, Color, Color, 0x0000, 0x0000, 0x0000, 0x0000, Color, Color, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,   // 0x0240 (576) pixels
		0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, Color, Color, 0x0000, 0x0000,   // 0x0250 (592) pixels
		0x0000, Color, Color, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,   // 0x0260 (608) pixels
		0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, Color, Color, Color, 0x0000, Color, Color, 0x0000, 0x0000, 0x0000, 0x0000,   // 0x0270 (624) pixels
		0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,   // 0x0280 (640) pixels
		0x0000, Color, Color, Color, Color, Color, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,   // 0x0290 (656) pixels
		0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, Color, Color, Color, 0x0000,   // 0x02A0 (672) pixels
	};
	ST7735_drawRoundRect(x, y, 32, 32, 7, color);
	ST7735_drawRoundRect(x+1, y+1, 30, 30, 5, color);
	ST7735_DrawImage(x+3, y+3, 26, 26, Sine_Icon_only);
}

