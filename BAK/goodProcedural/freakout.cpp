// FREAKOUT.CPP - breakout-type game demo

#include "freakout.h"

// THIS IS THE MAIN MESSAGE HANDLER OF THE SYSTEM
LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
	{
   PAINTSTRUCT ps;	// used in WM_PAINT

   // what is the message? 
   switch(msg)
		 {	
	    case WM_CREATE: // do initialization stuff here
				{
		     return(0);
				} break;
         
			case WM_MOUSEMOVE: // use mouse x-position to control the paddle
				{
				 mouse_x = (int)LOWORD(lparam) ;
				}

      case WM_PAINT:
				{
         // start painting
         BeginPaint(hwnd, &ps); // the window is now validated
         // end painting
         EndPaint(hwnd, &ps);
         return(0);
				} break;
      
	    case WM_DESTROY: 
				{
		     // kill the application			
		     PostQuitMessage(0);
		     return(0);
				}

	    default: break;
		 } // end switch

   // process any messages that we didn't take care of 
   return (DefWindowProc(hwnd, msg, wparam, lparam));
	} // end WinProc


// WINMAIN ////////////////////////////////////////////////////

 int WINAPI WinMain( HINSTANCE hinstance, HINSTANCE hprevinstance,
										 LPSTR lpcmdline, int ncmdshow )
	{
   WNDCLASS winclass;	// this will hold the class we create
   HWND hwnd;		      // generic window handle
   MSG msg;		        // generic message

   // first fill in the window class stucture
   winclass.style = CS_DBLCLKS | CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
   winclass.lpfnWndProc = WindowProc;
   winclass.cbClsExtra = 0;
   winclass.cbWndExtra = 0;
   winclass.hInstance = hinstance;
   winclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
   winclass.hCursor = LoadCursor(NULL, IDC_ARROW);
   winclass.hbrBackground	= (HBRUSH)GetStockObject(BLACK_BRUSH);
   winclass.lpszMenuName = NULL; 
   winclass.lpszClassName	= WINDOW_CLASS_NAME;

   // register the window class
   if( !RegisterClass(&winclass) ) return(0);

   // create the window - note the use of WS_POPUP to enable full screen
   if( !(hwnd = CreateWindow(
								  WINDOW_CLASS_NAME,     // class
								  "WIN3D Game Console",	 // title
								  WS_POPUP | WS_VISIBLE, // window style = full screen
								  0, 0,	                 // initial x,y
   							  GetSystemMetrics(SM_CXSCREEN),  // intial width
								  GetSystemMetrics(SM_CYSCREEN),  // initial height
								  NULL,	     // handle to parent 
								  NULL,	     // handle to menu
								  hinstance, // handle to instance of this app
								  NULL )     // pointer to extra creation params
			) )	
      return(FALSE);

   // hide mouse
   ShowCursor(FALSE);

   // save the window handle and instance in a global
   main_window_handle = hwnd;
   main_instance      = hinstance;

   // perform all game console specific initialization
   Game_Init();

   // ENTER MAIN EVENT LOOP
   while( TRUE )
		 {
	    if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
				{ 
		     // test if this is a quit
         if( msg.message == WM_QUIT ) break;
	          
    		 // translate any accelerator keys
		     TranslateMessage(&msg);
                
		     // send the message to the window proc
		     DispatchMessage(&msg);
				} // end if
             
      // main game processing goes here
      Game_Main();
               
		 } // end while

   // shutdown game and release all resources
   Game_Shutdown();

   // show mouse
   ShowCursor(TRUE);

   // return to Windows like this
   return(msg.wParam);

	}
// WinMain()


////// T3DX GAME PROGRAMMING CONSOLE FUNCTIONS ////////////////////

int Game_Init(void *parms)
	{
   // this function is where you do all the initialization for your game

	 // initialize graphics
   DD_Init(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP);

   // seed the random number generator so game is different each play
   srand(Start_Clock());

   // set the paddle position here to the middle bottom
   paddle_x = PADDLE_START_X;

   // set ball starting position and velocity
   ball_x = 8 + rand()%(SCREEN_WIDTH-16);
   ball_y = BALL_START_Y;
   ball_dx = MAX_BALL_X_VELOCITY - ENGLISH ;
   ball_dy = BALL_Y_VELOCITY ; // + rand()%2;

   // transition to start level state
   game_state = GAME_STATE_START_LEVEL;

	 return(TRUE);   // return success
	}
// Game_Init()


// GAME_MAIN - this is the workhorse of your game 
//           - it will be called continuously in real-time 
//           - this is like main() in C - all the calls for your game go here!
int Game_Main(void *parms)
	{
   char buffer[120]; // used to print text

 // START
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

 // RUN
   else if( game_state == GAME_STATE_RUN )
    {
     // start the timing clock
     Start_Clock();

     // clear drawing surface for the next frame of animation
     Draw_Rectangle(0, 0, SCREEN_WIDTH-1, SCREEN_HEIGHT-1, BACKGROUND_COLOR);

     // DRAW THE BLOCKS
     Draw_Blocks();

     // DRAW THE PADDLE
		 Draw_Paddle() ;

		 // MOVE THE BALL
		 Move_Ball() ;

     // TEST IF BALL HIT ANY BLOCKS OR THE PADDLE
     Process_Ball_Hits();

     // DRAW THE BALL
		 Draw_Ball() ;
 
     // draw the game info
     sprintf(buffer, "SUPER-FREAKMOUSE!   Score=%d   Level=%d   Blocks=%d\
     Misses=%d   Streak=%d   'Home' to pause, 'Esc' to exit",
			       score, level, ((level - 1)*48 + blocks_hit), misses, high_streak ) ;
 
		 // change color depending on the score
     if( score <= LO_SCORE )
		   Draw_Text_GDI(buffer, 8, SCREEN_HEIGHT-16, LO_SCORE_COLOR) ;
		 else if( score < 0 )
		        Draw_Text_GDI(buffer, 8, SCREEN_HEIGHT-16, NEG_SCORE_COLOR) ;
        else if( score < HI_SCORE )
			         Draw_Text_GDI(buffer, 8, SCREEN_HEIGHT-16, POS_SCORE_COLOR) ;
           else Draw_Text_GDI(buffer, 8, SCREEN_HEIGHT-16, HI_SCORE_COLOR) ;

     // flip the surfaces
     DD_Flip();

     // check if user has pressed a key
     Check_Keys() ;

     // sync to approx 33 fps
     Wait_Clock(30);

		} // end if

 // PAUSE
   else if( game_state == GAME_STATE_PAUSE )
    {
     // start the timing clock
     Start_Clock();

     // clear drawing surface for the next frame of animation
     Draw_Rectangle(0, 0, SCREEN_WIDTH-1, SCREEN_HEIGHT-1, BACKGROUND_COLOR);

     // draw blocks
     Draw_Blocks();

     // draw the paddle
     Draw_Rectangle( paddle_x, PADDLE_Y, paddle_x + PADDLE_WIDTH, 
                     PADDLE_Y + PADDLE_HEIGHT, PADDLE_COLOR );

     // draw the ball
     Draw_Rectangle( ball_x, ball_y, ball_x+BALL_SIZE, 
                     ball_y+BALL_SIZE, BALL_COLOR );

     // draw the game info
     sprintf(buffer,"Game Paused!  Press 'End' to resume.\
     Score=%d  Level=%d  Blocks=%d  Misses=%d  Streak=%d",
			       score, level, ((level - 1)*48 + blocks_hit), misses, high_streak ) ;
		 Draw_Text_GDI(buffer, 8, SCREEN_HEIGHT-16, NEG_SCORE_COLOR) ;

     // flip the surfaces
     DD_Flip();

     // check if user has pressed a key
     Check_Keys() ;

     // sync to approx 33 fps
     Wait_Clock(30);

	} // end if

 // SHUTDOWN
 return(TRUE);
	
 }
// Game_Main() /////////////////////////////////////////////////


void Init_Blocks(void)
	{
   // initialize the block field
   for( int row=0; row < NUM_BLOCK_ROWS; row++ )
     for( int col=0; col < NUM_BLOCK_COLUMNS; col++ )
        blocks[row][col] = row*22 + col*4 + 3 ;
		 //row*16 + col*3 + 16

	 // make a noise to indicate a new level is starting
   MessageBeep(MB_ICONASTERISK);
	} // end Init_Blocks


void Draw_Blocks(void)
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
      for( int col=0; col < NUM_BLOCK_COLUMNS; col++ )
        {
         // draw next block (if there is one)
         if (blocks[row][col] != 0)
           {
            // draw shadow then block     
            Draw_Rectangle( x1-4, y1+4, x1+BLOCK_WIDTH-4,
							              y1+BLOCK_HEIGHT+4, SHADOW_COLOR );

            Draw_Rectangle( x1, y1, x1+BLOCK_WIDTH,
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


void Draw_Paddle(void)
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
	 Draw_Rectangle( paddle_x, PADDLE_Y, 
                   paddle_x + PADDLE_WIDTH, 
                   PADDLE_Y + PADDLE_HEIGHT, PADDLE_COLOR );
	}
// Draw_Paddle()


void Move_Ball(void)
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

		    // keep track of misses
		    misses++ ;

        // decrement the score except for initial miss
        if( misses > 0 ) score -= ((level*SCORE_MULTIPLIER) + BASE_SCORE); 

			 } // end if
	}
// Move_Ball()


void Process_Ball_Hits(void)
{
// this function tests if the ball has hit a block or the paddle
// if so, the ball is bounced and the block is removed from the playfield
// Note: very cheesy collision algorithm :)

// first test for ball block collisions
// the algorithm basically tests the ball against each block's bounding box
// this is inefficient, but easy to implement, later we'll see a better way

int x1 = BLOCK_ORIGIN_X, // current rendering position
    y1 = BLOCK_ORIGIN_Y; 
   
int ball_lx = ball_x + BALL_SIZE , // extract leading edge of ball
    ball_ly = ball_y + BALL_SIZE ;

// TEST IF BALL IS CLOSE TO AND APPROACHING THE PADDLE
if( (ball_ly >= PADDLE_Y) && ball_dy > 0 )
  {
   // test for collision with paddle
   if( ball_lx >= paddle_x && ball_lx <= paddle_x + PADDLE_WIDTH )
       // && (ball_ly >= PADDLE_Y && ball_ly <= PADDLE_Y + PADDLE_HEIGHT) )
		 {
       // reflect ball
       ball_dy = -ball_dy;

       // push ball out of paddle since it made contact
       ball_y += ball_dy;

       // add a little ENGLISH to ball based on motion of paddle
			 int paddle_direction = paddle_x - paddle_prev_x ;

       if( paddle_direction > 0 ) // paddle moving right
         ball_dx -= (rand() % ENGLISH);
       else if( paddle_direction < 0 ) // paddle moving left
           ball_dx += (rand() % ENGLISH);
           else // not moving
               ball_dx += (-(ENGLISH/2) + (rand() % ENGLISH));

			 // re-set streak
			 current_streak = 0 ;
       
       // test if there are no blocks (all blocks have been hit),
			 // if so, send a message to game loop to start another level
       if( blocks_hit >= (NUM_BLOCK_ROWS * NUM_BLOCK_COLUMNS) )
         {
          game_state = GAME_STATE_START_LEVEL;
          level++ ;
         } // end if
			 else
           // make a little noise to indicate ball hitting the paddle
           MessageBeep(MB_OK);

      return; 
     } // end if(collision)
  } // end if(ball near paddle)

int ball_cx = ball_x + (BALL_SIZE/2),  // compute center of ball
    ball_cy = ball_y + (BALL_SIZE/2);

// NOW SCAN THRU ALL THE BLOCKS AND SEE IF BALL HIT BLOCKS
for( int row = 0; row < NUM_BLOCK_ROWS; row++ )
   {    
    // reset column position
    x1 = BLOCK_ORIGIN_X;

    // scan the cols in this row of blocks
    for( int col = 0; col < NUM_BLOCK_COLUMNS; col++ )
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

               return ; // that's it -- no more blocks

              } // end if(block here)
          } // end if(ball has hit)

        // advance column position
        x1 += BLOCK_X_GAP;

			} // end for(col)

    // advance to next row position
    y1 += BLOCK_Y_GAP;

   } // end for(row)
 }
// Process_Ball_Hits()


void Draw_Ball(void)     
	{
   // watch out for ball velocity getting out of hand
   if( ball_dx > MAX_BALL_X_VELOCITY )
	   ball_dx = MAX_BALL_X_VELOCITY ;
   else
	     if( ball_dx < -MAX_BALL_X_VELOCITY ) ball_dx = -MAX_BALL_X_VELOCITY ;    

   Draw_Rectangle( ball_x, ball_y, ball_x+BALL_SIZE, 
                   ball_y+BALL_SIZE, BALL_COLOR );
	 }
// Draw_Ball()


void Check_Keys(void)
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
          game_state = GAME_STATE_SHUTDOWN;
          // send message to windows to exit
          PostMessage(main_window_handle, WM_DESTROY, 0, 0);
				 } // end if
	}
// Check_Keys()


int Game_Shutdown(void *parms)
	{
   // shut everything down and release resources
   DD_Shutdown();

   return(TRUE);   // return success
	}
// Game_Shutdown()