#include <rtthread.h>
#include "drv_lcd.h"

#define GL_RGB_32_to_16(rgb) (((((unsigned int)(rgb)) & 0xFF) >> 3) | ((((unsigned int)(rgb)) & 0xFC00) >> 5) | ((((unsigned int)(rgb)) & 0xF80000) >> 8))
void gfx_draw_pixel(int x, int y, unsigned int rgb)
{
	lcd_draw_point(x, y, GL_RGB_32_to_16(rgb));
}

struct EXTERNAL_GFX_OP
{
	void (*draw_pixel)(int x, int y, unsigned int rgb);
	void (*fill_rect)(int x0, int y0, int x1, int y1, unsigned int rgb);
} my_gfx_op;
extern void startHelloStar(void* phy_fb, int width, int height, int color_bytes, struct EXTERNAL_GFX_OP* gfx_op);

static char guilite_stack[512];
static struct rt_thread thread_guilite;

static void thread_guilite_entry(void *param)
{
	my_gfx_op.draw_pixel = gfx_draw_pixel;
	my_gfx_op.fill_rect = RT_NULL;
	startHelloStar(RT_NULL, 128, 160, 2, &my_gfx_op);
}

static int guilite_demo(void)
{
	rt_thread_init(&thread_guilite,
								"guilite",
								thread_guilite_entry,
								RT_NULL,
								&guilite_stack[0],
								sizeof(guilite_stack),
								5,
								5 
								);
	rt_thread_startup(&thread_guilite);
}
INIT_APP_EXPORT(guilite_demo);