#include "drv_spi.h"
#include "drv_lcd.h"

#include <rtdevice.h>
#include <board.h>

rt_uint16_t BACK_COLOR = WHITE, FORE_COLOR = BLACK;

static struct rt_spi_device *spi_dev_lcd;

static int rt_hw_lcd_config(void)
{
    spi_dev_lcd = (struct rt_spi_device *)rt_device_find("spi3");

    rt_pin_mode(LCD_DCx_PIN, PIN_MODE_OUTPUT);
    rt_pin_mode(LCD_RES_PIN, PIN_MODE_OUTPUT);

    rt_pin_mode(LCD_PWR_PIN, PIN_MODE_OUTPUT);
    rt_pin_write(LCD_PWR_PIN, PIN_LOW);

    rt_pin_write(LCD_RES_PIN, PIN_LOW);
    //wait at least 100ms for reset
    rt_thread_delay(RT_TICK_PER_SECOND / 100);
    rt_pin_write(LCD_RES_PIN, PIN_HIGH);
    rt_thread_delay(RT_TICK_PER_SECOND / 100);

    return RT_EOK;
}

static void lcd_write_cmd(const rt_uint8_t cmd)
{
    rt_pin_write(LCD_DCx_PIN, PIN_LOW);
    rt_spi_send(spi_dev_lcd, &cmd, 1);
}

static void lcd_write_data(const rt_uint8_t data)
{
    rt_pin_write(LCD_DCx_PIN, PIN_HIGH);

    rt_spi_send(spi_dev_lcd, &data, 1);
}

rt_err_t lcd_write_half_word(const rt_uint16_t da)
{
    char data[2] = {0};
    data[0] = da >> 8;
    data[1] = da;

    rt_pin_write(LCD_DCx_PIN, PIN_HIGH);
    rt_spi_send(spi_dev_lcd, data, 2);
}

void lcd_address_set(rt_uint16_t x_start, rt_uint16_t y_start, 
                    rt_uint16_t x_end, rt_uint16_t y_end)
{
    lcd_write_cmd(0x2a);
    lcd_write_data(x_start >> 8);
    lcd_write_data(x_start);
    lcd_write_data(x_end >> 8);
    lcd_write_data(x_end);
    lcd_write_cmd(0x2b);
    lcd_write_data(y_start >> 8);
    lcd_write_data(y_start);
    lcd_write_data(y_end >> 8);
    lcd_write_data(y_end);
    lcd_write_cmd(0x2C);
}

void lcd_fill(rt_uint16_t x_start, rt_uint16_t y_start, rt_uint16_t x_end, rt_uint16_t y_end, rt_uint16_t color)
{
    rt_uint16_t i = 0, j = 0;
    rt_uint32_t size = 0, size_remain = 0;
    rt_uint8_t *fill_buf = RT_NULL;

    size = (x_end - x_start) * (y_end - y_start) * 2;

    if (size > LCD_CLEAR_SEND_NUMBER)
    {
        size_remain = size - LCD_CLEAR_SEND_NUMBER;
        size = LCD_CLEAR_SEND_NUMBER;
    }

    lcd_address_set(x_start, y_start, x_end, y_end);

    fill_buf = (rt_uint8_t *)rt_malloc(size);
    if (fill_buf)
    {
        while (1)
        {
            for (i = 0; i < size / 2; i++)
            {
                fill_buf[2 * i] = color >> 8;
                fill_buf[2 * i + 1] = color;
            }
            rt_pin_write(LCD_DCx_PIN, PIN_HIGH);
            rt_spi_send(spi_dev_lcd, fill_buf, size);

            if (size_remain == 0)
                break;

            if (size_remain > LCD_CLEAR_SEND_NUMBER)
            {
                size_remain = size_remain - LCD_CLEAR_SEND_NUMBER;
            }
            else
            {
                size = size_remain;
                size_remain = 0;
            }
        }
        rt_free(fill_buf);
    }
    else
    {
        for (i = y_start; i <= y_end; i++)
        {
            for (j = x_start; j <= x_end; j++)lcd_write_half_word(color);
        }
    }
}

void lcd_clear(rt_uint16_t color)
{
    lcd_fill(0, 0, 240,240, color);
}

void lcd_draw_point(rt_uint16_t x, rt_uint16_t y, rt_uint16_t color)
{
    lcd_address_set(x, y, x, y);
    lcd_write_half_word(color);
}

rt_err_t lcd_show_image(rt_uint16_t x, rt_uint16_t y, rt_uint16_t length, rt_uint16_t wide, const rt_uint8_t *p)
{
    lcd_address_set(x, y, x + length - 1, y + wide - 1);
    rt_pin_write(LCD_DCx_PIN, PIN_HIGH);
    for(int i = 0; i < 10; i++)
    {
        rt_spi_send(spi_dev_lcd, p + (i * length * wide / 5), length * wide / 5);
    }
    return RT_EOK;
}

rt_err_t lcd_open_window(rt_uint16_t x, rt_uint16_t y, rt_uint16_t length, rt_uint16_t wide)
{
    if (x + length > LCD_W || y + wide > LCD_H)
    {
        return -RT_ERROR;
    }

    lcd_address_set(x, y, x + length - 1, y + wide - 1);
    return RT_EOK;
}

#include "rp_logo.h"

static int rt_lcd_init(void)
{
    rt_hw_lcd_config();

    /* Memory Data Access Control */
    lcd_write_cmd(0x36);
    lcd_write_data(0x00);
    
    /* RGB 5-6-5-bit  */
    lcd_write_cmd(0x3A);
    lcd_write_data(0x65);

    /* Porch Setting */
    lcd_write_cmd(0xB2);
    lcd_write_data(0x0C);
    lcd_write_data(0x0C);
    lcd_write_data(0x00);
    lcd_write_data(0x33);
    lcd_write_data(0x33);

    /* Gate Control */
    lcd_write_cmd(0xB7);
    lcd_write_data(0x35);
    
    /* VCOM Setting */
    lcd_write_cmd(0xBB);
    lcd_write_data(0x19);

    /* LCM Control */
    lcd_write_cmd(0xC0);
    lcd_write_data(0x2C);

    /* VDV and VRH Command Enable */
    lcd_write_cmd(0xC2);
    lcd_write_data(0x01);

    /* VRH Set */
    lcd_write_cmd(0xC3);
    lcd_write_data(0x12);

    /* VDV Set */
    lcd_write_cmd(0xC4);
    lcd_write_data(0x20);

    /* Frame Rate Control in Normal Mode */
    lcd_write_cmd(0xC6);
    lcd_write_data(0x0F);

    /* Power Control 1 */
    lcd_write_cmd(0xD0);
    lcd_write_data(0xA4);
    lcd_write_data(0xA1);

    /* Display Inversion On */
    lcd_write_cmd(0x21);

    /* Sleep Out */
    lcd_write_cmd(0x11);

    /* wait for power stability */
    rt_thread_mdelay(100);

    /* display on */
    rt_pin_write(LCD_PWR_PIN, PIN_HIGH);
    lcd_write_cmd(0x29);

    lcd_show_image(0, 0, 240, 240, rp_logo);

    return RT_EOK;
}

static int rt_hw_lcd_init(void)
{
	rt_lcd_init();
	return 0;
}
INIT_APP_EXPORT(rt_hw_lcd_init);