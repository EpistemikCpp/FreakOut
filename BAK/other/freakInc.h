// freakInc.h

// watch for multiple inclusions
#ifndef __FREAKINC_H
#define __FREAKINC_H

// INCLUDES ///////////////////////////////////////////////////

#define WIN32_LEAN_AND_MEAN  // make sure all macros are included
//#define INITGUID            // include all GUIDs 

#include <windows.h>         // include important windows stuff
#include <windowsx.h> 
#include <mmsystem.h>

#include <iostream>        // include important C/C++ stuff
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
#include <ddraw.h>           // directX includes

  // screen parameters
  int SCREEN_WIDTH  = 800 , // size of screen
      SCREEN_HEIGHT = 600 ,
      SCREEN_BPP    =   8 , // bits per pixel
      MAX_COLORS    = 256 ; // maximum colors

int BLOCK_WIDTH        = 64 ,
    BLOCK_HEIGHT       = 16 ,
    BLOCK_ORIGIN_X     =  8 ,
    BLOCK_ORIGIN_Y     =  8 ,
    BLOCK_X_GAP        = 80 ,  // from left edge to left edge
    BLOCK_Y_GAP        = 32 ;  // from top to top

// paddle parameters
int PADDLE_START_X = (SCREEN_WIDTH/2 - 16) , 
    PADDLE_Y       = (SCREEN_HEIGHT - 32) ,  // paddle height doesn't change
    PADDLE_WIDTH   =  60 ,	  // Reg=60 ML=96 
    PADDLE_HEIGHT  =   8 ;		// Reg=8  ML=10

// ball parameters
int BALL_START_Y  = (SCREEN_HEIGHT/2) ,
    BALL_SIZE     =  10 ;	  // Reg=10  ML=15

 // color and score parameters
 // these generate errors unless <iostream> is #included above
 int BACKGROUND_COLOR    = 210 ,
     SHADOW_COLOR        = 203 ,
     PADDLE_COLOR        = 192 ,
     BALL_COLOR          = 255 ,
     MAX_BALL_X_VELOCITY =  15 ,	 // Mouse=15 Keys=12 ML=9
     BALL_Y_VELOCITY     =  12 ,	 // Mouse=12 Keys=9  ML=6
     ENGLISH             =   5 ;	 // Mouse=5  Keys=4  ML=3

 int BASE_SCORE          =  50 ,  // use these for keeping score
     SCORE_MULTIPLIER    =  50 ;  // Mouse=50/50 Keys=10/20 ML=10/10 

 int POS_SCORE_COLOR     =  64 ,  // use for displaying positive scores
     NEG_SCORE_COLOR     = 128 ,  // use for displaying negative scores 
     HI_SCORE_COLOR      = 192 ,  // use for displaying high scores 
     LO_SCORE_COLOR      = 195 ,  // use for displaying low scores 
     HI_SCORE          = 10000 ,
     LO_SCORE          = -1000 ;

#endif