// FREAKOUT.CPP - breakout-type game demo

#define WIN32_LEAN_AND_MEAN  // make sure all macros are included

/*** GAME VERSION - no #define is a REGULAR game *************/
//#define MARIE_LAURE // THOMAS // 

#include <windows.h>       // include important windows stuff
#include <windowsx.h> 
#include <mmsystem.h>

#include <iostream>   // NEED IN THIS FORM (w/o .h) FOR A C++ COMPILE

#include "blackbox.h"   // project header files
#include "freakout.h"

// these read the keyboard asynchronously
#define KEY_DOWN(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000) ? 1 : 0)
#define KEY_UP(vk_code)   ((GetAsyncKeyState(vk_code) & 0x8000) ? 0 : 1)

/// CONDITIONAL COMPILATION FOR DIFFERENT GAME VERSIONS //////

#if defined( MARIE_LAURE )
    #define GAME_STRING  "Marie-Laure Boop-Out!"
    int PADDLE_WIDTH      =  96 ,	  
        PADDLE_THICKNESS  =  10 ,		
    // ball parameters
        BALL_SIZE           =  15 ,	 
        MAX_BALL_X_VELOCITY =   9 ,	 
        BALL_Y_VELOCITY     =   8 , //6 ,	 
        ENGLISH             =   3 ,  
        BALL_COLOR          = 128 , // magenta
    // use these for keeping score
        BASE_DECREMENT    =   5 ,  
        DEC_MULTIPLIER    =  20 ;  
    
#elif defined( THOMAS )
    #define GAME_STRING  "Thomas Goon-Out!"
    int PADDLE_WIDTH      =  50 ,	   
        PADDLE_THICKNESS  =   7 ,		
    // ball parameters
        BALL_SIZE           =   8 ,	 
        MAX_BALL_X_VELOCITY =  18 ,	 
        BALL_Y_VELOCITY     =  16 ,	 
        ENGLISH             =   6 ,   
        BALL_COLOR          = 187 , // blue
    // use these for keeping score
        BASE_DECREMENT    =  10 ,  
        DEC_MULTIPLIER    =  10 ;   
    
#else   // REGULAR GAME
    #define GAME_STRING "SUPER-FREAKMOUSE!" 
    int PADDLE_WIDTH      =  48 ,	  
        PADDLE_THICKNESS  =   8 ,		
    // ball parameters
        BALL_SIZE           =   8 , // 6 -> rows 3 & 10 incollidable
                                     // 8 -> rows 3, 6 & 10 incollidable
                                     // 10 -> row 6 incollidable
        MAX_BALL_X_VELOCITY =  15 ,	 
        BALL_Y_VELOCITY     =  14 ,	 
        ENGLISH             =   5 ,  
        BALL_COLOR          =  64 , // cyan // 255 , // white // 
    // use these for keeping score
        BASE_DECREMENT    =  25 ,  
        DEC_MULTIPLIER    =  50 ;   
#endif
    
// GLOBALS //////////////////////////////////////////////////
// - are not const so they will show up in the ClassView

// screen bits per pixel
int SCREEN_BPP    =   8 ; // palette is defined in blackbox::DD_Init()

// block parameters
int 
   BLOCK_X_GAP    = SCREEN_WIDTH / NUM_BLOCK_COLS , // left edge to left edge
   BLOCK_Y_GAP    = ((int)(SCREEN_HEIGHT*0.4)) / NUM_BLOCK_ROWS ,// top to top
   BLOCK_WIDTH    = (int)( BLOCK_X_GAP * 0.8 ) ,
   BLOCK_HEIGHT   = (int)( BLOCK_Y_GAP * 0.8 ) ,
   BLOCK_ORIGIN_X = (int)( (BLOCK_X_GAP * 0.2)/2 ) ,// half the x-gap
   BLOCK_ORIGIN_Y =  4 ,

// ball parameter

// paddle parameters
   PADDLE_START_X = SCREEN_WIDTH/2 - PADDLE_THICKNESS/2 , // centered 
   PADDLE_HEIGHT  = 32 ,// paddle height doesn't change = 32 pixels from bottom
   PADDLE_Y       = SCREEN_HEIGHT - PADDLE_HEIGHT ,
   BALL_START_Y   = PADDLE_Y ;

// color and score parameters
// these generate errors unless <iostream> is #included above
int
   BACKGROUND_COLOR    = 210 , // grey
   SHADOW_COLOR        = 203 , // black
   PADDLE_COLOR        = 192 , // yellow

   POS_SCORE_COLOR     = 255 ,  // white
   NEG_SCORE_COLOR     = 128 ,  // magenta 
   HI_SCORE_COLOR      = 192 ,  // yellow
   LO_SCORE_COLOR      = 195 ,  // black 
   SUPER_SCORE_COLOR   =  64 ,  // cyan
   HI_SCORE          = 10000 ,
   SUPER_SCORE       = 30000 ,
   LO_SCORE          = -1000 ,

// printing and timing parameters
   NUM_BLOCKS = NUM_BLOCK_ROWS * NUM_BLOCK_COLS , // see Freakout.h
   TEXT_HEIGHT = PADDLE_HEIGHT/2 ,
   TEXT_Y = SCREEN_HEIGHT - TEXT_HEIGHT ,
   FPS = 30 , // frames per second
   WAIT_INTERVAL = 1000/FPS ;

// states for game loop
int GAME_STATE_START_LEVEL = 1 ,
    GAME_STATE_RUN         = 2 ,
    GAME_STATE_SHUTDOWN    = 3 ,
    GAME_STATE_PAUSE       = 5 ;

  // CREATE A BLACKBOX OBJECT - NEEDS TO BE GLOBAL
  Blackbox bb ;


////// FUNCTION DEFINITIONS ////////////////////

// CONSTRUCTOR - DO INITIALIZATION
Freak::Freak()
  {
   score = 0;          // the score
   level = 1;          // the current level

   blocks_hit = 0 ;    // number of blocks hit
   misses	    = 0 ;	   // number of times ball goes by paddle

   high_streak = current_streak = 0 ;  // keep track of consecutive blocks hit
  }
// CONSTRUCTOR

int Freak::Game_Init(HWND hand)
	{
   // this function is where you do all the initialization for your game

	 // initialize graphics
   bb.DD_Init(hand, SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP);

   // seed the random number generator so game is different each play
   srand(bb.Start_Clock());

   // set the paddle position here to the middle bottom
   paddle_x = paddle_prev_x = PADDLE_START_X;

   // set ball starting position and velocity
   ball_x = 8 + rand()%(SCREEN_WIDTH-16);
   ball_y = BALL_START_Y;
   ball_dx = -MAX_BALL_X_VELOCITY + ENGLISH ; // move up to start
   ball_dy = BALL_Y_VELOCITY ; // + rand()%2;

   // transition to start level state
   game_state = GAME_STATE_START_LEVEL;

	 return(TRUE);   // return success
	}
// Game_Init()


// GAME_MAIN - this is the workhorse of your game ////////////////////////////
//           - it will be called continuously in real-time 
//           - this is like main() in C - all the calls for your game go here!
int Freak::Game_Main(HWND main_window_handle, int mouse_x)
	{
   char buffer[120]; // used to print text

 // START STATE
   if( game_state == GAME_STATE_START_LEVEL )
     {
      // get a new level ready to run

      // initialize the blocks
      Init_Blocks();

      // reset block counter
      blocks_hit = 0;

      // transition to run state
      game_state = GAME_STATE_RUN;

     } // end if

 // RUN STATE
   else if( game_state == GAME_STATE_RUN )
    {
     // start the timing clock
     bb.Start_Clock();

     // clear drawing surface for the next frame of animation
     bb.Draw_Rectangle(0, 0, SCREEN_WIDTH-1, SCREEN_HEIGHT-1, BACKGROUND_COLOR);

     // DRAW THE BLOCKS
     Draw_Blocks();

     // DRAW THE PADDLE
		 Draw_Paddle(mouse_x) ;

		 // MOVE THE BALL
		 Move_Ball() ;

     // TEST IF BALL HIT ANY BLOCKS OR THE PADDLE
     Process_Ball_Hits();

     // DRAW THE BALL
		 Draw_Ball() ;
 
     // draw the game info
     char *msg = GAME_STRING;
     // THIS WAS THE ONLY WAY TO MODIFY THE STRING - strcat() DID NOT WORK!
     sprintf( buffer, "%s   Score=%d   Level=%d   Blocks=%d\
   Misses=%d   Streak=%d   'Home' to pause, 'Esc' to exit",
              msg, score, level, ((level - 1)*NUM_BLOCKS + blocks_hit),
              misses, high_streak ) ;
 
		 // change color depending on the score
     if( score <= LO_SCORE )
		   bb.Draw_Text_GDI(buffer, 8, TEXT_Y, LO_SCORE_COLOR) ;
		 else if( score < 0 )
		        bb.Draw_Text_GDI(buffer, 8, TEXT_Y, NEG_SCORE_COLOR) ;
        else if( score > SUPER_SCORE )
			         bb.Draw_Text_GDI(buffer, 8, TEXT_Y, SUPER_SCORE_COLOR) ;
        else if( score > HI_SCORE )
			         bb.Draw_Text_GDI(buffer, 8, TEXT_Y, HI_SCORE_COLOR) ;
            else bb.Draw_Text_GDI(buffer, 8, TEXT_Y, POS_SCORE_COLOR) ;

     // flip the surfaces
     bb.DD_Flip();

     // check if user has pressed a key
     Check_Keys(main_window_handle) ;

     // sync to approx 33 fps
     bb.Wait_Clock((DWORD)WAIT_INTERVAL);

		} // end if

 // PAUSE STATE
   else if( game_state == GAME_STATE_PAUSE )
    {
     // start the timing clock
     bb.Start_Clock();

     // clear drawing surface for the next frame of animation
     bb.Draw_Rectangle(0, 0, SCREEN_WIDTH-1, SCREEN_HEIGHT-1, BACKGROUND_COLOR);

     // draw blocks
     Draw_Blocks();

     // draw the paddle
     bb.Draw_Rectangle( paddle_x, PADDLE_Y, paddle_x + PADDLE_WIDTH, 
                        PADDLE_Y + PADDLE_THICKNESS, PADDLE_COLOR );

     // draw the ball
     bb.Draw_Rectangle( ball_x, ball_y, ball_x+BALL_SIZE, 
                        ball_y+BALL_SIZE, BALL_COLOR );

     // draw the game info
     sprintf( buffer,"Game Paused!  Press 'End' to resume.\
     Score=%d  Level=%d  Blocks=%d  Misses=%d  Streak=%d",
			        score, level, ((level - 1)*NUM_BLOCKS + blocks_hit), 
              misses, high_streak ) ;

     bb.Draw_Text_GDI(buffer, 8, TEXT_Y, 48) ; // red

     // flip the surfaces
     bb.DD_Flip();

     // check if user has pressed a key
     Check_Keys(main_window_handle) ;

     // sync to approx 33 fps
     bb.Wait_Clock((DWORD)WAIT_INTERVAL);

	} // end if

 // SHUTDOWN STATE
 return(TRUE);
 }
// Game_Main() /////////////////////////////////////////////////


void Freak::Init_Blocks(void)
	{
   // initialize color values for the block field
   for( int row=0; row < NUM_BLOCK_ROWS; row++ )
     for( int col=0; col < NUM_BLOCK_COLS; col++ )
        blocks[row][col] = row*22 + col*4 + 3 ;//(row+1) * (col+1) ; //
		                       //row*16 + col*3 + 16

	 // make a noise to indicate a new level is starting
   if( level > 1 ) MessageBeep(MB_ICONASTERISK) ;
	}
// Init_Blocks()

void Freak::Draw_Blocks(void)
	{
   // this function draws all the blocks in row major form
   int x1 = BLOCK_ORIGIN_X, // used to track current position
       y1 = BLOCK_ORIGIN_Y; 

   // draw all the blocks
   for( int row=0; row < NUM_BLOCK_ROWS; row++ )
     {    
      // reset column position
      x1 = BLOCK_ORIGIN_X;

      // draw this row of blocks
      for( int col=0; col < NUM_BLOCK_COLS; col++ )
        {
         // draw next block (if there is one)
         if (blocks[row][col] != 0)
           {
            // draw shadow then block     
            bb.Draw_Rectangle( x1-4, y1+4, x1+BLOCK_WIDTH-4,
							                 y1+BLOCK_HEIGHT+4, SHADOW_COLOR );

            bb.Draw_Rectangle( x1, y1, x1+BLOCK_WIDTH,
                               y1+BLOCK_HEIGHT, blocks[row][col] );
           } // end if

         // advance column position
         x1 += BLOCK_X_GAP;
				} // end for(col)

      // advance to next row position
      y1 += BLOCK_Y_GAP;
     } // end for(row)
	}
// Draw_Blocks()

void Freak::Draw_Paddle(int mouse_x)
	{
	 // keep track of direction paddle is moving
	 paddle_prev_x = paddle_x ;
   // move the paddle
   paddle_x = mouse_x ;
 
   // make sure paddle doesn't go off screen
   if( paddle_x > (SCREEN_WIDTH - PADDLE_WIDTH) )
     paddle_x = SCREEN_WIDTH - PADDLE_WIDTH;
   else
		   if( paddle_x < 0 ) paddle_x = 0;
  
	 // draw paddle
	 bb.Draw_Rectangle( paddle_x, PADDLE_Y, paddle_x + PADDLE_WIDTH, 
                      PADDLE_Y + PADDLE_THICKNESS, PADDLE_COLOR );
	}
// Draw_Paddle()

void Freak::Move_Ball(void)
	{
   // move the ball
   ball_x += ball_dx;
   ball_y += ball_dy;

   // keep ball on screen, if the ball hits the edge of 
   // screen then bounce it by reflecting its velocity
	 // first x-axis
   if( ball_x > (SCREEN_WIDTH - BALL_SIZE) || ball_x < 0 ) 
     {
      // reflect x-axis velocity
      ball_dx = -ball_dx;

			// update position 
      ball_x += ball_dx;
     } // end if

   // now y-axis
	 // test if ball went by all the blocks
   if( ball_y < 0 ) 
     {
      // reflect y-axis velocity
      ball_dy = -ball_dy;

      // update position 
      ball_y += ball_dy;
     } // end if
   else 
     // penalize player for missing the ball
     if( ball_y > (SCREEN_HEIGHT - BALL_SIZE) )
			 {
        // make a little noise to indicate missing the ball
        MessageBeep(MB_ICONEXCLAMATION);

        // reflect y-axis velocity
        ball_dy = -ball_dy;

				// update position 
        ball_y += ball_dy;

		    // re-set streak
		    current_streak = 0 ;

		    // keep track of misses and 
        // decrement the score except for the initial miss
        if( ++misses > 1 ) score -= ((level*DEC_MULTIPLIER) + BASE_DECREMENT); 

			 } // end if
	}
// Move_Ball()

void Freak::Process_Ball_Hits(void)
{
// this function tests if the ball has hit a block or the paddle
// if so, the ball is bounced and the block is removed from the playfield
// Note: very cheesy collision algorithm :)

// first test for ball block collisions
// the algorithm basically tests the ball against each block's bounding box
// this is inefficient, but easy to implement, later we'll see a better way

// extract leading edge of ball
int ball_lx = ball_x + BALL_SIZE , 
    ball_ly = ball_y + BALL_SIZE ;

// TEST IF BALL IS CLOSE TO AND APPROACHING THE PADDLE
if( (ball_ly >= PADDLE_Y) && ball_dy > 0 )
  {
   // test for collision with paddle
   if( ((ball_lx >= paddle_x) && (ball_x <= paddle_x + PADDLE_WIDTH)) )
		 {
       // reflect ball
       ball_dy = -ball_dy;

       // push ball out of paddle since it made contact
       ball_y += ball_dy;

       // add a little ENGLISH to ball based on motion of paddle
			 int paddle_direction = paddle_x - paddle_prev_x ;

       if( paddle_direction > 0 ) // paddle moving right
         ball_dx -= (rand() % ENGLISH);
       else
           if( paddle_direction < 0 ) // paddle moving left
             ball_dx += (rand() % ENGLISH);
             else // not moving
                 ball_dx += ( -(ENGLISH/2) + (rand() % ENGLISH) );

			 // re-set streak
			 current_streak = 0 ;
       
       // test if there are no blocks (all blocks have been hit),
			 // if so, send a message to game loop to start another level
       if( blocks_hit >= NUM_BLOCKS-40 )
         {
          game_state = GAME_STATE_START_LEVEL;
          level++ ;
         } // end if
			 else
           // make a little noise to indicate ball hitting the paddle
           MessageBeep(MB_OK);

      return; 
     } // endif(collision)
  } // endif(ball near paddle)

int y1 = BLOCK_ORIGIN_Y ; // current rendering position
     
int ball_cx = ball_x + (BALL_SIZE/2),  // compute center of ball
    ball_cy = ball_y + (BALL_SIZE/2);

// NOW SCAN THRU ALL THE BLOCKS AND SEE IF BALL HIT BLOCKS
for( int row = 0; row < NUM_BLOCK_ROWS; row++ )
   {    
    // reset column position
    int x1 = BLOCK_ORIGIN_X;

    // scan the cols in this row of blocks
    for( int col = 0; col < NUM_BLOCK_COLS; col++ )
       {
        // if there is a block here then test it against ball
        if( blocks[row][col] != 0 )
          {
           // test ball against bounding box of block
           if( (ball_cx > x1) && (ball_cx < x1+BLOCK_WIDTH) &&     
               (ball_cy > y1) && (ball_cy < y1+BLOCK_HEIGHT) )
              {
               // remove the block
               blocks[row][col] = 0; 

               // increment global block counter, so we know 
               // when to start a new level
               blocks_hit++;

							 current_streak++ ;
							 // keep track of streak of blocks hit on one shot
							 if( current_streak > high_streak )
								 high_streak = current_streak ;

               // bounce the ball
               ball_dy = -ball_dy;

               // add a little ENGLISH
               ball_dx += (-1 + rand() % (ENGLISH-2));

               // make a little noise to indicate ball hitting a block
               MessageBeep(MB_ICONHAND);

               // add some points
               score += (level * (abs(ball_dx) + current_streak)) ;

               return ; // that's it -- no more blocks to check

              } // end if(block here)
          } // end if(ball has hit)

        // advance column position
        x1 += BLOCK_X_GAP;
			} // endfor(col)

    // advance to next row position
    y1 += BLOCK_Y_GAP;
   } // endfor(row)
 }
// Process_Ball_Hits()

void Freak::Draw_Ball(void)     
	{
   // watch out for ball velocity getting out of hand
   if( ball_dx > MAX_BALL_X_VELOCITY )
	   ball_dx = MAX_BALL_X_VELOCITY ;
   else
	     if( ball_dx < -MAX_BALL_X_VELOCITY ) ball_dx = -MAX_BALL_X_VELOCITY ;    

   bb.Draw_Rectangle( ball_x, ball_y, ball_x+BALL_SIZE, 
                      ball_y+BALL_SIZE, BALL_COLOR );
	 }
// Draw_Ball()

void Freak::Check_Keys(HWND main_window_handle)
	{
	 // check if user is trying to pause
   if( KEY_DOWN(VK_HOME) ) game_state = GAME_STATE_PAUSE ;

   // check if user is trying to resume
   else
		   if( KEY_DOWN(VK_END) ) game_state = GAME_STATE_RUN ;

   // check if user is trying to exit
   else
		   if( KEY_DOWN(VK_ESCAPE) )
				 {
          // set exit state
          game_state = GAME_STATE_SHUTDOWN ;
          // send message to windows to exit
          PostMessage(main_window_handle, WM_DESTROY, 0, 0);
				 } // end if
	}
// Check_Keys()

int Freak::Game_Shutdown(void *parms)
	{
   // shut everything down and release resources
   bb.DD_Shutdown();

   return(TRUE);   // return success
	}
// Game_Shutdown()