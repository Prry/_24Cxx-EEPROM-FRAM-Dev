
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

#define AT24CXX_EXPORT_MSH				/* 导出到RTT msh测试 */

#define	AT24CXX_I2C_BUS		  "i2c1"	/* i2c linked */
#define	AT24CXX_DEVICE_NAME	  "24cxx"	/* register device name */

static struct rt_device rt_at24c_dev;	 /* at24cxx device */
static struct rt_i2c_bus_device *rt_at24c_i2c;

static void page_write_delay(void) 
{
	uint16_t i;
	
	i = 0xFFFF;
	while(i--);
}

static int hw_i2c_send_then_recv(rt_uint8_t slave_addr, const void *send_buff, 
							 rt_uint32_t send_size, void *recv_buff, rt_uint32_t recv_size)
{
	struct rt_i2c_msg ee24_msg[2];
	rt_uint8_t ret_size = 0;

	ee24_msg[0].addr  = slave_addr;
	ee24_msg[0].flags = RT_I2C_WR;
	ee24_msg[0].buf   = (rt_uint8_t*)send_buff;
	ee24_msg[0].len   = send_size;
	ee24_msg[1].addr  = slave_addr;
	ee24_msg[1].flags = RT_I2C_RD;	
	ee24_msg[1].buf   = (rt_uint8_t*)recv_buff;
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

static int hw_i2c_send_then_send(rt_uint8_t slave_addr, const void *send_buff1,
							 rt_uint32_t send_size1,const void *send_buff2, rt_uint32_t send_size2)
{
	struct rt_i2c_msg ee24_msg[2];
	rt_uint8_t ret_size = 0;

	ee24_msg[0].buf   = (rt_uint8_t*)send_buff1;
	ee24_msg[0].addr  = slave_addr;
	ee24_msg[0].flags = RT_I2C_WR;
	ee24_msg[0].len   = send_size1;
	ee24_msg[1].addr  = slave_addr;
	ee24_msg[1].flags = RT_I2C_WR  | RT_I2C_NO_START;
	ee24_msg[1].buf   = (rt_uint8_t*)send_buff2;
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
 * @brief  FM24CL04 device init
 */
const _24cxx_dev_t at24cxx_dev =
{
	hw_i2c_send_then_recv,
	hw_i2c_send_then_send,
	0x50,						/* eeprom address */
	_24C04_E,					/* eeprom model,eg FM24CL04 */
	RT_NULL,					/* no write protect */
	RT_NULL,					/* delay function,FRAM no need */
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
	rt_int16_t ret = _24CXX_OK;
	
  	p = (_24cxx_dev_t*)dev->user_data;
	
  	ret = _24cxx_read(p, pos, buffer, size); 
	if (_24CXX_OK == ret)
	{
	  	return size;
	}
	else
	{
		return RT_ERROR;
	}
}

static rt_size_t rt_at24cxx_write(rt_device_t dev, rt_off_t pos, const void *buffer, rt_size_t size)
{
    _24cxx_dev_t *p = RT_NULL;
	rt_int16_t ret = _24CXX_OK;
	
  	p = (_24cxx_dev_t*)dev->user_data;
	
  	ret = _24cxx_write(p, pos, (rt_uint8_t*)buffer, size); 
	if (_24CXX_OK == ret)
	{
	  	return size;
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


/* 导出到msh shell */
#ifdef AT24CXX_EXPORT_MSH

static int read_from_eeprom(rt_device_t pdev, int addr, int size)
{
  	rt_uint8_t *pbuf = RT_NULL;
	rt_size_t ret_size = 0;
	rt_uint16_t index = 0;
	
	pbuf = (rt_uint8_t*)rt_malloc(512);
	
	if (RT_NULL == pbuf)
	{
		LOG_E("malloc memory falied\n");
		
		return -1;
	}
	ret_size = rt_device_read(pdev, addr, pbuf, 512);
	
	if (ret_size != 512)
	{
		LOG_E("read 24cxx failed\n");
		goto __exit;
	}
	
	for (index=0; index<size; index++, addr++)
	{
		if ((index % 16) == 0) 
		{
		  	rt_kprintf("\n %.4x|  ", addr);
		}
		else if((index % 8) == 0 ) 
		{
		  	rt_kprintf("  ");
		}
		rt_kprintf("%.2x ", *(pbuf+index));
	}
	rt_kprintf("\n\n");
__exit:	
	rt_free(pbuf);
	
	return 0;
}

static int write_to_eeprom(rt_device_t pdev, int addr)
{
  	rt_uint8_t *pbuf = RT_NULL;
	rt_uint16_t index = 0;
	rt_size_t ret_size = 0;
	
	pbuf = (rt_uint8_t*)rt_malloc(512);
	
	if (RT_NULL == pbuf)
	{
		LOG_E("malloc memory falied\n");
	}
	
	for(index=0; index<512; index++)
	{/* fill data */
		*(pbuf+index) = index&0xff;
	}
	
	ret_size = rt_device_write(pdev, addr, pbuf, 512);
	
	if (ret_size != 512)
	{
		LOG_E("write 24cxx failed\n");
		goto __exit;
	}
	
	for (index=0, addr=0; index<512; index++, addr++)
	{
		if ((index % 16) == 0) 
		{
		  	rt_kprintf("\n %.4x|  ", addr);
		}
		else if ((index % 8) == 0) 
		{
		  	rt_kprintf("  ");
		}
		rt_kprintf("%.2x ", *(pbuf+index));
	}
	rt_kprintf("\n\n");
__exit:
	rt_free(pbuf);
	
	return 0;
}

static int erase_to_eeprom(rt_device_t pdev, int addr)
{
  	rt_uint8_t *pbuf = RT_NULL;
	rt_size_t ret_size = 0;
	
	pbuf = (rt_uint8_t*)rt_malloc(512);
	
	if (RT_NULL == pbuf)
	{
		LOG_E("malloc memory falied\n");
	}
	
	rt_memset(pbuf, 0, 512);
	
	ret_size = rt_device_write(pdev, addr, pbuf, 512);
	
	if (ret_size != 512)
	{
		LOG_E("erase 24cxx failed\n");
	}
	else
	{
		LOG_D("erase eeprom succeed\n");
	}
	
	
	rt_free(pbuf);
	
	return 0;
}

int _24cxx_wr(int argc, char **argv)
{
  	rt_err_t ret = RT_EOK;
	rt_device_t pdev = RT_NULL;
  	int op = 0;
	
	if (argc < 3)
	{
		goto __usage;
	}
	
	op = argv[2][0];
	pdev = rt_device_find(argv[1]);
	
	if (RT_NULL == pdev)
	{
	  	LOG_E("\'%s\' device not found\n", argv[1]);
		return -1;
	}
	ret = rt_device_open(pdev, RT_DEVICE_OFLAG_RDWR);
	
	if (RT_EOK != ret)
	{
		LOG_E("\'%s\' device open failed\n", argv[1]);
	  	goto __exit;
	}
	switch(op)
	{
	case 'r':
		rt_kprintf("  reading 512 bytes from 24cxx\n");
		read_from_eeprom(pdev, 0, 512);
		break;
		
	case 'w':
		rt_kprintf("  writing 0x00-0xff into 24cxx\n");
		write_to_eeprom(pdev, 0);
		break;
		
	case 'e':
		rt_kprintf("  erase 0x00 into 24Cxx\n");
		erase_to_eeprom(pdev, 0);
		break;
		
	default:
	  	rt_device_close(pdev);
	  	goto __usage;
		break;
	}
__exit:
	rt_device_close(pdev);
	
	return 0;
	
__usage:
  	rt_kprintf("usage:\n");
	rt_kprintf("_24cxx_wr [eeprom dev] r     - read 24cxx \n");
	rt_kprintf("_24cxx_wr [eeprom dev] w     - write 24cxx \n");
	rt_kprintf("_24cxx_wr [eeprom dev] e     - erase 24cxx \n");
	
	return -1;
}
MSH_CMD_EXPORT(_24cxx_wr, _24cxx read/write program);

#endif