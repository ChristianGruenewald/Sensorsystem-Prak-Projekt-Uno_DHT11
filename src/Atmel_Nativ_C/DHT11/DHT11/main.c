/*
 * main.c
 *
 * Created: 12/1/2021 11:12:08 AM
 *  Author: Christian Grünewald
 */ 
//Defines
//define microcontroller clock speed
#define F_CPU 16000000UL
//define for USART:
#define USART_BAUDRATE 9600 // Desired Baud Rate
#define BAUD_PRESCALER (((F_CPU / (USART_BAUDRATE * 16UL))) - 1)

#define ASYNCHRONOUS (0<<UMSEL00) // USART Mode Selection

#define DISABLED    (0<<UPM00)
#define EVEN_PARITY (2<<UPM00)
#define ODD_PARITY  (3<<UPM00)
#define PARITY_MODE  DISABLED // USART Parity Bit Selection

#define ONE_BIT (0<<USBS0)
#define TWO_BIT (1<<USBS0)
#define STOP_BIT ONE_BIT      // USART Stop Bit Selection

#define FIVE_BIT  (0<<UCSZ00)
#define SIX_BIT   (1<<UCSZ00)
#define SEVEN_BIT (2<<UCSZ00)
#define EIGHT_BIT (3<<UCSZ00)
#define DATA_BIT   EIGHT_BIT  // USART Data Bit Selection

//Define for DHT11
//	Test DHT11 (Temperatur und Luftfeuchtigkeitssensor) für AVR-Mikrocontroller
//	Getestet mit: ATtiny2313, ATmega8 (WinAVR-20100110)
//
//		 _________
//		|  -+-+-  |
//		| +-+-+-+ |
//		| +-+-+-+ |
//		|  -+-+-  |
//		| +-+-+-+ |
//		|_________|
//		  | | | |
//		  1 2 3 4
//
//	1. VCC (3 to 5V power)
//	2. Data out (Pullup 4,7k)
//	3. Not connected
//	4. Ground
#define DDR_DHT11			DDRB
#define PORT_DHT11			PORTB
#define PIN_DHT11			PINB
#define DHT11				PD4// Pullup 4,7k

#define DHT11_out_low		DDR_DHT11 |= (1 << DHT11)
#define DHT11_in			DDR_DHT11 &= ~(1 << DHT11)	// Hi Über Pullup
#define DHT11_is_hi			PIN_DHT11 & (1 << DHT11)
#define DHT11_is_low		!(PIN_DHT11 & (1 << DHT11))



//includes
#include <xc.h>
#include <avr/io.h>
#include <stdlib.h>
#include <util/delay.h>


void SetupUART()
{
	// Set Baud Rate
	UBRR0H = BAUD_PRESCALER >> 8;
	UBRR0L = BAUD_PRESCALER;
	
	// Set Frame Format
	UCSR0C = ASYNCHRONOUS | PARITY_MODE | STOP_BIT | DATA_BIT;
	
	// Enable Receiver and Transmitter
	UCSR0B = (1<<RXEN0) | (1<<TXEN0);
}

void UART_Transmit( unsigned char data )
{
	while (( UCSR0A & (1<<UDRE0)) == 0) {}; // Do nothing until UDR is ready
	UDR0 = data; //8Bit data
}

unsigned char UART_READ()
{
	while ( !(UCSR0A & (1<<RXC0)) ); /* Wait for data to be received */
	return UDR0;
}




int main(void)
{
	SetupUART();
	unsigned char s;
    while(1)
	{
		s=UART_READ();  
		switch(s)
		{
			case 'U':
			case 'u':
			UART_Transmit('R');
			break;
			default:
			UART_Transmit('F');
			break;
		}
    }
}