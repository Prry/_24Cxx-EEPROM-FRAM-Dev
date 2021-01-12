
/*
 * Copyright (c) 2021 panrui
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-01-12     panrui      the first version
 */
#include <stdint.h>
#include <rtthread.h>
#include <rtdevice.h>
#include "at24cxx.h"
#include "_24cxx_dev.h"	

#define DBG_TAG "24cxx"
#define DBG_LVL DBG_LOG
#include <rtdbg.h>

#define	AT24CXX_I2C_BUS		  "i2c1"	/* i2c linked */
#define	AT24CXX_DEVICE_NAME	  "24cxx"	/* register device name */

static struct rt_device rt_at24c_dev;	 /* at24cxx device */
static struct rt_i2c_bus_device *rt_at24c_i2c;

/**
 * @brief  delay function
 * @param  none
 * @retval none
 */
static void page_write_delay(void) 
{
	uint16_t i;
	
	i = 0xFFFF;
	while(i--);
}

static int hw_i2c_send_then_recv(uint8_t slave_addr, const void *send_buff, 
							 uint32_t send_size, void *recv_buff, uint32_t recv_size)
{
	struct rt_i2c_msg ee24_msg[2];
	uint8_t 	ret_size = 0;

	ee24_msg[0].addr  = slave_addr;
	ee24_msg[0].flags = RT_I2C_WR;
	ee24_msg[0].buf   = (uint8_t*)send_buff;
	ee24_msg[0].len   = send_size;
	ee24_msg[1].addr  = slave_addr;
	ee24_msg[1].flags = RT_I2C_RD;	
	ee24_msg[1].buf   = (uint8_t*)recv_buff;
	ee24_msg[1].len   = recv_size;

	ret_size = rt_i2c_transfer(rt_at24c_i2c, ee24_msg, 2);

	if(ret_size == 2)
	{
		return _24CXX_OK;
	}
	else
	{
		return _24CXX_ERR_I2C_WR;	
	}
}

static int hw_i2c_send_then_send(uint8_t slave_addr, const void *send_buff1,
							 uint32_t send_size1,const void *send_buff2, uint32_t send_size2)
{
	struct rt_i2c_msg ee24_msg[2];
	uint8_t ret_size = 0;

	ee24_msg[0].buf   = (uint8_t*)send_buff1;
	ee24_msg[0].addr  = slave_addr;
	ee24_msg[0].flags = RT_I2C_WR;
	ee24_msg[0].len   = send_size1;
	ee24_msg[1].addr  = slave_addr;
	ee24_msg[1].flags = RT_I2C_WR  | RT_I2C_NO_START;
	ee24_msg[1].buf   = (uint8_t*)send_buff2;
	ee24_msg[1].len   = send_size2;
	ret_size = rt_i2c_transfer(rt_at24c_i2c, ee24_msg, 2);
	
	if(ret_size == 2)
	{
		return _24CXX_OK;
	}
	else
	{
		return _24CXX_ERR_I2C_WR;	
	}
}

/**
 * @brief  device init
 */
const _24cxx_dev_t at24cxx_dev =
{
	hw_i2c_send_then_recv,
	hw_i2c_send_then_send,
	0x50,						/* eeprom address */
	_24C02_E,					/* eeprom model,eg AT24C02 */
	RT_NULL,					/* no write protect */
	page_write_delay,			/* delay function */
};

static rt_err_t rt_at24cxx_open(rt_device_t dev, rt_uint16_t flag)
{
    if (dev->rx_indicate != RT_NULL)
    {
        /* open interrupt */
    }

    return RT_EOK;
}

static rt_size_t rt_at24cxx_read(rt_device_t dev, rt_off_t pos, void *buffer, rt_size_t size)
{
  	_24cxx_dev_t *p = RT_NULL;
	int16_t ret = _24CXX_OK;
	
  	p = (_24cxx_dev_t*)dev->user_data;
	
  	ret = _24cxx_read(p, pos, buffer, size); 
	if (_24CXX_OK == ret)
	{
	  	return RT_EOK;
	}
	else
	{
		return RT_ERROR;
	}
}

static rt_size_t rt_at24cxx_write(rt_device_t dev, rt_off_t pos, const void *buffer, rt_size_t size)
{
    _24cxx_dev_t *p = RT_NULL;
	int16_t ret = _24CXX_OK;
	
  	p = (_24cxx_dev_t*)dev->user_data;
	
  	ret = _24cxx_write(p, pos, (uint8_t*)buffer, size); 
	if (_24CXX_OK == ret)
	{
	  	return RT_EOK;
	}
	else
	{
		return RT_ERROR;
	}
}

int rt_hw_at24cxx_init(void)
{		
    rt_at24c_i2c = rt_i2c_bus_device_find(AT24CXX_I2C_BUS);
    if (rt_at24c_i2c == RT_NULL)
    {
        LOG_E("i2c bus device %s not found!\r\n", AT24CXX_I2C_BUS);
        return -RT_ERROR;
    }				 	

    /* register rtc device */
    rt_at24c_dev.type   = RT_Device_Class_Char;
    rt_at24c_dev.init   = RT_NULL;
    rt_at24c_dev.open   = rt_at24cxx_open;
    rt_at24c_dev.close  = RT_NULL;
    rt_at24c_dev.read   = rt_at24cxx_read;
    rt_at24c_dev.write  = rt_at24cxx_write;
    rt_at24c_dev.control= RT_NULL;
    rt_at24c_dev.user_data 	= (void*)&at24cxx_dev;	/* ee24cxx device */;		
    rt_device_register(&rt_at24c_dev, AT24CXX_DEVICE_NAME, RT_DEVICE_FLAG_RDWR);
		
	LOG_D("at24cxx init succeed!\r\n");
	
    return 0;
}
INIT_DEVICE_EXPORT(rt_hw_at24cxx_init);

