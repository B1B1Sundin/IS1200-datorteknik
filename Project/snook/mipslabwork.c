/* mipslabwork.c

   This file written 2015 by F Lundevall
   Updated 2017-04-21 by F Lundevall
   This file should be changed by YOU! So you must
   add comment(s) here with your name(s) and date(s):

   This file modified 2017-04-31 by Ture Teknolog 

   Modified by Jonathan Hansson and Yohanna Sundin 2022-03-03

   For copyright and licensing, see file COPYING */

#include <stdint.h>   /* Declarations of uint_32 and the like */
#include <pic32mx.h>  /* Declarations of system-specific addresses etc */
#include "mipslab.h"  /* Declatations for these labs */
#define PERIOD ((80000000 / 256) / 10)

volatile int *portE = (volatile int *)0xbf886110;
volatile int *trisE = (volatile int *)0xbf886100;

int TMR2counter = 0x0;  //used for interrupts in timer
int direction = RIGHT; // the current direction of the snake
int snake_size;     //length of snake body
unsigned short lfsr = 0xACE1u; //used for random number generator
unsigned bit;

Coord food_pos;           //coordinates for food
Coord snakeBody_pos[200]; // an array with the cooordinates for the snakes body


// Clears pixels of the display previous frame
void clear_screen()
{
  int i, j;
  for (i = 1; i < HEIGHT - 1; i++)
  {
    for (j = 1; j < LENGTH - 1; j++)
    {
      pixels[i][j] = 0;
    }
  }
}
/*
Values for start of new game
*/
void start_values()
{
  snakeBody_pos[0].x_pos = 34;
  snakeBody_pos[0].y_pos = 16;

  snakeBody_pos[1].x_pos = 33;
  snakeBody_pos[1].y_pos = 16;

  snakeBody_pos[2].x_pos = 32;
  snakeBody_pos[2].y_pos = 16;

  snakeBody_pos[3].x_pos = 31;
  snakeBody_pos[3].y_pos = 16;

  snakeBody_pos[4].x_pos = 30;
  snakeBody_pos[4].y_pos = 16;

  snakeBody_pos[5].x_pos = 29;
  snakeBody_pos[5].y_pos = 16;

  snakeBody_pos[6].x_pos = 28;
  snakeBody_pos[6].y_pos = 16;

  snakeBody_pos[7].x_pos = 27;
  snakeBody_pos[7].y_pos = 16;

  snakeBody_pos[8].x_pos = 26;
  snakeBody_pos[8].y_pos = 16;

  snakeBody_pos[9].x_pos = 25;
  snakeBody_pos[9].y_pos = 16;

  snakeBody_pos[10].x_pos = 24;
  snakeBody_pos[10].y_pos = 16;

  snakeBody_pos[11].x_pos = 23;
  snakeBody_pos[11].y_pos = 16;

  snakeBody_pos[12].x_pos = 22;
  snakeBody_pos[12].y_pos = 16;

  snakeBody_pos[13].x_pos = 21;
  snakeBody_pos[13].y_pos = 16;

  snakeBody_pos[14].x_pos = 20;
  snakeBody_pos[14].y_pos = 16;

  snakeBody_pos[15].x_pos = 19;
  snakeBody_pos[15].y_pos = 16;

  snakeBody_pos[16].x_pos = 18;
  snakeBody_pos[16].y_pos = 16;

  snakeBody_pos[17].x_pos = 17;
  snakeBody_pos[17].y_pos = 16;

  food_pos.x_pos = 96;
  food_pos.y_pos = 16;
  snake_size = 18;
  *portE = 0x0;
  
  //makes entire screen white at start of new game
  int i, j;
  for (j = 0; j < 128; j++)
  {
    for (i = 0; i < 32; i++)
    {
      pixels [i][j] = 1;
    }
  }
}


/*
random number generator for food_randomizer
*/
unsigned random_food()
{
  bit = ((lfsr >> 0) ^ (lfsr >> 2) ^ (lfsr >> 3) ^ (lfsr >> 5)) & 1;
  return lfsr = (lfsr >> 1) | (bit << 15);
}

/*
spawns new food at random location and makes sure it doesnt spawn in snake body
if food spawns in snake or in or too close to the edge it spawns a new one instead
*/
int food_randomizer()
{
  int food_x = random_food() % 123 + 2 ;
  int food_y = random_food() % 29 + 2;
  int i;
  for (i = 0; i < snake_size; i++)
  {
    if (food_x == snakeBody_pos[i].x_pos && food_y == snakeBody_pos[i].y_pos)
      {
      if (food_x <= 2 || food_x >= 125 || food_y <= 2 || food_y >= 29) 
      food_randomizer();
      }
  }

  food_pos.x_pos = food_x;
  food_pos.y_pos = food_y;
}

//check if food is taken, adds to score and body length, spawns new food
void food_eat(Coord next_pos)
{
  if (next_pos.x_pos == food_pos.x_pos && next_pos.y_pos == food_pos.y_pos)
  {
    snake_size++;
    *portE += 1;
    food_randomizer();
  }
}

/*
Rearranges data for snake movement
Moves position values up one step in the array to make room for the new "head"/index 0 in array
*/
void updateMovement_snake()
{
  int i;
  for (i = snake_size - 1; i >= 0; i--)
  {
    snakeBody_pos[i + 1] = snakeBody_pos[i];
  }
}

/*
Checks if snake "head" collides with a wall or itself
Returns 1 if snake head is in a position where it will die
*/
int collision(struct Coord next_pos)
{
  int i;
  for (i = 1; i < snake_size; i++)
  {
    if (next_pos.x_pos == snakeBody_pos[i].x_pos && next_pos.y_pos == snakeBody_pos[i].y_pos)
      return 1;
  }

  if ((next_pos.x_pos == 127 || next_pos.x_pos == 0) || (next_pos.y_pos == 31 || next_pos.y_pos == 0))
  {
    return 1;
  }
  return 0;
}

//Removes last position in array for snake body so it moves forward
void removelast()
{
  snakeBody_pos[snake_size].x_pos = 0;
  snakeBody_pos[snake_size].y_pos = 0;
}

/*
Updates one frame

Creates copy of "head" coordinates and adds or subtracts 1 to x or y coordinates depending on direction for snake movement
Calls collision() and resets game if dead
Calls food_eat() to see if food was eaten
Updates snake movement
Lights up pixels for current frame and clears pixel for tail of previous frame
*/
void frame_update()
{
  Coord next_pos;
  next_pos.x_pos = snakeBody_pos[0].x_pos;
  next_pos.y_pos = snakeBody_pos[0].y_pos;

  switch (direction)
  {
  case RIGHT:
    next_pos.x_pos++; // pixels[][++]
    break;
  case LEFT:
    next_pos.x_pos--; // pixels[][--]
    break;
  case UP:
    next_pos.y_pos--; // pixels[--][]
    break;
  case DOWN:
    next_pos.y_pos++; // pixels[++][]
    break;
  }

  if (collision(next_pos) == 1)
  {
    clear_screen();
    start_values();
    direction = RIGHT;
    goto dead;
  }

  food_eat(next_pos);
  updateMovement_snake();
  removelast();

  snakeBody_pos[0].x_pos = next_pos.x_pos;
  snakeBody_pos[0].y_pos = next_pos.y_pos;
  

  int i;
  for (i = 0; i < snake_size; i++)
  {
    pixels[snakeBody_pos[i].y_pos][snakeBody_pos[i].x_pos] = 1;
  }
  pixels[food_pos.y_pos][food_pos.x_pos] = 1;

  pixels[snakeBody_pos[snake_size].y_pos][snakeBody_pos[snake_size].x_pos] = 0;

  dead:
  return;
}

/* 
Modified interrupt Service Routine with implemented game logic
Delays game loop and calls clear screen and frame update functions
when timer counter hits 2 a new frame is shown
 */
void user_isr( void )
{
  
  if ((IFS(0) & 0x100)) // Check bit 8 in IFS(0), if 1 = Interrupt request has occurred, 0 = No interrupt request has occurred
  {
    IFSCLR(0) = 0x100; // Clear the bit 8 which represents the timer interrupt status flag
    TMR2counter++;
    if (TMR2counter == 2)
    {
      switch (getbtns())
        {
        case 0x0:
          break;
        case 0x1:
          if (direction != LEFT)
          direction = RIGHT;
          break;
        case 0x2:
          if (direction != UP)
          direction = DOWN;
          break;
        case 0x4:
          if (direction != DOWN)
          direction = UP;
          break;
        case 0x8:
          if (direction != RIGHT)
          direction = LEFT;
          break;
        }

      clear_screen();
      frame_update();
      TMR2counter = 0;
    }
    
  }
  
  return;
}

/* Lab-specific initialization goes here. Intitialize port E, bits 0-7, set S outputs. */
void labinit(void)
{
   //initialize led to zero
    *trisE &= 0xffffff00;
    
    //initialize port to zero
    *portE &= 0xffffff00;
    *portE = 0x0;

    TRISFSET = 0x02;// F port lies on bit 2
    
    TRISDSET = 0x0fe0; // bits 5 - 11
    
    //TimerIntz
    T2CON = 0x0;    //Stops Timer2 and clear control register
    TMR2 = 0x0;     //Clear contents of TMR2
    PR2 = 0x7a12;   //Load PR2 register
    T2CONSET = 0x8070;//
    
    IFSCLR(0);      //Clear the interuption flag
    
    IEC(0) = IEC(0) | 0x100;
    IPC(2) = IPC(2) | 0x1F;

    enable_interrupt(); //Enables interrupts globally
    
    return;
}



/* This function is called repetitively from the main program */
void labwork( void )
{
  display_update();
}

