// freakout.h - Header file for demo game

// watch for multiple inclusions
#ifndef __FREAKOUT_H
#define __FREAKOUT_H

// INCLUDES ///////////////////////////////////////////////////

#define WIN32_LEAN_AND_MEAN // include all macros
#define INITGUID            // include all GUIDs 

#include <windows.h>        // include important windows stuff
#include <windowsx.h> 
#include <mmsystem.h>

#include <iostream>  // need C++ header format here to force a C++ compile
#include <conio.h>
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <math.h>
#include <io.h>
#include <fcntl.h>
#include <ddraw.h>          // directX includes

#include "blackbox.h"       // game library includes

// DEFINES & CONSTANTS ///////////////////////////////////////////

#define WINDOW_CLASS_NAME "MHS_WIN3DCLASS"  // class name

// states for game loop
#define GAME_STATE_START_LEVEL  1
#define GAME_STATE_RUN          2
#define GAME_STATE_SHUTDOWN     3
#define GAME_STATE_PAUSE        5

// block defines
#define NUM_BLOCK_ROWS          8
#define NUM_BLOCK_COLUMNS       10

#define BLOCK_WIDTH             64
#define BLOCK_HEIGHT            16
#define BLOCK_ORIGIN_X          8
#define BLOCK_ORIGIN_Y          8
#define BLOCK_X_GAP             80  // from left edge to left edge
#define BLOCK_Y_GAP             32  // from top to top

// paddle defines
#define PADDLE_START_X   (SCREEN_WIDTH/2 - 16) // SCREEN_WIDTH is in Blackbox.h
#define PADDLE_Y         (SCREEN_HEIGHT - 32)  // paddle height doesn't change
#define PADDLE_WIDTH     60	  // Reg=60 ML=96 
#define PADDLE_HEIGHT     8		// Reg=8  ML=10

// ball defines
#define BALL_START_Y     (SCREEN_HEIGHT/2)
#define BALL_SIZE        10	  // Reg=10  ML=15


// these generate errors unless <iostream> is #included above
const int BACKGROUND_COLOR    = 210 ;
const int SHADOW_COLOR        = 203 ;
const int PADDLE_COLOR        = 192 ;
const int BALL_COLOR          = 255 ;
const int MAX_BALL_X_VELOCITY =  15 ;	 // Mouse=15 Keys=12 ML=9
const int BALL_Y_VELOCITY     =  12 ;	 // Mouse=12 Keys=9  ML=6
const int ENGLISH             =   5 ;	 // Mouse=5  Keys=4  ML=3

const int BASE_SCORE          =  50 ;  // use these for keeping score
const int SCORE_MULTIPLIER    =  50 ;  // Mouse=50/50 Keys=10/20 ML=10/10 

const int POS_SCORE_COLOR     =  64 ;  // use for displaying positive scores
const int NEG_SCORE_COLOR     = 128 ;  // use for displaying negative scores 
const int HI_SCORE_COLOR      = 192 ;  // use for displaying high scores 
const int LO_SCORE_COLOR      = 195 ;  // use for displaying low scores 
const int HI_SCORE          = 10000 ;
const int LO_SCORE          = -1000 ;

// FUNCTION PROTOTYPES ///////////////////////////////////////

int Game_Init(void *parms = NULL);
int Game_Shutdown(void *parms = NULL);
int Game_Main(void *parms = NULL);
void Init_Blocks(void) ;
void Draw_Blocks(void) ;
void Draw_Paddle(void) ;
void Draw_Ball(void) ;
void Move_Ball(void) ;
void Process_Ball_Hits(void) ;
void Check_Keys(void) ;

// GLOBALS ////////////////////////////////////////////////////

HWND main_window_handle  = NULL;     // save the window handle
HINSTANCE main_instance  = NULL;     // save the instance
int game_state ;

int paddle_x = 0, paddle_prev_x = 0 ; // track position of paddle

int ball_x  = 0, ball_y   = 0; // track position of ball
int ball_dx = 0, ball_dy  = 0; // velocity of ball

int score = 0;          // the score
int level = 1;          // the current level

int mouse_x = 0 ;       // keep track of mouse x-position

int blocks_hit = 0 ;    // track number of blocks hit
int misses	   = -1 ;	  // number of times ball goes by paddle

int high_streak    = 0 ;  // keep track of consecutive blocks hit
int current_streak = 0 ;

// this contains the game grid data   
UCHAR blocks[NUM_BLOCK_ROWS][NUM_BLOCK_COLUMNS];     

#endif
