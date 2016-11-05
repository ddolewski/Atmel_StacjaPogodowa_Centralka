#include <avr/io.h>

#define ACK 1
#define NACK 0

#define SCL_I2C			0
#define SDA_I2C			1
#define SCL_SDA_High	PORTC |= (1 << SCL_I2C) | (1 << SDA_I2C)

void TWI_Init(char TWBRValue);
unsigned char TWI_Start(void);
unsigned char TWI_Stop(void);
unsigned char TWI_Write(char data);
char TWI_Read(char ack, unsigned char * err);