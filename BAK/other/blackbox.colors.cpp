// BLACKBOX.CPP - Game Engine 
 
#define INITGUID  // NEED THIS FOR LINKER TO FIND SYMBOL 'IID_IDIRECTDRAW4'

#include <windows.h>    // include important windows stuff
#include <windowsx.h> 
#include <mmsystem.h>

#include <iostream>     // include important C/C++ stuff

#include <ddraw.h>      // directX includes

#include "blackbox.h"   // game library includes
                                   
/* NEED A GLOBAL LPDIRECTDRAWSURFACE4 TO USE AS THE DEFAULT PARAMETER IN THE 
   DIRECT_DRAW FUNCTIONS - THIS PARAMETER NEEDS TO BE DECLARED IN BLACKBOX.CPP
   AND REFERENCED AS AN EXTERN IN BLACKBOX.H -- FROM BLACKBOX.H IT IS VISIBLE 
   IN FREAKOUT.CPP & FREAKMAIN.CPP */
 LPDIRECTDRAWSURFACE4  lpddsback;    // dd back surface


// FUNCTION DEFINITIONS /////////////////////////////////////////////

// CONSTRUCTOR - DO INITIALIZATION
Blackbox::Blackbox(int screen_h, int screen_w, int screen_b)
  {
  lpdd         = NULL;   // dd object
  lpddsprimary = NULL;   // dd primary surface
  lpddsback    = NULL;   // dd back surface
  lpddpal      = NULL;   // a pointer to the created dd palette
  lpddclipper  = NULL;   // dd clipper
  clock_start = 0;       // used for timing

// these define the general clipping rectangle
    min_clip_x = 0 ;                       
    max_clip_x = screen_w-1 ;
    min_clip_y = 0 ;
    max_clip_y = screen_h-1;

// these are overwritten globally by DD_Init()
    screen_width  = screen_w ;       // width of screen
    screen_height = screen_h ;       // height of screen
    screen_bpp    = screen_b ;       // bits per pixel
  }
// CONSTRUCTOR

// this function initializes Directdraw
int Blackbox::DD_Init(HWND main_window_handle, int width, int height, int bpp)
	{
   LPDIRECTDRAW lpdd1 = NULL; // used to query for the latest interface

   // create object and test for error
   if( DirectDrawCreate(NULL, &lpdd1, NULL) != DD_OK )
     return 0;

   // query for the latest version of the interface (4)
   if( lpdd1->QueryInterface(IID_IDirectDraw4, (LPVOID *)&lpdd) != DD_OK ) 
     return 0;

   // set cooperation level to windowed mode normal
   if( lpdd->SetCooperativeLevel(main_window_handle,
                                 DDSCL_ALLOWMODEX | DDSCL_FULLSCREEN |
                                 DDSCL_EXCLUSIVE | DDSCL_ALLOWREBOOT) != DD_OK )
     return 0;

   // set the display mode
   if( lpdd->SetDisplayMode(width, height, bpp, 0, 0) != DD_OK )
     return 0;

   // set globals
   screen_height = height;
   screen_width  = width;
   screen_bpp    = bpp;

   // Create the primary surface
   memset(&ddsd, 0, sizeof(ddsd));
   ddsd.dwSize = sizeof(ddsd);
   ddsd.dwFlags = DDSD_CAPS | DDSD_BACKBUFFERCOUNT;

   // we need to let dd know that we want a complex 
   // flippable surface structure, set flags for that
   ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE | DDSCAPS_FLIP | DDSCAPS_COMPLEX;

   // set the backbuffer count to 1
   ddsd.dwBackBufferCount = 1;

   // create the primary surface
   lpdd->CreateSurface(&ddsd, &lpddsprimary, NULL);

   // query for the backbuffer i.e the secondary surface
   ddscaps.dwCaps = DDSCAPS_BACKBUFFER;
   lpddsprimary->GetAttachedSurface(&ddscaps, &lpddsback);

   // CREATE AND ATTACH PALETTE
   // define palette data
   Define_palette() ;

   // now create the palette object
   if( lpdd->CreatePalette(DDPCAPS_8BIT | DDPCAPS_INITIALIZE | DDPCAPS_ALLOW256,
		                       palette, &lpddpal, NULL) != DD_OK )
     return 0;

   // attach the palette to the primary
   if( lpddsprimary->SetPalette(lpddpal) != DD_OK ) return(0);


   // clear out both primary and secondary surfaces
   DD_Fill_Surface(lpddsprimary, 0);
   DD_Fill_Surface(lpddsback, 0);

   // attach a clipper to the screen
   RECT screen_rect = {0, 0, screen_width, screen_height};
   lpddclipper = DD_Attach_Clipper(lpddsback, 1, &screen_rect);

   // return success
   return(TRUE);
	}
// DD_Init()

///////////////////////////////////////////////////////////////

int Blackbox::DD_Shutdown(void)
{
 // this function release all the resources directdraw
 // allocated, mainly to com objects

 // release the clipper first
 if( lpddclipper ) lpddclipper->Release();

 // release the palette
 if( lpddpal ) lpddpal->Release();

 // release the secondary surface
 if( lpddsback ) lpddsback->Release();

 // release the primary surface
 if( lpddsprimary ) lpddsprimary->Release();

 // finally, the main dd object
 if( lpdd ) lpdd->Release();

 // return success
 return(TRUE);
}
// DD_Shutdown()

///////////////////////////////////////////////////////////////

// create a R,G,B,GR gradient palette
void Blackbox::Define_palette(void)
  {
   // set all values to zero first - defensive programming
   memset(palette, 0, 256*sizeof(PALETTEENTRY));
   // SET COLORS
   for( int index = 0; index < 256; index++ )
     {
      // set each entry
      if (index == 0) {    // black for shadows
				palette[index].peBlue = 101 ;
        palette[index].peGreen = 255 ;
        palette[index].peRed = 199 ;
        // 255:199:101 = turquoise
        // 101:255:199 = bright yellow-green
        }
       else if (index < 64) // bright shades of red
           palette[index].peRed = 127 + index*2; 
        else if (index == 64)  // cyan for score
						{
						 palette[index].peBlue  = 227 ;
						 palette[index].peGreen = 227 ;
						}
         else if (index > 64 && index < 128) // bright shades of green
             palette[index].peGreen = 127 + (index-64)*2;
          else if (index == 128)  // magenta for score
						{
						 palette[index].peBlue = 199 ; //227 ;
						 palette[index].peRed  = 255 ; //227 ;
             palette[index].peGreen = 101 ;
             // 199:255:101 = true pink
             // 247:157:0 = violet
             // 157:247:0 = deep pink
						}
           else if (index > 128 && index < 192) // bright shades of blue
              palette[index].peBlue = 127 + (index-128)*2;
					  else if (index == 192)  // yellow for paddle
							{
						   palette[index].peRed = 247 ;// orange? // 227
						   palette[index].peGreen = 157 ; // 227
							}
						 else if (index > 192 && index < 256) // greys for background & ball
                palette[index].peRed = palette[index].peGreen = 
                   palette[index].peBlue = (index-192)*4;
      // set flags
      palette[index].peFlags = PC_NOCOLLAPSE;
     } // end for(index)
  }
// define_palette()

///////////////////////////////////////////////////////////////   

LPDIRECTDRAWCLIPPER Blackbox::DD_Attach_Clipper( LPDIRECTDRAWSURFACE4 lpdds,
                                                 int num_rects,
                                                 LPRECT clip_list )
{
// this function creates a clipper from the sent clip list and attaches
// it to the sent surface

 int index;                        // looping var
 LPDIRECTDRAWCLIPPER lpddclipper;  // pointer to the newly created dd clipper
 LPRGNDATA region_data;            // pointer to the region data that contains
                                   // the header and clip list

// first create the direct draw clipper
if( (lpdd->CreateClipper(0, &lpddclipper, NULL)) != DD_OK ) return(NULL);

// now create the clip list from the sent data

// first allocate memory for region data
region_data = (LPRGNDATA)malloc(sizeof(RGNDATAHEADER)+num_rects*sizeof(RECT));

// now copy the rects into region data
memcpy(region_data->Buffer, clip_list, sizeof(RECT)*num_rects);

// set up fields of header
region_data->rdh.dwSize          = sizeof(RGNDATAHEADER);
region_data->rdh.iType           = RDH_RECTANGLES;
region_data->rdh.nCount          = num_rects;
region_data->rdh.nRgnSize        = num_rects*sizeof(RECT);

region_data->rdh.rcBound.left    =  64000;
region_data->rdh.rcBound.top     =  64000;
region_data->rdh.rcBound.right   = -64000;
region_data->rdh.rcBound.bottom  = -64000;

// find bounds of all clipping regions
for( index=0; index<num_rects; index++ )
   {
    // test if the next rectangle unioned with the current bound is larger
    if( clip_list[index].left < region_data->rdh.rcBound.left )
      region_data->rdh.rcBound.left = clip_list[index].left;

    if( clip_list[index].right > region_data->rdh.rcBound.right )
      region_data->rdh.rcBound.right = clip_list[index].right;

    if( clip_list[index].top < region_data->rdh.rcBound.top )
      region_data->rdh.rcBound.top = clip_list[index].top;

    if( clip_list[index].bottom > region_data->rdh.rcBound.bottom )
      region_data->rdh.rcBound.bottom = clip_list[index].bottom;

   } // end for(index)

// now we have computed the bounding rectangle region and set up the data
// now let's set the clipping list

if( (lpddclipper->SetClipList(region_data, 0)) != DD_OK )
  {
   // release memory and return error
   free(region_data);
   return(NULL);
  } // end if

// now attach the clipper to the surface
if( (lpdds->SetClipper(lpddclipper)) != DD_OK )
  {
   // release memory and return error
   free(region_data);
   return(NULL);
  } // end if

// all is well, so release memory and send back the pointer to the new clipper
free(region_data);
return(lpddclipper);
}
// DD_Attach_Clipper()

///////////////////////////////////////////////////////////////
   
int Blackbox::DD_Flip(void)
{
 // this function flips the primary surface with the secondary surface

 // flip pages
 while( lpddsprimary->Flip(NULL, DDFLIP_WAIT) != DD_OK )
   ;// wait

 // return success
 return(TRUE);
}
// DD_Flip()

///////////////////////////////////////////////////////////////

DWORD Blackbox::Start_Clock(void)
{
 // this function starts the clock, that is, saves the current count,
 // use in conjunction with Wait_Clock()
 return(clock_start = Get_Clock());
}
// Start_Clock()

///////////////////////////////////////////////////////////////

DWORD Blackbox::Get_Clock(void)
{
 // this function returns the current tick count
 return(GetTickCount());
}
// Get_Clock()

///////////////////////////////////////////////////////////////

DWORD Blackbox::Wait_Clock(DWORD count)
{
 // this function is used to wait for a specific number of clicks
 // since the call to Start_Clock
 while( (Get_Clock() - clock_start) < count )
	; // wait
 
 return(Get_Clock());
}
// Wait_Clock()

///////////////////////////////////////////////////////////////

int Blackbox::DD_Fill_Surface(LPDIRECTDRAWSURFACE4 lpdds, int color)
{
 DDBLTFX ddbltfx; // this contains the DDBLTFX structure

 // clear out the structure and set the size field 
 DD_INIT_STRUCT(ddbltfx);

 // set the dwfillcolor field to the desired color
 ddbltfx.dwFillColor = color; 

 // ready to blt to surface
 lpdds->Blt( NULL,        // ptr to dest rectangle
             NULL,        // ptr to source surface, NA            
             NULL,        // ptr to source rectangle, NA
             DDBLT_COLORFILL | DDBLT_WAIT | DDBLT_ASYNC,   // fill and wait                   
             &ddbltfx );  // ptr to DDBLTFX structure

 // return success
 return(TRUE);
}
// DD_Fill_Surface()

///////////////////////////////////////////////////////////////   

int Blackbox::Draw_Rectangle( int x1, int y1, int x2, int y2, int color,
                              LPDIRECTDRAWSURFACE4 lpdds )
{
// this function uses directdraw to draw a filled rectangle

DDBLTFX ddbltfx; // this contains the DDBLTFX structure
RECT fill_area;  // this contains the destination rectangle

// clear out the structure and set the size field 
DD_INIT_STRUCT(ddbltfx);

// set the dwfillcolor field to the desired color
ddbltfx.dwFillColor = color; 

// fill in the destination rectangle data (your data)
fill_area.top    = y1;
fill_area.left   = x1;
fill_area.bottom = y2+1;
fill_area.right  = x2+1;

// ready to blt to surface, in this case blt to primary
lpdds->Blt( &fill_area,  // ptr to dest rectangle
            NULL,        // ptr to source surface, NA            
            NULL,        // ptr to source rectangle, NA
            DDBLT_COLORFILL | DDBLT_WAIT | DDBLT_ASYNC,  // fill and wait                   
            &ddbltfx );  // ptr to DDBLTFX structure

// return success
return(TRUE);
}
// Draw_Rectangle()

///////////////////////////////////////////////////////////////

int Blackbox::Draw_Text_GDI( char *text, int x, int y, int color,
									           LPDIRECTDRAWSURFACE4 lpdds )
{
 // this function draws the sent text on the sent surface 
 // using color index as the color in the palette

 HDC xdc; // the working dc

 // get the dc from surface
 if( lpdds->GetDC(&xdc) != DD_OK ) return(0);

 // set the colors for the text up
 SetTextColor( xdc, RGB( palette[color].peRed, palette[color].peGreen,
							  			   palette[color].peBlue ) );

 // set background mode to transparent so black isn't copied
 SetBkMode(xdc, TRANSPARENT);

 // draw the text
 TextOut(xdc, x, y, text, strlen(text));

 // release the dc
 lpdds->ReleaseDC(xdc);

 // return success
 return(TRUE);
}
// Draw_Text_GDI()
