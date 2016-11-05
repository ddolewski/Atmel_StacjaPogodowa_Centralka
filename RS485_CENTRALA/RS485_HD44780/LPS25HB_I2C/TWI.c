//******************************************************************************
//
//    THE SOFTWARE INCLUDED IN THIS FILE IS FOR GUIDANCE ONLY.
//    BTC KORPORACJA SHALL NOT BE HELD LIABLE FOR ANY DIRECT, INDIRECT
//    OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
//    FROM USE OF THIS SOFTWARE.
//
//******************************************************************************
#include "TWI.h"

#define ADRW_ACK 18
#define DATA_ACK 28
//-------------------------------------
// Generate START signal
//-------------------------------------
unsigned char TWI_Start(void)
{
	unsigned int timeout = 0xFFFF;
	TWCR = (1<<TWINT)|(1<<TWSTA)| (1<<TWEN);
	while (!(TWCR & (1<<TWINT)))
	{
		timeout--;
		if (timeout == 0)
		{
			return 1;
		}
	}
	
	return 0;
}
//-------------------------------------
// Generate STOP signal
//-------------------------------------
unsigned char TWI_Stop(void)
{
	unsigned int timeout = 0xFFFF;
	TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWSTO);
	while ((TWCR & (1<<TWSTO)))
	{
		timeout--;
		if (timeout == 0)
		{
			return 1;
		}
	}
	
	return 0;
}
//-------------------------------------
// Write byte to slave
//-------------------------------------
unsigned char TWI_Write(char data)
{
	unsigned int timeout = 0xFFFF;
	TWDR = data;
	TWCR = (1<<TWINT) | (1<<TWEN);
	while (!(TWCR & (1<<TWINT)))
	{
		timeout--;
		if (timeout == 0)
		{
			return 2;
		}
	}
	
	if((TWSR == ADRW_ACK) | (TWSR == DATA_ACK))
		return 0;
	else
		return 1;
}
//-------------------------------------
// Read byte from slave
//-------------------------------------
char TWI_Read(char ack, unsigned char * err)
{
	unsigned int timeout = 0xFFFF;
	TWCR = ack
	? ((1 << TWINT) | (1 << TWEN) | (1 << TWEA))
	: ((1 << TWINT) | (1 << TWEN)) ;
	while (!(TWCR & (1<<TWINT)))
	{
		timeout--;
		if (timeout == 0)
		{
			*err = 1;
			break;
		}
	}
	
	return TWDR;
}
//-------------------------------------
// Initialize TWI
//-------------------------------------
void TWI_Init(char TWBRValue)
{
	TWBR = TWBRValue; //default value 100
}