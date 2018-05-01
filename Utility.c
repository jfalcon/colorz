#include "WinMain.h"		// standard application includes
#include "Utility.h"		// useful application independent routines

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

/*/
/ / bool
/ /		AddClipboardText (HWND hWnd, const char *szText)
/ /
/ /		hWnd = Window handle for calling window
/ /		szText = string containing the text to add to the clipboard
/ /
/ / PURPOSE:
/ /		Adds the contents pointed to by szText to the
/ /		Windows clipboard. Returns false if unsuccessful.
/*/

bool
AddClipboardText (HWND hWnd, const char *szText)
{
	bool bReturn = false;

	// validate params
	if((szText != NULL) && OpenClipboard(hWnd))
	{
		HGLOBAL hGlobalBuff = NULL;
		char *szBuffer = NULL;

		// allocate a global buffer, note you can use
		// GMEM_DDESHARE if you want Win32s compatiblity
		hGlobalBuff = GlobalAlloc(GMEM_MOVEABLE, strlen(szText) + 1);
		szBuffer = (char *)GlobalLock(hGlobalBuff);

		if(szBuffer != NULL)
		{
			// clean out the current contents of the clipboard
			EmptyClipboard();

			// clean out our global buffer
			ZeroMemory(szBuffer, strlen(szText) + 1);

			// copy the data over to the global data (this way data will
			// be saved on the clipboard after the application exits)
			strncpy(szBuffer, szText, strlen(szText));
			GlobalUnlock(hGlobalBuff);

			// now we can set the data on the clipboard
			if(SetClipboardData(CF_TEXT, hGlobalBuff) != NULL) bReturn = true;
		}

		// clean up
		CloseClipboard();
	}

	return bReturn;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

/*/
/ / bool
/ /		MoveWnd (HWND hWnd, unsigned short nOrientation, HWND hWndPreferred)
/ /
/ / NOTES:
/ /		Use a bitwise OR with the MV_BOTTOM, MV_CENTER, MV_LEFT,
/ /		MV_MIDDLE, MV_RIGHT, and MV_TOP constants to position.
/ /
/ / PURPOSE:
/ /		Function to move a window in one of nine possible combinations,
/ /		either relative to a preferred window or the work area.
/ /
/ /	CREDITS:
/ /		This function originated as the CenterWindow function found in the book
/ /		"Win32 Programming" by Brent E. Rector and Joseph M. Newcomer.  I,
/ /		Jeremy Falcon, modified it to allow for movement control, and renamed
/ /		it similar to MoveWindow (Win32 API) to reflect the new functionality.
/*/

bool
MoveWnd (HWND hWnd, unsigned short nOrientation, HWND hWndPreferred)
{
	POINT	PreferredPoint = {0};
	RECT	WndRect = {0}, PreferredRect = {0};
	SIZE	WndSize = {0};

	// don't bother unless we have a valid window to center
	if(!IsWindow(hWnd))	return false;

	/*/
	/ / Move the window around the preferred window if it exists;
	/ / otherwise, move the window relevant to the work area.
	/*/
    if(hWndPreferred != NULL)
	{
        if(!IsWindow(hWndPreferred)) return false;
        GetWindowRect(hWndPreferred, &PreferredRect);
    }
    else
	{
		// attempt to get the dimensions for the work area
		if(!SystemParametersInfo(SPI_GETWORKAREA, sizeof(RECT), &PreferredRect, 0))
		{
			/*/
			/ / If the above call failed Explorer is not running correctly;
			/ / therefore, there is no work area (and taskbar). So, we'll
			/ / use the screen size instead for our coordinates.
			/*/
			SetRect(&PreferredRect, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN));
		}
	}

	// get the rect of the window (to be moved) and compute the width/height
	GetWindowRect(hWnd, &WndRect);
	WndSize.cx = (WndRect.right  - WndRect.left);
	WndSize.cy = (WndRect.bottom - WndRect.top);

	/*/
	/ / NOTE: We do not need to check for left and top because the point
	/ / already defaults to 0,0. Check for the rest and change accordingly.
	/*/

    // calc the left and top positions of the window to be moved
	if((nOrientation & MV_BOTTOM) == MV_BOTTOM)
		PreferredPoint.y = PreferredRect.bottom - WndSize.cy;

	if((nOrientation & MV_CENTER) == MV_CENTER)
		PreferredPoint.x = (PreferredRect.right - WndSize.cx) / 2;

	if((nOrientation & MV_MIDDLE) == MV_MIDDLE)
		PreferredPoint.y = (PreferredRect.bottom - WndSize.cy) / 2;

	if((nOrientation & MV_RIGHT) == MV_RIGHT)
		PreferredPoint.x = PreferredRect.right - WndSize.cx;

    // reposition the window
	return SetWindowPos(hWnd, NULL, PreferredPoint.x, PreferredPoint.y,
						0, 0, SWP_NOACTIVATE|SWP_NOSIZE|SWP_NOZORDER);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////