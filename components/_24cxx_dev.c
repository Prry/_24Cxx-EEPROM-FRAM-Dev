/*
 * File      : _24cxx_dev.c
 * 
 * This file is part of eeprom device for 24cxx,such as at24c02/at24c16.
 * COPYRIGHT (C) 2016
 *
 * Change Logs:
 * Date           Author       Version			Notes
 * 2018-09-01     Acuity      	1.00		first version.
 *
 * Depend on:
 * i2c_core.c
 *
 * Note:
 * 
 */
 
#include "i2c_core.h"
#include "_24cxx_dev.h"	


 /**
 * @brief  get page size
 * @param  model:eeprom model
 * @retval page size
 */  
static uint16_t get_eeprom_pagesize(_24_model_t model) 
{
	switch(model)
	{
	  	case _24C01_E:
		case _24C02_E:
		  return 8;
		  
		case _24C04_E:
		case _24C08_E:
		case _24C16_E:
		  return 16;
		  
		case _24C32_E:
		case _24C64_E:
		case _24C128_E:
		case _24C256_E:
		case _24C512_E:
		case _24C1024_E:
		  return 32;
		default:
	      return 0;
	}
}

  /**
 * @brief  get chip size
 * @param  model:eeprom model
 * @retval chip size,uinit:byte 
 */  
static uint32_t get_eeprom_chipsize(_24_model_t model) 
{
	switch(model)
	{
	  	case _24C01_E:
		  return 128u;
		case _24C02_E:
		  return 256u;
		case _24C04_E:
		  return 512u;
		case _24C08_E:
		  return 1024u;
		case _24C16_E:
		  return 2048u;
		case _24C32_E:
		  return 4096u;
		case _24C64_E:
		  return 8192u;
		case _24C128_E:
		  return 16384u;
		case _24C256_E:
		  return 32768u;
		case _24C512_E:
		  return 65536u;
		case _24C1024_E:
		  return 131072u;
		default:
	      return 0;
	}
}

/**
 * @brief  write one page. 
 * @param  pdev pointer to the eeprom device struct.
 * @param  addr the address of write to.
 * @param  pbuf the data to write.
 * @param  size number of bytes to write..
 * @retval return 0 if 0k,anything else is considered an error.
 */
static int16_t _24cxx_write_page(_24cxx_dev_t *pdev,uint32_t addr, uint8_t *pbuf,uint32_t size)
{
	struct i2c_dev_message ee24_msg[2];
	uint8_t		buf[2];
	uint8_t 	slave_addr,ret_size = 0;
	uint16_t	ee_page_size = 0;
	
	if(pdev == 0)
	  	return _24CXX_ERR_DEV_NONE;

	ee_page_size = get_eeprom_pagesize(pdev->model);	
	if(((addr % ee_page_size) + size) > ee_page_size) /*the over flow of page size*/
		return _24CXX_ERR_PAGE_SIZE;
	if(pdev->model > _24C16_E)
	{/*24c32-24c1024*/
		slave_addr = pdev->slave_addr;
		buf[0] = (addr >>8)& 0xff;
		buf[1] = addr & 0xff;
		ee24_msg[0].size  = 2;
	}
	else
	{/*24c01-24c16*/
		slave_addr = pdev->slave_addr | (addr>>8);
		buf[0] = addr & 0xff;
		ee24_msg[0].size  = 1;
	}
	ee24_msg[0].addr  = slave_addr;
	ee24_msg[0].flags = I2C_BUS_WR;
	ee24_msg[0].buff  = buf;
	ee24_msg[1].addr  = slave_addr;
	ee24_msg[1].flags = I2C_BUS_WR  | I2C_BUS_NO_START;	
	ee24_msg[1].buff  = pbuf;
	ee24_msg[1].size  = size;
	if(pdev->wp)		/*release write protect*/
	  	pdev->wp(0);
	ret_size = i2c_bus_xfer(pdev->i2c_dev,ee24_msg,2);
	if(pdev->wp)
	  	pdev->wp(1);	/*write protect*/
	if(ret_size == 2)
	  	return _24CXX_OK;
	else
		return _24CXX_ERR_I2C_WR;
}


/**
 * @brief  write data to eeprom.
 * @param  pdev pointer to the eeprom device struct.
 * @param  addr the address of write to.
 * @param  pbuf the data to write.
 * @param  size number of bytes to write..
 * @retval return 0 if 0k,anything else is considered an error.
 */
int16_t _24cxx_write(_24cxx_dev_t *pdev,uint32_t addr, uint8_t *pbuf, uint32_t size)
{
	uint8_t  	write_len,page_offset;
	int8_t 		error = 0;
	uint16_t	ee_page_size = 0;
	
	if(pdev == 0)
	  	return _24CXX_ERR_DEV_NONE;
	if((addr+size) > get_eeprom_chipsize(pdev->model))	/*the over flow of chip size*/
	  	return _24CXX_ERR_CHIP_SIZE;
	ee_page_size = get_eeprom_pagesize(pdev->model);
	while(size > 0)
	{
		page_offset = ee_page_size - (addr % ee_page_size);
		write_len   = size > page_offset ? page_offset : size;
		error = _24cxx_write_page(pdev,addr,pbuf,write_len);	
		if(error)
		  	break;
		size = size - write_len;
		if(size > 0)
		{
			pbuf = pbuf + write_len;
			addr = addr + write_len;
		}
		if(pdev->page_write_delay)
			pdev->page_write_delay();		/*eeprom need wait*/
	}
	return error;
}

/**
 * @brief  read from the eeprom.
 * @param  pdev pointer to the eeprom device struct.
 * @param  addr the address to read from.
 * @param  pbuf where to put read data.
 * @param  size number of bytes to read.
 * @retval return 0 if 0k,anything else is considered an error.
 */
int16_t _24cxx_read(_24cxx_dev_t *pdev,uint32_t addr, uint8_t *pbuf, uint32_t size)
{  
	struct i2c_dev_message ee24_msg[2];
	uint8_t	 buf[2];
	uint8_t  slave_addr,ret_size = 0;
	
	if(pdev == 0)
	  	return _24CXX_ERR_DEV_NONE;
	if((addr+size) > get_eeprom_chipsize(pdev->model))	 
	  	return _24CXX_ERR_CHIP_SIZE;
	if(pdev->model > _24C16_E)
	{
		slave_addr 	= pdev->slave_addr;
		buf[0] 		= (addr >>8)& 0xff;
		buf[1] 		= addr & 0xff;
		ee24_msg[0].size  = 2;
	}
	else
	{
		slave_addr = pdev->slave_addr | (addr>>8);
		buf[0] = addr & 0xff;
		ee24_msg[0].size  = 1;
	}
	ee24_msg[0].buff  = buf;
	ee24_msg[0].addr  = slave_addr;
	ee24_msg[0].flags = I2C_BUS_WR;
	ee24_msg[1].addr  = slave_addr;
	ee24_msg[1].flags = I2C_BUS_RD;
	ee24_msg[1].buff  = pbuf;
	ee24_msg[1].size  = size;
	ret_size = i2c_bus_xfer(pdev->i2c_dev,ee24_msg,2);
	if(ret_size == 2)
	  	return _24CXX_OK;
	else
	  	return _24CXX_ERR_I2C_WR;
}

/**
 * @brief  erase the eeprom.
 * @param  pdev pointer to the eeprom device struct.
 * @param  addr the address to erase from.
 * @param  data padding data.
 * @param  size number of bytes to erase.
 * @retval return 0 if 0k,anything else is considered an error.
 */
int16_t _24cxx_erase(_24cxx_dev_t *pdev,uint32_t addr, uint8_t data, uint32_t size)
{
	int8_t   error = 0;
	uint16_t i;
	
	if(pdev == 0)
	  	return _24CXX_ERR_DEV_NONE;
	if((addr+size) > get_eeprom_chipsize(pdev->model))	 
	  	return _24CXX_ERR_CHIP_SIZE;
	for(i = 0;i < size;i++)
	{
		error = _24cxx_write_page(pdev,addr,&data,size);
		if(error)
		  break;
	}
				
	return error;
}

