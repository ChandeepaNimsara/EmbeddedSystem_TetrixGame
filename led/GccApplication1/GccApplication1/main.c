#include <avr/io.h>
#include <stdlib.h>
#include <util/delay.h>
#include <time.h>
#include "string.h"
#include "stdio.h"
#include "avr/interrupt.h"
#define F_CPU 8000000UL

#define ATMEGA_LCD DDRB
#define DECODDER_2 DDRA
#define DDRD_2  DDRD

#define LED_DATA PORTC
#define DECODDER_DATA PORTA 
#define PORTD_DATA PORTD

volatile int downValue[]  = {0x08,0x09,0x0a,0x0b,0x0c,0x0d,0x0e,0x0f,0x80,0x90,0xa0,0xb0,0xc0,0xd0,0xe0,0xf0};
volatile int BlockType;
volatile int RotateBlock  = 0;
volatile int Now_PosiX = 1, Now_PosiY= 4;
volatile int After_PosiX = 1, After_PosiY = 4;
volatile int stop_Block  = 0;
volatile int matrix[17][9];

int Start_game = 0;
int Continue_game = 0;
int getFinalScore = 0;

void OnLedMatrix(int r, int c){
	matrix[r][c] = 1;
}

int getRow(int x) {
	int row = 0;
	for (int i = 1; i <= 8; i++){
		 row = row | (matrix[x][i] << (8-i));
	}
	return row;
}

int getCol(int x){
	int col = 0;
	for (int i = 1; i <= 8; i++){
		 col = col | (matrix[i][x] << (i-1));
	}
	return col;
}

//Line

int genarate_Line(int r, int c){
	if (RotateBlock  % 2 == 0){
		if (r > 16 || c < 2 || c > 7){
			return 0;
		}
		if (matrix[r][c] == 0 && matrix[r][c-1] == 0 && matrix[r][c+1] == 0){
			OnLedMatrix(r,c);
			OnLedMatrix(r,c-1);
			OnLedMatrix(r,c+1);
			return 1;
		}
	}
	else if (RotateBlock  % 2 == 1){
		if (r > 15 || c < 1 || c > 8){
			return 0;
		}
		if (matrix[r][c] == 0 && matrix[r-1][c] == 0 && matrix[r+1][c] == 0){
			OnLedMatrix(r,c);
			OnLedMatrix(r-1,c);
			OnLedMatrix(r+1,c);
			return 1;
		}
	}
	return 0;
}

//Box

int genarate_Box(int r, int c){
	if (r > 16 || c < 1 || c > 7) {
		return 0;
	}
	if (matrix[r][c] == 0 && matrix[r][c+1] == 0 && matrix[r-1][c] == 0 && matrix[r-1][c+1] == 0){
		OnLedMatrix(r,c);
		OnLedMatrix(r,c+1);
		OnLedMatrix(r-1,c);
		OnLedMatrix(r-1,c+1);
		return 1;
	}
	return 0;
}

//T

int genarate_T(int r, int c){
	if (RotateBlock  % 4 == 0){
		if (r > 16 || c < 2 || c > 7) return 0;
		if (matrix[r][c] == 0 && matrix[r][c-1] == 0 && matrix[r][c+1] == 0 && matrix[r-1][c] == 0){
			OnLedMatrix(r,c);
			OnLedMatrix(r,c-1);
			OnLedMatrix(r,c+1);
			OnLedMatrix(r-1,c);
			return 1;
		}
	}
	else if (RotateBlock % 4 == 1){
		if (r > 15 || c < 1 || c > 7) return 0;
		if (matrix[r][c] == 0 && matrix[r-1][c] == 0 && matrix[r][c+1] == 0 && matrix[r+1][c] == 0){
			OnLedMatrix(r,c);
			OnLedMatrix(r-1,c);
			OnLedMatrix(r,c+1);
			OnLedMatrix(r+1,c);
			return 1;
		}
	}
	else if (RotateBlock  % 4 == 2){
		if (r > 15 || c < 2 || c > 7) return 0;
		if (matrix[r][c] == 0 && matrix[r][c-1] == 0 && matrix[r][c+1] == 0 && matrix[r+1][c] == 0){
			OnLedMatrix(r,c);
			OnLedMatrix(r,c-1);
			OnLedMatrix(r,c+1);
			OnLedMatrix(r+1,c);
			return 1;
		}
	}
	else if (RotateBlock  % 4 == 3){
		if (r > 15 || c < 2 || c > 8) return 0;
		if (matrix[r][c] == 0 && matrix[r-1][c] == 0 && matrix[r][c-1] == 0 && matrix[r+1][c] == 0){
			OnLedMatrix(r,c);
			OnLedMatrix(r-1,c);
			OnLedMatrix(r,c-1);
			OnLedMatrix(r+1,c);
			return 1;
		}
	}
	return 0;
}

//L

int genarate_L(int r, int c){
	if (RotateBlock  % 4 == 0) {
		if (r > 16 || c < 1 || c > 7) return 0;
		if (matrix[r][c] == 0 && matrix[r][c+1] == 0 && matrix[r-1][c] == 0 && matrix[r-2][c] == 0){
			OnLedMatrix(r,c);
			OnLedMatrix(r,c+1);
			OnLedMatrix(r-1,c);
			OnLedMatrix(r-2,c);
			return 1;
		}
	}
	else if (RotateBlock  % 4 == 1) {
		if (r > 15 || c < 1 || c > 6) return 0;
		if (matrix[r][c] == 0 && matrix[r][c+1] == 0 && matrix[r][c+2] == 0 && matrix[r+1][c] == 0){
			OnLedMatrix(r,c);
			OnLedMatrix(r,c+1);
			OnLedMatrix(r,c+2);
			OnLedMatrix(r+1,c);
			return 1;
		}
	}
	else if (RotateBlock  % 4 == 2) {
		if (r > 14 || c < 2 || c > 8) return 0;
		if (matrix[r][c] == 0 && matrix[r][c+1] == 0 && matrix[r+1][c] == 0 && matrix[r+2][c] == 0){
			OnLedMatrix(r,c);
			OnLedMatrix(r,c-1);
			OnLedMatrix(r+1,c);
			OnLedMatrix(r+2,c);
			return 1;
		}
	}
	else if (RotateBlock  % 4 == 3) {
		if (r > 15 || c < 3 || c > 8) return 0;
		if (matrix[r][c] == 0 && matrix[r][c-1] == 0 && matrix[r][c-2] == 0 && matrix[r+1][c] == 0){
			OnLedMatrix(r,c);
			OnLedMatrix(r,c-1);
			OnLedMatrix(r,c-2);
			OnLedMatrix(r-1,c);
			return 1;
		}
	}
	return 0;
}

int getBlock(int r, int c, int BlockType){
	if (BlockType == 1){
		 return genarate_T(r,c);
	}
	else if (BlockType == 2){
		 return genarate_Line(r,c);
	}
	else if (BlockType == 3){
		 return genarate_Box(r,c);
	}
	else if (BlockType == 4){
		 return genarate_L(r,c);
	}
}

void Cancel_BlockType(int r, int c, int BlockType){
		
//T
	if (BlockType == 1) {
		if (RotateBlock  % 4 == 0){
			matrix[r][c]   = 0;
			matrix[r][c-1] = 0;
			matrix[r][c+1] = 0;
			matrix[r-1][c] = 0;
		}
		else if (RotateBlock  % 4 == 1){
			matrix[r][c]   = 0;
			matrix[r-1][c] = 0;
			matrix[r][c+1] = 0;
			matrix[r+1][c] = 0;
		}
		else if (RotateBlock  % 4 == 2){
			matrix[r][c]   = 0;
			matrix[r][c-1] = 0;
			matrix[r][c+1] = 0;
			matrix[r+1][c] = 0;
		}
		else if (RotateBlock  % 4 == 3){
			matrix[r][c]   = 0;
			matrix[r-1][c] = 0;
			matrix[r][c-1] = 0;
			matrix[r+1][c] = 0;
		}
	}
	
	//line
	else if (BlockType == 2) {
		if (RotateBlock  % 2 == 0){
			matrix[r][c]   = 0;
			matrix[r][c-1] = 0;
			matrix[r][c+1] = 0;
		}
		else if (RotateBlock  % 2 == 1){
			matrix[r][c]   = 0;
			matrix[r-1][c] = 0;
			matrix[r+1][c] = 0;
		}
	}
	
	//box
	else if (BlockType == 3) {
		matrix[r][c]     = 0;
		matrix[r][c+1]   = 0;
		matrix[r-1][c]   = 0;
		matrix[r-1][c+1] = 0;
	}
	
	//L
	else if (BlockType == 4) {
		if (RotateBlock % 4 == 0){
			matrix[r][c]   = 0;
			matrix[r][c+1] = 0;
			matrix[r-1][c] = 0;
			matrix[r-2][c] = 0;
		}
		else if (RotateBlock % 4 == 1){
			matrix[r][c]   = 0;
			matrix[r][c+1] = 0;
			matrix[r][c+2] = 0;
			matrix[r+1][c] = 0;
		}
		else if (RotateBlock % 4 == 2){
			matrix[r][c]   = 0;
			matrix[r][c-1] = 0;
			matrix[r+1][c] = 0;
			matrix[r+2][c] = 0;
		}
		else if (RotateBlock % 4 == 3){
			matrix[r][c]   = 0;
			matrix[r][c-1] = 0;
			matrix[r][c-2] = 0;
			matrix[r-1][c] = 0;
		}
	}
	
}

//-----------------------------------------------

int main(void){
	
	ATMEGA_LCD = 0X00;
	DECODDER_2 = 0XFF;
	DDRD_2 = 0X11 ;               //0b00010001
	
	memset(matrix, 0, sizeof(matrix));        //memset(void *ptr, int x, size_t n);
	
	GICR =  1<<INT1 | 1<<INT0 | 1<<INT2;
	MCUCR = 1 <<ISC11 | 1 << ISC01;
	MCUCSR = 0x00;
	sei();
	srand(time(0));    // Initialize random number generator.
	while(1){
		if (Start_game==1 && Continue_game ==1){
			Game_Play();
		}
		else if(Start_game == 1 && Continue_game==0){
			/*if(getFinalScore==0){
				PORTD_DATA = 0X10;
				_delay_ms(70);
				PORTD_DATA = 0X00;
				getFinalScore =1;
			}*/
			
			uint8_t temp = (PINB & 0X01);
			if(temp==0X01){
				Start_game = 1;
				Continue_game= 1;
			}
		}
		else{
			// initial
			if(PINB==0X01){
				Start_game = 1;
				Continue_game= 1;
			}
		}
	}
}

//------------------------------------------------------------

void makeBOARD(){ 
	cli();
	int i,j,k,p;
	int valArr[16];

	for (p = 0; p < 16; p++){
		 valArr[p] = getRow(p+1);
	}
	for(k = 0; k < 180; k++){
     //K and j - speed
		for(i = 1; i <= 16; i++){
			int data = valArr[i-1];
			for (j = 1; j <= 10; j++){
				DECODDER_DATA = downValue[i-1];
				LED_DATA = data;
			}
		}
	}
	sei();
}

//---------------------------------------------------

int matrixfull(){
	if(BlockType==1 && (matrix[1][3]==1 || matrix[1][4]==1 || matrix[1][5]==1)){
		 return 1;
	}
	else if(BlockType==2 && (matrix[1][3]==1 || matrix[1][4]==1 || matrix[1][5]==1)){
		 return 1;
	}
	else if(BlockType==3 && (matrix[1][4]==1 || matrix[1][5]==1)){
		 return 1;
	}
	else if(BlockType==4 && (matrix[1][4]==1 || matrix[1][5]==1)){
		return 1;
	}
	
	else{
		 return 0;
	}
}

//------------------------------------------------------

void right_coner(){  // change into left------> report code
	
	//T
	
	if (BlockType == 1){
		if (RotateBlock % 4 == 0 && Now_PosiY < 2){
		 Now_PosiY = 2;
		}
		else if (RotateBlock % 4 == 1 && Now_PosiY < 1){
			 Now_PosiY = 1;
		}
		else if (RotateBlock % 4 == 2 && Now_PosiY < 2){
			 Now_PosiY = 2;
		}
		else if (RotateBlock % 4 == 3 && Now_PosiY < 2){
			 Now_PosiY= 2;
		}
	}
	
	//Line
	
	else if (BlockType == 2){
		if (RotateBlock % 2 == 0 && Now_PosiY < 2){
			 Now_PosiY = 2;
		}
		else if (RotateBlock % 2 == 1 && Now_PosiY < 1){
			 Now_PosiY = 1;
		}
	}
	
	//Box
	
	else if (BlockType == 3){
		if (Now_PosiY < 1){
			 Now_PosiY= 1;
		}
	}
	
	//L
	
	else if (BlockType == 4){
		if (RotateBlock % 4 == 0 && Now_PosiY < 1){
		 Now_PosiY = 1;
		}
		else if (RotateBlock % 4 == 1 && Now_PosiY < 1){
			 Now_PosiY = 1;
		}
		else if (RotateBlock % 4 == 2 && Now_PosiY < 2){
			 Now_PosiY = 2;
		}
		else if (RotateBlock % 4 == 3 && Now_PosiY < 3){
			 Now_PosiY = 3;
		}
	}
}

void left_coner(){  // change into right ------> report code
	
	//T
	
	if (BlockType == 1){
		if (RotateBlock % 4 == 0 && Now_PosiY > 7){
			 Now_PosiY= 7;
		}
		else if (RotateBlock % 4 == 1 && Now_PosiY> 7){
			 Now_PosiY = 7;
		}
		else if (RotateBlock % 4 == 2 && Now_PosiY > 7){
			 Now_PosiY= 7;
		}
		else if (RotateBlock % 4 == 3 && Now_PosiY > 8){
			 Now_PosiY = 8;
		}
	}
	
	//Line
	
	else if (BlockType == 2){
		if (RotateBlock % 2 == 0 && Now_PosiY > 7){
			 Now_PosiY = 7;
		}
		else if (RotateBlock % 2 == 1 && Now_PosiY > 8){
			 Now_PosiY = 8;
		}
	}
	
	//Box
	
	else if (BlockType == 3){
		if (Now_PosiY > 7){
			 Now_PosiY = 7;
		}
	}
	
	//L
	
	else if (BlockType == 4){
		if (RotateBlock % 4 == 0 && Now_PosiY> 7){
			 Now_PosiY = 7;
		}
		else if (RotateBlock % 4 == 1 && Now_PosiY > 6){
			 Now_PosiY = 6;
		}
		else if (RotateBlock % 4 == 2 && Now_PosiY > 8){
			 Now_PosiY = 8;
		}
		else if (RotateBlock % 4 == 3 && Now_PosiY > 8){
			 Now_PosiY = 8;
		}
	}
}

int clearMatchedRow(){
	int i,j,k,p;
	int OVER = 0;

	for (i = 16; i >= 1; i--){
		if (getRow(i) == 255) {
			for (j = i-1; j >= 1; j--){   //***
				for (k = 1; k <= 8; k++){
					matrix[j+1][k] = matrix[j][k];
				}
			}
			for (p = 1; p <= 8; p++){
				matrix[1][p] = 0;
			}
			
			i++;                     //get score
			OVER = 1;
			 PORTD_DATA = 0X01;      //0b00000001
			_delay_ms(70);
			PORTD_DATA = 0X00;       //0b00000000
		}
	}
	return OVER;
}

void Game_Play(){

if(matrixfull()){
		memset(matrix,0,sizeof(matrix));
		makeBOARD();
		Continue_game=0;
		return;
	}
	
	if (Now_PosiX == 1) {
		BlockType = (rand() % 4) + 1;  
		RotateBlock=0;
	}
	
	if (getBlock(Now_PosiX, Now_PosiY, BlockType) == 0) {
		getBlock(After_PosiX, Now_PosiY, BlockType);
		stop_Block  = 1;
	}
	After_PosiX = Now_PosiX;

	makeBOARD();

	if(stop_Block  == 0) {
		Cancel_BlockType(Now_PosiX, Now_PosiY, BlockType);
		Now_PosiX++;
	}
	else {
		Now_PosiX = 1;
		Now_PosiY = 4;
		stop_Block  = 0;
	}

	if (clearMatchedRow()){
		 makeBOARD();
	}
}

//---------------------------------------------------------

ISR(INT0_vect){  ///left shift
	
	Cancel_BlockType(Now_PosiX, Now_PosiY, BlockType);
	After_PosiY = Now_PosiY;

	Now_PosiY++;
	if (getBlock(Now_PosiX,Now_PosiY,BlockType) == 0) {
		Now_PosiY = After_PosiY;
		getBlock(Now_PosiX,Now_PosiY,BlockType);
	}
}

ISR(INT1_vect){

	Cancel_BlockType(Now_PosiX, Now_PosiY, BlockType);
	After_PosiY= Now_PosiY;

	Now_PosiY--;
	
	if (getBlock(Now_PosiX,Now_PosiY,BlockType) == 0) {
		Now_PosiY= After_PosiY;
		
		getBlock(Now_PosiX,Now_PosiY,BlockType);
	}
}

ISR(INT2_vect){
	
	Cancel_BlockType(Now_PosiX,Now_PosiY,BlockType);
	RotateBlock = (RotateBlock + 1)%4;
	getBlock(Now_PosiX,Now_PosiY,BlockType);
}