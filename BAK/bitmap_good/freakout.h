// freakout.h - Header file for game

// watch for multiple inclusions
#ifndef FREAKPROJ_FREAKOUT_H
#define FREAKPROJ_FREAKOUT_H

#include <windef.h>  // NEED THIS FOR 'HWND' TYPE IN FUNCTION CALLS

// DEFINES & CONSTANTS ///////////////////////////////////////////

typedef unsigned char UCHAR ;

/* block parameters - these must be const in order to use
   them to set the blocks array */
const int NUM_BLOCK_ROWS  =  9 , // 8 , 
          NUM_BLOCK_COLS  = 14 ; //10 ; 

/////////////////////////////////////////////////////////////////
class Freak
{
 public:

   int game_state ,
       paddle_x , paddle_prev_x ; // track position of paddle

   int ball_x , ball_y ,   // track position of ball
       ball_dx , ball_dy ; // velocity of ball

   int score ,          // the score
       level ;          // the current level

   int blocks_hit ,     // track number of blocks hit
       misses ;	      // number of times ball goes by paddle

   int high_streak ,    // keep track of consecutive blocks hit
       current_streak ;

   // this contains the game grid data   
   UCHAR blocks[ NUM_BLOCK_ROWS ][ NUM_BLOCK_COLS ];     

   // FUNCTION PROTOTYPES 

   Freak( ); // CONSTRUCTOR

   void Game_Init         ( HWND      );
   void Game_Main         ( HWND, int );
   void Game_Shutdown     ( );

   void Init_Blocks       ( );
   void Draw_Blocks       ( );
   void Draw_Paddle       ( int  );
   void Draw_Ball         ( );
   void Move_Ball         ( );
   void Process_Ball_Hits ( );
   void Check_Keys        ( HWND );

};
// class Freak

#endif // FREAKPROJ_FREAKOUT_H
