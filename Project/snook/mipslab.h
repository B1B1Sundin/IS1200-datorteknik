/* mipslab.h
   Header file for all labs.
   This file written 2015 by F Lundevall
   Some parts are original code written by Axel Isaksson

   Latest update 2015-08-28 by F Lundevall

   For copyright and licensing, see file COPYING */

/* Declare display-related functions from mipslabfunc.c */

/* Used for the #define DIRECTIONS*/
#define UP 0
#define RIGHT 1
#define LEFT 2
#define DOWN 3

#define HEIGHT 32  // 32 pixels (the height of the screen)
#define LENGTH 128 // 128 pixels (the length of the screen)

/* Declare lab-related functions from mipslabfunc.c */
void display_init(void);
void display_string(int line, char *s);
void display_update(void); // modified display_update() used from labs
void labinit();

void labwork(void);

/* Declare bitmap array containing font NOT USED*/
extern const uint8_t const font[128 * 8];
/* Declare text buffer for display output NOT USED */
extern char textbuffer[4][16];

/* Written as part of asm lab: delay, time2string */
void time2string(char *, int);
/* Written as part of i/o lab: getbtns, getsw, enable_interrupt */
int getbtns(void);
void tick(unsigned int *timep);
int nextprime(int inval);

/**
 * Our own defined struct for coordinates.
 * Used for the screen, snake and food.
 */
typedef struct Coord
{
   int x_pos;
   int y_pos;
} Coord;

extern uint8_t pixels[32][128];  // All pixels(coordinates) for the screen
extern Coord snakeBody_pos[200]; // The coordinates for the snake

void clear_screen();
void start_values();
void updateMovement_snake();
void remove_last();
void frame_update();

void enable_interrupt(void);