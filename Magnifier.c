#include "WinMain.h"		// standard application includes
#include "Magnifier.h"		// handles the magnifier window
#include "Resource.h"		// resource identifiers

/*/
/ / --------------------------------------------------------------------------------------------------------------------
/ /  Author:		Jeremy Falcon
/ /  Purpose:		Manages all aspects of the magnifier window when grab color is active.
/ / --------------------------------------------------------------------------------------------------------------------
/*/

static HWND hMagnifier = NULL;
static HDC hScreenDC = NULL;
static int nOldScreenDC = 0;
static HBITMAP hScreenBitmap = NULL;
static int nScreenWidth, nScreenHeight;

#define MAG_WIDTH	101
#define MAG_HEIGHT	75

LRESULT CALLBACK MagnifierProc (HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

/*/
/ / Adds the contents pointed to by szText to the
/ / Windows clipboard. Returns false if unsuccessful.
/*/

bool
CreateMagnifier (const HWND hParent)
{
	// only create the window if we haven't done so already
	if((hMagnifier == NULL) && (hParent != NULL) && IsWindow(hParent))
	{
		WNDCLASSEX	cls;

		// use a class that has no background by default
		cls.cbSize        = sizeof(WNDCLASSEX);
		cls.style         = CS_SAVEBITS|CS_HREDRAW|CS_VREDRAW;
		cls.lpfnWndProc   = MagnifierProc;
		cls.cbClsExtra    = 0;
		cls.cbWndExtra    = 0;
		cls.hIcon         = NULL;
		cls.hIconSm       = NULL;
		cls.hCursor       = NULL;
		cls.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);
		cls.lpszMenuName  = NULL;
		cls.lpszClassName = "{DD924F5A-037E-479b-912E-DDF2651A65C6}";
		cls.hInstance     = GetWindowInstance(hParent);

		// create the magnifier window
		hMagnifier = CreateWindowEx(WS_EX_NOPARENTNOTIFY, (LPCSTR)RegisterClassEx(&cls), NULL, WS_POPUP|WS_BORDER,
			0, 0, MAG_WIDTH, MAG_HEIGHT, hParent, NULL, GetWindowInstance(hParent), NULL);

		return (hMagnifier != NULL);
	}
	else
		return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*/
/ / void
/ /		DestroyMagnifier (void)
/ /
/ / PURPOSE:
/ /		Kills the magnifier window.
/*/

void
DestroyMagnifier (void)
{
	// clean up
	if((hScreenDC != NULL) || (hScreenBitmap != NULL))
	{
		RestoreDC(hScreenDC, nOldScreenDC);
		DeleteDC(hScreenDC);
		DeleteObject(hScreenBitmap);

		hScreenDC = NULL;
		hScreenBitmap = NULL;
	}

	if((hMagnifier != NULL) && IsWindow(hMagnifier))
	{
		DestroyWindow(hMagnifier);
		hMagnifier = NULL;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*/
/ / void
/ /		ShowMagnifier (const bool bShow)
/ /
/ /			bShow = flag to show or hide the window
/ /
/ / PURPOSE:
/ /		Shows or hidews the magnifier window.
/*/

void
ShowMagnifier (const bool bShow)
{
	if((hMagnifier != NULL) && IsWindow(hMagnifier))
	{
		if(bShow)
		{
			HDC hTempDC;

			// clean up
			if((hScreenDC != NULL) || (hScreenBitmap != NULL))
			{
				RestoreDC(hScreenDC, nOldScreenDC);
				DeleteDC(hScreenDC);
				DeleteObject(hScreenBitmap);

				hScreenDC = NULL;
				hScreenBitmap = NULL;
			}

			// update the screenshot dc if we have to show the window
			hTempDC = GetDC(NULL);
			nScreenWidth = GetDeviceCaps(hScreenDC, HORZRES);
			nScreenHeight = GetDeviceCaps(hScreenDC, VERTRES);

			// we'll use the screen to form the background of the magnifier window
			hScreenDC = CreateCompatibleDC(hTempDC);
			nOldScreenDC = SaveDC(hScreenDC);
			hScreenBitmap = CreateCompatibleBitmap(hTempDC, nScreenWidth, nScreenHeight);

			// blit it to the dC we'll use to paint the background
			SelectObject(hScreenDC, hScreenBitmap);
			BitBlt(hScreenDC, 0, 0, nScreenWidth, nScreenHeight, hTempDC, 0, 0, SRCCOPY);
			ReleaseDC(NULL, hTempDC);

			ShowWindow(hMagnifier, SW_SHOWNA);
		}
		else
			ShowWindow(hMagnifier, SW_HIDE);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*/
/ / void
/ /		SetMagnifierPos (const HWND hWndxy, const int x, const int y)
/ /
/ /			hWndxy = handle of the window underneath the cursor
/ /			x = horz coordinate
/ /			y = vert coordinate
/ /
/ / PURPOSE:
/ /		Changes the position of the magnifier window if the cursor is not aligned over it.
/*/

void
SetMagnifierPos (const HWND hWndxy, const int x, const int y)
{
	if((hMagnifier != NULL) && (hMagnifier != hWndxy) && IsWindow(hMagnifier))
	{
		SetWindowPos(hMagnifier, HWND_TOPMOST, x, y, 0, 0, SWP_NOACTIVATE|SWP_NOSIZE);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*/
/ / LRESULT CALLBACK
/ /		MagnifierProc (HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
/ /
/ / PURPOSE:
/ /		Messaging procedure for the magnifier window. Processes
/ /		messages sent by calling the appropriate functions.
/*/

LRESULT CALLBACK
MagnifierProc (HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
		case WM_MOUSEMOVE:

			// only process if the control key is held down
			if((wParam & MK_CONTROL) == MK_CONTROL)
			{
				POINT c;

				// quick way to get screen rather than client coordinates
				GetCursorPos(&c);

				// set the position of the window to have the cursor centered in it
				SetWindowPos(hMagnifier, HWND_TOPMOST, c.x - 50, c.y - 37, 0, 0, SWP_NOACTIVATE|SWP_NOSIZE);

				// update its background to match what's on the screen
				InvalidateRgn(hMagnifier, NULL, true);
			}
			break;

		case WM_PAINT:
			{
				PAINTSTRUCT ps;
				POINT c;
				HDC hdc = BeginPaint(hWnd, &ps);
				GetCursorPos(&c);

				//BitBlt(hdc, 2, 2, MAG_WIDTH - 2, MAG_HEIGHT - 2, hScreenDC, 0, 0, SRCCOPY);
				//SetStretchBltMode(hdc, COLORONCOLOR);
				//StretchBlt(hdc, 0, 0, MAG_WIDTH, MAG_HEIGHT, hScreenDC,
				//	c.x - (MAG_WIDTH / 2), c.y - (MAG_HEIGHT / 2), MAG_WIDTH, MAG_HEIGHT, SRCCOPY);

				EndPaint(hWnd, &ps);
			}
			break;

		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////