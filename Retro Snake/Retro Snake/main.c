/*
 * Created: 02-Aug-22 10:39:22 AM
*/ 


#define F_CPU 1000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <avr/interrupt.h>
#include "lcd.h"

extern void lcd_backlight(char on);

const int N = 300;
const int NumberOfLevel = 4;
const int ScoreLevelChange = 2;
int pos[300][2] = {{2, 8}, {1, 8}};
int dir = 0;
int dr[] = {-1, 0, 1, 0};
int dc[] = {0, 1, 0, -1};
int snake_tail = 0, snake_head = 1;	
int in_game = 1, score = 0;
int food[2] = {-1, -1};
int buzzer_cnt = 0;
unsigned char buzzer_mask = 1 << 6;
int input0 = 0, input1 = 0, input_delay = 0;
int move_cnt = 0, a = 0, mode = 0, level = 0;
int b  = 0;
int highscore[2] = {0, 0};
	
unsigned char obstacles1[10][2][8] = {
	{{0x00,0x40,0x40,0x00,0x00,0x00,0x00,0x00}, {0x00,0x02,0x02,0x00,0x00,0x00,0x00,0x00}},
	{{0x00,0x00,0x20,0x20,0x20,0x20,0x00,0x00}, {0x00,0x00,0x04,0x04,0x04,0x04,0x00,0x00}},	
	{{0x00,0x70,0x40,0x40,0x00,0x00,0x00,0x00},	{0x00,0x0e,0x02,0x02,0x00,0x00,0x00,0x00}},
	{{0x00,0x00,0x24,0x24,0x24,0x24,0x24,0x24}, {0x00,0x00,0x24,0x24,0x24,0x24,0x24,0x24}}	
};

unsigned char obstacles2[10][2][8] = {
	{{0x00,0x00,0x00,0x00,0x00,0x40,0x40,0x00}, {0x00,0x00,0x00,0x00,0x00,0x02,0x02,0x00}},
	{{0x00,0x00,0x20,0x20,0x20,0x20,0x00,0x00}, {0x00,0x00,0x04,0x04,0x04,0x04,0x00,0x00}},
	{{0x00,0x00,0x00,0x00,0x40,0x40,0x70,0x00}, {0x00,0x00,0x00,0x00,0x02,0x02,0x0e,0x00}},
	{{0x24,0x24,0x24,0x24,0x24,0x24,0x24,0x24}, {0x24,0x24,0x24,0x24,0x24,0x24,0x24,0x24}}
};
unsigned char s_grid1[8][2] = {
	{0b00000000, 0b00000000},
	{0b00000000, 0b00000000},
	{0b00000000, 0b00000000},
	{0b00000000, 0b00000000},
	{0b00000000, 0b00000000},
	{0b00000000, 0b00000000},
	{0b00000000, 0b00000000},
	{0b00000000, 0b00000000},
};


unsigned char s_grid2[8][2] = {
	{0b00000000, 0b00000000},
	{0b00000000, 0b00000000},
	{0b00000000, 0b00000000},
	{0b00000000, 0b00000000},
	{0b00000000, 0b00000000},
	{0b00000000, 0b00000000},
	{0b00000000, 0b00000000},
	{0b00000000, 0b00000000},
};


unsigned char o_grid1[8][2] = {
	{0b00000000, 0b00000000},
	{0b00000000, 0b00000000},
	{0b00000000, 0b00000000},
	{0b00000000, 0b00000000},
	{0b00000000, 0b00000000},
	{0b00000000, 0b00000000},
	{0b00000000, 0b00000000},
	{0b00000000, 0b00000000},
};


unsigned char o_grid2[8][2] = {
	{0b00000000, 0b00000000},
	{0b00000000, 0b00000000},
	{0b00000000, 0b00000000},
	{0b00000000, 0b00000000},
	{0b00000000, 0b00000000},
	{0b00000000, 0b00000000},
	{0b00000000, 0b00000000},
	{0b00000000, 0b00000000},
};

void draw(){
	for(int i = 0; i < 8; i++){
		PORTC = (i << 2); 
		PORTB = s_grid1[i][1];
		PORTA = s_grid1[i][0];
		_delay_ms(1);
	}
	
	for(int i = 0; i < 8; i++){
		PORTC = ((i + 8) << 2);
		PORTB = s_grid2[i][1];
		PORTA = s_grid2[i][0];
		
		_delay_ms(1);
	}
	
	
	for(int i = 0; i < 8; i++){
		PORTC = ((i + 16) << 2);
		PORTB = o_grid1[i][1];
		PORTA = o_grid1[i][0];
		
		_delay_ms(1);
	}
	
	for(int i = 0; i < 8; i++){
		PORTC = ((i + 24) << 2);
		PORTB = o_grid2[i][1];
		PORTA = o_grid2[i][0];
		
		_delay_ms(1);
	}
	
}


int check_snake_obstacle(int x, int y){
	if(x < 8){
		if(y < 8){
			if(s_grid1[x][0] >> (7 - y) & 1)return 1;
			if(o_grid1[x][0] >> (7 - y) & 1)return 1;
		}
		else {
			if(s_grid1[x][1] >> (15 - y) & 1)return 1;
			if(o_grid1[x][1] >> (15 - y) & 1)return 1;
		}
	}
	else{
		if(y < 8){
			if(s_grid2[x -8][0] >> (7 - y) & 1)return 1;
			if(o_grid2[x -8][0] >> (7 - y) & 1)return 1;
		}
		else {
			if(s_grid2[x - 8][1] >> (15 - y) & 1)return 1;
			if(o_grid2[x - 8][1] >> (15 - y) & 1)return 1;
		}
	}
	return 0;
}

void update_food_pos(int x, int y){
	if(x < 8){
		if(y < 8){
			o_grid1[x][0] ^= 1 << (7 - y);
			s_grid1[x][0] ^= 1 << (7 - y);
			
		}
		else {
			o_grid1[x][1] ^= 1 << (15 - y);
			s_grid1[x][1] ^= 1 << (15 - y);
			
		}
	}
	else{
		if(y < 8){
			o_grid2[x - 8][0] ^= 1 << (7 - y);
			s_grid2[x - 8][0] ^= 1 << (7 - y);
			
		}
		else {
			o_grid2[x - 8][1] ^= 1 << (15 - y);
			s_grid2[x - 8][1] ^= 1 << (15 - y);
			
		}
	}
}
void create_food(){
	int t = 0;
	int x = 0, y = 0;
	while(++t < 10){
		x = rand() & 15;
		y = rand() & 15;
		if(!check_snake_obstacle(x, y) && !(x == food[0] && y == food[1])){
			food[0] = x;
			food[1] = y;
			update_food_pos(x, y);
			return;
		}
	}
	
	
	for(int i = 0; i < 16; i++){
		for(int j = 0; j < 16; j++){
			if(!check_snake_obstacle(i, j) && !(i == food[0] && j == food[1])){
				food[0] = i;
				food[1] = j;
				update_food_pos(i, j);
				return;
			}
		}
	}
	
	//ashbe na
	
}
int is_food(int x, int y){
	if(x == food[0] && y == food[1]){
		update_food_pos(x, y);
		create_food();
		return 1;	
	}
	return 0;
}

void grid_clear(){
	for(int i = 0; i < 8; i++){
		s_grid1[i][0] = 0;
		s_grid1[i][1] = 0;
		s_grid2[i][0] = 0;
		s_grid2[i][1] = 0;
		o_grid1[i][0] = 0;
		o_grid1[i][1] = 0;
		o_grid2[i][0] = 0;
		o_grid2[i][1] = 0;
	}
	food[0] = food[1] = -1;
	snake_tail = 0;
	snake_head = -1;
	
}

void modeSelect(){
	grid_clear();
	lcd_clrscr();
	lcd_gotoxy(0, 0);
	char s[16] = "Classic: Right";
	lcd_puts(s);
	lcd_gotoxy(0, 1);
	char h[16] = "Obstacle: Left";
	lcd_puts(h);
}

void scoreDisplay(){
	
	lcd_clrscr();
	lcd_gotoxy(0, 0);
	char s[16] = "Score: ";
	itoa(score, s + 7, 10);
	lcd_puts(s);
	lcd_gotoxy(0, 1);
	char h[16] = "HighScore: ";
	itoa(highscore[mode], h + 11, 10);
	lcd_puts(h);
}

void game_over(){
	scoreDisplay();
	_delay_ms(5000);
	in_game = 0;
	modeSelect();
}
int update_grid(int x, int y, int rem){
	if(rem == 0 && check_snake_obstacle(x, y))return 1;
	if(x < 8){
		if(y < 8){
			s_grid1[x][0] ^= 1 << (7 - y);
		
		}
		else {
			s_grid1[x][1] ^= 1 << (15 - y);
			
		}
	}
	else{
		if(y < 8){
			s_grid2[x - 8][0] ^= 1 << (7 - y);
		}
		else {
			s_grid2[x - 8][1] ^= 1 << (15 - y);
		}
	}
	return 0;
}
void buzzer_start(){
	if(buzzer_cnt == 0)PORTD ^= buzzer_mask;
	buzzer_cnt = 1;
}
void buzzer(){
	if(buzzer_cnt == 0)return;
	if(buzzer_cnt == 5){
		PORTD ^= buzzer_mask;
		buzzer_cnt = 0;
		return;
	}
	buzzer_cnt++;
}

void create_obstacle(){
	for(int i = 0; i < 8; i++){
		o_grid1[i][0] = obstacles1[level][0][i];
		o_grid1[i][1] = obstacles1[level][1][i];
		o_grid2[i][0] = obstacles2[level][0][i];
		o_grid2[i][1] = obstacles2[level][1][i];
	}
}

void game_start(){
	grid_clear();
	if(mode == 0 || level == 0)score = 0;
	scoreDisplay();
	dir = 1;
	move_cnt = 0;
	pos[++snake_head][0] = 3;
	pos[snake_head][1] = 0;
	
	update_grid(3, 0, 0);
	
	
	pos[++snake_head][0] = 2;
	pos[snake_head][1] = 0;
	update_grid(2, 0, 0);
	
	pos[++snake_head][0] = 1;
	pos[snake_head][1] = 0;
	update_grid(1, 0, 0);
	
	
	pos[++snake_head][0] = 0;
	pos[snake_head][1] = 0;
	update_grid(0, 0, 0);
	
	if(mode){
		create_obstacle();
	}
	
	create_food();
	
	
	in_game = 1;
	
}
void make_move(){
	int nw_head1 = pos[snake_head][0];
	int nw_head2 = pos[snake_head][1];
	
	nw_head1 += dr[dir];
	nw_head2 += dc[dir];
	
	if(nw_head1 < 0)nw_head1 += 16;
	if(nw_head2 < 0)nw_head2 += 16;
	if(nw_head1 >= 16)nw_head1 -= 16;
	if(nw_head2 >= 16)nw_head2 -= 16;
	
	if(snake_head == 299)snake_head = 0;
	else snake_head = snake_head + 1;
	pos[snake_head][0] = nw_head1;
	pos[snake_head][1] = nw_head2;
	
	int notun_level = 0;
	if(!is_food(nw_head1, nw_head2)){
		update_grid(pos[snake_tail][0], pos[snake_tail][1], 1);
		if(snake_tail == 299)snake_tail = 0;
		else snake_tail = snake_tail + 1;
	}else{
		buzzer_start();
		score++;
		if(mode == 1 && score % ScoreLevelChange == 0){
			if(level + 1 != NumberOfLevel)
				notun_level = 1;
			
			level = (level + 1 == NumberOfLevel ? level : level + 1);
			
			if(notun_level)game_start();
		}
		if(score > highscore[mode])highscore[mode] = score;
		scoreDisplay();
		
	}
	
	
	if(notun_level == 0 && update_grid(nw_head1, nw_head2, 0)){
		game_over();
		return;
	}
}

void move_right(){
	dir = (dir == 3 ? 0 : dir + 1);
}

void move_left(){
	dir = (dir == 0 ? 3 : dir - 1);
}


void select(int selectedMode){
	mode = selectedMode;
	level = 0;
	game_start();
}

void check(){
	if(input_delay)input_delay--;
	if(input_delay == 0){
		if((PIND & 1) == 0){
			if(input0){
				if(in_game)move_left();
				else select(1);
				input_delay = 10;
			}
		}
		if((PIND & 2) == 0){
			if(input1){
				if(in_game)move_right();
				else select(0);
				input_delay = 10;
			}
		}
	}
	
	input0 = PIND & 1;
	input1 = PIND & 2;
	
}
int main(void)
{
	
	lcd_init(LCD_ON_DISPLAY);
	lcd_backlight(0);
	_delay_ms(200);
	lcd_backlight(1);
	_delay_ms(200);
	
	DDRC = 0xFF;
	DDRB = 0xFF;
	DDRA = 0xFF;
	DDRD = 0x40;
	
    /* Replace with your application code */
	srand(time(0));
	in_game = 0;
	modeSelect();
	MCUSR = 1 << JTD;
	MCUSR = 1 << JTD;
	MCUSR = 1 << JTD;
	
	while (1) 
    {
		buzzer();
		check();
		if(in_game){
			draw();
			if(move_cnt == 10){
				make_move();//, a++;
				/*if(a == 15){
					if(b == 0)move_right();
					else move_left();
				}
				if(a == 16){
					if(b == 0)move_right();
					else move_left();
					a = 0;
					b ^= 1;
				}
				*/
				move_cnt = 0;
			}
			move_cnt++;
		}
    }
}

