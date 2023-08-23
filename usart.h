
#ifndef USART_H_
#define USART_H_

#define BAUD 9600
#define UBRR_VALUE ((F_CPU/16/BAUD)-1)

volatile int rxFlag = 0;
volatile unsigned char rxData;

void initUSART(void)
{
	UBRR0H = (unsigned char)(UBRR_VALUE>>8);//Set UBRR upper byte
	UBRR0L = (unsigned char)(UBRR_VALUE);	//Set UBRR lower byte
	UCSR0B = (1<<RXCIE0)|(1<<RXEN0)|(1<<TXEN0);		//Enable transmitter/receiver
	UCSR0C = (3<<UCSZ00); //8-data bits
	
	sei();// Enable Global Interrupt
}

void transmitUSART(unsigned char data)
{
	while(!(UCSR0A & (1<<UDRE0))); // wait till the transmit buffer is ready
	
	UDR0 = data; // Put data to the transmit buffer
	
}

ISR (USART0_RX_vect)
{
	rxFlag = 1;
	rxData = UDR0;
}

#endif /* USART_H_ */