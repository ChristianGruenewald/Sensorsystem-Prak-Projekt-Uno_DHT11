
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

#define DDR_DHT11			DDRD
#define PORT_DHT11			PORTD
#define PIN_DHT11			PIND
#define DHT11				PD4// Pullup 4,7k 

#define DHT11_out_low		DDR_DHT11 |= (1 << DHT11)
#define DHT11_in			DDR_DHT11 &= ~(1 << DHT11)	// Hi Über Pullup
#define DHT11_is_hi			PIN_DHT11 & (1 << DHT11)
#define DHT11_is_low		!(PIN_DHT11 & (1 << DHT11))

uint8_t dht11(unsigned char *feuchte, unsigned char *temperatur) 
{
	if (DHT11_is_low) {return 1;}									// Bus not free
	DHT11_out_low;													// MCU start signal (>=18MS) 
	_delay_ms(20);
	DHT11_in;
	//asm volatile ("nop");
	uint8_t timeout = 255;
	while(DHT11_is_hi) {if (!timeout--) {return 2;}}				// Wait for DHT’s response (20-40us)
	timeout = 255;
	while(DHT11_is_low) {if (!timeout--) {return 3;}}				// Response signal (80us)
	timeout = 255;	
	while(DHT11_is_hi) {if (!timeout--) {return 4;}}				// Preparation for sending data (80us)
	uint8_t dht11_data[5]={0};
	for(uint8_t i=0;i<5;i++)
	{
		uint8_t dht11byte = 0;
		for(uint8_t j=1;j<=8;j++)
		{
			timeout = 255;
			while(DHT11_is_low) {if (!timeout--) {return 5;}}		// Start to transmit 1-Bit (50 us)
			_delay_us(30);
			dht11byte <<= 1;
			if (DHT11_is_hi)										// Hi > 30us (70 us) -> Bit=1										
			{
				dht11byte |= 1;
				timeout = 255;
				while(DHT11_is_hi) {if (!timeout--) {return 6;}}
			}														// Hi <  30us (26-28 us) -> Bit=0	
		}
		dht11_data[i] = dht11byte;
	}
	if (dht11_data[0]+dht11_data[1]+dht11_data[2]+dht11_data[3] != dht11_data[4]) 	// Checksum							
	{
		//fuart_putc(dht11_data[0]);fuart_putc(dht11_data[1]);fuart_putc(dht11_data[2]);fuart_putc(dht11_data[3]);fuart_putc(dht11_data[4]);fuart_putc(13);
		return 7;													
	}	
	*feuchte = dht11_data[0];
	*temperatur = dht11_data[2];
	return 0;
}

/*
void dht11_logik_analyse (void) // ab ATMega8 (1K Byte RAM)
  {
	uint8_t data[900]={0};
  	if (DHT11_is_low) 	{return;}							
	DHT11_out_low						// MCU start signal (>=18MS) 
	_delay_ms(20);
	DHT11_in;
	asm volatile ("nop");
  	for(uint16_t i=0;i<900;i++)
	{
		for(uint8_t j=1;j<=8;j++)
		{	
			data[i] <<= 1;
			if (DHT11_is_hi)
			{
			data[i] |= 1;
			asm volatile ("nop");		// ck. 1us @ 8MHz
			}
		}
	}
  	for(uint16_t i=0;i<900;i++)
	{
		fuart_putc(data[i]);	// Nicht in Interrupt Modus (Der Buffer wird zu klein sein)
	}		
}
*/
