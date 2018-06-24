/**************************************************************************
 *
 *  Mark Sattolo (msattolo@sympatico.ca)
 * -----------------------------------------------
 *  $File: //depot/CppProj/VS6/FreakProj/Src/freakout.h $
 *  $Revision: #5 $
 *  $Change: 82 $
 *  $DateTime: 2008/12/29 10:02:24 $
 *
 * ---------------------------------------------------------------------
 *  freakout.h - Header file for game logic - CLASS Freak
 *		Created 2001, based on code in 
 *		'Tricks of the Windows Game Programming Gurus', by Andre Lamothe
 * 
 **************************************************************************/

// watch for multiple inclusions
#ifndef MHS_VS6_FREAKPROJ_FREAKOUT_H
#define MHS_VS6_FREAKPROJ_FREAKOUT_H

#include <windef.h>  // NEED THIS FOR 'HWND' TYPE IN FUNCTION CALLS

// DEFINES & CONSTANTS ///////////////////////////////////////////

typedef unsigned char UCHAR ;

/* block parameters - these must be const in order to use them to set the blocks array */
const UCHAR NUM_BLOCK_ROWS  =  9 , // 8 , 
            NUM_BLOCK_COLS  = 14 ; //10 ; 

/////////////////////////////////////////////////////////////////
class Freak
{
 protected:

   int game_state ,   // init, run, pause, shutdown
       score ,        // current score
       level ,        // current level
       string_color ;
   
   int paddle_x , paddle_prev_x ; // track position of paddle
   
   int ball_x ,  ball_y ,  // track position of ball
       ball_dx , ball_dy ; // velocity of ball
   
   int blocks_hit ,   // track number of blocks hit
       misses ;	      // number of times ball goes by paddle
   
   int high_streak ,  // keep track of consecutive blocks hit
       current_streak ;
   
   DWORD frame_start ; // used for frame timing
   
   // this contains the game grid data   
   UCHAR blocks[ NUM_BLOCK_ROWS ][ NUM_BLOCK_COLS ];     

// FUNCTIONS 

   void Init_Blocks       ();
   void Draw_Blocks       ();
   void Draw_Paddle       ( int );
   void Draw_Ball         ();
   void Move_Ball         ();
   void Process_Ball_Hits ();
   void Check_Keys        ( HWND );

 public:

   Freak(); // CONSTRUCTOR

   bool Game_Init     ( HWND );
   void Game_Main     ( HWND, int );
   void Game_Shutdown ();

   // timer functions
   void  Clock_wait( DWORD );
   DWORD Clock_read();

}; // CLASS Freak

#endif // MHS_VS6_FREAKPROJ_FREAKOUT_H
