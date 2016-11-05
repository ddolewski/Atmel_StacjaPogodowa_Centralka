/*
 * mkuart.h
 *
 *  Created on: 2010-09-04
 *       Autor: Miros³aw Kardaœ
 */

#ifndef MKUART_H_
#define MKUART_H_
#include <util/delay.h>
#include "global.h"

#define F_CPU			8000000
#define UART_BAUD		9600			// tu definiujemy interesuj¹c¹ nas prêdkoœæ
#define __UBRR F_CPU/16/UART_BAUD-1		// obliczamy UBRR dla U2X=0

// definicje na potrzeby RS485
//#define UART_DE_PORT	PORTB
//#define UART_DE_DIR		DDRB
//#define UART_DE_BIT		(1<<1)
//
//#define UART_DE_ODBIERANIE		UART_DE_PORT &= ~UART_DE_BIT
//#define UART_DE_NADAWANIE		UART_DE_PORT |= UART_DE_BIT

#define UART_RX_BUF_SIZE 32 // definiujemy bufor o rozmiarze 32 bajtów
// definiujemy maskê dla naszego bufora
#define UART_RX_BUF_MASK ( UART_RX_BUF_SIZE - 1)

#define UART_TX_BUF_SIZE 20 // definiujemy bufor o rozmiarze 16 bajtów
// definiujemy maskê dla naszego bufora
#define UART_TX_BUF_MASK ( UART_TX_BUF_SIZE - 1)

volatile systime_t uartTimeoutTimer;
volatile char UART_RxBuf[UART_RX_BUF_SIZE];

volatile uint8_t gRxBuffIndex;
volatile uint8_t gReceivedDataFlag;
volatile uint8_t gReceivedDataError;

// deklaracje funkcji publicznych

void USART_Init( uint16_t baud );

char uart_getc(void);
void uart_putc( char data );
void uart_puts(char *s);
void uart_putint(int value, int radix);
void USART_SendString(char s[]);
void USART_SendByte(uint8_t u8Data);
#endif /* MKUART_H_ */
