#include "WinMain.h"		// standard application includes
#include "Utility.h"		// useful application independent routines
#include "Resource.h"		// resource identifiers

/*/
/ / -----------------------------------------------------------------------
/ /
/ /  Author:		Jeremy Falcon
/ /  Copyright:		©2000-2006 Jeremy Falcon. All Rights Reserved.
/ /  Filename:		WinMain.c
/ /  Platforms:		Windows 95/98/ME, Windows NT/2000/XP/Vista; not tested on Win32s
/ /  Program:		COLORZ.EXE (Colorz 1.3.1)
/ /  Purpose:		Aids developers with color intensities (RGB & Hex).
/ /
/ /  Description:
/ /	   [Common Declarations]
/ /			This software is not subject to any export provision of
/ /			the United States Department of Commerce, and may be
/ /			exported to any country or planet.
/ /
/ /	   [License]
/ /			Permission is granted to anyone to use this software for any  
/ /			purpose on any computer system, and to alter it and 
/ /			redistribute it freely, subject to the following
/ /			restrictions:
/ /
/ /			1. The author is not responsible for the consequences of
/ /			   use of this software, no matter how awful, even if they
/ /			   arise from flaws in it.
/ /
/ /			2. The origin of this software must not be misrepresented,
/ /			   either by explicit claim or by omission. Since few users
/ /			   ever read sources, credits must appear in the documentation
/ /			   or about box.
/ /
/ /			3. Altered versions must be plainly marked as such, and
/ /			   must not be misrepresented as being the original software.
/ /
/ /			4. This notice may not be removed or altered.
/ /
/ /  Disclaimer:
/ /		THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF 
/ /		ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED 
/ /		TO THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A 
/ /		PARTICULAR PURPOSE.
/ /
/ /  Notes:
/ /		If you have any qeustions regarding the above clause, please
/ /		direct them to ironbird@gmail.com
/ / -----------------------------------------------------------------------
/*/

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

// file scope variables
static COLORREF	cForeground = RGB(255,255,255), cBackground = RGB(0,0,0);
static HBRUSH	hBackground = NULL;
static HFONT	hFontOutput = NULL;
static HICON	hIconLarge = NULL, hIconSmall = NULL;
static int		iCopyMode = 0;

// local prototypes
static void GrabColor	  (HWND hWnd, HWND hWndCtl);
static void UpdateDisplay (HWND hWnd, unsigned short nR, unsigned short nG, unsigned short nB);

// callbacks
bool CALLBACK DialogProc (HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

// message handlers
static void   Dialog_OnCommand			(HWND hWnd, int id, HWND hWndCtl, UINT codeNotify);
static HBRUSH Dialog_OnCtlColorStatic	(HWND hWnd, HDC hDC, HWND hChild, int type);
static void   Dialog_OnHScroll			(HWND hWnd, HWND hCtl, UINT code, int pos);
static bool   Dialog_OnInitDialog		(HWND hWnd, HWND hWndFocus, LPARAM lParam);
static void   Dialog_OnTimer			(HWND hWnd, UINT id);

///////////////////////////////////////////////////////////////////////////////////////////////////

/*/
/ / int WINAPI
/ /		WinMain (HINSTANCE, HINSTANCE, LPSTR, int)
/ /
/ / PURPOSE:
/ /		Entry point for the application.
/ /
/ / COMMENTS:
/ /		This function initializes the application and processes the
/ /		dialog box.
/*/

int WINAPI
WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	/*/
	/ /	Instead of using GetMessage(), TranslateMessage(), and DispatchMessage()
	/ /	for the main message loop, we simply create a Dialog Box and let Windows
	/ /	handle it internally.
	/*/

    DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOGBOX), NULL, (DLGPROC)DialogProc);

	// clean up
	if(hBackground != NULL) DeleteObject(hBackground);
	if(hFontOutput != NULL) DeleteObject(hFontOutput);
	if(hIconLarge != NULL) DestroyIcon(hIconLarge);
	if(hIconSmall != NULL) DestroyIcon(hIconSmall);

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

/*/
/ / bool CALLBACK
/ /		DialogProc (HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
/ /
/ / PURPOSE:
/ /		Dialog function for the Colorz dialog box.
/ /		Processes messages sent by calling the
/ /		appropriate functions
/*/

bool CALLBACK
DialogProc (HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
		case WM_COMMAND:				// Notification from a control
			return HANDLE_WM_COMMAND (hWnd, wParam, lParam, Dialog_OnCommand);

		case WM_CTLCOLORSTATIC:			// Paint a static control's color
			return HANDLE_WM_CTLCOLORSTATIC (hWnd, wParam, lParam, Dialog_OnCtlColorStatic);

		case WM_HSCROLL:				// Notification from a horizontal scrollbar
			return HANDLE_WM_HSCROLL (hWnd, wParam, lParam, Dialog_OnHScroll);

		case WM_INITDIALOG:				// Initialization of controls complete
			return HANDLE_WM_INITDIALOG (hWnd, wParam, lParam, Dialog_OnInitDialog);

		case WM_TIMER:					// Notification of timer interval
			return HANDLE_WM_TIMER (hWnd, wParam, lParam, Dialog_OnTimer);
	}

	return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

/*/
/ / static void
/ /		Dialog_OnCommand (HWND hWnd, int id, hWnd hWndCtl, UINT codeNotify)
/ /
/ /			hWnd		Window handle for the dialog box window
/ /			id			Specifies the identifier of the menu item, control, or accelerator.
/ /			hWndCtl		Handle of the control sending the message if the message
/ /						is from a control, otherwise, this parameter is NULL. 
/ /			codeNotify	Specifies the notification code if the message is from a control.
/ /						This parameter is 1 when the message is from an accelerator.
/ /						This parameter is 0 when the message is from a menu.
/ /
/ / PURPOSE:
/ /		Handle the keyboard and control notifications.
/ /		An OK button press, or Enter/Esc keypress
/ /		all dismiss the Colorz dialog box.
/*/

static void
Dialog_OnCommand (HWND hWnd, int id, HWND hWndCtl, UINT codeNotify)
{
	switch(id)
	{
		case IDC_CHECK_ONTOP:

			// toggle on/off whether or not the window is to stay always on top
			if(Button_GetCheck(hWndCtl) == BST_CHECKED)
				SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE|SWP_NOMOVE);
			else
				SetWindowPos(hWnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOSIZE|SWP_NOMOVE);
			break;

		case IDC_CHECK_GRABCLR:

			GrabColor(hWnd, hWndCtl);
			break;

		case IDC_RADIO_DECIMAL:
		case IDC_RADIO_FLOAT:
		case IDC_RADIO_HEX:

			// update the display output
			if(Button_GetCheck(GetDlgItem(hWnd, IDC_RADIO_BACKGND)) == BST_CHECKED)
				UpdateDisplay(hWnd, GetRValue(cBackground), GetGValue(cBackground), GetBValue(cBackground));
			else
				UpdateDisplay(hWnd, GetRValue(cForeground), GetGValue(cForeground), GetBValue(cForeground));
			break;

		case IDC_RADIO_BACKGND:	// do not use an auto button for this message

			// update scroll bars to reflect selected color
			ScrollBar_SetPos(GetDlgItem(hWnd,IDC_SCROLLBAR_R), GetRValue(cBackground), true);
			ScrollBar_SetPos(GetDlgItem(hWnd,IDC_SCROLLBAR_G), GetGValue(cBackground), true);
			ScrollBar_SetPos(GetDlgItem(hWnd,IDC_SCROLLBAR_B), GetBValue(cBackground), true);

			Button_SetCheck(GetDlgItem(hWnd, IDC_RADIO_FOREGND), BST_UNCHECKED);
			Button_SetCheck(hWndCtl, BST_CHECKED);

			// update the display output
			UpdateDisplay(hWnd, GetRValue(cBackground), GetGValue(cBackground), GetBValue(cBackground));
			break;

		case IDC_RADIO_FOREGND:	// do not use an auto button for this message

			// update scroll bars to reflect selected color
			ScrollBar_SetPos(GetDlgItem(hWnd,IDC_SCROLLBAR_R), GetRValue(cForeground), true);
			ScrollBar_SetPos(GetDlgItem(hWnd,IDC_SCROLLBAR_G), GetGValue(cForeground), true);
			ScrollBar_SetPos(GetDlgItem(hWnd,IDC_SCROLLBAR_B), GetBValue(cForeground), true);

			Button_SetCheck(GetDlgItem(hWnd, IDC_RADIO_BACKGND), BST_UNCHECKED);
			Button_SetCheck(hWndCtl, BST_CHECKED);

			// update the display output
			UpdateDisplay(hWnd, GetRValue(cForeground), GetGValue(cForeground), GetBValue(cForeground));
			break;

		case IDC_CHECK_AUTOCOPY:

			// force output control to repaint (because we highlight it)
			InvalidateRgn(GetDlgItem(hWnd, IDC_STATIC_OUTPUT), NULL, true);
			break;

		case IDCANCEL:

			// dismiss the Colorz dialog box
			EndDialog(hWnd, true);
			break;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////

/*/
/ / static HBRUSH
/ /		Dialog_OnCtlColorStatic (HWND hWnd, HDC hDC, HWND hChild, int type)
/ /
/ /			hWnd	Window handle for the dialog box window
/ /			hDC		Handle to device context for static control
/ /			hChild	Window handle for the static control
/ /			type	HOWDY
/ /
/ / PURPOSE:
/ /		Determines the colors of a static control.
/*/

static HBRUSH
Dialog_OnCtlColorStatic (HWND hWnd, HDC hDC, HWND hChild, int type)
{
	if(hChild == GetDlgItem(hWnd, IDC_STATIC_PREVIEW))
	{
		// create a brush for the background color
		if(hBackground) DeleteObject(hBackground);
		hBackground = CreateSolidBrush(cBackground);
		SetBkMode(hDC,TRANSPARENT);

		// set the forground color
		SetTextColor(hDC, cForeground);
		return hBackground;
	}

	else if(hChild == GetDlgItem(hWnd, IDC_STATIC_OUTPUT))
	{
		// create a brush for the background color
		if(hBackground) DeleteObject(hBackground);
		hBackground = CreateSolidBrush( GetSysColor(COLOR_3DFACE));
		SetBkMode(hDC, TRANSPARENT);

		// if we are in auto copy mode, highlight the text
		if(Button_GetCheck(GetDlgItem(hWnd, IDC_CHECK_AUTOCOPY)) == BST_CHECKED)
			SetTextColor(hDC, GetSysColor(COLOR_HIGHLIGHT));
		else
			SetTextColor(hDC, GetSysColor(COLOR_BTNTEXT));

		return hBackground;
	}

	// allow for default processing
	else
		return FORWARD_WM_CTLCOLORSTATIC(hWnd, hDC, hChild, DefWindowProc);
}

///////////////////////////////////////////////////////////////////////////////////////////////////

/*/
/ / static void
/ /		Dialog_OnHScroll (HWND hWnd, HWND hCtl, UINT code, int pos)
/ /
/ /			hWnd	Window handle for the dialog box window
/ /			hCtl	Handle of the control sending the message
/ /			code	Specifies the notification code if the message is from a control.
/ /			pos		Position user moved the scroll bar to
/ /
/ / PURPOSE:
/ /		Handle the scroll bar notifications.
/ /		and update the other controls as needed.
/*/

static void
Dialog_OnHScroll (HWND hWnd, HWND hCtl, UINT code, int pos)
{
	HWND hR = NULL, hG = NULL, hB = NULL;

	int nPos = 0;				// used to track current position
	unsigned short nR, nG, nB;	// used to set the current color

	// if in grayscale mode, get handlws to all the scrollbars
	hR = GetDlgItem(hWnd,IDC_SCROLLBAR_R);
	hG = GetDlgItem(hWnd,IDC_SCROLLBAR_G);
	hB = GetDlgItem(hWnd,IDC_SCROLLBAR_B);

	switch(code)
	{
		/*/
		/ / This code handles common notifications from the scroll bars
		/ / and if the user wants to keep his/her values in grayscale, it
		/ / makes sure the bars are in uniform. (same value for all)
		/*/

		case SB_THUMBPOSITION:

			// user releases thumb
			if(Button_GetCheck(GetDlgItem(hWnd, IDC_CHECK_GRAYSCL)) == BST_CHECKED)
			{
				ScrollBar_SetPos(hR, pos, true);
				ScrollBar_SetPos(hG, pos, true);
				ScrollBar_SetPos(hB, pos, true);
			}
			else
				ScrollBar_SetPos(hCtl, pos, true);
			break;

		case SB_THUMBTRACK:

			// user drags thumb
			if(Button_GetCheck(GetDlgItem(hWnd, IDC_CHECK_GRAYSCL)) == BST_CHECKED)
			{
				ScrollBar_SetPos(hR, pos, true);
				ScrollBar_SetPos(hG, pos, true);
				ScrollBar_SetPos(hB, pos, true);
			}
			else
				ScrollBar_SetPos(hCtl, pos, true);
			break;

		case SB_LINERIGHT:

			// user clicks right arrow
			nPos = ScrollBar_GetPos(hCtl);
			nPos += 1;

			if(Button_GetCheck(GetDlgItem(hWnd, IDC_CHECK_GRAYSCL)) == BST_CHECKED)
			{
				ScrollBar_SetPos(hR, nPos, true);
				ScrollBar_SetPos(hG, nPos, true);
				ScrollBar_SetPos(hB, nPos, true);
			}
			else
				ScrollBar_SetPos(hCtl, nPos, true);
			break;
			
		case SB_LINELEFT:

			// user clicks left arrow
			nPos = ScrollBar_GetPos(hCtl);
			nPos -= 1;

			if(Button_GetCheck(GetDlgItem(hWnd, IDC_CHECK_GRAYSCL)) == BST_CHECKED)
			{
				ScrollBar_SetPos(hR, nPos, true);
				ScrollBar_SetPos(hG, nPos, true);
				ScrollBar_SetPos(hB, nPos, true);
			}
			else
				ScrollBar_SetPos(hCtl, nPos, true);
			break;

		case SB_PAGERIGHT:

			// user pages right
			nPos = ScrollBar_GetPos(hCtl);
			nPos += 5;

			if(Button_GetCheck(GetDlgItem(hWnd, IDC_CHECK_GRAYSCL)) == BST_CHECKED)
			{
				ScrollBar_SetPos(hR, nPos, true);
				ScrollBar_SetPos(hG, nPos, true);
				ScrollBar_SetPos(hB, nPos, true);
			}
			else
				ScrollBar_SetPos(hCtl, nPos, true);
			break;

		case SB_PAGELEFT:

			// user pages left
			nPos = ScrollBar_GetPos(hCtl);
			nPos -= 5;

			if(Button_GetCheck(GetDlgItem(hWnd, IDC_CHECK_GRAYSCL)) == BST_CHECKED)
			{
				ScrollBar_SetPos(hR, nPos, true);
				ScrollBar_SetPos(hG, nPos, true);
				ScrollBar_SetPos(hB, nPos, true);
			}
			else
				ScrollBar_SetPos(hCtl, nPos, true);
			break;
	}

	// set the preview pane to match the new color
	nR = (unsigned short)ScrollBar_GetPos(hR);
	nG = (unsigned short)ScrollBar_GetPos(hG);
	nB = (unsigned short)ScrollBar_GetPos(hB);

	// update the display
	UpdateDisplay(hWnd, nR, nG, nB);
}

///////////////////////////////////////////////////////////////////////////////////////////////////

/*/
/ / static bool
/ /		Dialog_OnInitDialog (HWND hWnd, HWND hWndFocus, LPARAM lParam)
/ /
/ / PURPOSE:
/ /		Function to handle any initialization necessary before
/ /		the newly created window is shown.
/*/

static bool
Dialog_OnInitDialog (HWND hWnd, HWND hWndFocus, LPARAM lParam)
{
	HINSTANCE hInst = GetWindowInstance(hWnd);
	HDC hDC = GetDC(hWnd);
	SCROLLINFO si = {0};

	// center the main window in the work area
	MoveWnd(hWnd, MV_CENTER|MV_MIDDLE, NULL);

	// create a fixed width font for Arial Narrow at 11 points (use a truetype if possible)
	hFontOutput = CreateFont(-MulDiv(11, GetDeviceCaps(hDC, LOGPIXELSY), 72),
		0, 0, 0, FW_NORMAL, 0, 0, 0, ANSI_CHARSET, OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS,
		ANTIALIASED_QUALITY, FF_DONTCARE, "Arial Narrow");

	// set the preview pane's font to use the system font
	SetWindowFont(GetDlgItem(hWnd, IDC_STATIC_PREVIEW), NULL, true);

	// set the output pane's font to use a fixed width font
	SetWindowFont(GetDlgItem(hWnd, IDC_STATIC_OUTPUT), hFontOutput, true);

	// set range values for the R, G, and B scrollbars
	si.cbSize = sizeof(si);
	si.nMin = 0;
	si.nMax = 255;
	si.fMask = SIF_RANGE;

	// apply our range values to the scroll bars
	SetScrollInfo(GetDlgItem(hWnd,IDC_SCROLLBAR_R), SB_CTL, &si, true);
	SetScrollInfo(GetDlgItem(hWnd,IDC_SCROLLBAR_G), SB_CTL, &si, true);
	SetScrollInfo(GetDlgItem(hWnd,IDC_SCROLLBAR_B), SB_CTL, &si, true);

	// set default values for controls
	Button_SetCheck(GetDlgItem(hWnd, IDC_RADIO_BACKGND), BM_SETCHECK);
	Button_SetCheck(GetDlgItem(hWnd, IDC_RADIO_DECIMAL), BM_SETCHECK);

	/*/
	/ / Use our custom icon for the dialog instead of the Windows'
	/ / default icon on the title bar (and when using Alt+Tab)
	/*/

	// large icon to use
	hIconLarge = (HICON)LoadImage(hInst, MAKEINTRESOURCE(IDR_MAINFRAME), IMAGE_ICON,
		GetSystemMetrics(SM_CXICON), GetSystemMetrics(SM_CYICON), LR_DEFAULTCOLOR);

	// small icon to use
	hIconSmall = (HICON)LoadImage(hInst, MAKEINTRESOURCE(IDR_MAINFRAME), IMAGE_ICON,
		GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CYSMICON), LR_DEFAULTCOLOR);

	// set the icons
	SendMessage(hWnd, WM_SETICON, (WPARAM)ICON_BIG, (LPARAM)hIconLarge);
	SendMessage(hWnd, WM_SETICON, (WPARAM)ICON_SMALL, (LPARAM)hIconSmall);

	// clean up
	ReleaseDC(hWnd, hDC);

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

/*/
/ / static void
/ /		Dialog_OnTimer (HWND hWnd, UINT id)
/ /
/ /			hWnd		Window handle for the dialog box window
/ /			id			Specifies the identifier of the timer
/ /
/ / PURPOSE:
/ /		Receives timer notifications. This handles the Grab Color processing.
/*/

static void
Dialog_OnTimer (HWND hWnd, UINT id)
{
	short nKey = 0;

	// is the user pressing either CTRL key? (only process if so)
	nKey = GetAsyncKeyState(VK_CONTROL);

	// GetAsyncKeyState() will set the most significant bit if the key was down
	if(nKey & 0x8000)
	{
		static POINT spoint;
		POINT cursor;

		GetCursorPos(&cursor);

		// don't bother if the cursor hasn't moved since the last tick
		if((cursor.x != spoint.x) && (cursor.y != spoint.y))
		{
			HDC hDC = NULL;
			HWND hWndxy = NULL, hPreview = NULL;
			unsigned short nR = 0, nG = 0, nB = 0;
			COLORREF rgb = 0;

			spoint.x = cursor.x;
			spoint.y = cursor.y;

			// get a handle to the window under the cursor
			hWndxy = WindowFromPoint(spoint);

			// get a handle to the preview pane
			hPreview = GetDlgItem(hWnd,IDC_STATIC_PREVIEW);

			// get the dc for the window in question
			hDC = GetDC(hWndxy);

			// GetPixel() requires client coordinates
			ScreenToClient(hWndxy, &cursor);
			rgb = GetPixel(hDC, cursor.x, cursor.y);

			/*/
			/ / The following is a workaround for the GetPixel API. If the
			/ / cursor is over a titlebar or other non-client areas of the
			/ / window it'll return -1 for the RGB intensities. In this case
			/ / we use BitBlt to copy the color to our control and extract
			/ / the the intensities from that.
			/*/

			// check to see if intensities are valid
			if(rgb == -1)
			{
				HDC hDCPrev = GetDC(hPreview);

				BitBlt(hDCPrev, 0, 0, 1, 1, hDC, cursor.x, cursor.y, SRCCOPY);
				rgb = GetPixel(hDCPrev, 0, 0);

				// clean up
				ReleaseDC(hPreview, hDCPrev);
			}

			// clean up
			ReleaseDC(hWndxy, hDC);

			// extract specific color intensities
			nR = GetRValue(rgb);
			nG = GetGValue(rgb);
			nB = GetBValue(rgb);

			// update the display
			UpdateDisplay(hWnd, nR, nG, nB);
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////

/*/
/ / static void
/ /		GrabColor (HWND hWnd, HWND hWndCtl)
/ /
/ /			hWnd		Window handle for the dialog box window
/ /			hWndCtl		Window handle for the preview pane control
/ /
/ / PURPOSE:
/ /		Initialize and prepares for the grab color
/ /		process to begin
/*/

static void
GrabColor (HWND hWnd, HWND hWndCtl)
{
	static bool bOnTop = false, bGrayscale = false;
	HWND hWndOnTop = NULL, hWndGrayScale = NULL;
	HWND hWndR = NULL, hWndG = NULL, hWndB = NULL;

	// get handles to the controls we need to deal with
	hWndOnTop = GetDlgItem(hWnd, IDC_CHECK_ONTOP);
	hWndGrayScale = GetDlgItem(hWnd, IDC_CHECK_GRAYSCL);

	hWndR = GetDlgItem(hWnd, IDC_SCROLLBAR_R);
	hWndG = GetDlgItem(hWnd, IDC_SCROLLBAR_G);
	hWndB = GetDlgItem(hWnd, IDC_SCROLLBAR_B);

	/*/
	/ / The following will check and uncheck the appropriate controls
	/ / and start and end the grab color process depending on state.
	/*/

	if(Button_GetCheck(hWndCtl) == BST_CHECKED)
	{
		// save the state of the on top and grayscale options (because we change them)
		bOnTop = (Button_GetCheck(hWndOnTop) == BST_CHECKED) ? true : false;
		bGrayscale = (Button_GetCheck(hWndGrayScale) == BST_CHECKED) ? true : false;

		// initiate the process modifying state of the controls
		Button_SetCheck(hWndOnTop, BST_CHECKED);
		Button_SetCheck(hWndGrayScale, BST_UNCHECKED);

		Button_Enable(hWndOnTop,false);
		Button_Enable(hWndGrayScale,false);

		ScrollBar_Enable(hWndR, ESB_DISABLE_BOTH);
		ScrollBar_Enable(hWndG, ESB_DISABLE_BOTH);
		ScrollBar_Enable(hWndB, ESB_DISABLE_BOTH);

		// the window will always be on top during grab color mode
		SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE|SWP_NOMOVE);

		/*/
		/ / The timer is set to trigger every 55 milliseconds. It is
		/ / pointless to set it any faster for this application because
		/ / due to the hardeware timer's limitations you'll never get a
		/ / message in under a 54.925 millsecond interval (without MM,
		/ / ASM, or QueryPerformanceCounter workarounds). Accuracy is
		/ / not imperative here, so we don't care.
		/*/

		SetTimer(hWnd, ID_TIMER, 55, NULL);
	}
	else
	{
		unsigned short nR = 0, nG = 0, nB = 0;

		// terminate the process by restoring the controls
		Button_Enable(hWndOnTop, true);
		Button_Enable(hWndGrayScale, true);

		ScrollBar_Enable(hWndR, ESB_ENABLE_BOTH);
		ScrollBar_Enable(hWndG, ESB_ENABLE_BOTH);
		ScrollBar_Enable(hWndB, ESB_ENABLE_BOTH);

		// kill the timer we used to update the preview window
		KillTimer(hWnd, ID_TIMER);

		// restore the on top and grayscale settings to what it was when we entered
		Button_SetCheck(hWndOnTop, (bOnTop) ? BST_CHECKED : BST_UNCHECKED);
		Button_SetCheck(hWndGrayScale, (bGrayscale) ? BST_CHECKED : BST_UNCHECKED);

		// restore the window's pos settings as well
		SetWindowPos(hWnd, (bOnTop) ? HWND_TOPMOST : HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOSIZE|SWP_NOMOVE);

		// do we set the scrollbars to match the current foreground or background color?
		if(Button_GetCheck(GetDlgItem(hWnd, IDC_RADIO_BACKGND)) == BST_CHECKED)
		{
			nR = GetRValue(cBackground);
			nG = GetGValue(cBackground);
			nB = GetBValue(cBackground);
		}
		else
		{
			nR = GetRValue(cForeground);
			nG = GetGValue(cForeground);
			nB = GetBValue(cForeground);
		}

		// update scroll bars to reflect selected color
		ScrollBar_SetPos(hWndR, nR, true);
		ScrollBar_SetPos(hWndG, nG, true);
		ScrollBar_SetPos(hWndB, nB, true);
	}

}

///////////////////////////////////////////////////////////////////////////////////////////////////

static void
UpdateDisplay (HWND hWnd, unsigned short nR, unsigned short nG, unsigned short nB)
{
	char szBuffer[20] = {0};
	HWND hPreview = NULL, hOutput = NULL;

	// get handles to the preview pane and output label
	hPreview = GetDlgItem(hWnd, IDC_STATIC_PREVIEW);
	hOutput = GetDlgItem(hWnd, IDC_STATIC_OUTPUT);

	// format the output for decimal display
	if(Button_GetCheck(GetDlgItem(hWnd, IDC_RADIO_DECIMAL)) == BST_CHECKED)
		wsprintf(szBuffer, "%hu,%hu,%hu", nR, nG, nB);

	// format the output for float display
	else if(Button_GetCheck(GetDlgItem(hWnd, IDC_RADIO_FLOAT)) == BST_CHECKED)
		sprintf(szBuffer, "%.3f,%.3f,%.3f", ((float)nR / 255.0f), ((float)nG / 255.0f), ((float)nB / 255.0f));

	// format the output for hexadecimal display
	else if(Button_GetCheck(GetDlgItem(hWnd, IDC_RADIO_HEX)) == BST_CHECKED)
		wsprintf(szBuffer, "#%02X%02X%02X", nR, nG, nB);

	// update label with new data
	SetWindowText(hOutput, szBuffer);

	// post data to clipboard if auto copy is selected
	if(Button_GetCheck(GetDlgItem(hWnd, IDC_CHECK_AUTOCOPY)) == BST_CHECKED)
		AddClipboardText(hWnd, szBuffer);

	if(Button_GetCheck(GetDlgItem(hWnd, IDC_RADIO_BACKGND)) == BST_CHECKED)
		// set the background color of the static control
		cBackground = RGB(nR,nG,nB);
	else
		// set the foreground color of the static control
		cForeground = RGB(nR,nG,nB);

	// force the preview control to repaint itself
	InvalidateRgn(hPreview, NULL, true);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////