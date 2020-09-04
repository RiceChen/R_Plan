#ifndef __DRV_LCD_H__
#define __DRV_LCD_H__

#include <rtthread.h>

#define LCD_W 240
#define LCD_H 240

#define LCD_PWR_PIN (6)
#define LCD_DCx_PIN (4)
#define LCD_RES_PIN (7)
#define LCD_CSx_PIN (3)
#define LCD_CLEAR_SEND_NUMBER (5760)

#define WHITE            0xFFFF
#define BLACK            0x0000
#define BLUE             0x001F
#define BRED             0XF81F
#define GRED             0XFFE0
#define GBLUE            0X07FF
#define RED              0xF800
#define MAGENTA          0xF81F
#define GREEN            0x07E0
#define CYAN             0x7FFF
#define YELLOW           0xFFE0
#define BROWN            0XBC40
#define BRRED            0XFC07
#define GRAY             0X8430
#define GRAY175          0XAD75
#define GRAY151          0X94B2
#define GRAY187          0XBDD7
#define GRAY240          0XF79E

void lcd_address_set(rt_uint16_t x_start, rt_uint16_t y_start, rt_uint16_t x_end, rt_uint16_t y_end);
void lcd_fill(rt_uint16_t x_start, rt_uint16_t y_start, rt_uint16_t x_end, rt_uint16_t y_end, rt_uint16_t color);
void lcd_clear(rt_uint16_t color);
void lcd_draw_point(rt_uint16_t x, rt_uint16_t y, rt_uint16_t color);
rt_err_t lcd_show_image(rt_uint16_t x, rt_uint16_t y, rt_uint16_t length, rt_uint16_t wide, const rt_uint8_t *p);
rt_err_t lcd_open_window(rt_uint16_t x, rt_uint16_t y, rt_uint16_t length, rt_uint16_t wide);

#endif
