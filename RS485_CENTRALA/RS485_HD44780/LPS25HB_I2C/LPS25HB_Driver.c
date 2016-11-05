#include "LPS25HB_Driver.h"

//funkcja zapisu jednego bajtu po I2C
unsigned char LPS25HB_WriteRegister(char wrAddr, char wrData) 
{
	unsigned char err = 0;
	err = TWI_Start();
	err = TWI_Write(LPS25HB_WRITE_ADDR);
	err = TWI_Write(wrAddr);
	err = TWI_Write(wrData);
	err = TWI_Stop();
	
	return err;
}

//funkcja odczytu jednego bajtu po I2C
char LPS25HB_ReadRegister(char rAddr, unsigned char * err) 
{
	unsigned char data = 0;

	*err = TWI_Start();
	*err = TWI_Write(LPS25HB_WRITE_ADDR);
	*err = TWI_Write(rAddr);
	*err = TWI_Start();
	*err = TWI_Write(LPS25HB_READ_ADDR);
	data = TWI_Read(NACK, err);
	*err = TWI_Stop();
	
	return data;
}

// funkcja odczytujaca ID urzadzenia (0xBD)
char LPS25HB_GetDevID(unsigned char * err) 
{
	return LPS25HB_ReadRegister(WHO_AM_I, err);
}

// This function converts pressure in mbar to altitude in meters, using 1976 US
// Standard Atmosphere model (note that this formula only applies to a
// height of 11 km, or about 36000 ft)
// If altimeter setting (QNH, barometric pressure adjusted to sea
// level) is given, this function returns an indicated altitude
// compensated for actual regional pressure; otherwise, it returns
// the pressure altitude above the standard pressure level of 1013.25
// mbar or 29.9213 inHg
float ConvertPressureToAltitudeMeters(float pressure_mbar, float altimeter_setting_mbar) 
{
	return (1 - pow(pressure_mbar / altimeter_setting_mbar, (float)0.190263)) * (float)44330.8;
}

// converts pressure in inHg to altitude in feet; see notes above
float ConvertPressureToAltitudeFeet(float pressure_inHg, float altimeter_setting_inHg) 
{
	return (1 - pow(pressure_inHg / altimeter_setting_inHg, (float)0.190263)) * (float)145442;
}


// funkcja odczytu i konwersji 16 bitowej wartosci temperatury z 2 rejestrow MSB i LSB
// return: temperatura w Celsjuszach
float LPS25HB_GetTemperatureC(unsigned char * err) 
{
	char value;
	int result;
	value = LPS25HB_ReadRegister(TEMP_OUT_H, err);
	result = (int) value;
	value = LPS25HB_ReadRegister(TEMP_OUT_L, err);
	result = (result << 8) | value;
	
	return ((float)result / (float)480) + (float)42.5;
}

// funkcja odczytu i konwersji 16 bitowej wartosci temperatury z 2 rejestrow MSB i LSB
// return: temperatura w Fahrenheitach
float LPS25HB_GetTemperatureF(unsigned char * err) 
{
	char value;
	int result;
	value = LPS25HB_ReadRegister(TEMP_OUT_H, err);
	result = (int) value;
	value = LPS25HB_ReadRegister(TEMP_OUT_L, err);
	result = (result << 8) | value;
	
	return ((float)result / (float)480 * (float)1.8) + (float)108.5;
}

// funkcja odczytu i konwersji 24 bitowej wartosci cisnienia z 3 rejestrow MSB, LSB i MIDDLE
// return: cisnienie w mbar
float LPS25HB_GetPressureMillibars(unsigned char * err) 
{
	char value;
	unsigned long result;
	value = LPS25HB_ReadRegister(PRESS_OUT_H, err);
	result = (unsigned long) value;
	value = LPS25HB_ReadRegister(PRESS_OUT_L, err);
	result = (result << 8) | value;
	value = LPS25HB_ReadRegister(PRESS_OUT_XL, err);
	result = (result << 8) | value;
	return (float)result/(float)4096;
}

//wlaczenie zasilania do czujnika (bit PD w CTRL_REG1)
void LPS25HB_SetActive(unsigned char * err) 
{
	char reg_val;
	reg_val = LPS25HB_ReadRegister(CTRL_REG1, err);
	LPS25HB_WriteRegister(CTRL_REG1, reg_val | 0x80);
}

//funkcja inicjalizujaca/konfigurujaca prace czujnika
unsigned char LPS25HB_Init(void) 
{
	unsigned char error = 0;
	// PD = 1, ODR[2:0] = '100' (25 Hz cisnienie & temperatura)
	error = LPS25HB_WriteRegister(CTRL_REG1,0xC0); 
	
	// 0x00 - resetuje rejestr i kasuje bit FIFO_MEAN_DEC (wylaczenie autoinkrementacji adresow danych rejestrow, tzn.
	// przy odczycie rejestrow czujnika ten sam inkrementuje adres rejestru do odczytu o 1)
	error = LPS25HB_WriteRegister(CTRL_REG2,0x00); 
	
	//resetuje kontrolnie rejestr do domyslnych
	error = LPS25HB_WriteRegister(CTRL_REG3,0x00);
	
	// ustawiam bit DRDY (Data-ready signal on INT_DRDY pin) - mozna zadac odczytu czujnika gdy pin DRDY ma stan wysoki
	// wtedy wiemy ze dane pomiarowe sa gotowe do odczytu po I2C
	error = LPS25HB_WriteRegister(CTRL_REG4,0x01);
	
	return error;
}

//funkcja odczytu rejestru statusu
char LPS25HB_GetStatus(unsigned char * err) 
{
	return LPS25HB_ReadRegister(STATUS_REG, err);
}

void LPS25HB_Handler(lps25hb_out_data_t * lpsDta)
{
	char statusReg = 0;
	//iddev = Who_Am_I(); //testowo
	statusReg = LPS25HB_ReadRegister(STATUS_REG, &gLPS25HBError); // Read status register
	//uart_putint(statusReg, 10);
				
	if (statusReg & 0x01) // New data for temperature is available
	{
		lpsDta->temperature = LPS25HB_GetTemperatureC(&gLPS25HBError);   // Read temperature
		//sprintf(lcdBuff3, "%.1f*C", lps25hb.temperature);
		//uart_puts("lps pomiar temperatury wykonany\n\r");
	}
				
	if (statusReg & 0x02) // New data for pressure is available
	{
		lpsDta->pressure = LPS25HB_GetPressureMillibars(&gLPS25HBError); // Read pressure
		//sprintf(lcdBuff4, "%.1fhPa", lps25hb.pressure);
		//uart_puts("lps pomiar cisnienia wykonany\n\r");
	}
}