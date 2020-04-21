
#include <stdint.h>
#include "i2c_core.h"
#include "i2c_hw.h"
#include "_24cxx_dev.h"	
#include "fm24clxx.h"

#define	USE_24CXX_TEST		/*test enable*/

int hw_i2c_send_then_recv(uint8_t slave_addr, const void *send_buff, 
							 uint32_t send_size, void *recv_buff, uint32_t recv_size)
{
	struct i2c_dev_message ee24_msg[2];
	uint8_t 	ret_size = 0;
	
	ee24_msg[0].addr  = slave_addr;
	ee24_msg[0].flags = I2C_BUS_WR;
	ee24_msg[0].buff  = (uint8_t*)send_buff;
	ee24_msg[0].size  = send_size;
	ee24_msg[1].addr  = slave_addr;
	ee24_msg[1].flags = I2C_BUS_RD;	
	ee24_msg[1].buff  = (uint8_t*)recv_buff;
	ee24_msg[1].size  = recv_size;

	ret_size = i2c_bus_xfer(&i2c1_dev, ee24_msg, 2);

	if(ret_size == 2)
	{
		return _24CXX_OK;
	}
	else
	{
		return _24CXX_ERR_I2C_WR;	
	}
}

int hw_i2c_send_then_send(uint8_t slave_addr, const void *send_buff1,
							 uint32_t send_size1,const void *send_buff2, uint32_t send_size2)
{
	struct i2c_dev_message ee24_msg[2];
	uint8_t ret_size = 0;
	
	ee24_msg[0].buff  = (uint8_t*)send_buff1;
	ee24_msg[0].addr  = slave_addr;
	ee24_msg[0].flags = I2C_BUS_WR;
	ee24_msg[0].size  = send_size1;
	ee24_msg[1].addr  = slave_addr;
	ee24_msg[1].flags = I2C_BUS_WR  | I2C_BUS_NO_START;
	ee24_msg[1].buff  = (uint8_t*)send_buff2;
	ee24_msg[1].size  = send_size2;
	ret_size = i2c_bus_xfer(&i2c1_dev, ee24_msg, 2);
	
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
const _24cxx_dev_t fm24clxx_dev =
{
	hw_i2c_send_then_recv,
	hw_i2c_send_then_send,
	0x50,				/*fram address*/
	_24C16_E,			/*eeprom model,eg FM24C16*/
	0,					/*no write protect*/
	0,					/*delay fun,FRAM no need*/
};


/**
 * @brief  test
 * @param  none
 * @retval none
 */
#ifdef	USE_24CXX_TEST

#include <string.h>

#define		WR_SIZE			(2*1024)
#define		WR_ADDR			0

static uint8_t write_buf[WR_SIZE];
static uint8_t read_buf[WR_SIZE];

int16_t _24cxx_test(void)
{
	uint32_t i,error = 0;
	_24cxx_dev_t *p = (_24cxx_dev_t*)&fm24clxx_dev;	
	
	_24cxx_read(p,WR_ADDR,read_buf,WR_SIZE); 
	_24cxx_read(p,WR_ADDR,read_buf,WR_SIZE);
	
	for( i=0; i<WR_SIZE; i++)  
	{   
		write_buf[i] = i;   
	}
	error = _24cxx_write(p,WR_ADDR, write_buf, WR_SIZE);	 
	_24cxx_read(p,WR_ADDR, read_buf, WR_SIZE); 

	memset(write_buf,0,sizeof(write_buf));  
	error = _24cxx_write(p,WR_ADDR, write_buf, WR_SIZE);	
	_24cxx_read(p,WR_ADDR, read_buf,WR_SIZE); 
	
	return error;
}

#endif

