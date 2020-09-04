#include <string.h>

#define DBG_ENABLE
#define DBG_SECTION_NAME "sht3x"
#define DBG_LEVEL DBG_LOG
#define DBG_COLOR
#include <rtdbg.h>

#include "sht3x.h"

#define CRC_POLY 0x31 

/**
 * This function write 2-byte cmd to SHT3x 
 *
 * @param dev the pointer of device driver structure
 * @param cmd the 16bit command to be sent to SHT3x
 *
 * @return the cmd transfer status, RT_EOK reprensents setting successfully.
 */
static rt_err_t write_cmd(sht3x_device_t dev, rt_uint16_t cmd)
{
    struct rt_i2c_msg msgs;
    rt_uint8_t buf[2] ;
    buf[0] = cmd >> 8 ;
    buf[1] = cmd & 0xFF ;

    msgs.addr = dev->sht3x_addr;
    msgs.flags = RT_I2C_WR;
    msgs.buf = buf;
    msgs.len = 2;

    if (rt_i2c_transfer(dev->i2c, &msgs, 1) == 1)
        return RT_EOK;
    else
        return -RT_ERROR;
}

/**
 *  This function read len bytes from dev and store the bytes in to buf 
 * 
 * @param dev the pointer of device driver structure
 * @param buf the pointer to buffer array 
 * @param len the number of bytes to be read 
 * 
 * @return the i2c read status, RT_EOK represents success.
 */
static rt_err_t read_bytes(sht3x_device_t dev, rt_uint8_t * buf, rt_uint8_t len)
{
    if(rt_i2c_master_recv(dev->i2c, dev->sht3x_addr, RT_I2C_RD, buf, len) == len){
        return RT_EOK ;
    }else
    {
        return - RT_ERROR ;
    }
    
}

/**
 * This function calculate CRC value of bytes in buffer
 * CRC_POLY is predefined as 0x31
 * 
 * @param buf the pointer to buffer array 
 * @param len the length of buffer array 
 * 
 * @return calculated CRC value.
 */
static rt_uint8_t crc8(rt_uint8_t * buf, rt_uint8_t len)
{
    rt_uint8_t crc = 0xFF ;
    rt_uint8_t i, j ;
    for(j = len; j; j --){
        crc ^= *buf ++ ;
        for( i = 8; i; i --){
            crc = (crc & 0x80) ? (crc << 1) ^ CRC_POLY : (crc << 1) ;
        }
    }
    return crc ;
}

/** 
 * This function read temperature and humidity by single shot mode 
 * Attention: 
 *  - rt_thread_mdelay() is called to wait for SHT3x to be ready to read
 *  - the temperature and humidity is stored in the device driver structure
 * 
 * @param dev the pointer of device driver structure
 * 
 * @return the status of read data from SHT3x, RT_EOK means success.
 */
rt_err_t sht3x_read_singleshot(sht3x_device_t dev)
{
    rt_uint8_t temp[6] ;
    rt_err_t result;
    RT_ASSERT(dev);

    result = rt_mutex_take(dev->lock, RT_WAITING_FOREVER);
    if (result == RT_EOK)
    {
        if( write_cmd(dev, dev->cmd_readout) == RT_EOK)
        {
			// wait about 5 ms
			rt_thread_mdelay(5);
            if(read_bytes(dev, temp, 6) == RT_EOK){
                if(crc8(temp, 2) == temp[2]){
                    dev->temperature = -45.0 + (temp[1] | temp[0] << 8) * 175.0 / (0xFFFF - 1) ; //sensor temperature convert to reality
                    result = RT_EOK ;
                }else{
                    result = -RT_ERROR ;
                }
                if(crc8(temp + 3, 2) == temp[5]){
                    dev->humidity = (temp[4] | temp[3] << 8) * 0.0015259022 ; //sensor humidity convert to reality
                }else{
                    result = -RT_ERROR ;
                }
            }else
            {
                result = -RT_ERROR ;
            }
        }
        else
        {
            result = -RT_ERROR ;
        }
	rt_mutex_release(dev->lock);
    }
    else
    {
        LOG_E("Taking mutex of SHT3x failed.");
        result = -RT_ERROR;
    }
	
    return result;
}

/**
 * This function resets all parameter with default
 *
 * @param dev the pointer of device driver structure
 *
 * @return the softreset status, RT_EOK reprensents setting successfully.
 */
rt_err_t sht3x_softreset(sht3x_device_t dev)
{
    RT_ASSERT(dev);
    
    if( write_cmd(dev, CMD_SOFT_RESET) == RT_EOK)
    {
        return RT_EOK;
    }else{
        return RT_ERROR ;
    }
}

/** 
 * This function clear the status register in SHT3x  
 * 
 * @param dev the pointer of device driver structure
 * 
 * @return the command transfer status, RT_EOK means success.
 */
rt_err_t sht3x_clear_status(sht3x_device_t dev)
{
    RT_ASSERT(dev);

    if( write_cmd(dev, CMD_CLEAR_STATUS) == RT_EOK)
    {
        return RT_EOK ;
    }else{
        return RT_ERROR ;
    }
}

/**
 * This function read the status register from SHT3x
 * Attention:
 *  - the status word is stored in device driver structure
 * 
 * @param dev the pointer of device driver structure
 * 
 * @return the command transfer status, RT_EOK means success.
 */
rt_err_t sht3x_read_status(sht3x_device_t dev)
{
    rt_uint8_t buf[3];
    RT_ASSERT(dev);
    if( write_cmd(dev, CMD_READ_STATUS) == RT_EOK)
    {
        read_bytes(dev, buf, 3);
        if( crc8(buf, 2) == buf[2])
        {
            dev->status.status_word = ( buf[0] << 8 ) | buf[1];
            return RT_EOK ;
        }
    }

    return -RT_ERROR ;
}

/**
 * This function enable heater
 * @param dev the pointer of device driver structure
 * 
 * @return the command transfer status, RT_EOK means success.
 */
rt_err_t sht3x_enable_heater(sht3x_device_t dev)
{
    RT_ASSERT(dev);
    if( write_cmd(dev, CMD_HEATER_ENABLE) == RT_EOK)
    {
        return RT_EOK ;
    }else
    {
        return -RT_ERROR ;
    }
}

/**
 * This function disable heater
 * @param dev the pointer of device driver structure
 * 
 * @return the command transfer status, RT_EOK means success.
 */
rt_err_t sht3x_disable_heater(sht3x_device_t dev)
{
    RT_ASSERT(dev);
    if( write_cmd(dev, CMD_HEATER_DISABLE) == RT_EOK)
    {
        return RT_EOK ;
    }else
    {
        return -RT_ERROR ;
    }
}

/**
 * This function write accelerated response time command to SHT3x
 * @param dev the pointer of device driver structure
 * 
 * @return the command transfer status, RT_EOK means success.
 */
rt_err_t sht3x_acc_resp_time(sht3x_device_t dev)
{
    RT_ASSERT(dev);
    if( write_cmd(dev, CMD_ART) == RT_EOK)
    {
        return RT_EOK ;
    }else
    {
        return -RT_ERROR ;
    }
}

/**
 * This function write break command to SHT3x to break out of continuous readout mode
 * @param dev the pointer of device driver structure
 * 
 * @return the command transfer status, RT_EOK means success.
 */
rt_err_t sht3x_break(sht3x_device_t dev)
{
    RT_ASSERT(dev);
    if( write_cmd(dev, CMD_BREAK) == RT_EOK)
    {
        return RT_EOK ;
    }else
    {
        return -RT_ERROR ;
    }
}

/**
 * This function initializes sht3x registered device driver
 *
 * @param i2c_bus_name the name of i2c device used by SHT3x device
 * @param sht3x_addr the address of SHT3x device
 * 
 * @return the pointer to sht3x device driver structure.
 */
sht3x_device_t sht3x_init(const char *i2c_bus_name, rt_uint8_t sht3x_addr)
{
    sht3x_device_t dev;
    RT_ASSERT(i2c_bus_name);

    dev = rt_calloc(1, sizeof(struct sht3x_device));
    if (dev == RT_NULL)
    {
        LOG_E("Can't allocate memory for sht3x device on '%s' ", i2c_bus_name);
        return RT_NULL;
    }

    if(sht3x_addr == SHT3X_ADDR_PD || sht3x_addr == SHT3X_ADDR_PU){
        dev->sht3x_addr = sht3x_addr ;
    }else{
        LOG_E("Illegal sht3x address:'%x'", sht3x_addr);
        rt_free(dev);
        return RT_NULL;
    }
    
    dev->i2c = rt_i2c_bus_device_find(i2c_bus_name);

    if (dev->i2c == RT_NULL)
    {
        LOG_E("Can't find sht3x device on '%s' ", i2c_bus_name);
        rt_free(dev);
        return RT_NULL;
    }

    dev->lock = rt_mutex_create("mutex_sht3x", RT_IPC_FLAG_FIFO);
    if (dev->lock == RT_NULL)
    {
        LOG_E("Can't create mutex for sht3x device on '%s' ", i2c_bus_name);
        rt_free(dev);
        return RT_NULL;
    }

    // I2C streching disabled, medium repeatability for default single shot readout
    dev->cmd_readout = CMD_MEAS_POLLING_M ;

	// clear the status register
	sht3x_clear_status(dev);
	
    return dev;
}

/**
 * This function releases memory and deletes mutex lock
 *
 * @param dev the pointer of device driver structure
 */
void sht3x_deinit(sht3x_device_t dev)
{
    RT_ASSERT(dev);

    rt_mutex_delete(dev->lock);

    rt_free(dev);
}

/**
 * This function is exported to MSH commands list 
 * Usage example:
 *  - sht3x probe i2c1 pu : initialize sht3x device on i2c1 bus with address pin pulled up(i2c address 0x45)
 *  - sht3x probe i2c1: initialize sht3x device one i2c1 bus with address pin pulled down by default(i2c address 0x44)
 *  - sht3x read: read and print temperature and humidity from previously initialized sht3x
 */
void sht3x(int argc, char *argv[])
{
    static sht3x_device_t dev = RT_NULL;
    rt_uint8_t sht_addr = SHT3X_ADDR_PD ;
    
    if (argc > 1)
    {
        if (!strcmp(argv[1], "probe"))
        {
            if (argc >= 3)
            {
                /* initialize the sensor when first probe */
                if (!dev || strcmp(dev->i2c->parent.parent.name, argv[2]))
                {
                    /* deinit the old device */
                    if(dev)
                    {
						rt_kprintf("Deinit sht3x");
                        sht3x_deinit(dev);
                    }
                    // no else needed here
                    if( argc > 3)
                    {
                        if( !strcmp("pd", argv[3]))
                        {
                            sht_addr = SHT3X_ADDR_PD ;
                        }
                        else if( !strcmp("pu", argv[3]))
                        {
                            sht_addr = SHT3X_ADDR_PU ;
                        }
                        else
                        {
                            rt_kprintf("Illegal sht3x address, using 0x44 by default\n");
                            sht_addr = SHT3X_ADDR_PD ; // pulled down by default: 0x44 
                        }
                    }
                    // no else needed here

                    dev = sht3x_init(argv[2], sht_addr);
                    if(!dev)
                    {
                        rt_kprintf("sht3x probe failed, check input args\n");
                    }else
					{
						rt_kprintf("sht3x probed, addr:0x%x\n", sht_addr) ;
					}
                }
            }
            else
            {
                rt_kprintf("sht3x probe <i2c dev name>   - probe sensor by given name\n");
            }
        }
        else if (!strcmp(argv[1], "read"))
        {
            if (dev)
            {
                /* read the sensor data */
                sht3x_read_singleshot(dev);

                rt_kprintf("sht3x humidity   : %d.%d \n", (int)dev->humidity, (int)(dev->humidity * 10) % 10);
                rt_kprintf("sht3x temperature: %d.%d \n", (int)dev->temperature, (int)(dev->temperature * 10) % 10);
            }
            else
            {
                rt_kprintf("Please using 'sht3x probe <i2c dev name> <pu/pd>' first\n");
            }
        }
		else if (!strcmp(argv[1], "status"))
		{
			if(dev)
			{
				if(sht3x_read_status(dev) == RT_EOK)
				{
					rt_kprintf("sht3x status:\n");
					rt_kprintf("\tchecksum:\t%d\t- 0 means checksum correct\n", dev->status.bits.checksum_ok);
					rt_kprintf("\tcommand:\t%d\t- 0 means last cmd executed OK\n", dev->status.bits.command_ok);
					rt_kprintf("\treset deteced:\t%d\n", dev->status.bits.reset_detected);
					rt_kprintf("\talert pending:\t%d\n", dev->status.bits.alert_pending);
					rt_kprintf("\tT track alert:\t%d\n", dev->status.bits.T_tracking_alert);
					rt_kprintf("\tRH track alert:\t%d\n", dev->status.bits.RH_tracking_alert);
					rt_kprintf("\theater enabled:\t%d\n", dev->status.bits.heater);
				}else
				{
					rt_kprintf("sht3x status not read\n");
				}
			}else
			{
				rt_kprintf("Please using 'sht3x probe <i2c dev name> <pu/pd>' first\n");
			}
		}
		else if (!strcmp(argv[1], "reset"))
		{
			if(dev)
			{
				if(sht3x_softreset(dev) == RT_EOK)
				{
					rt_kprintf("sht3x reset cmd sent\n");
				}else
				{
					rt_kprintf("sht3x reset cmd not sent\n");
				}
			}else
			{
				rt_kprintf("Please using 'sht3x probe <i2c dev name> <pu/pd>' first\n");
			}
		}
		else if (!strcmp(argv[1], "heater"))
		{
			if(dev)
			{
				if( !strcmp(argv[2], "on"))
				{
					if(sht3x_enable_heater(dev) == RT_EOK)
					{
						rt_kprintf("sht3x heater cmd sent\n");
					}else
					{
						rt_kprintf("sht3x heater cmd not sent\n");
					}
				}else if( !strcmp(argv[2], "off"))
				{
					if(sht3x_disable_heater(dev) == RT_EOK)
					{
						rt_kprintf("sht3x heater cmd sent\n");
					}else
					{
						rt_kprintf("sht3x heater cmd not sent\n");
					}
				}
				else{
					rt_kprintf("Please input correct format:sht3x heater on/off\n");
				}
			}else
			{
				rt_kprintf("Please using 'sht3x probe <i2c dev name> <pu/pd>' first\n");
			}
		}
        else
        {
            rt_kprintf("Unknown command. Enter 'sht3x' for help\n");
        }
    }
    else
    {
        rt_kprintf("Usage:\n");
        rt_kprintf("\tsht3x probe <i2c dev name> <pu/pd> -- probe sensor by i2c dev name and pull config\n");
        rt_kprintf("\tsht3x read -- read sensor sht3x data\n");
		rt_kprintf("\tsht3x status -- status register of sht3x\n");
		rt_kprintf("\tsht3x reset -- send soft reset command to sht3x\n");
		rt_kprintf("\tsht3x heater <on/off> -- turn on/off heater of sht3x\n");
    }
}

MSH_CMD_EXPORT(sht3x, sht3x sensor);
