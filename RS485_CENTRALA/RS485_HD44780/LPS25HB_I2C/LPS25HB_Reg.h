#define REF_P_XL                0x08
#define REF_P_L                 0x09
#define REF_P_H                 0x0A
#define WHO_AM_I                0x0F
#define RES_CONF                0x10
#define CTRL_REG1               0x20
#define CTRL_REG2               0x21
#define CTRL_REG3               0x22
#define CTRL_REG4               0x23
#define INTERRUPT_CFG           0x24
#define INT_SOURCE              0x25
#define STATUS_REG              0x27
#define PRESS_OUT_XL            0x28
#define PRESS_OUT_L             0x29
#define PRESS_OUT_H             0x2A
#define TEMP_OUT_L              0x2B
#define TEMP_OUT_H              0x2C
#define FIFO_CTRL               0x2E
#define FIFO_STATUS             0x2F
#define THS_P_L                 0x30
#define THS_P_H                 0x31
#define RPDS_L                  0x39
#define RPDS_H                  0x3A

//	Command:		SAD[6:1]		SAD[0] = SA0	R/W		SAD+R/W
//	Read:			101110XR/W		0				1		10111001 (B9h) gdy pin wyboru adresu podci퉓anie do GND
//	Write:			101110XR/W		0				0		10111000 (B8h) gdy pin wyboru adresu podci퉓anie do GND
//	Read:			101110XR/W		1				1		10111011 (BBh) gdy pin wyboru adresu podci퉓anie do +3V3
//	Write:			101110XR/W		1				0		10111010 (BAh) gdy pin wyboru adresu podci퉓anie do +3V3

#define LPS25HB_READ_ADDR		0xBB //SA0 = VCC
#define LPS25HB_WRITE_ADDR		0xBA //SA0 = VCC

//#define LPS25HB_READ_ADDR			0xB9 //SA0 = GND
//#define LPS25HB_WRITE_ADDR		0xB8 //SA0 = GND

#define LPS25HB_DEVICE          0xBD