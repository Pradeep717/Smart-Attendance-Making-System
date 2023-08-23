
#ifndef UTILITYDISPLAY_H_
#define UTILITYDISPLAY_H_


#define MAX_LENGTH 10
int data1[MAX_LENGTH];


void displayWelcome(void)
{
	Lcd_CmdWrite(0x01);
	Lcd_CmdWrite(0x80);
	Lcd_DataWrite('W');
	Lcd_DataWrite('E');
	Lcd_DataWrite('L');
	Lcd_DataWrite('C');
	Lcd_DataWrite('O');
	Lcd_DataWrite('M');
	Lcd_DataWrite('E');
}

void displayStart(void)
{
	Lcd_CmdWrite(0x80);
	for (uint8_t i = 0; i < 16; i++) {
		Lcd_DataWrite(' '); // Write empty space to clear the character
	}
	Lcd_CmdWrite(0x80);
	Lcd_DataWrite('M');
	Lcd_DataWrite('O');
	Lcd_DataWrite('D');
	Lcd_DataWrite('U');
	Lcd_DataWrite('L');
	Lcd_DataWrite('E');
	Lcd_DataWrite(' ');
	Lcd_DataWrite('N');
	Lcd_DataWrite('U');
	Lcd_DataWrite('M');
	Lcd_DataWrite('B');
	Lcd_DataWrite('E');
	Lcd_DataWrite('R');
	Lcd_DataWrite(':');
	Lcd_CmdWrite(0xC0);
	
	//Lcd_CmdWrite(0x01);
}

void displayStartNew(unsigned char x)
{
	if (x)
	{
		Lcd_CmdWrite(0x01);
		Lcd_CmdWrite(0x14);
		Lcd_DataWrite(x);
	}
	
}

void displayScan(void)
{
	//Lcd_CmdWrite(0x01);
	//Lcd_CmdWrite(0x80);
	Lcd_DataWrite('S');
	Lcd_DataWrite('C');
	Lcd_DataWrite('A');
	Lcd_DataWrite('N');
	Lcd_DataWrite(' ');
	Lcd_DataWrite('I');
	Lcd_DataWrite('D');
	Lcd_DataWrite(' ');
	Lcd_CmdWrite(0X0F);
	
}


void displayCount(void)
{
	Lcd_DataWrite('C');
	Lcd_DataWrite('O');
	Lcd_DataWrite('U');
	Lcd_DataWrite('N');
	Lcd_DataWrite('T');
	Lcd_DataWrite(':');
}


void displayData(void)
{
	Lcd_CmdWrite(0xC0);
	for (uint8_t i = 0; i < 16; i++) {
		Lcd_DataWrite(' '); // Write empty space to clear the character
	}
	Lcd_CmdWrite(0xC0);
	displayCount();
    Lcd_DataWrite(data1[1]+0x30);
    Lcd_DataWrite(data1[0]+0x30);
}


#endif /* UTILITYDISPLAY_H_ */