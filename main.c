#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <stdbool.h>

#include "lcd.h"
#include "usart.h"
#include "utilityDisplay.h"

#define BUZZER_PIN 2 // Buzzer connected to pin 2 (PD2)
#define ESP8266 3 // esp8266 to pin 3 (PD3)
#define Module1 4// module 6301 PB4
#define Module2 5// module 6302 PB5
#define Module3 6// module 6303 PB6
#define Module4 7// module 6304 PB7

extern int rand();

void initPorts(void);
unsigned char keypad();
void displayInput(unsigned char input[4]);
void lcd_print(char *str);

///////////////I2C
void i2c_init(void);
void i2c_start(void);
void i2c_write(unsigned char data);
unsigned char i2c_read(unsigned char ackVal);
void i2c_stop();
void rtc_init(void);
void rtc_setTime(unsigned char h, unsigned char m, unsigned char s);
void rtc_gettime(unsigned char *h, unsigned char *m, unsigned char *s);
void send_packedBCD(unsigned char data);
void sendValue(uint32_t);


void setPowerLED(void){PORTB |= 0B10000000;};
void setStatusLED(void){PORTB|=0B01000000;};
void clearStatusLED(void){PORTB &= 0B10111111;};
void setStartCommunication(void){PORTD |= 0B10000000;};
int getButtonStatus(void){return PIND & 0B00000010;};

const unsigned char keymap[16] PROGMEM = {'1', '2', '3', 'A', '4', '5', '6', 'B', '7', '8', '9', 'C', '*', '0', '#', 'D'};
const unsigned char correctCode[4] = {'1', '2', '3', '4'};
const unsigned char correctCodeMod1[4] = {'6', '3', '0', '1'};
const unsigned char correctCodeMod2[4] = {'6', '3', '0', '2'};
const unsigned char correctCodeMod3[4] = {'6', '3', '0', '3'};
const unsigned char correctCodeMod4[4] = {'6', '3', '0', '4'};	

// Define an array of correct codes for each module
const unsigned char *correctCodeMods[4] = {
	correctCodeMod1,
	correctCodeMod2,
	correctCodeMod3,
	correctCodeMod4
};

// Define an array of module pin numbers
const uint8_t modulePins[4] = {
	Module1,
	Module2,
	Module3,
	Module4
};

bool restart =true;


int main(void)
{
    uint32_t randINT;
    randINT=(rand()%1000000);
    sendValue(randINT);

    //PORTE &= ~(1 << trs); // esp8266
    PORTB &=~(1<<Module1);
    PORTB &=~(1<<Module2);
    PORTB &=~(1<<Module3);
    PORTB &=~(1<<Module4);
    DDRC = 0x0F; // Make PC0 to PC3=O/P for rows and PC4 to PC7=I/P for columns
    _delay_ms(1);
    PORTC = 0xF0; // all rows = 0 columns = 1
    initPorts();
    Lcd_init();
    initUSART();
    setPowerLED();
    displayWelcome();
    _delay_ms(4000);
    while(!getButtonStatus()){}
    setStartCommunication();
    displayStart();
	

	
	while(restart){
		restart = false;
		
    bool isCorrect = false;
    do {
		PORTB &=~(1<<Module1);
		PORTB &=~(1<<Module2);
		PORTB &=~(1<<Module3);
		PORTB &=~(1<<Module4);
		PORTD &= ~(1 << ESP8266);
		PORTD &= ~(1 << BUZZER_PIN);
        unsigned char input[4] ="XXXX";
        int inputIndex = 0;
        bool inputComplete = false;
        while (!inputComplete)
        {
            PORTC = 0xF0;
            _delay_ms(25);
            if (PINC!=0xF0)
            {
                unsigned char key = keypad();
                if (key >= '0' && key <= '9')
                {
                    input[inputIndex++] = key;
                    displayInput(input);
                    if (inputIndex == 4)
                    {
                        inputComplete = true;
                    }
                }
            }
        }

        for (int moduleIndex = 0; moduleIndex < 4; moduleIndex++)
        {
            PORTB &= ~(1 << modulePins[moduleIndex]);
			PORTB &=~(1<<Module1);
			PORTB &=~(1<<Module2);
			PORTB &=~(1<<Module3);
			PORTB &=~(1<<Module4);

            isCorrect = true;
            for (int i = 0; i < 4; i++) {
                if (input[i] != correctCodeMods[moduleIndex][i]) {
                    isCorrect = false;
                    break;
                }
            }

            if (isCorrect)
            {
                Lcd_CmdWrite(0xC0);
                Lcd_DataWrite('C');
                Lcd_DataWrite('O');
                Lcd_DataWrite('R');
                Lcd_DataWrite('R');
                Lcd_DataWrite('E');
                Lcd_DataWrite('C');
                Lcd_DataWrite('T');

                PORTB |= (1 << modulePins[moduleIndex]);

                PORTD |= (1 << BUZZER_PIN); // Activate buzzer
                _delay_ms(500); // Wait for 20ms
                PORTD &= ~(1 << BUZZER_PIN); // Deactivate buzzer
                PORTD |= (1 << ESP8266); // esp8266
                //PORTE |= (1 << trs); // esp8266

                break;
            }
        }
		 
			
		 
		
		
        if (!isCorrect)
        {
			 Lcd_CmdWrite(0xC0);
			 Lcd_DataWrite('I');
			 Lcd_DataWrite('N');
			 Lcd_DataWrite('C');
			 Lcd_DataWrite('O');
			 Lcd_DataWrite('R');
			 Lcd_DataWrite('R');
			 Lcd_DataWrite('E');
			 Lcd_DataWrite('C');
			 Lcd_DataWrite('T');
			 Lcd_DataWrite(' ');

			 for (int i=0;i<5;i++)
			 {
				 PORTD &= ~(1 << ESP8266); // esp8266
				 //PORTE &= ~(1 << trs); // esp8266

				 if (i==4)
				 {
					 PORTD |= (1 << BUZZER_PIN); // Activate buzzer
					 _delay_ms(1000); // Wait for 20ms
					 PORTD &= ~(1 << BUZZER_PIN); // Deactivate buzzer
					 }else{
					 PORTD |= (1 << BUZZER_PIN); // Activate buzzer
					 _delay_ms(150); // Wait for 20ms
					 PORTD &= ~(1 << BUZZER_PIN); // Deactivate buzzer
					 _delay_ms(100);
				 }
			 }
			
            _delay_ms(2000);
            Lcd_CmdWrite(0x01);
            displayStart();
            inputIndex = 0;
            inputComplete = false;
        }

    } while (!isCorrect);

	
    _delay_ms(2000);
    Lcd_CmdWrite(0x01);
    lcd_print("Scan ID ...");
	Lcd_CmdWrite(0xC0);
	lcd_print("Press # to exit.");
	
	//Lcd_CmdWrite(0x01);
   // Lcd_CmdWrite(0XC0);
	
	// unsigned char input;
	 bool inputComplete = false;
	 while (!inputComplete)
	 {
		 PORTC = 0xF0;
		 _delay_ms(25);
		 if (PINC!=0xF0)
		 {
			 unsigned char key = keypad();
			 if (!(key >= '0' && key <= '9'))
			 {
				 if (key=='#')
				 {
					// Lcd_CmdWrite(0x01);
					Lcd_CmdWrite(0x01);
					 restart =true;
					 
					 isCorrect=false;
					 inputComplete =true;
					 displayStart();
				 }
				 //Lcd_DataWrite(key);
				 
			 }
		 }
	 }
	
	
	
	}
	
    while (1) {
        //Lcd_CmdWrite(0xC5);
       /* unsigned char h,m,s;
        rtc_gettime(&h,&m,&s);
        send_packedBCD(h);
        Lcd_DataWrite(':');
        send_packedBCD(m);
        Lcd_DataWrite(':');
        send_packedBCD(s);
        _delay_ms(1000);*/
    }
	return 0;
}




void initPorts(void)
{
	Lcd_PortConfig();
	DDRB |= 0B11000000;
	DDRD &= 0B11111100;
	DDRD |= 0B10000000;
	DDRD |= (1 << BUZZER_PIN); // Set buzzer pin as output
	DDRD |= (1 << ESP8266); // Set ESP8266 pin as output
	//DDRE |= (1 << trs); // Set ESP8266 pin as output
	
}

//Keypad function to get value from keypad
unsigned char keypad()
{
	unsigned char key = 0;
	for (int row = 0; row < 4; row++)
	{
		PORTC = ~(1 << row); // Set one row to low state
		_delay_ms(5);
		for (int col = 0; col < 4; col++)
		{
			if ((PINC & (1 << (col + 4))) == 0) // Check if button is pressed
			{
				key = pgm_read_byte_near(keymap + row * 4 + col); // Get character from keymap
				while ((PINC & (1 << (col + 4))) == 0); // Wait for button release
			}
			if (key != 0) break;
			
		}
		if (key != 0) break;
	}
	return key;
}

void displayInput(unsigned char input[4])
{
	//Lcd_CmdWrite(0x01);
	Lcd_CmdWrite(0xC0);
	for (int i = 0; i < 4; i++)
	{
		if (input[i] != '\0')
		{
			Lcd_DataWrite(input[i]);
		}
		else
		{
			Lcd_DataWrite('_');
		}
	}
}

void lcd_print(char *str)
{
	unsigned char i=0;
	while(str[i]!=0)
	{
		Lcd_DataWrite(str[i]);
		i++;
	}
}

void sendValue(uint32_t value)
{
	// Convert the 32-bit value into four 8-bit bytes
	uint8_t byte1 = (value >> 24) & 0xFF;
	uint8_t byte2 = (value >> 16) & 0xFF;
	uint8_t byte3 = (value >> 8) & 0xFF;
	uint8_t byte4 = value & 0xFF;

	// Send the bytes over the USART (Serial) connection
	transmitUSART(byte1);
	transmitUSART(byte2);
	transmitUSART(byte3);
	transmitUSART(byte4);
}

////////////////////////////////////////I2C
void i2c_init(void)
{
	TWSR =0X00;
	TWBR =0X48;
	TWCR =0X04;
}


void i2c_start(void)
{
	TWCR=(1<<TWINT)|(1<<TWSTA)|(1<<TWEN);
	while((TWCR&(1<<TWINT))==0);
}

void i2c_write(unsigned char data)
{
	TWDR=data;
	TWCR=(1<<TWINT)|(1<<TWEN);
	while((TWCR&(1<<TWINT))==0);
}


unsigned char i2c_read(unsigned char ackVal)
{
	TWCR=(1<<TWINT)|(1<<TWEN)|(ackVal<<TWEA);
	while((TWCR&(1<<TWINT))==0);
	return TWDR;
}


void i2c_stop()
{
	TWCR=(1<<TWINT)|(1<<TWSTO)|(1<<TWEN);
	for(int x=0;x<100;x++);
}

void rtc_init(void)
{
	i2c_init();
	i2c_start();
	i2c_write(0xD0);
	i2c_write(0x07);
	i2c_write(0x00);
	i2c_stop();
}

void rtc_setTime(unsigned char h, unsigned char m, unsigned char s)
{
	i2c_start();
	i2c_write(0xD0);
	i2c_write(0x00);
	i2c_write(s);
	i2c_write(m);
	i2c_write(h);
	i2c_stop();
}

void rtc_gettime(unsigned char *h, unsigned char *m, unsigned char *s)
{
	i2c_start();
	i2c_write(0xD0);
	i2c_write(0x00);
	i2c_stop();
	
	i2c_start();
	i2c_write(0xD1);
	unsigned char ss=i2c_read(1);
	unsigned char mm=i2c_read(1);
	unsigned char hh=i2c_read(0);
	i2c_stop();
	
	*s = ((ss & 0b01110000) >> 4) * 10 + (ss & 0b00001111); // Convert BCD to decimal
	*m = ((mm & 0b01110000) >> 4) * 10 + (mm & 0b00001111); // Convert BCD to decimal
	*h = ((hh & 0b00110000) >> 4) * 10 + (hh & 0b00001111); // Convert BCD to decimal
}

void send_packedBCD(unsigned char data)
{
	Lcd_DataWrite(48+(data>>4));
	Lcd_DataWrite(48+(data&15));
}
