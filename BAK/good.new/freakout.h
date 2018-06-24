// freakout.h - Header file for demo game

// watch for multiple inclusions
#ifndef __FREAKOUT_H
#define __FREAKOUT_H

#include <windef.h>  // NEED THIS FOR 'HWND' TYPE IN FUNCTION CALLS

// DEFINES & CONSTANTS ///////////////////////////////////////////

typedef unsigned char UCHAR;

/* block parameters - these must be const in order to use
   them to set the blocks array */
const int NUM_BLOCK_ROWS  = 10 , // 8 , 
          NUM_BLOCK_COLS  = 16 ; //10 ; 

//////////////
class Freak
{
 public:

  int game_state ,
      paddle_x , paddle_prev_x ; // track position of paddle

  int ball_x , ball_y , // track position of ball
      ball_dx , ball_dy ; // velocity of ball

  int score ,          // the score
      level ;          // the current level

  int blocks_hit ,     // track number of blocks hit
      misses ;	       // number of times ball goes by paddle

  int high_streak ,    // keep track of consecutive blocks hit
      current_streak ;

  // this contains the game grid data   
  UCHAR blocks[NUM_BLOCK_ROWS][NUM_BLOCK_COLS];     


// FUNCTION PROTOTYPES ///////////////////////////////////////

 Freak(); // CONSTRUCTOR
 int Game_Init(HWND);
 int Game_Shutdown(void *parms = NULL);
 int Game_Main(HWND, int);
 void Init_Blocks(void) ;
 void Draw_Blocks(void) ;
 void Draw_Paddle(int) ;
 void Draw_Ball(void) ;
 void Move_Ball(void) ;
 void Process_Ball_Hits(void) ;
 void Check_Keys(HWND) ;

};
// class Freak

#endif
