#include <math.h>
#include "TWI.h"
#include "LPS25HB_Reg.h"

typedef struct
{
	float pressure;
	float temperature;
}lps25hb_out_data_t;

unsigned char gLPS25HBError;

char LPS25HB_GetDevID(unsigned char * err);
char LPS25HB_ReadRegister(char rAddr, unsigned char * err);
unsigned char LPS25HB_WriteRegister(char wrAddr, char wrData);
float LPS25HB_GetTemperatureF(unsigned char * err);
float LPS25HB_GetTemperatureC(unsigned char * err);
float LPS25HB_GetPressureMillibars(unsigned char * err);
unsigned char LPS25HB_Init(void);
char LPS25HB_GetStatusReg(unsigned char * err);
void LPS25HB_SetActive(unsigned char * err);
char LPS25HB_GetStatus(unsigned char * err);
void LPS25HB_Handler(lps25hb_out_data_t * lpsDta);