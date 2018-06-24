/**************************************************************************
 *
 *  Mark Sattolo (msattolo@sympatico.ca)
 * -----------------------------------------------
 *  $File: //depot/CppProj/VS6/FreakProj/Src/freakout.cpp $
 *  $Revision: #6 $
 *  $Change: 82 $
 *  $DateTime: 2008/12/29 10:02:24 $
 *
 * ---------------------------------------------------------------------
 *  freakout.cpp - main game logic - CLASS Freak
 *		Created 2001, based on code in 
 *		'Tricks of the Windows Game Programming Gurus', by Andre Lamothe
 * 
 **************************************************************************/

#define WIN32_LEAN_AND_MEAN  // make sure all macros are included

#include "FreakProj.h"  // Windows & C++ includes
#include "blackbox.h"
#include "freakout.h"

// read the keyboard asynchronously
#define KEY_DOWN(vk_code) ( ( GetAsyncKeyState(vk_code) & 0x8000 ) ? 1 : 0 )
#define   KEY_UP(vk_code) ( ( GetAsyncKeyState(vk_code) & 0x8000 ) ? 0 : 1 )

// original in Blackbox.cpp
extern int BALL_SIZE ;

///////// CONDITIONAL COMPILATION FOR DIFFERENT GAME VERSIONS /////////////////

/************ GAME VERSION - no #define is a REGULAR game *************/

// use Win32_MarieLaure build configuration
#if defined( MARIE_LAURE )
    char *GAME_STRING = "Marie-Laure Rainbow!" ;
    char *BALL_BITMAP = "C:\\PROG\\CppProj\\VS6\\FreakProj\\bitmaps\\happy_bitmap2.bmp" ;
    int PADDLE_WIDTH      =  96 ,	  
        PADDLE_THICKNESS  =  10 ,		
    // ball parameters
        //BALL_SIZE         =  15 ,	// not needed when using a bitmap 
        MAX_BALL_DX       =   9 ,	 
        MAX_BALL_DY       =  12 , 
        START_BALL_DY     =   8 ,	 
        BALL_DY_INCREMENT =   1 ,
        ENGLISH           =   3 ,  
        BALL_COLOR        = PINK ,
    // use these for keeping score
        BASE_DECREMENT    =   5 ,  
        DEC_MULTIPLIER    =  20 ;  
    
// use Win32_Thomas build configuration
#elif defined( THOMAS )
    // CURRENT SETTINGS MAKE YELLOW ROW INCOLLIDABLE AT LEVEL 2
    char *GAME_STRING = "Thomas freaks out!!" ;
    char *BALL_BITMAP = "C:\\PROG\\CppProj\\VS6\\FreakProj\\bitmaps\\happy_bitmap256-7.bmp" ;
    int PADDLE_WIDTH      =  50 ,	   
        PADDLE_THICKNESS  =   7 ,		
    // ball parameters
        //BALL_SIZE         =   9 ,	// not needed when using a bitmap
        MAX_BALL_DX       =  18 ,	 
        MAX_BALL_DY       =  30 ,	 
        START_BALL_DY     =  14 ,	 
        BALL_DY_INCREMENT =   3 ,
        ENGLISH           =   6 ,   
        BALL_COLOR        = CYAN ,
    // use these for keeping score
        BASE_DECREMENT    =  10 ,  
        DEC_MULTIPLIER    =  10 ;   
    
#else   // REGULAR GAME -- DEBUG OR RELEASE
    // CURRENT SETTINGS MAKE DGREEN & RED ROWS INCOLLIDABLE AT LEVEL 5
    char *GAME_STRING = "RAINBOW FREAKOUT!" ;
    /* THIS WAS THE ONLY WAY TO USE A DIFFERENT STRING FOR DIFFERENT VERSIONS
       -> strcat() DID NOT WORK! */
    char *BALL_BITMAP = "C:\\PROG\\CppProj\\VS6\\FreakProj\\bitmaps\\happy_bitmap5.bmp" ;
    int PADDLE_WIDTH      =  48 ,	  
        PADDLE_THICKNESS  =   8 ,		
    // ball parameters
        //BALL_SIZE         =   8 , // 6 -> ROWS 3 & 10 INCOLLIDABLE
                                  // 8 -> ROWS 3, 6 & 10 INCOLLIDABLE
                                  // 10 -> ROW 6 INCOLLIDABLE
        MAX_BALL_DX       =  15 ,	 
        MAX_BALL_DY       =  24 ,	 
        START_BALL_DY     =  10 ,	// 16 -> MAKES ROWS 4,7 INCOLLIDABLE
        BALL_DY_INCREMENT =   2 ,
    /* initial BALL_SIZE of 8, BLOCK_THICKNESS of 14, START_BALL_DY of 12, 
       and BALL_DY_INCREMENT of 2 gives the first two levels good before 
       level 3 has rows 1,5,9 incollidable (at ball_dy of 16) */

        ENGLISH           =   5 ,  
        BALL_COLOR        =  YELLOW ,
    // use these for keeping score
        BASE_DECREMENT    =  25 ,  
        DEC_MULTIPLIER    =  50 ;   
#endif

///// GLOBALS ////////////////////////////////////////////////////////////////
//  - are not const so they will show up in the ClassView

    // screen bits per pixel
int SCREEN_BPP    = 8 , // see Blackbox::definePalette() for colors
    // shadow placement
    SHADOW_OFFSET = 4 ;

// BLOCK PARAMETERS
int BLOCK_X_GAP = SCREEN_WIDTH / NUM_BLOCK_COLS , // left edge to left edge

/* PARAMETERS BLOCK_WIDTH, BLOCK_THICKNESS & BLOCK_Y_GAP ALSO SEEM
   TO DETERMINE WHETHER ROWS WILL BE INCOLLIDABLE OR NOT 
   - DEPENDING ON WHAT LEVEL OF RESOURCES ARE BEING USED ??  */

    BLOCK_WIDTH = (int)( BLOCK_X_GAP * 0.8 ) , // 32 pixels

/* THICKNESS THAT WORKS DEPENDS ON BALL SPEED & SIZE !!!
   - 15 & 14 WORK BUT 13 MAKES ROWS 3 AND 10 INCOLLIDABLE  */

    BLOCK_THICKNESS = 16 , //(int)( BLOCK_WIDTH * 0.5 ) , 
//   BLOCK_THICKNESS = (int)( BLOCK_Y_GAP * 0.7 ) ,
   // 0.75 GIVES 15 PIXEL THICKNESS BLOCKS AND WORKS
   // 0.70 GIVES 14 PIXEL THICKNESS BLOCKS AND MAKES ROW 5 INCOLLIDABLE!

    BLOCK_Y_GAP = (int)( BLOCK_THICKNESS * 2 ) ,  // top to top
//   BLOCK_Y_GAP     = ((int)(SCREEN_HEIGHT*0.4)) / NUM_BLOCK_ROWS ,// top to top
   // 0.4 GIVES 12 PIXEL THICKNESS AND MAKES ROWS 1,5,8 INCOLLIDABLE
   // 0.5 GIVES 16 PIXEL THICKNESS AND WORKS

    BLOCK_ORIGIN_X = (int)( (BLOCK_X_GAP * 0.2)/2 ) ,// half the space
    BLOCK_ORIGIN_Y =  4 ;

// PADDLE PARAMETERS
int PADDLE_START_X = SCREEN_WIDTH/2 - PADDLE_THICKNESS/2 , // centered 
    PADDLE_HEIGHT  = 32 , // paddle height doesn't change -> 32 pixels from bottom
    PADDLE_Y       = SCREEN_HEIGHT - PADDLE_HEIGHT ;

// COLOR AND SCORE PARAMETERS
int BACKGROUND_COLOR    = GREY , 
    SHADOW_COLOR        = BLACK ,
    PADDLE_COLOR        = DGREEN , 

    POS_SCORE_COLOR     = WHITE ,  
    NEG_SCORE_COLOR     = RED ,  
    GOOD_SCORE_COLOR    = ORANGE ,  
    BAD_SCORE_COLOR     = BLACK ,  
    SUPER_SCORE_COLOR   = LGREEN ,
    ULTRA_SCORE_COLOR   = MAGENTA ,
    PAUSE_COLOR         = DGREEN ,
   
    GOOD_SCORE          = 10000 ,
    SUPER_SCORE         = 25000 ,
    ULTRA_SCORE         = 50000 ,
    BAD_SCORE           = -1000 ;

// PRINTING AND TIMING PARAMETERS
int NUM_BLOCKS = NUM_BLOCK_ROWS * NUM_BLOCK_COLS ; // see freakout.h

int TEXT_HEIGHT = PADDLE_HEIGHT/2 ,
    TEXT_Y = SCREEN_HEIGHT - TEXT_HEIGHT ;

int FPS = 30 , // frames per second
    WAIT_INTERVAL = 1000/FPS ;

// STATES FOR GAME LOOP
int GAME_STATE_START    = 1   ,
    GAME_STATE_RUN      = 4   ,
    GAME_STATE_PAUSE    = 32  ,
    GAME_STATE_SHUTDOWN = 256 ;

// CREATE A BLACKBOX OBJECT - NEEDS TO BE GLOBAL
   Blackbox bb ;

// DECLARE THE STATIC MEMBER
   LPDIRECTDRAWSURFACE4 Blackbox::lpddsback = NULL ;  // dd back surface

///////// FUNCTION DEFINITIONS ///////////////////////////////////////////////

// CONSTRUCTOR
Freak::Freak()
{
  score = 0 ;  // initial score
  level = 1 ;  // starting level

  frame_start = 0 ; // for timing each frame

  blocks_hit = 0 ; // number of blocks hit
  misses	   = 0 ; // number of times ball goes by the paddle

  high_streak = current_streak = 0 ; // keep track of consecutive blocks hit
}

//=============================================================================

// DO ALL THE INITIALIZATION FOR YOUR GAME
bool Freak::Game_Init( HWND hwnd )
{
  // initialize graphics
  if( ! bb.DD_Init( hwnd, BALL_BITMAP, SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP ) )
  {
    LPCTSTR msg = "Could NOT initialize the Direct Draw Graphics!" ;
    fprintf( stderr, msg );
    MessageBox( hwnd, msg, "PROBLEM", MB_ICONWARNING );
    game_state = GAME_STATE_SHUTDOWN ;
    return FALSE ;
  }

  // seed the random number generator so game is different each play
  srand( GetTickCount() );

  // set the paddle position here to the middle bottom
  paddle_x = paddle_prev_x = PADDLE_START_X ;

  // set ball starting position and velocity
  ball_x  = 8 + rand()%(SCREEN_WIDTH-16);
  ball_y  = PADDLE_Y ; // start moving from paddle
  ball_dx = MAX_BALL_DX - ENGLISH ; 
  ball_dy = -START_BALL_DY ; // move up to start

  // transition to start state
  game_state = GAME_STATE_START ;

  return TRUE ;

}// Freak::Game_Init()

//=============================================================================

/* GAME_MAIN - this is the workhorse of your game 
             - it will be called continuously in real-time 
             - this is like main() in C - all the calls for your game go here! */
void Freak::Game_Main( HWND main_window_handle, int mouse_x )
{
  char buffer[ 120 ]; // used to print text

  // START STATE
  if( game_state == GAME_STATE_START )
  {
    // get a new level ready to run

    // initialize the blocks
    Init_Blocks();

    // reset block counter
    blocks_hit = 0 ;

    if( level == 1 )
      // START IN PAUSE STATE TO GIVE EVERYTHING A CHANCE TO FINISH INIT PROPERLY
      game_state = GAME_STATE_PAUSE ;
    else
    {
      // For subsequent games, just pause a bit
      Sleep( 100 );
      game_state = GAME_STATE_RUN ;
    }

  }// end if GAME_STATE_START

  // RUN OR PAUSE STATE
  else
  {
    // get the start time for this frame
    frame_start = Clock_read();
    
    // clear drawing surface for the next frame of animation
    bb.DD_Blt_Rect( 0, 0, SCREEN_WIDTH-1, SCREEN_HEIGHT-1, BACKGROUND_COLOR );
    
    Draw_Blocks();
    
    // some differences depending on state of RUN or PAUSE
    if( game_state == GAME_STATE_RUN )
    {
      Draw_Paddle( mouse_x );
      
      // fill the info string
      sprintf( buffer, "%s  Score=%d  Level=%d  Blocks=%d \
 Misses=%d  Streak=%d  'End' to pause, 'Esc' to exit",
               GAME_STRING, score, level, ((level-1)*NUM_BLOCKS + blocks_hit),
               misses, high_streak  );
      
	    // change color depending on the score
      if( score <= BAD_SCORE )
	      string_color = BAD_SCORE_COLOR ;
	      else if( score < 0 )
		      string_color = NEG_SCORE_COLOR ;
          else if( score > ULTRA_SCORE )
		        string_color = ULTRA_SCORE_COLOR ;
            else if( score > SUPER_SCORE )
		          string_color = SUPER_SCORE_COLOR ;
              else if( score > GOOD_SCORE )
			          string_color = GOOD_SCORE_COLOR ;
                else
                    string_color = POS_SCORE_COLOR ;
      
	    Move_Ball();
      
      // TEST IF BALL HIT ANY BLOCKS OR THE PADDLE
      Process_Ball_Hits();
    }
    else
    if( game_state == GAME_STATE_PAUSE )
    {
      // draw the paddle - NOT MOVING
      bb.DD_Blt_Rect( paddle_x, PADDLE_Y, paddle_x + PADDLE_WIDTH, 
                      PADDLE_Y + PADDLE_THICKNESS, PADDLE_COLOR   );

      // fill the info string
      sprintf( buffer, "Game Paused!  Press 'Home' to resume.\
 Score=%d  Level=%d  Blocks=%d  Misses=%d  Streak=%d",
	             score, level, ((level - 1)*NUM_BLOCKS + blocks_hit), misses, high_streak);
    
      string_color = PAUSE_COLOR ;

    }// end RUN/PAUSE differential code
    
    // DRAW THE BALL
	  Draw_Ball();
    
    // draw the info string
    bb.GDI_Draw_Text( buffer, 8, TEXT_Y, string_color );
    
    // flip the surfaces
    bb.DD_Flip();
    
    // check if user has pressed a key
    Check_Keys( main_window_handle );
    
    // sync to desired FPS
    Clock_wait( (DWORD)WAIT_INTERVAL );
    
  }// end GAME_STATE_RUN or GAME_STATE_PAUSE

}// Freak::Game_Main()

//=============================================================================

void Freak::Init_Blocks()
{
  UCHAR block_row_color_shift  = MAX_COLORS / NUM_BLOCK_ROWS ,
        block_col_color_shift  = block_row_color_shift / NUM_BLOCK_COLS ,
        block_base_color_shift = PINK + 1 ;
  
  // initialize color values for the block field
  for( UCHAR row=0; row < NUM_BLOCK_ROWS; row++ )
    for( UCHAR col=0; col < NUM_BLOCK_COLS; col++ )
      blocks[row][col] = ( row * block_row_color_shift ) + ( col * block_col_color_shift )
                         + block_base_color_shift ;
  
  // make a noise to indicate a new level is starting
  if( level > 1 )
    MessageBeep( MB_ICONASTERISK );

}// Freak::Init_Blocks()

//=============================================================================

void Freak::Draw_Blocks()
{
  // this function draws all the blocks in row major form
  int x1 = BLOCK_ORIGIN_X, // used to track current position
      y1 = BLOCK_ORIGIN_Y ; 
  
  // draw all the blocks
  for( int row=0; row < NUM_BLOCK_ROWS; row++ )
  {    
    // reset column position
    x1 = BLOCK_ORIGIN_X ;
    
    // draw this row of blocks
    for( int col=0; col < NUM_BLOCK_COLS; col++ )
    {
      // draw next block (if there is one)
      if( blocks[row][col] != 0 )
      {
        // draw shadow then block     
        bb.DD_Blt_Rect( x1-SHADOW_OFFSET, y1+SHADOW_OFFSET,
                        x1+BLOCK_WIDTH-SHADOW_OFFSET,
				    			      y1+BLOCK_THICKNESS+SHADOW_OFFSET, SHADOW_COLOR );
        
        bb.DD_Blt_Rect( x1, y1, x1+BLOCK_WIDTH,
                        y1+BLOCK_THICKNESS, blocks[row][col] );
      } // end if
      
      // advance column position
      x1 += BLOCK_X_GAP ;
    
    }// end for(col)
    
    // advance to next row position
    y1 += BLOCK_Y_GAP ;
  
  }// end for(row)

}// Freak::Draw_Blocks()

//=============================================================================

void Freak::Draw_Paddle( int mouse_x )
{
  // keep track of direction paddle is moving
  paddle_prev_x = paddle_x ;
  // move the paddle
  paddle_x = mouse_x ;
 
  // make sure paddle doesn't go off screen
  if( paddle_x > (SCREEN_WIDTH - PADDLE_WIDTH) )
    paddle_x = SCREEN_WIDTH - PADDLE_WIDTH;
  else
    if( paddle_x < 0 )
      paddle_x = 0;
  
  // draw paddle
  bb.DD_Blt_Rect( paddle_x, PADDLE_Y, paddle_x + PADDLE_WIDTH, 
                  PADDLE_Y + PADDLE_THICKNESS, PADDLE_COLOR    );
  
}// Freak::Draw_Paddle()

//=============================================================================

void Freak::Move_Ball()
{
  // move the ball
  ball_x += ball_dx ;
  ball_y += ball_dy ;
  
  // KEEP BALL ON SCREEN, IF THE BALL HITS THE EDGE OF 
  // SCREEN THEN BOUNCE IT BY REFLECTING ITS VELOCITY
  
  // first x-axis
  if( ball_x > (SCREEN_WIDTH - BALL_SIZE) || ball_x < 0 ) 
  {
    // reflect x-axis velocity
    ball_dx = -ball_dx ;
	  // update position 
    ball_x += ball_dx ;
  }
  
  // now y-axis
  // test if ball went by all the blocks
  if( ball_y < 0 ) 
  {
    // reflect y-axis velocity
    ball_dy = -ball_dy ;
    // update position 
    ball_y += ball_dy ;
  }
  else 
    // penalize player for missing the ball
    if( ball_y > (SCREEN_HEIGHT - BALL_SIZE) )
    {
      // make a little noise to indicate missing the ball
      MessageBeep( MB_ICONEXCLAMATION );
      
      // reflect y-axis velocity
      ball_dy = -ball_dy ;
	  	// update position 
      ball_y += ball_dy ;
      
		  // re-set streak
		  current_streak = 0 ;

		  // keep track of misses and 
      // decrement the score except for the initial miss
      if( ++misses > 1 )
				score -= ((level*DEC_MULTIPLIER) + BASE_DECREMENT); 
      
    } // end if

}// Freak::Move_Ball()

//=============================================================================

void Freak::Process_Ball_Hits()
{
  // test if the ball has hit a block or the paddle
  // if so, the ball is bounced and the block is removed from the field
  // Note: very cheesy collision algorithm :)
  
  // first test for ball block collisions
  // the algorithm basically tests the ball against each block's bounding box
  // this is inefficient, but easy to implement, later we'll see a better way
  
  // extract leading edge of ball
  int ball_lx = ball_x + BALL_SIZE , 
      ball_ly = ball_y + BALL_SIZE ;
  
  // TEST IF BALL IS CLOSE TO AND APPROACHING THE PADDLE
  if( (ball_ly >= PADDLE_Y) && (ball_dy > 0) )
  {
    // test for collision with paddle
    if( (ball_lx >= paddle_x) && (ball_x <= (paddle_x + PADDLE_WIDTH)) )
    {
      // reflect ball
      ball_dy = -ball_dy ;
      
      // push ball out of paddle since it made contact
      ball_y += ball_dy ;
      
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
      
      /* test if there are no blocks (all blocks have been hit),
		     if so, send a message to game loop to start another level  */
      if( blocks_hit >= NUM_BLOCKS )
      {
        game_state = GAME_STATE_START ;
        level++ ;
        
        // increase vertical speed if not yet at max
        if( abs(ball_dy) < MAX_BALL_DY )
          if( ball_dy >= 0 )
	          ball_dy = ball_dy + BALL_DY_INCREMENT ;
          else
	            ball_dy = ball_dy - BALL_DY_INCREMENT ;
      }
		  else
         // make a little noise to indicate ball hitting the paddle
         MessageBeep( MB_OK );
      
      return ;
    
    }// if( collision )
  
  }// if( ball near paddle )
  
  int y1 = BLOCK_ORIGIN_Y ; // current rendering position
  
  int ball_cx = ball_x + BALL_SIZE/2 , // compute center of ball
      ball_cy = ball_y + BALL_SIZE/2 ;
  
  // NOW SCAN THRU ALL THE BLOCKS AND SEE IF BALL HIT BLOCKS
  for( int row = 0; row < NUM_BLOCK_ROWS; row++ )
  {    
    // reset column position
    int x1 = BLOCK_ORIGIN_X ;
    
    // scan the cols in this row of blocks
    for( int col = 0; col < NUM_BLOCK_COLS; col++ )
    {
      // if there is a block here then test it against ball
      if( blocks[row][col] != 0 )
      {
        // test ball against bounding box of block
        if( (ball_cx > x1) && (ball_cx < x1+BLOCK_WIDTH) &&     
            (ball_cy > y1) && (ball_cy < y1+BLOCK_THICKNESS) )
        {
          // remove the block
          blocks[row][col] = 0 ;
          
          // increment global block counter, so we know 
          // when to start a new level
          blocks_hit++ ;
          
			    current_streak++ ;
			    // keep track of streak of blocks hit on one shot
			    if( current_streak > high_streak )
			      high_streak = current_streak ;
          
          // bounce the ball
          ball_dy = -ball_dy ;
          
          // add a little ENGLISH
          ball_dx += (-1 + rand() % (ENGLISH-2));
          
          // make a little noise to indicate ball hitting a block
          MessageBeep( MB_ICONHAND );
          
          // add some points
          score += level * (abs(ball_dx)+abs(ball_dy)+(current_streak/5));
          
          return ; // that's it -- no more blocks to check
          
        }// if( block here )
      
      }// if( ball has hit )
      
      // advance column position
      x1 += BLOCK_X_GAP ;
      
    }// for( col )
    
    // advance to next row position
    y1 += BLOCK_Y_GAP ;
    
  }// for( row )

}// Freak::Process_Ball_Hits()

//=============================================================================

void Freak::Draw_Ball()     
{
  // watch out for ball velocity getting out of hand
  if( ball_dx > MAX_BALL_DX )
    ball_dx = MAX_BALL_DX ;
  else
    if( ball_dx < -MAX_BALL_DX )
      ball_dx = -MAX_BALL_DX ;    
  
  // ORIGINAL
//  bb.DD_Blt_Rect( ball_x, ball_y, ball_x+BALL_SIZE, 
//                  ball_y+BALL_SIZE, BALL_COLOR      );
  
  // DRAW THE HAPPY_BITMAP AS THE BALL
  bb.DD_Draw_Surface( ball_x, ball_y );
  
}// Freak::Draw_Ball()

//=============================================================================

void Freak::Check_Keys( HWND main_window_handle )
{
  // check if user is trying to pause
  if( KEY_DOWN(VK_END) )
    game_state = GAME_STATE_PAUSE ;
  
  // check if user is trying to resume
  else
    if( KEY_DOWN(VK_HOME) )
      game_state = GAME_STATE_RUN ;
  
    // check if user is trying to exit
    else
      if( KEY_DOWN(VK_ESCAPE) )
      {
        // set exit state
        game_state = GAME_STATE_SHUTDOWN ;
        
        // send a message to Windows to exit
        /*MHS: post a 'Destroy' msg to the message queue
							 - we will retrieve and process it from the main event loop in Winmain();
							   ultimately WindowProc() will post the Quit message we need 
								 to exit the main game loop in WinMain()	*/
        PostMessage( main_window_handle, WM_DESTROY, 0, 0 );
      }

}// Freak::Check_Keys()

//=============================================================================

void Freak::Game_Shutdown()
{
  // shut everything down and release resources
  bb.DD_Shutdown();
}

//=============================================================================

DWORD Freak::Clock_read()
{
  // return the current tick count
  return GetTickCount();
}

//=============================================================================

void Freak::Clock_wait( DWORD count )
{
  // wait for a specific number of ticks since the start of frame processing
  while( (Clock_read() - frame_start) < count )
    ; // wait
}
