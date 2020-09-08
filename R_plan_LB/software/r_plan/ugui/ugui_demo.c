#include "ugui.h"
#include "ugui_config.h"
#include "drv_lcd.h"
#include "drv_gpio.h"
#include <drivers/pin.h>

#define MAX_OBJECTS        10

UG_GUI gui_demo;

UG_WINDOW window;
UG_OBJECT obj_buff_wnd[MAX_OBJECTS];
UG_BUTTON next_btn;
UG_BUTTON last_btn;


static char ugui_thread_stack[1025];
static struct rt_thread ugui_thread;

struct rt_event event;
#define KEY_RIGHT_EVENT  (1)
#define KEY_LEFT_EVENT   (1<<1)


void UserPixelSetFunction(UG_S16 x , UG_S16 y ,UG_COLOR c )
{
    lcd_draw_point(x, y, c);
}

void window_callback( UG_MESSAGE* msg )
{
    if(msg->type == MSG_TYPE_OBJECT)
    {
        if (msg->id == OBJ_TYPE_BUTTON)
        {
            switch(msg->sub_id)
            {
                case BTN_ID_0:
                {
                    rt_kprintf("BTN_ID_0\n");
                    break;
                }
                case BTN_ID_1:
                {
                    rt_kprintf("BTN_ID_1\n");
                    break;
                }
            }
            
        }
    }
}

static void ugui_thread_entry(void *param)
{
    rt_uint32_t evt = 0;
    while(1)
    {
        evt = 0;
        rt_thread_delay(1);
        UG_Update();
        if(rt_event_recv(&event,
                        KEY_LEFT_EVENT | KEY_RIGHT_EVENT,
                        RT_EVENT_FLAG_OR | RT_EVENT_FLAG_CLEAR,
                        RT_WAITING_NO,
                        &evt) == RT_EOK)
        {
            if(evt == KEY_LEFT_EVENT)
            {
                int x = UG_WindowGetInnerWidth(&window) - 20;
                int y = UG_WindowGetInnerHeight(&window) - 20;
                UG_TouchUpdate(x, y, TOUCH_STATE_PRESSED);
            }
            else if(evt == KEY_RIGHT_EVENT)
            {
                int x = UG_WindowGetInnerWidth(&window) - 20;
                int y = UG_WindowGetInnerHeight(&window) - 60;
                UG_TouchUpdate(x, y, TOUCH_STATE_PRESSED);
            }
        }
        else
        {
            UG_TouchUpdate(-1,-1,TOUCH_STATE_RELEASED);
        }
    } 
}

void key_right_handle(void *args)
{
    rt_event_send(&event, KEY_RIGHT_EVENT);
}

void key_left_handle(void *args)
{
    rt_event_send(&event, KEY_LEFT_EVENT);
}

#define KEY_RIGHT_PIN   12
#define KEY_LEFT_PIN   13

int ugui_demo()
{
    UG_Init(&gui_demo, UserPixelSetFunction, LCD_W, LCD_H);

    
    UG_WindowCreate(&window, obj_buff_wnd, MAX_OBJECTS, window_callback);
    UG_WindowSetTitleText(&window, "R-PLAN");
    UG_WindowSetTitleTextFont(&window, &FONT_8X8);

    UG_ButtonCreate(&window, &next_btn, BTN_ID_0, UG_WindowGetInnerWidth(&window) - 40, UG_WindowGetInnerHeight(&window) - 80, UG_WindowGetInnerWidth(&window) - 5, UG_WindowGetInnerHeight(&window) - 45);
    UG_ButtonCreate(&window, &last_btn, BTN_ID_1, UG_WindowGetInnerWidth(&window) - 40, UG_WindowGetInnerHeight(&window) - 40, UG_WindowGetInnerWidth(&window) - 5, UG_WindowGetInnerHeight(&window) - 5);

    UG_ButtonSetFont(&window, BTN_ID_0, &FONT_8X8);
    UG_ButtonSetBackColor(&window, BTN_ID_0, C_GRAY);
    UG_ButtonSetText(&window, BTN_ID_0, ">" );

    UG_ButtonSetFont(&window, BTN_ID_1, &FONT_8X8);
    UG_ButtonSetBackColor(&window, BTN_ID_1, C_GRAY);
    UG_ButtonSetText(&window, BTN_ID_1, "<");

    UG_WindowShow(&window);
    UG_WindowSetBackColor(&window, C_BLACK) ;

    rt_pin_mode(KEY_RIGHT_PIN , PIN_MODE_INPUT_PULLUP);
    rt_pin_attach_irq(KEY_RIGHT_PIN , PIN_IRQ_MODE_FALLING , key_right_handle, RT_NULL);
    rt_pin_irq_enable(KEY_RIGHT_PIN , PIN_IRQ_ENABLE);

    rt_pin_mode(KEY_LEFT_PIN , PIN_MODE_INPUT_PULLUP);
    rt_pin_attach_irq(KEY_LEFT_PIN , PIN_IRQ_MODE_FALLING , key_left_handle, RT_NULL);
    rt_pin_irq_enable(KEY_LEFT_PIN , PIN_IRQ_ENABLE);

    rt_event_init(&event, "ugui_evt", RT_IPC_FLAG_FIFO);

    rt_thread_init(&ugui_thread,
                   "ugui",
                   ugui_thread_entry,
                   RT_NULL,
                   &ugui_thread_stack[0],
                   sizeof(ugui_thread_stack),
                   10, 20);
    rt_thread_startup(&ugui_thread);

    return 0;
}

MSH_CMD_EXPORT(ugui_demo, ugui_demo);