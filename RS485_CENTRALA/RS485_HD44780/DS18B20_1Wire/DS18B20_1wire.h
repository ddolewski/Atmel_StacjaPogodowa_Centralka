#define WE 2
#define PORT_1Wire PIND
#define SET_1 DDRD &= ~ (1 << WE)
#define SET_0 DDRD |= (1 << WE)
/************************************************************************/
/*						VARIABLES FOR DS18B20                           */
/************************************************************************/
unsigned char tempH, tempL, temperature_DS18B20;
float real_tempDS18B20;
unsigned char gDS18B20Error;

unsigned char _1Wire_Init(void);
unsigned char _1Wire_ReadByte(void);
unsigned char _1Wire_ReadBit(void);
void _1Wire_WriteBit(char bit);
void _1Wire_SendByte(char byte);
/************************************************************************/
/*						1-WIRE DEVICES SUPPORT                          */
/************************************************************************/
unsigned char DS18B20_StartMeasurement(void);
unsigned char DS18B20_GetTemperature(void);