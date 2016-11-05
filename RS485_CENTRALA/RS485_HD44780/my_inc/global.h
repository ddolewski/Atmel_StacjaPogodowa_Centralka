/*
 * global.h
 *
 * Created: 2016-02-25 20:37:02
 *  Author: Doles
 */ 


#ifndef GLOBAL_H_
#define GLOBAL_H_

//#define DEBUG_GLOBAL
//#define DEBUG_LCD

typedef enum
{
	NONE_ERROR,
	SHT_ERROR,
	LPS_ERROR,
	DS18B20_ERROR,
	RS485_TIMEOUT_ERROR,
	RS485_RX_ERROR
}device_errors_t;

typedef enum
{
	FALSE = 0,
	TRUE = (!FALSE)
} bool_t;

typedef uint32_t systime_t;



#endif /* GLOBAL_H_ */