/**************************************************************************
 *
 *  Mark Sattolo (msattolo@sympatico.ca)
 * -----------------------------------------------
 *  $File: //depot/CppProj/VS6/FreakProj/Src/freakMain.cpp $
 *  $Revision: #4 $
 *  $Change: 70 $
 *  $DateTime: 2008/02/14 00:21:50 $
 *
 * ---------------------------------------------------------------------
 *  freakMain.cpp - WinMain() and WindowProc() implementation
 *		Created 2001, based on code in 
 *		'Tricks of the Windows Game Programming Gurus', by Andre Lamothe
 * 
 **************************************************************************/

#include "FreakProj.h"  // Windows & C++ includes
#include "blackbox.h"
#include "freakout.h"

#define WINDOW_CLASS_NAME "MHS_FREAKOUT_WINCLASS" 
#define WINDOW_TITLE      "RAINBOW FREAKOUT GAME"

// NEED A GLOBAL TO PASS THE MOUSE X-POSITION
int mouse_x = 0 ;   

// MAIN MESSAGE HANDLER OF THE SYSTEM
LRESULT CALLBACK WindowProc( HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam )
{
  PAINTSTRUCT ps ;
  
  // what is the message? 
  switch( msg )
  {	
	  case WM_CREATE: // do initialization stuff here
			return 0 ;
         
	  case WM_MOUSEMOVE: // use mouse x-position to control the paddle
			mouse_x = (int)LOWORD( lparam );
      break ;

    case WM_PAINT:
      BeginPaint( hwnd, &ps );
			// the window is now validated
      EndPaint( hwnd, &ps );
      return 0 ;
      
	  case WM_DESTROY: 
		  // kill the application			
		  PostQuitMessage( 0 );
		  return 0 ;

	  default: break ;

  } // switch( msg )

  // process any messages that we didn't take care of 
  return DefWindowProc( hwnd, msg, wparam, lparam );

} // WindowProc()


///// WINMAIN ////////////////////////////////////////////////////////////////

int WINAPI WinMain( HINSTANCE hinstance, HINSTANCE hprevinstance,
 										LPSTR lpcmdline, int ncmdshow )
{
  WNDCLASS winclass ;       // this will hold the class we create
  HWND main_window_handle ; // window handle
  MSG msg ;		              // generic message

  // first fill in the window class stucture
  winclass.style         = CS_DBLCLKS | CS_OWNDC | CS_HREDRAW | CS_VREDRAW ;
  winclass.lpfnWndProc   = WindowProc ;
  winclass.cbClsExtra    = 0 ;
  winclass.cbWndExtra    = 0 ;
  winclass.hInstance     = hinstance ;
  winclass.hIcon         = LoadIcon( NULL, IDI_APPLICATION );
  winclass.hCursor       = LoadCursor( NULL, IDC_ARROW );
  winclass.hbrBackground = (HBRUSH)GetStockObject( BLACK_BRUSH );
  winclass.lpszMenuName  = NULL ; 
  winclass.lpszClassName = WINDOW_CLASS_NAME ;
  
  // register the window class
  if( !RegisterClass( &winclass ) )
		return 0 ;
  
  // create the window - note the use of WS_POPUP to enable full screen
  if( !( main_window_handle =
           CreateWindow(
							 WINDOW_CLASS_NAME,     // class
							 WINDOW_TITLE,          // title
							 WS_POPUP | WS_VISIBLE, // window style == full screen
							 0, 0,	                // initial x, y
   						 GetSystemMetrics( SM_CXSCREEN ),  // intial width
							 GetSystemMetrics( SM_CYSCREEN ),  // initial height
							 NULL,	     // handle to parent 
							 NULL,	     // handle to menu
							 hinstance,  // handle to instance of this app
							 NULL  )     // pointer to extra creation params
       )  
	  )
		return false ;
  
  // hide mouse
  ShowCursor( false );
  
  // CREATE A FREAKOUT OBJECT
  Freak theGame ;
  
  // perform all game initialization
  bool initOK = theGame.Game_Init( main_window_handle );
  
  // IF INIT WAS OK, ENTER MAIN EVENT LOOP
  while( initOK )
  {
    // see if any messages are in our message queue
	  if( PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) )
    { 
		  // test if this is a Quit -- NEED this to exit from the loop
      if( msg.message == WM_QUIT )
        break ;
	          
    	// translate any accelerator keys
		  TranslateMessage( &msg );
                
		  // send the message to WindowProc() for processing
		  DispatchMessage( &msg );
    }
             
    // main game processing goes here
    theGame.Game_Main( main_window_handle, mouse_x );
              
  }// end while

  // shutdown the game and release all resources
  theGame.Game_Shutdown();

  // show mouse
  ShowCursor( true );

  // return to Windows like this
  return( msg.wParam );

} // WinMain()
