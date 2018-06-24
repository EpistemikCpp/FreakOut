// freakMain.cpp

// TODO: store high values in registry - from Game_Shutdown()

#include <windows.h>       // include important windows stuff
#include <windowsx.h> 
#include <mmsystem.h>

#include <iostream>        // include important C/C++ stuff

#include "blackbox.h"
#include "freakout.h"

#define WINDOW_CLASS_NAME "MHS_WIN3DCLASS" 
#define WINDOW_TITLE      "WIN3D Game Console"

// NEED A GLOBAL TO PASS THE MOUSE X-POSITION
int mouse_x = 0 ;   

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
        } break ;

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


// WINMAIN ///////////////////////////////////////////////////////

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
								  WINDOW_TITLE,          // title
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

   // save the window handle in a global
   HWND main_window_handle = hwnd;

   // CREATE A FREAKOUT OBJECT
   Freak theGame ;

   // perform all game initialization
   theGame.Game_Init(main_window_handle);

   // ENTER MAIN EVENT LOOP
   while( TRUE )
		 {
	    if( PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) )
				{ 
		     // test if this is a quit
         if( msg.message == WM_QUIT ) break;
	          
    		 // translate any accelerator keys
		     TranslateMessage(&msg);
                
		     // send the message to the window proc
		     DispatchMessage(&msg);
				}
        // end if(PeekMessage)
             
      // main game processing goes here
      theGame.Game_Main(main_window_handle, mouse_x);
               
		 } // end while

   // shutdown game and release all resources
   theGame.Game_Shutdown();

   // show mouse
   ShowCursor(TRUE);

   // return to Windows like this
   return(msg.wParam);

	}
// WinMain()
