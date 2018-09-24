#ifndef	_MAIN_H_
#define _MAIN_H_

#define	MODULE_VERSION	"1.00"			

/*stm32f1 hardware*/
#include "stm32f10x.h"
#include "hardware.h"
#include "nvic.h"
#include "time.h"
#include "systick_delay.h"

/*driver*/
#include "i2c_hw.h"
#include "fm24clxx.h"
#include "at24cxx.h"

/*application*/
#include "board.h"

#endif 
