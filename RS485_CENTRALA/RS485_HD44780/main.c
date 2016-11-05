/*
 * RS485_HD44780.c
 *
 * Created: 2016-01-16 19:31:04
 * Author : Doles
 */ 

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdio.h>

#include "string.h"
#include "HD44780/HD44780.h"
#include "USART/mkuart.h"
#include "DS18B20_1Wire/DS18B20_1wire.h"
#include "LPS25HB_I2C/LPS25HB_Driver.h"
#include "global.h"
#include "systime.h"

#define START_BYTE_S	83	//'S'
#define START_BYTE_T	84	//'T'
#define START_BYTE_A	65	//'A'

#define END_BYTE_CR		13	//'\n'
#define END_BYTE_LF		10	//'\r'

#define CHAR_S			83
#define CHAR_H			72
#define CHAR_T			84
#define CHAR_E			69
#define CHAR_R			82

#define ALWAYS			1

typedef union
{
	unsigned char tempBytes[4];
	float tempFloat;
}real_temp;

typedef struct sht_measurement_t
{
	real_temp shtTemp;
	int shtHum;
}sht_measurement_t;

typedef struct lcdDisplayData_t
{
	uint8_t page;
	systime_t pageCounter;
}lcdDisplayData_t;

static lcdDisplayData_t dispDta;

static volatile systime_t sysTimer = 0;	//timer systemowy odmierzajacy czas w ms
static systime_t ds18b20InitTimer = 0;
static systime_t ds18b20MeasureTimer = 0;
static systime_t lps25hbTimer = 0;
static systime_t displayTimer = 0;
static systime_t pageTimer = 0;

static char lcdBuff1[20] = {0};
static char lcdBuff2[20] = {0};
static char lcdBuff3[20] = {0};
static char lcdBuff4[20] = {0};
	
static sht_measurement_t shtResults;
static lps25hb_out_data_t lps25hb;
static uint8_t isDS18B20 = 1;

static uint8_t lcdBuff1LenOld;
static uint8_t lcdBuff1LenAct;

static uint8_t lcdBuff2LenOld;
static uint8_t lcdBuff2LenAct;

static uint8_t lcdBuff3LenOld;
static uint8_t lcdBuff3LenAct;

static uint8_t lcdBuff4LenOld;
static uint8_t lcdBuff4LenAct;

void Display_MainCounter(lcdDisplayData_t * display);
void Display_ShowPage(lcdDisplayData_t * display);
void Display_ChangePage(lcdDisplayData_t * display);
static void Peripheral_Init(void);

int main(void)
{
	Peripheral_Init();
	
    while (ALWAYS) 
    {		
		Display_MainCounter(&dispDta);
		Display_ChangePage(&dispDta);
		Display_ShowPage(&dispDta);
				
		if (gReceivedDataFlag)
		{
			gRxBuffIndex = 0;
			gReceivedDataFlag = 0;
			uartTimeoutTimer = systimeGet();
			
			uart_puts("sprawdzam!!!!\n\r");
			if (UART_RxBuf[0] == START_BYTE_S && UART_RxBuf[1] == START_BYTE_T && UART_RxBuf[2] == START_BYTE_A)
			{
				uart_puts("start bytes!!!!\n\r");
				if (UART_RxBuf[8] == END_BYTE_CR && UART_RxBuf[9] == END_BYTE_LF)
				{
					uart_puts("end bytes!!!!\n\r");
					if (UART_RxBuf[3] == CHAR_S && UART_RxBuf[4] == CHAR_H && UART_RxBuf[5] == CHAR_T && UART_RxBuf[6] == CHAR_E && UART_RxBuf[7] == CHAR_R)
					{
						gReceivedDataError = SHT_ERROR;
						uart_puts("bledne dane!!!!\n\r");
					}
					else
					{
						shtResults.shtHum = (int)UART_RxBuf[3];
						shtResults.shtTemp.tempBytes[0] = UART_RxBuf[4];
						shtResults.shtTemp.tempBytes[1] = UART_RxBuf[5];
						shtResults.shtTemp.tempBytes[2] = UART_RxBuf[6];
						shtResults.shtTemp.tempBytes[3] = UART_RxBuf[7];
						gReceivedDataError = NONE_ERROR;
						
						uart_puts("odebralem dane\n\r");
						//PORTC |= (1 << 7);						
					}
					
					memset(UART_RxBuf, NULL, sizeof(UART_RxBuf));
				}
			}
		}
		else
		{
			if (systimeTimeoutControl(&uartTimeoutTimer, 5000))
			{
				gReceivedDataError = RS485_TIMEOUT_ERROR;
				uart_puts("rs485 timeout!!!!\n\r");
			}
		}

		if (systimeTimeoutControl(&ds18b20InitTimer, 300))
		{
			isDS18B20 = DS18B20_StartMeasurement();
			if (isDS18B20 == 0)
			{
				gDS18B20Error = DS18B20_ERROR;
			}
			else
			{
				gDS18B20Error = NONE_ERROR;
			}
		}
		
		if (systimeTimeoutControl(&ds18b20MeasureTimer, 800))
		{
			isDS18B20 = DS18B20_GetTemperature();
			if (isDS18B20 == 0)
			{
				gDS18B20Error = DS18B20_ERROR;
			}
			else
			{
				gDS18B20Error = NONE_ERROR;
			}
		}
		
		if (systimeTimeoutControl(&lps25hbTimer, 5000))
		{
			LPS25HB_Handler(&lps25hb);
		}
    }
}

void Display_ChangePage(lcdDisplayData_t * display)
{
	if (display->pageCounter == 5)
	{
		if (display->page == 1)
		{
			display->page = 2;
		}
		else if (display->page == 2)
		{
			display->page = 1;
		}
		else
		{
			asm("nop");
		}
		//uart_puts("display->pageCounter = ");
		//uart_putint(display->pageCounter, 10);
		//uart_puts("\n\r");
		//uart_puts("display->page = ");
		//uart_putint(display->page, 10);
		//uart_puts("\n\r");
		
		LCD_Clear();
		display->pageCounter = 0;
	}

#ifdef DISPLAY_PAGE1_TEST
	display->page = 1;

#elif DISPLAY_PAGE2_TEST
	display->page = 2;
#endif
}

void Display_ShowPage(lcdDisplayData_t * display)
{
	if (systimeTimeoutControl(&pageTimer, 100))
	{
		if (display->page == 1)
		{
			lcdBuff1LenOld = strlen(lcdBuff1);
			lcdBuff2LenOld = strlen(lcdBuff2);
			
			if (gReceivedDataError == NONE_ERROR)
			{
				sprintf(lcdBuff1, "T_zew: %.1f*C", shtResults.shtTemp.tempFloat);
			}
			else if (gReceivedDataError == RS485_RX_ERROR)
			{
				sprintf(lcdBuff1, "T_zew: WAIT...");
			}
			else if (gReceivedDataError == SHT_ERROR)
			{
				sprintf(lcdBuff1, "T_zew: SHT ERR");
			}
			else if (gReceivedDataError == RS485_TIMEOUT_ERROR)
			{
				sprintf(lcdBuff1, "T_zew: TIMEOUT");
			}
			else
			{
				asm("nop");
			}
			
			if (gDS18B20Error == DS18B20_ERROR)
			{
				sprintf(lcdBuff2, "T_wew: DS ERR");
			}
			else
			{
				sprintf(lcdBuff2, "T_wew: %.1f*C", real_tempDS18B20);
			}
			
			LCD_GoTo(0,0);
			LCD_WriteText(lcdBuff1);
			LCD_GoTo(0,1);
			LCD_WriteText(lcdBuff2);
			
			lcdBuff1LenAct = strlen(lcdBuff1);
			lcdBuff2LenAct= strlen(lcdBuff2);
			
			if (lcdBuff1LenOld != lcdBuff1LenAct || lcdBuff2LenOld != lcdBuff2LenAct)
			{
				LCD_Clear();
			}
		}
		else if (display->page == 2)
		{
			lcdBuff3LenOld = strlen(lcdBuff3);
			lcdBuff4LenOld = strlen(lcdBuff4);
			
			if (gReceivedDataError == NONE_ERROR)
			{
				sprintf(lcdBuff3, "Wilg.: %i%%RH", shtResults.shtHum); //RS485_RX_ERROR
			}
			else if (gReceivedDataError == RS485_RX_ERROR)
			{
				sprintf(lcdBuff3, "Wilg.: WAIT...");
			}
			else if (gReceivedDataError == SHT_ERROR)
			{
				sprintf(lcdBuff3, "Wilg.: SHT ERR");
			}
			else if (gReceivedDataError == RS485_TIMEOUT_ERROR)
			{
				sprintf(lcdBuff3, "Wilg.: TIMEOUT");
			}
			else
			{
				asm("nop");
			}
			
			if (gLPS25HBError != 0)
			{
				sprintf(lcdBuff4, "Cisn.: LPS ERR");
			}
			else
			{
				if (lps25hb.pressure > (float)1100)
				{
					sprintf(lcdBuff4, "Cisn.: LPS ERR");
				}
				else
				{
					sprintf(lcdBuff4, "Cisn.: %.1fhPa", lps25hb.pressure);
				}
			}
			
			LCD_GoTo(0,0);
			LCD_WriteText(lcdBuff3);
			LCD_GoTo(0,1);
			LCD_WriteText(lcdBuff4);
			
			lcdBuff3LenAct = strlen(lcdBuff3);
			lcdBuff4LenAct= strlen(lcdBuff4);
			
			if (lcdBuff3LenOld != lcdBuff3LenAct || lcdBuff4LenOld != lcdBuff4LenAct)
			{
				LCD_Clear();
			}
		}
		else
		{
			asm("nop");
		}
	}
}

void Display_MainCounter(lcdDisplayData_t * display)
{
	if (systimeTimeoutControl(&displayTimer, 1000))
	{
		display->pageCounter++;
#ifdef DEBUG_LCD
		uart_puts("display->pageCounter = ");
		uart_putint(display->pageCounter, 10);
		uart_puts("\n\r");
#endif
	}
}

static void Peripheral_Init(void)
{
	_delay_ms(30);
	LCD_Initialize();
	LCD_Clear();
	LCD_GoTo(0,0);
	LCD_WriteText("Uruchamianie...");
		
	systimeInit();
	sei();
	
	USART_Init(__UBRR);
	
	SCL_SDA_High;
	TWI_Init(25);
	uart_puts("I2C init\n\r");

	systimeDelayMs(200);
	gLPS25HBError = LPS25HB_Init();
	if (gLPS25HBError != 1)
	{
		uart_puts("lps config success\n\r");
	}
	else
	{
		uart_puts("lps config error\n\r");
	}

	systimeDelayMs(1000);
	LPS25HB_Handler(&lps25hb);
	uart_puts("lps measure success\n\r");
	
	isDS18B20 = DS18B20_StartMeasurement();
	uart_puts("ds18b20 start sequence\n\r");
	systimeDelayMs(1200);
	isDS18B20 = DS18B20_GetTemperature();
	systimeDelayMs("ds18b20 measure success\n\r");

	dispDta.page = 1;
	dispDta.pageCounter = 0;
	
	gRxBuffIndex = 0;
	gReceivedDataFlag = 0;
	gReceivedDataError = RS485_RX_ERROR;
	memset(UART_RxBuf, 0, sizeof(UART_RxBuf));
	
	LCD_Clear();	
	uart_puts("lcd clear\n\r");
	uart_puts("INITIALIZAION SUCCESS\n\r");
}