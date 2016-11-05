/*
 * BIBLIOTEKA MAGISTRALI 1WIRE
 *
 * Created: 2012-07-14 10:27:43
 *  Author: doles
 */ 
#include <avr/io.h>
#include <util/delay.h>
#include "DS18B20_1wire.h"
/*************INICJALIZACJA MAGISTRALI - IMPULS RESET I ODPOWIED SLAVE***********************/
unsigned char _1Wire_Init(void)
{
	unsigned char init;
	SET_0;											//Master zwiera do masy magistralê
	_delay_us(500);									//stan 0 trwa 500us (minimum 480us)
	SET_1;											//Master zwalnia magistralê, czyli zwiera do Vcc
	_delay_us(30);									//stan 1 przed impulsem obecnoœci urz¹dzenia slave (musi trwaæ 15-60us)
	
	if(bit_is_clear(PORT_1Wire, WE)) init = 1;		//sprawdza urz¹dzenie pod³¹czone do magistrali
	else init = 0;
				
	_delay_us(470);	
				
	if(bit_is_set(PORT_1Wire, WE)) init = 1;		//ponownie sprawdza pod³¹czenie do magistrali
	else init = 0;

	return init;									//zwraca zmienn¹ która jest efektem pod³¹czenia do magistrali
													//1 - pod³¹czono urz¹dzenie, 0 - nic nie pod³¹czono
}
/*************WYSY£ANIE 1 BITU************************/
void _1Wire_WriteBit(char bit)
{
	SET_0;				//Master zwiera magistralê do masy
	_delay_us(20);		//po up³ywie minimum 15us nastêpuje zapis 0 lub 1 (poni¿ej)
	if(bit == 1)
	{
		SET_1;			//Master ustawia 1 po up³ywie minimum 15us, czeka i nadal ustawia 1 -> "1"
		_delay_us(80);
		SET_1;
	}
	if(bit == 0)
	{
		SET_0;			//Master po up³ywie minimum 15us ustawia magistralê na 0, czeka i zwalnia j¹
		_delay_us(80);
		SET_1;
	}
}
/*******************************ODBIERANIE 1 BITU*********************************************/
unsigned char _1Wire_ReadBit(void)
{
	unsigned char init = 0;
	SET_0;						//Master zwiera magistralê do masy na minimum 1us
	_delay_us(5);
	SET_1;						//Master zwalnia magistralê i Slave mo¿e przesy³aæ dane
	_delay_us(15);
	
	if(bit_is_set(PORT_1Wire, WE)) init = 1;	//sprawdza urz¹dzenie pod³¹czone do magistrali
	else init = 0;
	
	return init;
}
/********************************WYSY£ANIE DANYCH***********************************************/
void _1Wire_SendByte(char byte)
{
	unsigned char i, temp;
	for (i = 0; i < 8; i++)
	{
		temp = byte >> i;
		temp &= 0x01;
		_1Wire_WriteBit(temp);
	}
	_delay_us(100);
}
/********************************ODBIERANIE DANYCH***********************************************/
unsigned char _1Wire_ReadByte(void)
{
	unsigned char i;
	unsigned char read_value = 0;
	
	for (i = 0; i < 8; i++)
	{
		if (_1Wire_ReadBit()) read_value |= 0x01 << i;
		_delay_us(15);
	}
	return read_value;
}

unsigned char DS18B20_StartMeasurement(void)
{
	unsigned char init = 1;
	init = _1Wire_Init();
	_1Wire_SendByte(0xCC); //pomiñ sprawdzanie ROM
	_1Wire_SendByte(0x44); //rozkaz pomiaru i konwersji temperatury	
	return init;
}

unsigned char DS18B20_GetTemperature(void)
{
	unsigned char init = 1;
	init = _1Wire_Init();
	_1Wire_SendByte(0xCC); //pomiñ sprawdzanie ROM
	_1Wire_SendByte(0xBE); //sprawdzanie poprawnoœci scratchpadu
	tempL = _1Wire_ReadByte();
	tempH = _1Wire_ReadByte();
	real_tempDS18B20 = tempL + (tempH*256);
	real_tempDS18B20 = real_tempDS18B20 / 16;
	
	return init;
}