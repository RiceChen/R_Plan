#ifndef __SHT3X_H__
#define __SHT3X_H__

#include <rthw.h>
#include <rtthread.h>
#include <rtdevice.h>

#define SHT3X_REPETABILITY_LOW 0
#define SHT3X_REPETABILITY_MEDIUM 1
#define SHT3X_REPETABILITY_HIGH 2

/* sht3x commands define */
// read serial number:
#define CMD_READ_SERIALNBR  0x3780     
// read status register:
#define CMD_READ_STATUS     0xF32D     
// clear status register:
#define CMD_CLEAR_STATUS    0x3041  
// enabled heater:
#define CMD_HEATER_ENABLE   0x306D   
// disable heater  
#define CMD_HEATER_DISABLE  0x3066      
// soft reset
#define CMD_SOFT_RESET      0x30A2      
// accelerated response time
#define CMD_ART             0x2B32
// break, stop periodic data acquisition mode
#define CMD_BREAK           0x3093

// measurement: polling, high repeatability
#define CMD_MEAS_POLLING_H 0x2400  
// measurement: polling, medium repeatability
#define CMD_MEAS_POLLING_M 0x240B  
// measurement: polling, low repeatability
#define CMD_MEAS_POLLING_L 0x2416 

#define SHT3X_ADDR_PD 0x44 // addr pin pulled down: 0x44
#define SHT3X_ADDR_PU 0x45 // addr pin pulled down: 0x45

typedef union sht3x_status_word
{
    rt_uint16_t status_word;
    struct status_bits
    {
        rt_uint16_t checksum_ok : 1 ;
        rt_uint16_t command_ok : 1 ;
        rt_uint16_t reserved_3 : 2 ;
        rt_uint16_t reset_detected : 1 ;
        rt_uint16_t reserved_2 : 5 ;
        rt_uint16_t T_tracking_alert : 1 ;
        rt_uint16_t RH_tracking_alert : 1 ;
        rt_uint16_t reserved_1 : 1 ;
        rt_uint16_t heater : 1 ;
        rt_uint16_t reserved_0 : 1 ;
        rt_uint16_t alert_pending: 1 ;
    } bits;
}sht3x_status;

struct sht3x_device
{
    struct rt_i2c_bus_device *i2c;
    rt_uint8_t sht3x_addr ;
    rt_mutex_t lock;
    float temperature ;
    float humidity ;
    /* the command for data readout: change it as you need */ 
    rt_uint16_t cmd_readout;
    sht3x_status status ;
};
typedef struct sht3x_device *sht3x_device_t;

/**
 * This function write 2-byte cmd to SHT3x 
 *
 * @param dev the pointer of device driver structure
 * @param cmd the 16bit command to be sent to SHT3x
 *
 * @return the cmd transfer status, RT_EOK reprensents setting successfully.
 */
static rt_err_t write_cmd(sht3x_device_t dev, rt_uint16_t cmd);

/**
 *  This function read len bytes from dev and store the bytes in to buf 
 * 
 * @param dev the pointer of device driver structure
 * @param buf the pointer to buffer array 
 * @param len the number of bytes to be read 
 * 
 * @return the i2c read status, RT_EOK represents success.
 */
static rt_err_t read_bytes(sht3x_device_t dev, rt_uint8_t * buf, rt_uint8_t len);

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
rt_err_t sht3x_read_singleshot(sht3x_device_t dev);

/**
 * This function resets all parameter with default
 *
 * @param dev the pointer of device driver structure
 *
 * @return the softreset status,RT_EOK reprensents setting successfully.
 */
rt_err_t sht3x_softreset(sht3x_device_t dev);

/** 
 * This function clear the status register in SHT3x  
 * 
 * @param dev the pointer of device driver structure
 * 
 * @return the command transfer status, RT_EOK means success.
 */
rt_err_t sht3x_clear_status(sht3x_device_t dev);

/**
 * This function read the status register from SHT3x
 * Attention:
 *  - the status word is stored in device driver structure
 * 
 * @param dev the pointer of device driver structure
 * 
 * @return the command transfer status, RT_EOK means success.
 */
rt_err_t sht3x_read_status(sht3x_device_t dev);

/**
 * This function enable heater
 * @param dev the pointer of device driver structure
 * 
 * @return the command transfer status, RT_EOK means success.
 */
rt_err_t sht3x_enable_heater(sht3x_device_t dev);

/**
 * This function disable heater
 * @param dev the pointer of device driver structure
 * 
 * @return the command transfer status, RT_EOK means success.
 */
rt_err_t sht3x_disable_heater(sht3x_device_t dev);

/**
 * This function write accelerated response time command to SHT3x
 * @param dev the pointer of device driver structure
 * 
 * @return the command transfer status, RT_EOK means success.
 */
rt_err_t sht3x_acc_resp_time(sht3x_device_t dev);

/**
 * This function write break command to SHT3x to break out of continuous readout mode
 * @param dev the pointer of device driver structure
 * 
 * @return the command transfer status, RT_EOK means success.
 */
rt_err_t sht3x_break(sht3x_device_t dev);

/**
 * This function initializes sht3x registered device driver
 *
 * @param dev the name of sht3x device
 *
 * @return the sht3x device.
 */
sht3x_device_t sht3x_init(const char *i2c_bus_name, rt_uint8_t sht3x_addr);

/**
 * This function releases memory and deletes mutex lock
 *
 * @param dev the pointer of device driver structure
 */
void sht3x_deinit(sht3x_device_t dev);

#endif /* _SHT30_H__ */
