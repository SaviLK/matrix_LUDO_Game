/*
 * Ludo_code.cpp
 *
 * Author : Savi
 */ 

#ifndef F_CPU
#define F_CPU 8000000UL  //define clock speed of MCU
#endif

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>


#include "max7219.h"		//Header file of MAX7219 matrix display library
#include "7seg_max7219.h"   //Header file for MAX7219 seven segment display



int px=0;		//Initialize display variables

int no_players = 0;   //No of players playing the game
char chance;		//Find the player having the chance
char winner;		//Store the winner

/*Each player has to move distance of 28 units (dots on display)*/
int remaining_distance_p1 = 28, remaining_distance_p2 = 28, remaining_distance_p3 = 28, remaining_distance_p4 = 28;


/*Variables for initial positions of players*/
const int init_A = 101, init_B = 102, init_C = 103, init_D = 104;


/*Variables for current position of the players*/
int curnt_Pos_playerA, curnt_Pos_playerB, curnt_Pos_playerC, curnt_Pos_playerD;



bool onGame = true;		// to identify whether the game is over

char RND_number;		// To store random number



/*-------------------------------------------------------Configuring USART communication settings-------------------------------------------*/

#define BaudRate 9600									//Baud rate of USART communication
#define Prescaler ((F_CPU/(BaudRate*16UL))-1)			//Prescaler value (value for UBRRn register



//Initialize USART communication
void USART_INIT()
{
	//Set Baud rates
	UBRR0H = Prescaler >> 8;
	UBRR0L = Prescaler;
	
	//Enable receiver and transmitter
	UCSR0B = (1<<RXEN0)|(1<<TXEN0);
	
	//Set Frame format: Asynchronous, 8bit data, 1 stop bit
	UCSR0C = (0<<UMSEL00)|(3<<UCSZ00)|(0<<USBS0)|(0<<UPM00);
}



//USART Transmit data
void USART_Transmit(uint8_t Data)
{
	while (( UCSR0A & (1<<UDRE0)) == 0) {}; // Do nothing until UDR is ready
	UDR0 = Data;
}



//USART receive data
uint8_t USART_Receive()
{
	uint8_t DataByte;
	while (( UCSR0A & (1<<RXC0)) == 0) {}; // Do nothing until data have been received
	DataByte = UDR0 ;
	return DataByte;
}


/*------------------------------------------------------------Configuring 8x8 display-----------------------------------------------------*/


int Disp_Array[8][8] =
   {{0,0,0,0,0,0,0,0},
	{0,0,0,1,1,0,0,0},
	{0,0,0,1,1,0,0,0},
	{0,1,1,1,1,1,1,0},
	{0,1,1,1,1,1,1,0},
	{0,0,0,1,1,0,0,0},
	{0,0,0,1,1,0,0,0},
	{0,0,0,0,0,0,0,0}};
		
		
int Disp_Number[8][8] =
   {{1,0,0,0,0,0,0,0},
	{1,0,0,0,0,0,0,0},
	{1,1,1,0,0,0,0,0},
	{1,0,1,0,0,1,1,1},
	{1,0,1,0,0,0,0,1},
	{0,0,0,0,0,1,1,1},
	{0,0,0,0,0,1,0,0},
	{0,0,0,0,0,1,1,1}};
		

/*Function to update display*/
void update_Disp()
{
	for (int i=0;i<8;i++)
	{
		for (int j=0; j<8; j++)
		{
			if (Disp_Array[i][j]>=1)
			{
				MAX7219_set_pixel_8X8(i,j,true);
			}
		}
	}
}

//Display the numbers to select no of players playing the game
void Display_number()
{
	for (int i=0;i<8;i++)
	{
		for (int j=0; j<8; j++)
		{
			if (Disp_Number[i][j]>=1)
			{
				MAX7219_set_pixel_8X8(i,j,true);
			}
		}
	}
}


/*Setting the path of the player. path has set as follows

	*  * 12 13 14 15  *  * 
	*  * 11  *  * 16  *  * 
   08 09 10  *  * 17 18 19 
   07  *  *  *  *  *  * 20 
   06  *  *  *  *  *  * 21 
   05 04 03  *  * 24 23 22 
	*  * 02  *  * 25  *  * 
	*  * 01 28 27 26  *  * 


*/
void set_Path(int point, bool status)
{
	if (status == true)		//Turn on related pixels on 8x8 display
	{
		if(point == 1)
		{
			Disp_Array[0][5] = 1;
		}
		
		else if (point == 2)
		{
			Disp_Array[1][5] = 1;
		}
		
		else if (point == 3)
		{
			Disp_Array[2][5] = 1;
		}
		
		else if (point == 4)
		{
			Disp_Array[2][6] = 1;
		}
		
		else if (point == 5)
		{
			Disp_Array[2][7] = 1;
		}
		
		else if (point == 6)
		{
			Disp_Array[3][7] = 1;
		}
		
		else if (point == 7)
		{
			Disp_Array[4][7] = 1;
		}
		
		else if (point == 8)
		{
			Disp_Array[5][7] = 1;
		}
		
		else if (point == 9)
		{
			Disp_Array[5][6] = 1;
		}
		
		else if (point == 10)
		{
			Disp_Array[5][5] = 1;
		}
		
		else if (point == 11)
		{
			Disp_Array[6][5] = 1;
		}
		
		else if (point == 12)
		{
			Disp_Array[7][5] = 1;
		}
		
		else if (point == 13)
		{
			Disp_Array[7][4] = 1;
		}
		
		else if (point == 14)
		{
			Disp_Array[7][3] = 1;
		}
		
		else if (point == 15)
		{
			Disp_Array[7][2] = 1;
		}
		
		else if (point == 16)
		{
			Disp_Array[6][2] = 1;
		}
		
		else if (point == 17)
		{
			Disp_Array[5][2] = 1;
		}
		
		else if (point == 18)
		{
			Disp_Array[5][1] = 1;
		}
		
		else if (point == 19)
		{
			Disp_Array[5][0] = 1;
		}
		
		else if (point == 20)
		{
			Disp_Array[4][0] = 1;
		}
		
		else if (point == 21)
		{
			Disp_Array[3][0] = 1;
		}
		
		else if (point == 22)
		{
			Disp_Array[2][0] = 1;
		}
		
		else if (point == 23)
		{
			Disp_Array[2][1] = 1;
		}
		
		else if (point == 24)
		{
			Disp_Array[2][2] = 1;
		}
		
		else if (point == 25)
		{
			Disp_Array[1][2] = 1;
		}
		
		else if (point == 26)
		{
			Disp_Array[0][2] = 1;
		}
		
		else if (point == 27)
		{
			Disp_Array[0][3] = 1;
		}
		
		else if (point == 28)
		{
			Disp_Array[0][4] = 1;
		}
		
		else if (point == 101)     //position of player 01 in yard
		{
			Disp_Array[0][6] = 1;
		}
		
		else if (point == 102)		// position of player 02 in yard (in 4 player mode)
		{
			Disp_Array[6][7] = 1;
		}
		
		else if (point == 103)		//position of player 03 in yard (in 4 player mode) player 02 in 2 player mode
		{
			Disp_Array[7][1] = 1;
		}
		
		else if (point == 104)		// position of player 04 in yard (in 4 player mode)
		{
			Disp_Array[1][0] = 1;
		}
	}
	
	
	
	else //Turn off ralated pixels in 8x8 display
	{
		if(point == 1)
		{
			Disp_Array[0][5] = 0;
		}
		
		else if (point == 2)
		{
			Disp_Array[1][5] = 0;
		}
		
		else if (point == 3)
		{
			Disp_Array[2][5] = 0;
		}
		
		else if (point == 4)
		{
			Disp_Array[2][6] = 0;
		}
		
		else if (point == 5)
		{
			Disp_Array[2][7] = 0;
		}
		
		else if (point == 6)
		{
			Disp_Array[3][7] = 0;
		}
		
		else if (point == 7)
		{
			Disp_Array[4][7] = 0;
		}
		
		else if (point == 8)
		{
			Disp_Array[5][7] = 0;
		}
		
		else if (point == 9)
		{
			Disp_Array[5][6] = 0;
		}
		
		else if (point == 10)
		{
			Disp_Array[5][5] = 0;
		}
		
		else if (point == 11)
		{
			Disp_Array[6][5] = 0;
		}
		
		else if (point == 12)
		{
			Disp_Array[7][5] = 0;
		}
		
		else if (point == 13)
		{
			Disp_Array[7][4] = 0;
		}
		
		else if (point == 14)
		{
			Disp_Array[7][3] = 0;
		}
		
		else if (point == 15)
		{
			Disp_Array[7][2] = 0;
		}
		
		else if (point == 16)
		{
			Disp_Array[6][2] = 0;
		}
		
		else if (point == 17)
		{
			Disp_Array[5][2] = 0;
		}
		
		else if (point == 18)
		{
			Disp_Array[5][1] = 0;
		}
		
		else if (point == 19)
		{
			Disp_Array[5][0] = 0;
		}
		
		else if (point == 20)
		{
			Disp_Array[4][0] = 0;
		}
		
		else if (point == 21)
		{
			Disp_Array[3][0] = 0;
		}
		
		else if (point == 22)
		{
			Disp_Array[2][0] = 0;
		}
		
		else if (point == 23)
		{
			Disp_Array[2][1] = 0;
		}
		
		else if (point == 24)
		{
			Disp_Array[2][2] = 0;
		}
		
		else if (point == 25)
		{
			Disp_Array[1][2] = 0;
		}
		
		else if (point == 26)
		{
			Disp_Array[0][2] = 0;
		}
		
		else if (point == 27)
		{
			Disp_Array[0][3] = 0;
		}
		
		else if (point == 28)
		{
			Disp_Array[0][4] = 0;
		}
		
		else if (point == 101)     //position of player 01 in yard
		{
			Disp_Array[0][6] = 0;
		}
		
		else if (point == 102)		// position of player 02 in yard (in 4 player mode)
		{
			Disp_Array[6][7] = 0;
		}
		
		else if (point == 103)		//position of player 03 in yard (in 4 player mode) player 02 in 2 player mode
		{
			Disp_Array[7][1] = 0;
		}
		
		else if (point == 104)		// position of player 04 in yard (in 4 player mode)
		{
			Disp_Array[1][0] = 0;
		}
	}
}



/*----------------------------------------checking for safe----------------------------------------------*/
bool check_Safe (int position)
{
	bool IsSafe = false;	// to check whether the player is in a safe position or not
	
	if (position == 1 || position == 8 || position == 15 || position == 22)
	{
		IsSafe = true;
	}
	else 
	{
		IsSafe = false;
	}
	
	return IsSafe;
}


	
void LUDO(){		//Word Ludo
	MAX7219_set_column_8X8(px,0b01111110);			//L
	MAX7219_set_column_8X8(px-1,0b00000010);
	MAX7219_set_column_8X8(px-2,0b00000010);
	MAX7219_set_column_8X8(px-3,0b00000010);
	
	MAX7219_set_column_8X8(px-5,0b01111100);       //U
	MAX7219_set_column_8X8(px-6,0b00000010);
	MAX7219_set_column_8X8(px-7,0b00000010);
	MAX7219_set_column_8X8(px-8,0b00000010);
	MAX7219_set_column_8X8(px-9,0b01111100);
	
	MAX7219_set_column_8X8(px-11,0b01111110);      //D
	MAX7219_set_column_8X8(px-12,0b01000010);
	MAX7219_set_column_8X8(px-13,0b01000010);
	MAX7219_set_column_8X8(px-14,0b00111100);
	
	MAX7219_set_column_8X8(px-16,0b00111100);       //O
	MAX7219_set_column_8X8(px-17,0b01000010);
	MAX7219_set_column_8X8(px-18,0b01000010);
	MAX7219_set_column_8X8(px-19,0b01000010);
	MAX7219_set_column_8X8(px-20,0b00111100);
}


void WINNER(){		//Word Ludo
	MAX7219_set_column_8X8(px,0b01111110);			//W
	MAX7219_set_column_8X8(px-1,0b00000100);
	MAX7219_set_column_8X8(px-2,0b00111000);
	MAX7219_set_column_8X8(px-3,0b00000100);
	MAX7219_set_column_8X8(px-4,0b01111110);	
	
	MAX7219_set_column_8X8(px-6,0b01000010);		//I
	MAX7219_set_column_8X8(px-7,0b01111110);	
	MAX7219_set_column_8X8(px-8,0b01000010);
	
	MAX7219_set_column_8X8(px-10,0b01111110);		//N
	MAX7219_set_column_8X8(px-11,0b00100000);
	MAX7219_set_column_8X8(px-12,0b00010000);
	MAX7219_set_column_8X8(px-13,0b00001000);
	MAX7219_set_column_8X8(px-14,0b00000100);
	MAX7219_set_column_8X8(px-15,0b01111110);		
	
	MAX7219_set_column_8X8(px-17,0b01111110);		//N
	MAX7219_set_column_8X8(px-18,0b00100000);
	MAX7219_set_column_8X8(px-19,0b00010000);
	MAX7219_set_column_8X8(px-20,0b00001000);
	MAX7219_set_column_8X8(px-21,0b00000100);
	MAX7219_set_column_8X8(px-22,0b01111110);
	
	MAX7219_set_column_8X8(px-24,0b01111110);		//E
	MAX7219_set_column_8X8(px-25,0b01010010);
	MAX7219_set_column_8X8(px-26,0b01010010);
	MAX7219_set_column_8X8(px-27,0b01000010);
	
	MAX7219_set_column_8X8(px-29,0b01111110);		//R
	MAX7219_set_column_8X8(px-30,0b01011000);
	MAX7219_set_column_8X8(px-31,0b01010100);
	MAX7219_set_column_8X8(px-32,0b01110010);
}


//Fucntion to display number 01 on matrix display
void Disp_01 ()
{
	MAX7219_clear_8X8();
	MAX7219_set_column_8X8(5, 0b00100010);
	MAX7219_set_column_8X8(4, 0b01111110);
	MAX7219_set_column_8X8(3, 0b00000010);
}

//Function to display number 02 on matrix display
void Disp_02()
{
	MAX7219_clear_8X8();
	MAX7219_set_column_8X8(5, 0b00100010);
	MAX7219_set_column_8X8(4, 0b01000110);
	MAX7219_set_column_8X8(3, 0b01001010);
	MAX7219_set_column_8X8(2, 0b00110010);
}

//Function to display number 03 on matrix display
void Disp_03()
{
	MAX7219_clear_8X8();
	MAX7219_set_column_8X8(5, 0b00100100);
	MAX7219_set_column_8X8(4, 0b01000010);
	MAX7219_set_column_8X8(3, 0b01011010);
	MAX7219_set_column_8X8(2, 0b00100100);
}

// Function to display 04 on matrix display
void Disp_04()
{
	MAX7219_clear_8X8();
	MAX7219_set_column_8X8(2,0b00001000);
	MAX7219_set_column_8X8(3,0b01111110);
	MAX7219_set_column_8X8(4,0b00101000);
	MAX7219_set_column_8X8(5,0b00011000);
	MAX7219_set_column_8X8(6,0b00001000);
}


void Disp_name(){				//Display the name LUDO
	for (px=0;px<30;px++)
	{
		LUDO();
		_delay_ms(150);
		MAX7219_clear_8X8();
		_delay_ms(10);
	}
	MAX7219_clear_8X8();
	px=0;
}


void Disp_winner()
{
	for (px=0;px<40;px++)
	{
		WINNER();
		_delay_ms(150);
		MAX7219_clear_8X8();
		_delay_ms(10);
	}
	MAX7219_clear_8X8();
	px=0;
}


//Function for simple animation
void Animation ()
{
	MAX7219_clear_8X8();
	for(int i=0; i<8; i++)
	{
		for (int j=0; j<8; j++)
		{
			MAX7219_set_pixel_8X8(i,j,true);
			_delay_ms(50);
		}
	}
	_delay_ms(500);
	MAX7219_clear_8X8();
}
/*---------------------------------------Configuring seven segment display----------------------------------------------*/

void disp_seg(char L){	
	if (L == '1')
	{
		MAX7219_set_row_7SEG(1, 0b00110000);
	}
	else if (L == '2')
	{
		MAX7219_set_row_7SEG(1, 0b01101101);
	}
	else if (L == '3')
	{
		MAX7219_set_row_7SEG(1, 0b01111001);
	}
	else if (L == '4')
	{
		MAX7219_set_row_7SEG(1, 0b00110011);
	}
	else if (L == '5')
	{
		MAX7219_set_row_7SEG(1, 0b01011011);
	}
	else if (L == '6')
	{
		MAX7219_set_row_7SEG(1, 0b01011111);
	}
}


/*---------------------Set Current position of players-----------------------------*/
void set_curnt_pos()
{
	set_Path(curnt_Pos_playerA,true);
	set_Path(curnt_Pos_playerB,true);
	set_Path(curnt_Pos_playerC,true);
	set_Path(curnt_Pos_playerD,true);
}



/*----------------------------------------Check for displace opponent---------------------------------------------------*/

void check(int player_pos, int oponant_pos, int oponant_distance, int oponant_init, char player)
{
	if (player_pos == oponant_pos)
	{
		int oponant_previous = oponant_pos;
		
		for(oponant_distance; oponant_distance<28; oponant_distance++)
		{
			if (oponant_previous == 1)
			{
				oponant_pos = 28;
				set_Path(oponant_previous, false);
				
				//Update current position of opponant player
				if (oponant_init == 101)
				{
					curnt_Pos_playerA = oponant_pos;
				}
				else if (oponant_init == 102)
				{
					curnt_Pos_playerB = oponant_pos;
				}
				else if (oponant_init == 103)
				{
					curnt_Pos_playerC = oponant_pos;
				}
				else if (oponant_init == 104)
				{
					curnt_Pos_playerD = oponant_pos;
				}
				
				
				set_curnt_pos();			//Set current pos of all players
				MAX7219_clear_8X8();
				update_Disp();							//Move point and update display
				_delay_ms(200);
				oponant_previous = oponant_pos;
			}
			else
			{
				oponant_pos--;
				set_Path(oponant_previous, false);
				
				
				//Update current position and remaining distance of opponant player
				if (oponant_init == 101)
				{
					curnt_Pos_playerA = oponant_pos;
				}
				else if (oponant_init == 102)
				{
					curnt_Pos_playerB = oponant_pos;
				}
				else if (oponant_init == 103)
				{
					curnt_Pos_playerC = oponant_pos;
				}
				else if (oponant_init == 104)
				{
					curnt_Pos_playerD = oponant_pos;
				}
				
				
				set_curnt_pos();			//Set current position of all players
				
				MAX7219_clear_8X8();
				update_Disp();							//Move point and update display
				_delay_ms(200);
				oponant_previous = oponant_pos;
			}
		}
		
		set_Path(oponant_pos,false);
		
		//Update current position and remaining distance of opponent player
		if (oponant_init == 101)
		{
			curnt_Pos_playerA = oponant_init;
			remaining_distance_p1 = 28;
		}
		else if (oponant_init == 102)
		{
			curnt_Pos_playerB = oponant_init;
			remaining_distance_p2 = 28;
		}
		else if (oponant_init == 103)
		{
			curnt_Pos_playerC = oponant_init;
			remaining_distance_p3 = 28;
		}
		else if (oponant_init == 104)
		{
			curnt_Pos_playerD = oponant_init;
			remaining_distance_p4 = 28;
		}
		
		
		/*Update the next chance*/
		if (player == 'A')
		{
			chance = 'A';
		}
		else if (player == 'B')
		{
			chance = 'B';
		}
		else if (player == 'C')
		{
			chance = 'C';
		}
		else if (player == 'D')
		{
			chance = 'D';
		}
		
		
		
		set_curnt_pos();			//Set current pos of all players
		MAX7219_clear_8X8();
		update_Disp();		//Send player A to his yard
	}
}

/*----------------------------------------------------converting to a number-------------------------------------------------*/

//Return number according to char value
int set_number()
{
	if (RND_number=='1')
	{
		return 1;
	}
	else if (RND_number == '2')
	{
		return 2;
	}
	else if (RND_number == '3')
	{
		return 3;
	}
	else if (RND_number == '4')
	{
		return 4;
	}
	else if (RND_number == '5')
	{
		return 5;
	}
	else if (RND_number == '6')
	{
		return 6;
	}
	else
	{
		return 0;
	}
}






/*------------------------------------------------------------Player A-----------------------------------------------------*/

/*Function to move the point of player A*/
void playerA (int Random_number)
{
	int previous_Pos = curnt_Pos_playerA;
	
	if (curnt_Pos_playerA == init_A)
	{
		if (Random_number == 6)
		{
			curnt_Pos_playerA = 1;
			set_Path(101,false);
			set_curnt_pos();
			MAX7219_clear_8X8();
			update_Disp();			//Set positions on display and update
			_delay_ms(500);
		}
	}
	else if (remaining_distance_p1 >= Random_number)
	{
		for (int i=0; i<Random_number; i++)
		{
			if (previous_Pos == 28)
			{
				curnt_Pos_playerA = 1;
				remaining_distance_p1--;
				set_Path(previous_Pos, false);
				set_curnt_pos();
				MAX7219_clear_8X8();
				update_Disp();							//Move point and update display
				_delay_ms(500);
				previous_Pos = curnt_Pos_playerA;
			}
			else
			{
				curnt_Pos_playerA++;
				remaining_distance_p1--;
				set_Path(previous_Pos, false);
				set_curnt_pos();
				MAX7219_clear_8X8();
				update_Disp();							//Move point and update display
				_delay_ms(500);
				previous_Pos = curnt_Pos_playerA;
			}
		}
	}
	
	/*If random number is not equal to 1 or 6 chance goes to next player*/
	if (!(Random_number == 6))
	{
		if (no_players == 2)
		{
			chance = 'C';
		}
		else if (no_players == 4)
		{
			chance = 'B';
		}
	}
	
	/*If remainig distance is 0, then the game is over*/
	if (remaining_distance_p1 == 0)
	{
		chance = '0';
		no_players = -1;
		onGame = false;
		winner = 'A';
	}
	
	
	/* If oponant player is on the current position of the player, and not a safe position oponant player returns to his yard*/
	
	if (!(check_Safe(curnt_Pos_playerA)))
	{
		//Checking for player B
		check(curnt_Pos_playerA,curnt_Pos_playerB,remaining_distance_p2,init_B,'A');
		
		//Checking for player C
		check(curnt_Pos_playerA,curnt_Pos_playerC,remaining_distance_p3,init_C,'A');
		
		//Checking for player D
		check(curnt_Pos_playerA,curnt_Pos_playerD,remaining_distance_p4,init_D,'A');
	}
	
}




/*------------------------------------------------------------Player B-----------------------------------------------------*/

/*Function to move the point of player B*/
void playerB (int Random_number)
{
	int previous_Pos = curnt_Pos_playerB;
	
	if (curnt_Pos_playerB == init_B)
	{
		if (Random_number == 6)
		{
			curnt_Pos_playerB = 8;
			set_Path(102,false);
			set_curnt_pos();
			MAX7219_clear_8X8();
			update_Disp();			//Set positions on display and update
			_delay_ms(500);
		}
	}
	else if (remaining_distance_p2 >= Random_number)
	{
		for (int i=0; i<Random_number; i++)
		{
			if (previous_Pos == 28)
			{
				curnt_Pos_playerB = 1;
				remaining_distance_p2--;
				set_Path(previous_Pos, false);
				set_curnt_pos();
				MAX7219_clear_8X8();
				update_Disp();							//Move point and update display
				_delay_ms(500);
				previous_Pos = curnt_Pos_playerB;
			}
			else
			{
				curnt_Pos_playerB++;
				remaining_distance_p2--;
				set_Path(previous_Pos, false);
				set_curnt_pos();
				MAX7219_clear_8X8();
				update_Disp();							//Move point and update display
				_delay_ms(500);
				previous_Pos = curnt_Pos_playerB;
			}
		}
	}
	
	/*If random number is not equal to 1 or 6 chance goes to next player*/
	if (!(Random_number == 6))
	{
		chance = 'C';
	}
	
	/*If remainig distance is 0, then the game is over*/
	if (remaining_distance_p2 == 0)
	{
		chance = '0';
		no_players = -1;
		onGame = false;
		winner = 'B';
	}
	
	
	/* If oponant player is on the current position of the player and not in safe place, oponant player returns to his yard*/
	
	if (!(check_Safe(curnt_Pos_playerB)))
	{
		//Checking for player A
		check(curnt_Pos_playerB,curnt_Pos_playerA,remaining_distance_p1,init_A,'B');
		
		//Checking for player C
		check(curnt_Pos_playerB,curnt_Pos_playerC,remaining_distance_p3,init_C,'B');
		
		//Checking for player D
		check(curnt_Pos_playerB,curnt_Pos_playerD,remaining_distance_p4,init_D,'B');
	}
}



/*------------------------------------------------------------Player C-----------------------------------------------------*/

/*Function to move the point of player C*/
void playerC (int Random_number)
{
	int previous_Pos = curnt_Pos_playerC;
	
	if (curnt_Pos_playerC == init_C)
	{
		if (Random_number == 6)
		{
			curnt_Pos_playerC = 15;
			set_Path(103,false);
			set_curnt_pos();
			MAX7219_clear_8X8();
			update_Disp();			//Set positions on display and update
			_delay_ms(500);
		}
	}
	else if (remaining_distance_p3 >= Random_number)
	{
		for (int i=0; i<Random_number; i++)
		{
			if (previous_Pos == 28)
			{
				curnt_Pos_playerC = 1;
				remaining_distance_p3--;
				set_Path(previous_Pos, false);
				set_curnt_pos();
				MAX7219_clear_8X8();
				update_Disp();							//Move point and update display
				_delay_ms(500);
				previous_Pos = curnt_Pos_playerC;
			}
			else
			{
				curnt_Pos_playerC++;
				remaining_distance_p3--;
				set_Path(previous_Pos, false);
				set_curnt_pos();
				MAX7219_clear_8X8();
				update_Disp();							//Move point and update display
				_delay_ms(500);
				previous_Pos = curnt_Pos_playerC;
			}
		}
	}
	
	/*If random number is not equal to 1 or 6 chance goes to next player*/
	if (!(Random_number == 6))
	{
		if (no_players == 2)
		{
			chance = 'A';
		}
		else if (no_players == 4)
		{
			chance = 'D';
		}
	}
	
	/*If remainig distance is 0, then the game is over*/
	if (remaining_distance_p3 == 0)
	{
		chance = '0';
		no_players = -1;
		onGame = false;
		winner = 'C';
	}
	
	
	/* If oponant player is on the current position of the player and not in a safe position, oponant player returns to his yard*/
	
	
	if (!(check_Safe(curnt_Pos_playerC)))
	{
		//Checking for player A
		check(curnt_Pos_playerC,curnt_Pos_playerA,remaining_distance_p1,init_A,'C');
		
		//Checking for player B
		check(curnt_Pos_playerC,curnt_Pos_playerB,remaining_distance_p2,init_B,'C');
		
		//Checking for player D
		check(curnt_Pos_playerC,curnt_Pos_playerD,remaining_distance_p4,init_D, 'C');
	}
}




/*------------------------------------------------------------Player D-----------------------------------------------------*/

/*Function to move the point of player D*/
void playerD (int Random_number)
{
	int previous_Pos = curnt_Pos_playerD;
	
	if (curnt_Pos_playerD == init_D)
	{
		if (Random_number == 6)
		{
			curnt_Pos_playerD = 22;
			set_Path(104,false);
			set_curnt_pos();
			MAX7219_clear_8X8();
			update_Disp();			//Set positions on display and update
			_delay_ms(500);
		}
	}
	else if (remaining_distance_p4 >= Random_number)
	{
		for (int i=0; i<Random_number; i++)
		{
			if (previous_Pos == 28)
			{
				curnt_Pos_playerD = 1;
				remaining_distance_p4--;
				set_Path(previous_Pos, false);
				set_curnt_pos();
				MAX7219_clear_8X8();
				update_Disp();							//Move point and update display
				_delay_ms(500);
				previous_Pos = curnt_Pos_playerD;
			}
			else
			{
				curnt_Pos_playerD++;
				remaining_distance_p4--;
				set_Path(previous_Pos, false);
				set_curnt_pos();
				MAX7219_clear_8X8();
				update_Disp();							//Move point and update display
				_delay_ms(500);
				previous_Pos = curnt_Pos_playerD;
			}
		}
	}
	
	/*If random number is not equal to 1 or 6 chance goes to next player*/
	if (!(Random_number == 6))
	{
		chance = 'A';
	}
	
	/*If remainig distance is 0, then the game is over*/
	if (remaining_distance_p4 == 0)
	{
		chance = '0';
		no_players = -1;
		onGame = false;
		winner = 'D';
	}
	
	
	/* If oponant player is on the current position of the player and not in a safe position, oponant player returns to his yard*/
	
	
	if (!(check_Safe(curnt_Pos_playerD)))
	{
		//Checking for player A
		check(curnt_Pos_playerD,curnt_Pos_playerA,remaining_distance_p1,init_A,'D');
		
		//Checking for player B
		check(curnt_Pos_playerD,curnt_Pos_playerB,remaining_distance_p2,init_B,'D');
		
		//Checking for player C
		check(curnt_Pos_playerD,curnt_Pos_playerC,remaining_distance_p3,init_C,'D');
	}
}




/*------------------------------------------------------------main function-----------------------------------------------------*/




int main(void)
{
	USART_INIT();		//Initialize USART communication
	
	DDRC = 0x00;		//Set port C as input
	
	DDRD &= ~(1<<DDD3); // Set D3 pin of port D as Input for interrupt (for reset pin)
	EICRA |= (1<<ISC10)|(1<<ISC11); // Set INT1 trigger on rising edge
	EIMSK |= (1<<INT1); //Enable INT1
	
	
	sei();		//Enable global interrupt
	
	MAX7219_init_8X8();				//Initialize Display
	MAX7219_set_intensity_8X8(15);	//Brightness of the display			
	
	MAX7219_init_7SEG();				//Initialize seven segment display
	MAX7219_set_intensity_7SEG(15);		//Brightness of seven segment display
	
	while (1)
	{		
		Disp_name();			//Display name LUDO
		no_players = 0;
		
	
		/*Select no of players that playing
		if player 2 button is pressed 2 players can play the game
		if player 4 button is pressed 4 players can play the game */
	
		while (no_players == 0) 
		{
			Display_number();
			if (!(PINC & (1<<PINC1)))
			{
				no_players = 2;
				MAX7219_clear_8X8();		//Clear matrix display
				set_Path(101,true);
				set_Path(103, true);
				curnt_Pos_playerA = init_A;
				curnt_Pos_playerC = init_C;
				chance = 'A';
				onGame = true;
			}
			else if (!(PINC & (1<<PINC3)))
			{
				no_players = 4;
				MAX7219_clear_8X8();		//Clear matrix display
				set_Path(101,true);
				set_Path(103,true);
				set_Path(104,true);
				set_Path(102,true);
				curnt_Pos_playerA = init_A;
				curnt_Pos_playerB = init_B;
				curnt_Pos_playerC = init_C;
				curnt_Pos_playerD = init_D;
				chance = 'A';
				onGame = true;
			}
		}
	
	
	
		while (onGame) 
		{
			Animation();	//Dispplay simple Animation
			
			//Count Down
			Disp_03();
			_delay_ms(1000);
			Disp_02();
			_delay_ms(1000);
			Disp_01();
			_delay_ms(1000);
			
			while (no_players == 2)
			{
				update_Disp();
				#define Player_01 (1<<PINC0) 
				#define Player_02 (1<<PINC2)
			
				while (chance == 'A')		//When the chance is to player 01
				{
					MAX7219_clear_8X8();
					Disp_Array[0][7] = 1;
					update_Disp();
					if (!(PINC & Player_01))
					{
						USART_Transmit('L');			//Send "L" to RNG MCU to receive random number
						RND_number = USART_Receive();	//Set RND number to number received by RNG MCU
						disp_seg(RND_number);
						_delay_ms(1000);
						MAX7219_clear_7SEG();
						playerA(set_number());
					}
					Disp_Array[0][7] = 0;
					MAX7219_clear_8X8();
				}
			
				while (chance == 'C')		//when chance is to player 02
				{
					MAX7219_clear_8X8();
					Disp_Array[7][0] = 1;
					update_Disp();
					if (!(PINC & Player_02))
					{
						USART_Transmit('L');			//Send "L" to RNG MCU to receive random number
						RND_number = USART_Receive();	//Set RND number to number received by RNG MCU
						disp_seg(RND_number);
						_delay_ms(1000);
						MAX7219_clear_7SEG();
						playerC(set_number());
					}
					Disp_Array[7][0] = 0;
					MAX7219_clear_8X8();
				}
			}
			while (no_players == 4)
			{
				update_Disp();
				
				//Define inputs of players
				#define Player_01 (1<<PINC0)
				#define Player_02 (1<<PINC1)
				#define Player_03 (1<<PINC2)
				#define Player_04 (1<<PINC3)
				
				while (chance == 'A')		//When the chance is to player 01
				{
					MAX7219_clear_8X8();
					Disp_Array[0][7] = 1;
					update_Disp();
					if (!(PINC & Player_01))
					{
						USART_Transmit('L');			//Send "L" to RNG MCU to receive random number
						RND_number = USART_Receive();	//Set RND number to number received by RNG MCU
						disp_seg(RND_number);
						_delay_ms(1000);
						MAX7219_clear_7SEG();
						playerA(set_number());
					}
					Disp_Array[0][7] = 0;
					MAX7219_clear_8X8();
				}
				
				
				
				while (chance == 'B')		//When the chance is to player 02
				{
					MAX7219_clear_8X8();
					Disp_Array[7][7] = 1;
					update_Disp();
					if (!(PINC & Player_02))
					{
						USART_Transmit('L');			//Send "L" to RNG MCU to receive random number
						RND_number = USART_Receive();	//Set RND number to number received by RNG MCU
						disp_seg(RND_number);
						_delay_ms(1000);
						MAX7219_clear_7SEG();
						playerB(set_number());
					}
					Disp_Array[7][7] = 0;
					MAX7219_clear_8X8();
				}
				
				
				while (chance == 'C')		//When the chance is to player 03
				{
					MAX7219_clear_8X8();
					Disp_Array[7][0] = 1;
					update_Disp();
					if (!(PINC & Player_03))
					{
						USART_Transmit('L');			//Send "L" to RNG MCU to receive random number
						RND_number = USART_Receive();	//Set RND number to number received by RNG MCU
						disp_seg(RND_number);
						_delay_ms(1000);
						MAX7219_clear_7SEG();
						playerC(set_number());
					}
					Disp_Array[7][0] = 0;
					MAX7219_clear_8X8();
				}
				
				
				while (chance == 'D')		//When the chance is to player 04
				{
					MAX7219_clear_8X8();
					Disp_Array[0][0] = 1;
					update_Disp();
					if (!(PINC & Player_04))
					{
						USART_Transmit('L');			//Send "L" to RNG MCU to receive random number
						RND_number = USART_Receive();	//Set RND number to number received by RNG MCU
						disp_seg(RND_number);
						_delay_ms(1000);
						MAX7219_clear_7SEG();
						playerD(set_number());
					}
					Disp_Array[0][0] = 0;
					MAX7219_clear_8X8();
				}
			}
		}
		
		
		//When game is over
		while (no_players == -1)
		{
				Animation();	//Display simple animation
				Disp_winner();	//Display word Winner
				if (winner == 'A')
				{
					Disp_01();
				}
				else if (winner == 'B')
				{
					Disp_02();
				}
				else if (winner == 'C')
				{
					Disp_03();
				}
				else if (winner == 'D')
				{
					Disp_04();
				}
				
				_delay_ms(1500);
			
		}	
	}
}


ISR(INT1_vect)
{
	//reset all variables
	no_players = -10;
	chance = '0';
	onGame = false;
}
