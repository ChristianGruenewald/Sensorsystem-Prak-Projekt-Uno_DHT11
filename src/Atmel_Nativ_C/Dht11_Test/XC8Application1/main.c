/*
 * main.c
 *
 * Created: 11/19/2021 5:14:53 PM
 *  Author: Christian Grünewald
 */
//define microcontroller clock speed for UART and delay:
#define F_CPU 16000000UL 

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


//Port where DHT sensor is connected
//DHT 11 is connected to PB5, Port B, Pin 0
#define DHT_DDR DDRB 
#define DHT_PORT PORTB
#define DHT_PIN PINB
#define DHT_INPUTPIN 5
//timeout retries
#define DHT_TIMEOUT 200

#define DHT_Is_LOW
#define  DHT_Is_HIGH


#include <util/delay.h>
#include <xc.h>
#include <avr/io.h>
#include <util/delay.h>
#include <string.h>

//Global Vars:
int8_t humidity_high=0;
int8_t humidity_low=0;
int8_t temperature_high=0;
int8_t temperature_low=0;


unsigned char humi_high_first=1;
unsigned char humi_high_last=1;
unsigned char humi_low_first=1;
unsigned char humi_low_last=1;


char temp_high_first=1;
char temp_high_last=1;
char temp_low_first=1;
char temp_low_last=1;

int8_t Bitmask=0b00110000; //or 48, look in ASCII binary table

unsigned char error_String[]={"reading error!"};

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

void UART_Transmit( int8_t data )
{
	while (( UCSR0A & (1<<UDRE0)) == 0) {}; // Do nothing until UDR is ready
	UDR0 = data;
}

unsigned char UART_Read()
{
	while ( !(UCSR0A & (1<<RXC0)) ); /* Wait for data to be received */
	return UDR0;
}

int8_t Read_DHT11()
{
	uint8_t bits[5];
	uint8_t i,j = 0;

	memset(bits, 0, sizeof(bits));

	//prepare correct port and pin of DHT sensor
	DHT_DDR |= (1 << DHT_INPUTPIN); //output
	DHT_PORT |= (1 << DHT_INPUTPIN); //high
	_delay_ms(100);

	//begin send request
	DHT_PORT &= ~(1 << DHT_INPUTPIN); //low
	_delay_ms(18);
	DHT_PORT |= (1 << DHT_INPUTPIN); //high
	DHT_DDR &= ~(1 << DHT_INPUTPIN); //input
	_delay_us(40);

	//check first start condition
	if((DHT_PIN & (1<<DHT_INPUTPIN))) {
		return -1;
	}
	_delay_us(80);
	
	//check second start condition
	if(!(DHT_PIN & (1<<DHT_INPUTPIN))) {
		return -1;
	}
	_delay_us(80);

	//read-in data
	uint16_t timeoutcounter = 0;
	for (j=0; j<5; j++) { //for each byte (5 total)
		uint8_t result = 0;
		for(i=0; i<8; i++) {//for each bit in each byte (8 total)
			timeoutcounter = 0;
			while(!(DHT_PIN & (1<<DHT_INPUTPIN))) { //wait for an high input (non blocking)
				timeoutcounter++;
				if(timeoutcounter > DHT_TIMEOUT) {
					return -1;
				}
			}
			_delay_us(30);
			if(DHT_PIN & (1<<DHT_INPUTPIN))
			result |= (1<<(7-i));
			timeoutcounter = 0;
			while(DHT_PIN & (1<<DHT_INPUTPIN)) {
				timeoutcounter++;
				if(timeoutcounter > DHT_TIMEOUT) {
					return -1;
				}
			}
		}
		bits[j] = result;
	}

	//reset port
	DHT_DDR |= (1<<DHT_INPUTPIN); //output
	DHT_PORT |= (1<<DHT_INPUTPIN); //low
	_delay_ms(100);

	//compare checksum
	if ((uint8_t)(bits[0] + bits[1] + bits[2] + bits[3]) == bits[4]) {
		//return temperature and humidity
		temperature_high = bits[2];
		temperature_low=bits[3];
		humidity_high = bits[0];
		humidity_low=bits[1];
		return 0;
	}

	return -1;
	
}

void send_string(char s[])
{
	int i =0;
	
	while (s[i] != 0x00)
	{
		UART_Transmit(s[i]);
		i++;
	}
}


void ConvertToChar()
{
	//8 bit decomposition e.g. humidity=37 (8 Bit, dec) 
	humi_high_last=humidity_high%10; //37%10= 7 and save as char
	humidity_high=humidity_high/10; //37/10=3 
	humi_high_first=humidity_high%10; //3%10=3
	//now for decimal points:
	humi_low_last=humidity_low%10; //37%10= 7 and save as char
	humidity_low=humidity_low/10; //37/10=3
	humi_low_first=humidity_low%10; //3%10=3
	
	//same for temperature:
	temp_high_last=temperature_high%10;
	temperature_high=temperature_high/10;
	temp_high_first=temperature_high%10;
	//now for decimal points:
	temp_low_last=temperature_low%10;
	temperature_low=temperature_low/10;
	temp_low_first=temperature_low%10;
	
	//convert from dec to ASCII
	//for humi:
	//´high
	humi_high_last|=Bitmask;
	humi_high_first|=Bitmask;
	//low:
	humi_low_last|=Bitmask;
	humi_low_first|=Bitmask;
	//for temp:
	//high
	temp_high_first|=Bitmask;
	temp_high_last|=Bitmask;
	//low
	temp_low_last|=Bitmask;
	temp_low_first|=Bitmask;
}


int main(void)
{ 
	SetupUART();
    while(1) 
    {
		unsigned char cmd=UART_Read();
		switch(cmd)
		{
			case 'U':
			case  'u':
			if(Read_DHT11()!=-1)
			{
				ConvertToChar();
				unsigned char humi[3]={humi_high_first, humi_high_last,'.',humi_low_first,humi_low_last};
				unsigned char temp[4]={temp_high_first,temp_high_last,'.',temp_low_first,temp_low_last};
				send_string(humi);
				UART_Transmit(';');
				send_string(temp);
				UART_Transmit('\n');
				UART_Transmit('\r');
			}
			else
			{
				send_string(error_String);
				UART_Transmit('\n');
				UART_Transmit('\r');		
			}
			break;
			default:
			break;
		}
    }
}