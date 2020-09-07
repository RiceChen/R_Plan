/*
利用 IO 模拟 SPI 注册到总线
 */

#include <rtthread.h>
#include <rthw.h>
#include <rtdevice.h>
#include <stdio.h>
#include <string.h>

#include "typedef.h"
#include "spi_pub.h"

/* 增加 I2S 相关的头文件 */
#include "arm_arch.h"
#include "sys_ctrl_pub.h"
#include "drv_model_pub.h"
#include "mem_pub.h"
#include "sys_config.h"
#include "error.h"
#include "bk_rtos_pub.h"
#include "typedef.h"
#include "icu_pub.h"
#include "i2s.h"
#include "i2s_pub.h"
#include "gpio_pub.h"
#include "gpio.h"

#define SOFT_SPI_MISO   (4)
#define SOFT_SPI_MOSI   (5)
#define SOFT_SPI_CS     (3)
#define SOFT_SPI_SCLK   (2)

struct soft_spi_dev
{
    struct rt_spi_bus *spi_bus;
};

static struct soft_spi_dev *spi_dev;

static void soft_spi_init()
{
    rt_pin_mode(SOFT_SPI_CS,   PIN_MODE_OUTPUT);
    rt_pin_mode(SOFT_SPI_SCLK, PIN_MODE_OUTPUT);
    rt_pin_mode(SOFT_SPI_MOSI, PIN_MODE_OUTPUT);
    rt_pin_mode(SOFT_SPI_MISO, PIN_MODE_OUTPUT);

    rt_pin_write(SOFT_SPI_SCLK, PIN_LOW);
    rt_pin_write(SOFT_SPI_MOSI, PIN_LOW);
    rt_pin_write(SOFT_SPI_CS,   PIN_HIGH);
}

static void soft_spi_cs_enable(int enable)
{
    rt_uint32_t val;
    volatile rt_uint32_t *gpio_cfg_addr;
    gpio_cfg_addr = (volatile rt_uint32_t *)(GPIO_BASE_ADDR + SOFT_SPI_CS * 4);
    
    val = REG_READ(gpio_cfg_addr);

    val &= ~GCFG_OUTPUT_BIT;
    val |= (enable & 0x01) << GCFG_OUTPUT_POS;
    REG_WRITE(gpio_cfg_addr, val);
}

static void soft_spi_ReadWriteByte(uint8 byte)
{
    uint8 bit = 0;
    rt_uint32_t sck_reg_val, mosi_reg_val;
    volatile rt_uint32_t *gpio_cfg_addr_sck,*gpio_cfg_addr_mosi,*gpio_cfg_addr_miso;
    gpio_cfg_addr_sck  = (volatile rt_uint32_t *)(GPIO_BASE_ADDR + SOFT_SPI_SCLK * 4);
    gpio_cfg_addr_mosi = (volatile rt_uint32_t *)(GPIO_BASE_ADDR + SOFT_SPI_MOSI * 4);
    sck_reg_val = REG_READ(gpio_cfg_addr_sck);
    mosi_reg_val = REG_READ(gpio_cfg_addr_mosi);
    
    for(bit=8; bit>0; bit--)
    {   
        sck_reg_val &= ~GCFG_OUTPUT_BIT;
        sck_reg_val |= (0) << GCFG_OUTPUT_POS;
        REG_WRITE(gpio_cfg_addr_sck, sck_reg_val);
        if(byte&0x80)
        {
            mosi_reg_val &= ~GCFG_OUTPUT_BIT;
            mosi_reg_val |= (0x01) << GCFG_OUTPUT_POS;
            REG_WRITE(gpio_cfg_addr_mosi, mosi_reg_val);
        }
        else
        {
            mosi_reg_val &= ~GCFG_OUTPUT_BIT;
            mosi_reg_val |= (0) << GCFG_OUTPUT_POS;
            REG_WRITE(gpio_cfg_addr_mosi, mosi_reg_val);
        }
        byte<<=1;

        sck_reg_val &= ~GCFG_OUTPUT_BIT;
        sck_reg_val |= (0x01) << GCFG_OUTPUT_POS;
        REG_WRITE(gpio_cfg_addr_sck, sck_reg_val);
    }
    sck_reg_val &= ~GCFG_OUTPUT_BIT;
    sck_reg_val |= (0) << GCFG_OUTPUT_POS;
    REG_WRITE(gpio_cfg_addr_sck, sck_reg_val);
}

rt_err_t _soft_spi_configure(struct rt_spi_device *dev,struct rt_spi_configuration *cfg)
{
    return RT_EOK;
}

rt_uint32_t _soft_spi_xfer(struct rt_spi_device* device, struct rt_spi_message* message)
{

    struct rt_spi_configuration *config = &device->config;
    rt_uint32_t size = message->length;

    if (message->cs_take)
    {
        soft_spi_cs_enable(0);
    }

    const rt_uint8_t *send_ptr = message->send_buf;
    rt_uint8_t *recv_ptr = message->recv_buf;
    while (size--)
    {
        rt_uint8_t data = 0xFF;

        if (send_ptr != RT_NULL)
        {
            data = *send_ptr++;
            soft_spi_ReadWriteByte(data);
        }
    }
    
    if (message->cs_release)
    {
        soft_spi_cs_enable(1);
    }

    return message->length;
}

static struct rt_spi_ops soft_spi_ops = 
{
    .configure = _soft_spi_configure,
    .xfer = _soft_spi_xfer
};

int rt_soft_spi_bus_register(char *name)
{
    int result = RT_EOK;
    struct rt_spi_bus *spi_bus = RT_NULL;

    if(spi_dev)
    {
       return RT_EOK; 
    }

    spi_dev = rt_malloc(sizeof(struct soft_spi_dev));
    if(!spi_dev)
    {
        rt_kprintf("[soft spi]:malloc memory for spi_dev failed\n");
        result = -RT_ENOMEM;
        goto _exit;
    }
    memset(spi_dev,0,sizeof(struct soft_spi_dev));

    spi_bus = rt_malloc(sizeof(struct rt_spi_bus));
    if(!spi_bus)
    {
        rt_kprintf("[soft spi]:malloc memory for spi_bus failed\n");
        result = -RT_ENOMEM;
        goto _exit;
    }
    memset(spi_bus,0,sizeof(struct rt_spi_bus));

    spi_bus->parent.user_data = spi_dev;
    rt_spi_bus_register(spi_bus, name, &soft_spi_ops);
    
    return result;

_exit:
    if (spi_dev)
    {
        rt_free(spi_dev);
        spi_dev = RT_NULL;
    }

    if (spi_bus)
    {
        rt_free(spi_bus);
        spi_bus = RT_NULL;
    }
    return result;
}

static struct rt_spi_device *soft_spi_device = RT_NULL;
int rt_soft_spi_device_init(void)
{
    int result = RT_EOK;

    rt_kprintf("[soft spi]:rt_soft_spi_device_init \n");

    soft_spi_init();

    if(soft_spi_device)
    {
        return RT_EOK;
    }
    soft_spi_device = rt_malloc(sizeof(struct rt_spi_device));
    if(!soft_spi_device)
    {
        rt_kprintf("[soft spi]:malloc memory for soft spi_device failed\n");
        result = -RT_ENOMEM;
    }
    memset(soft_spi_device,0,sizeof(struct rt_spi_device));

    result = rt_soft_spi_bus_register("soft_spi");
    if(result != RT_EOK)
    {
        rt_kprintf("[soft spi]:register soft spi bus error : %d !!!\n",result);
        goto _exit;
    }

    result = rt_spi_bus_attach_device(soft_spi_device,"spi3","soft_spi",NULL);
    if(result != RT_EOK)
    {
        rt_kprintf("[soft spi]:attact spi bus error :%d !!!\n",result);
        goto _exit;
    }
    rt_kprintf("[soft spi]:rt_soft_spi_device init ok\n");
    return RT_EOK;

_exit:
    if(soft_spi_device)
    {
        rt_free(soft_spi_device);
        soft_spi_device = RT_NULL;
    }
    return result;
}
INIT_BOARD_EXPORT(rt_soft_spi_device_init);