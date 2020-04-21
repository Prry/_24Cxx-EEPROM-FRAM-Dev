#ifndef _24Cxx_DEV_H_
#define _24Cxx_DEV_H_

#include <stdint.h>

/*operation state*/
enum _24CXX_ERROR
{
	_24CXX_OK 				= 0,	
	_24CXX_ERR_I2C_WR 		= 1,
	_24CXX_ERR_DEV_NONE 	= 2,
	_24CXX_ERR_PAGE_WRITE	= 3,
	_24CXX_ERR_PAGE_SIZE	= 4,
	_24CXX_ERR_CHIP_SIZE	= 5,
};

/*eeprom model*/
typedef enum 
{
  	_24C01_E = 1,
	_24C02_E,
	_24C04_E,
	_24C08_E,
	_24C16_E,
	_24C32_E,
	_24C64_E,
	_24C128_E,
	_24C256_E,
	_24C512_E,
	_24C1024_E,
}_24_model_t;

/*24cxx eeprom devcie struct*/
typedef struct 
{
	int (*i2c_send_thend_recv)(uint8_t slave_addr, const void *send_buff, 	/* i2c bus fun */
				uint32_t send_size, void *recv_buff, uint32_t recv_size);
	int (*i2c_send_thend_send)(uint8_t slave_addr, const void *send_buff1,	/* i2c bus fun */
				uint32_t send_size1,const void *send_buff2, uint32_t send_size2);
	uint8_t	slave_addr;	/*eeprom i2c addr*/
	//uint8_t	type;		/*eeprom type, 0:eeprom;1:fram*/
	_24_model_t	model;		/*eeprom model*/
	void(*wp)(uint8_t ctrl);		/*protect of write function*/
	void(*page_write_delay)(void);	/*there is a delay in continuous writin for EEPROM,FRAM not need*/
	
}_24cxx_dev_t;

/*extern function*/
extern int16_t _24cxx_read(_24cxx_dev_t *pdev,uint32_t addr, uint8_t *pbuf, uint32_t size);
extern int16_t _24cxx_write(_24cxx_dev_t *pdev,uint32_t addr,uint8_t *pbuf,uint32_t size);
extern int16_t _24cxx_erase(_24cxx_dev_t *pdev,uint32_t addr,uint8_t data, uint32_t size);

#endif
