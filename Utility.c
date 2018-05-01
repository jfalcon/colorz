#include "WinMain.h"		// standard application includes
#include "Utility.h"		// useful application independent routines
#include "Magnifier.h"		// handles the magnifier window
#include "Resource.h"		// resource identifiers

/*/
/ / --------------------------------------------------------------------------------------------------------------------
/ /  Author:		Jeremy Falcon
/ /  Purpose:		Miscellaneous utility routines.
/ / --------------------------------------------------------------------------------------------------------------------
/*/

#define COLORZ_REGISTRY_KEY			"Software\\Colorz"
#define COLORZ_REGISTRY_LEFT_VALUE	"Left"
#define COLORZ_REGISTRY_TOP_VALUE	"Top"

/*/
/ / Adds the contents pointed to by szText to the clipboard. Returns false if unsuccessful.
/*/

bool
AddClipboardText (HWND hWnd, const char *szText)
{
	bool bReturn = false;

	if(IsWindow(hWnd) && (szText != NULL) && OpenClipboard(hWnd))
	{
		HGLOBAL hGlobalBuff = NULL;
		char *szBuffer = NULL;

		// allocate a global buffer (you can use GMEM_DDESHARE if you want Win32s compatibility)
		hGlobalBuff = GlobalAlloc(GMEM_MOVEABLE, strlen(szText) + 1);
		szBuffer = (char *)GlobalLock(hGlobalBuff);

		if(szBuffer != NULL)
		{
			EmptyClipboard();
			SecureZeroMemory(szBuffer, strlen(szText) + 1);

			// by using the global buffer data will still be saved on the clipboard after the application exits
			strncpy(szBuffer, szText, strlen(szText));
			GlobalUnlock(hGlobalBuff);

			// now we can set the data on the clipboard
			if(SetClipboardData(CF_TEXT, hGlobalBuff) != NULL) bReturn = true;
		}

		CloseClipboard();
	}

	return bReturn;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*/
/ /	Aligns a window in one of nine possible combinations, either relative to a preferred window or the work area.
/ / Use a bitwise OR with the ALIGN_BOTTOM, ALIGN_CENTER, ALIGN_LEFT, ALIGN_MIDDLE, ALIGN_RIGHT, and ALIGN_TOP
/ / constants to determine the position the window will end up in.
/*/

bool
AlignWindow (HWND hWnd, HWND hWndParent, unsigned short nOrientation)
{
	if(IsWindow(hWnd))
	{
		POINT	NewPosition = {0};
		RECT	WndRect = {0}, ContainerRect = {0};
		SIZE	WndSize = {0};

		/*/
		/ / Move the target window in relation to the a parent window if desired; otherwise, move the window relevant
		/ / to the desktop's work area.
		/*/

		if(IsWindow(hWndParent))
		{
			GetWindowRect(hWndParent, &ContainerRect);
		}
		else
		{
			// attempt to get the dimensions for the desktop's work area
			if(!SystemParametersInfo(SPI_GETWORKAREA, sizeof(RECT), &ContainerRect, 0))
			{
				/*/
				/ / If the above call failed then explorer is not running; so, there is no work area (and taskbar).
				/ / As such, we'll use the screen size instead for our coordinates.
				/*/
				SetRect(&ContainerRect, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN));
			}
		}

		// get the rect of the window to be moved and compute the width/height
		GetWindowRect(hWnd, &WndRect);

		WndSize.cx = (WndRect.right  - WndRect.left);
		WndSize.cy = (WndRect.bottom - WndRect.top);

		// calc the left and top positions of the window to be moved (ALIGN_TOP and ALIGN_LEFT can be ignored)
		if((nOrientation & ALIGN_CENTER) == ALIGN_CENTER) NewPosition.x = (ContainerRect.right - WndSize.cx) / 2;
		else if((nOrientation & ALIGN_RIGHT)  == ALIGN_RIGHT)  NewPosition.x = ContainerRect.right - WndSize.cx;

		if((nOrientation & ALIGN_BOTTOM) == ALIGN_BOTTOM) NewPosition.y = ContainerRect.bottom - WndSize.cy;
		else if((nOrientation & ALIGN_MIDDLE) == ALIGN_MIDDLE) NewPosition.y = (ContainerRect.bottom - WndSize.cy) / 2;

		return SetWindowPos(hWnd, NULL, NewPosition.x, NewPosition.y, 0, 0, SWP_NOACTIVATE|SWP_NOSIZE|SWP_NOZORDER);
	}
		return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*/
/ / Finds the closest web safe color for the passed color value.
/*/

COLORREF
GetWebSafeColor (LPCOLORREF pInput)
{
	unsigned short nR = 0, nG = 0, nB = 0;

	if(pInput != NULL)
	{
		nR = GetRValue(*pInput);
		nG = GetGValue(*pInput);
		nB = GetBValue(*pInput);

		// to be web safe, each channel must be a multiple of 0x33, don't forget to round off
		nR = (unsigned short)((int)(((float)nR / (float)WEBSAFE) + 0.5f) * WEBSAFE);
		nG = (unsigned short)((int)(((float)nG / (float)WEBSAFE) + 0.5f) * WEBSAFE);
		nB = (unsigned short)((int)(((float)nB / (float)WEBSAFE) + 0.5f) * WEBSAFE);
	}

	return RGB(nR, nG, nB);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*/
/ / Gets the saved position of the main window from the registry.
/*/

bool
GetWindowPosition (LPPOINT)
{
	HKEY hKey;
	POINT point = {0};

	if((RegOpenKeyEx(HKEY_CURRENT_USER, COLORZ_REGISTRY_KEY, 0, KEY_READ, &hKey) == ERROR_SUCCESS) && (hKey != NULL))
	{
		POINT point;

		RegSetValueEx(hKey, COLORZ_REGISTRY_LEFT_VALUE, 0, REG_DWORD, (LPBYTE)&rect.left, (DWORD)sizeof(rect.left));
		RegSetValueEx(hKey, COLORZ_REGISTRY_TOP_VALUE, 0, REG_DWORD, (LPBYTE)&rect.top, (DWORD)sizeof(rect.top));
		RegCloseKey(hKey);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*/
/ / Saves the current position of the main window to the registry.
/*/

void
SaveWindowPosition (HWND hWnd)
{
	if(IsWindow(hWnd))
	{
		HKEY hKey;

		RegCreateKeyEx(HKEY_CURRENT_USER, COLORZ_REGISTRY_KEY,
			0, NULL, REG_OPTION_VOLATILE, KEY_SET_VALUE, NULL, &hKey, NULL);

		if(hKey != NULL)
		{
			RECT rect;

			GetWindowRect(hWnd, &rect);
			RegSetValueEx(hKey, COLORZ_REGISTRY_LEFT_VALUE, 0, REG_DWORD, (LPBYTE)&rect.left, (DWORD)sizeof(rect.left));
			RegSetValueEx(hKey, COLORZ_REGISTRY_TOP_VALUE, 0, REG_DWORD, (LPBYTE)&rect.top, (DWORD)sizeof(rect.top));
			RegCloseKey(hKey);
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*/
/ / Allows us to control the values/paging of the R, G, B scrollbars.
/*/

void
SetScrollbarRange (HWND hWnd, unsigned int nPageSize, bool bWebSafe)
{
	if(IsWindow(hWnd))
	{
		SCROLLINFO si = {0};
		bool bRedraw;

		// set range values for the R, G, and B scrollbars
		si.cbSize = sizeof(si);
		si.nMin = 0;
		si.nMax = (bWebSafe) ? 5 : 255;
		si.nPage = nPageSize;
		si.fMask = SIF_RANGE;

		// don't redraw if scrollbars are disabled
		bRedraw = (Button_GetCheck(GetDlgItem(hWnd, IDC_CHECK_GRABCLR)) == BST_CHECKED) ? false : true;

		SetScrollInfo(GetDlgItem(hWnd, IDC_SCROLLBAR_R), SB_CTL, &si, bRedraw);
		SetScrollInfo(GetDlgItem(hWnd, IDC_SCROLLBAR_G), SB_CTL, &si, bRedraw);
		SetScrollInfo(GetDlgItem(hWnd, IDC_SCROLLBAR_B), SB_CTL, &si, bRedraw);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*/
/ / Toggles the grab color state by adjusting controls accordingly.
/*/

void
ToggleGrabColor (HWND hWnd, HWND hWndCtl, LPCOLORREF pcBackground, LPCOLORREF pcForeground)
{
	if(IsWindow(hWnd) && IsWindow(hWndCtl) && (pcBackground != NULL) && (pcForeground != NULL))
	{
		static bool bOnTop = false, bGrayscale = false;
		HWND hWndOnTop = NULL, hWndGrayScale = NULL;
		HWND hWndR = NULL, hWndG = NULL, hWndB = NULL;

		hWndOnTop = GetDlgItem(hWnd, IDC_CHECK_ONTOP);
		hWndGrayScale = GetDlgItem(hWnd, IDC_CHECK_GRAYSCALE);
		hWndR = GetDlgItem(hWnd, IDC_SCROLLBAR_R);
		hWndG = GetDlgItem(hWnd, IDC_SCROLLBAR_G);
		hWndB = GetDlgItem(hWnd, IDC_SCROLLBAR_B);

		if(Button_GetCheck(hWndCtl) == BST_CHECKED)
		{
			// save the state of the on top and grayscale options since we override them
			bOnTop = (Button_GetCheck(hWndOnTop) == BST_CHECKED) ? true : false;
			bGrayscale = (Button_GetCheck(hWndGrayScale) == BST_CHECKED) ? true : false;

			// initiate the process by modifying state of the controls
			Button_SetCheck(hWndOnTop, BST_CHECKED);
			Button_SetCheck(hWndGrayScale, BST_UNCHECKED);

			Button_Enable(hWndOnTop, false);
			Button_Enable(hWndGrayScale, false);

			ScrollBar_Enable(hWndR, ESB_DISABLE_BOTH);
			ScrollBar_Enable(hWndG, ESB_DISABLE_BOTH);
			ScrollBar_Enable(hWndB, ESB_DISABLE_BOTH);

			// the main dialog will always be on top during grab color mode
			SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE|SWP_NOMOVE);

			/*/
			/ / The timer is set to trigger every 55 milliseconds. It is pointless to set it any faster for this application
			/ / because due to the hardware timer's limitations you'll never get a message in under a 54.925 millisecond
			/ / interval (without MM, ASM, or QueryPerformanceCounter workarounds). Accuracy is not imperative anyway.
			/*/

			SetTimer(hWnd, ID_TIMER, 55, NULL);

			// create the magnifier window now in case we need to show it during the key press / timer interval
			CreateMagnifier(hWnd);
		}
		else
		{
			unsigned short nR = 0, nG = 0, nB = 0;

			Button_Enable(hWndOnTop, true);
			Button_Enable(hWndGrayScale, true);

			ScrollBar_Enable(hWndR, ESB_ENABLE_BOTH);
			ScrollBar_Enable(hWndG, ESB_ENABLE_BOTH);
			ScrollBar_Enable(hWndB, ESB_ENABLE_BOTH);

			DestroyMagnifier();
			KillTimer(hWnd, ID_TIMER);

			// restore the on top and grayscale settings to what it was when we toggled on
			Button_SetCheck(hWndOnTop, (bOnTop) ? BST_CHECKED : BST_UNCHECKED);
			Button_SetCheck(hWndGrayScale, (bGrayscale) ? BST_CHECKED : BST_UNCHECKED);

			// restore the window's pos settings as well
			SetWindowPos(hWnd, (bOnTop) ? HWND_TOPMOST : HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOSIZE|SWP_NOMOVE);

			// do we set the scrollbars to match the current foreground or background color?
			if(Button_GetCheck(GetDlgItem(hWnd, IDC_RADIO_BACKGND)) == BST_CHECKED)
			{
				nR = GetRValue(*pcBackground);
				nG = GetGValue(*pcBackground);
				nB = GetBValue(*pcBackground);
			}
			else
			{
				nR = GetRValue(*pcForeground);
				nG = GetGValue(*pcForeground);
				nB = GetBValue(*pcForeground);
			}

			// update scroll bars to reflect selected color
			if(Button_GetCheck(GetDlgItem(hWnd, IDC_CHECK_WEBSAFE)) == BST_CHECKED)
			{
				// set the correct value if in web safe mode
				ScrollBar_SetPos(hWndR, nR / WEBSAFE, true);
				ScrollBar_SetPos(hWndG, nG / WEBSAFE, true);
				ScrollBar_SetPos(hWndB, nB / WEBSAFE, true);
			}
			else
			{
				ScrollBar_SetPos(hWndR, nR, true);
				ScrollBar_SetPos(hWndG, nG, true);
				ScrollBar_SetPos(hWndB, nB, true);
			}
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*/
/ / Updates the UI to reflect the current color we wish to set.
/*/

void
UpdateDisplayColor (HWND hWnd, LPCOLORREF pcColor, LPCOLORREF pcBackground, LPCOLORREF pcForeground)
{
	if(IsWindow(hWnd) && (pcColor != NULL) && (pcBackground != NULL) && (pcForeground != NULL))
	{
		char szBuffer[20] = {0};
		unsigned short nR, nG, nB;
		HWND hPreview = NULL, hOutput = NULL;

		// extract specific color intensities
		nR = GetRValue(*pcColor);
		nG = GetGValue(*pcColor);
		nB = GetBValue(*pcColor);

		// get handles to the preview pane and output label
		hPreview = GetDlgItem(hWnd, IDC_STATIC_PREVIEW);
		hOutput = GetDlgItem(hWnd, IDC_STATIC_OUTPUT);

		// format the output for decimal display
		if(Button_GetCheck(GetDlgItem(hWnd, IDC_RADIO_DECIMAL)) == BST_CHECKED)
			wsprintf(szBuffer, "%hu, %hu, %hu", nR, nG, nB);

		// format the output for float display
		else if(Button_GetCheck(GetDlgItem(hWnd, IDC_RADIO_FLOAT)) == BST_CHECKED)
			sprintf(szBuffer, "%.3f, %.3f, %.3f", ((float)nR / 255.0f), ((float)nG / 255.0f), ((float)nB / 255.0f));

		// format the output for markup/hexadecimal display
		else if(Button_GetCheck(GetDlgItem(hWnd, IDC_RADIO_MARKUP)) == BST_CHECKED)
			wsprintf(szBuffer, "#%02X%02X%02X", nR, nG, nB);

		// update label with new data
		SetWindowText(hOutput, szBuffer);

		// post data to clipboard if auto copy is selected
		if(Button_GetCheck(GetDlgItem(hWnd, IDC_CHECK_AUTOCOPY)) == BST_CHECKED)
			AddClipboardText(hWnd, szBuffer);

		if(Button_GetCheck(GetDlgItem(hWnd, IDC_RADIO_BACKGND)) == BST_CHECKED)
			// set the background color of the static control
			*pcBackground = RGB(nR, nG, nB);
		else
			// set the foreground color of the static control
			*pcForeground = RGB(nR, nG, nB);

		// force the preview control to repaint itself
		InvalidateRgn(hPreview, NULL, true);
	}
}