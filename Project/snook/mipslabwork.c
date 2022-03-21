/* mipslabwork.c

   This file written 2015 by F Lundevall
   Updated 2017-04-21 by F Lundevall
   This file should be changed by YOU! So you must
   add comment(s) here with your name(s) and date(s):

   This file modified 2017-04-31 by Ture Teknolog 

   Modified by Jonathan Hansson and Yohanna Sundin 2022-03-03

   For copyright and licensing, see file COPYING */

#include <stdint.h>                      /* Declarations of uint_32 and the like */
#include <pic32mx.h>                     /* Declarations of system-specific addresses etc */
#include "mipslab.h"                     /* Declatations for these labs */
#define PERIOD ((80 000 000 / 256) / 10) /* 80 MHz maximum frequency/prescale/timeout(wrap) */

volatile int *portE = (volatile int *)0xbf886110;
volatile int *trisE = (volatile int *)0xbf886100;

int TMR2counter = 0x0;         // Used to count occured interrupts
int direction = RIGHT;         // The current direction of the snake
int snake_size;                // The length of snake body
unsigned short lfsr = 0xACE1u; // Used for random number generator
unsigned bit;

Coord food_pos;           // Coordinates for food
Coord snakeBody_pos[200]; // An array with the cooordinates for the snakes body

/**
 * Clears pixels of the display.
 * Used to clear the previous frame.
 */
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

/**
 * Starting values and coordinates for the game
 * 
 */
void start_values()
{
  food_pos.x_pos = 96;
  food_pos.y_pos = 16;
  snake_size = 18;
  *portE = 0x0;

  // Initalize coords for snake body. [17][16] -  [34][16]
  int k;
  int head_start = 34;
  for (k = 0; k < snake_size; k++)
  {
    snakeBody_pos[k].x_pos = head_start - k;
    snakeBody_pos[k].y_pos = 16;
  }

  // Sets screen, whole screen turns white at start of new game
  int i, j;
  for (j = 0; j < 128; j++)
  {
    for (i = 0; i < 32; i++)
    {
      pixels[i][j] = 1;
    }
  }
}

/**
 * Random number generator.
 * Used for food_randomizer()
 */
unsigned random_food()
{
  bit = ((lfsr >> 0) ^ (lfsr >> 2) ^ (lfsr >> 3) ^ (lfsr >> 5)) & 1;
  return lfsr = (lfsr >> 1) | (bit << 15);
}

/**
 * Spawns new food at random location. 
 * Checks if food spawns in the body of the snake or too close to the edge of the screen.
 * CHANGE function!! Move out the inner if loop and make it independently
 */
int food_randomizer()
{
  int food_x = random_food() % 123 + 2;
  int food_y = random_food() % 29 + 2;
  int i;
  for (i = 0; i < snake_size; i++)
  {
    if (food_x == snakeBody_pos[i].x_pos && food_y == snakeBody_pos[i].y_pos)
    {
      if (food_x <= 2 || food_x >= 125 || food_y <= 2 || food_y >= 29)
        food_randomizer();
    }
    /** SUGGESTED SOLUTION (test first with chipkit)
     * if (food_x == snakeBody_pos[i].x_pos && food_y == snakeBody_pos[i].y_pos) {
     *    food_randomizer();
     *   } 
     * else if (food_x <= 2 || food_x >= 125 || food_y <= 2 || food_y >= 29){
     *    food_randomizer();
     * }
     */
  }
  food_pos.x_pos = food_x;
  food_pos.y_pos = food_y;
}

/**
 * Checks if the snake will eat the food in the next coordinates.
 * Increases the length of the snake and add 1 to game score.
 */
void food_eat(Coord next_pos)
{
  if (next_pos.x_pos == food_pos.x_pos && next_pos.y_pos == food_pos.y_pos)
  {
    snake_size++;
    *portE += 1;
    food_randomizer();
  }
}

/**
 * Updates the movement of the snake. 
 * Moves elements(coordinates) up one step in the array 
 * to make room for the new "head" at index 0.
 */
void updateMovement_snake()
{
  int i;
  for (i = snake_size - 1; i >= 0; i--)
  {
    snakeBody_pos[i + 1] = snakeBody_pos[i];
  }
}

/**
 * Checks if the snake head will collide, 
 * either with a wall or itself in the next coordinate.
 * @param next_pos 
 * @return int, 1 if it will collide, 0 for not.
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

/**
 * Removes the "tail" in the snake body array. 
 * The index for tail is the same as snake_size.
 */
void removelast()
{
  snakeBody_pos[snake_size].x_pos = 0;
  snakeBody_pos[snake_size].y_pos = 0;
}

/**
 * Updates one frame of the game play
 * Creates a copy of "head" coordinates named next_pos. 
 * Depending on direction, we will add/subtract the x or y coordinate.
 * Calls collision(), if returned true the game will reset. 
 * Calls food_eat(). 
 * Updates the movement of the snake, using updateMovement_snake() and removeLast().
 * Sets snake head coord to next_pos coord.
 * Lights up pixels for current frame coords and clears pixel for tail of previous frame
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

/**
 * Modified interrupt Service Routine with implemented game logic.
 * Delays game loop, calls clear screen and frame update functions.
 * When timer counter hits "2", a new frame is shown.
 */
void user_isr(void)
{
  /**
   * Check bit 8 in IFS(0), which represents the timer interrupt status flag.
   * 1 = Interrupt request has occurred, 
   * 0 = No interrupt request has occurred
   */
  if ((IFS(0) & 0x100))
  {
    IFSCLR(0) = 0x100;
    TMR2counter++;

    if (TMR2counter == 2)
    {
      /**
       * Player can't choose the opposite direction of current direction
       */
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

/**
 * Lab-specific initialization goes here. 
 * Intitialize register and timer.
 */
void labinit(void)
{
  *trisE &= 0xffffff00; // Initialize led to zero
  *portE &= 0xffffff00;
  *portE = 0x0; // Reset value of portE

  TRISFSET = 0x02;   // The portF(btn1) is on bit 2
  TRISDSET = 0x0fe0; // The portE(btn4-2) is on bits 11:5

  //TimerIntz
  T2CON = 0x0;       // Stops Timer2 and clears control register
  TMR2 = 0x0;        // Clear contents of TMR2
  PR2 = 0x7a12;      // Load PR2 register // CHANGE TO #DEFINE PERIOD
  T2CONSET = 0x8070; //

  IFSCLR(0); // Clear the interuption flag

  IEC(0) = IEC(0) | 0x100; // enable interrupt flag
  IPC(2) = IPC(2) | 0x1F;  // set priority to 111(7)

  enable_interrupt(); //Enables interrupts globally

  return;
}

/**
 * This function is called repetitively from the main program
 * Constantly updates the game.
 */
void labwork(void)
{
  display_update();
}
