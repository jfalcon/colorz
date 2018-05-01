#include "WinMain.h"		// standard application includes
#include "Utility.h"		// useful application independent routines
#include "Magnifier.h"		// handles the magnifier window
#include "Resource.h"		// resource identifiers

/*/
/ / --------------------------------------------------------------------------------------------------------------------
/ /  Author:		Jeremy Falcon
/ /  Purpose:		Entry point and handles windowing for the main application dialog.
/ / --------------------------------------------------------------------------------------------------------------------
/*/

#define SCROLL_LINE			1
#define SCROLL_PAGE_NORMAL	5
#define SCROLL_PAGE_WEBSAFE	1

static COLORREF	cForeground = RGB(255,255,255), cBackground = RGB(0,0,0);
static HBRUSH	hBackground = NULL;
static HFONT	hFontOutput = NULL;
static HICON	hIconLarge = NULL, hIconSmall = NULL;

bool CALLBACK DialogProc (HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

static void   Dialog_OnCommand			(HWND hWnd, int id, HWND hWndCtl, UINT codeNotify);
static HBRUSH Dialog_OnCtlColorStatic	(HWND hWnd, HDC hDC, HWND hChild, int type);
static void   Dialog_OnHScroll			(HWND hWnd, HWND hCtl, UINT code, int pos);
static bool   Dialog_OnInitDialog		(HWND hWnd, HWND hWndFocus, LPARAM lParam);
static void   Dialog_OnTimer			(HWND hWnd, UINT id);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*/
/ /	This function is the entry point for the application. It initializes the application and processes the dialog box.
/*/

int WINAPI
WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	INITCOMMONCONTROLSEX cc;

	cc.dwSize = sizeof(INITCOMMONCONTROLSEX);
	cc.dwICC = ICC_STANDARD_CLASSES;

	// init routines from the common control lib to use xp/vista/7 themes
	if(InitCommonControlsEx(&cc))
	{
		/*/
		/ /	Instead of using GetMessage(), TranslateMessage(), and DispatchMessage() for the main message loop, we
		/ /	simply create a Dialog Box and let the operating system handle it internally.
		/*/

		DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOGBOX), NULL, (DLGPROC)DialogProc);

		// clean up
		if(hBackground != NULL) DeleteObject(hBackground);
		if(hFontOutput != NULL) DeleteObject(hFontOutput);
		if(hIconLarge != NULL) DestroyIcon(hIconLarge);
		if(hIconSmall != NULL) DestroyIcon(hIconSmall);

		return true;
	}
	else
		return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*/
/ / Messaging procedure for the main dialog box. Processes messages sent by calling the appropriate functions.
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

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*/
/ /	Handle the keyboard and control notifications.
/*/

static void
Dialog_OnCommand (HWND hWnd, int id, HWND hWndCtl, UINT codeNotify)
{
	switch(id)
	{
		case IDC_CHECK_WEBSAFE:

			if(Button_GetCheck(hWndCtl) == BST_CHECKED)
			{
				bool bRedraw;

				// change the current colors to the closest web safe ones
				cBackground = GetWebSafeColor(&cBackground);
				cForeground = GetWebSafeColor(&cForeground);

				// don't redraw if scrollbars are disabled
				bRedraw = (Button_GetCheck(GetDlgItem(hWnd, IDC_CHECK_GRABCLR)) == BST_CHECKED) ? false : true;

				// update the scrollbars
				SetScrollbarRange(hWnd, SCROLL_PAGE_WEBSAFE, true);

				// update the display output depending on background or foreground mode
				if(Button_GetCheck(GetDlgItem(hWnd, IDC_RADIO_BACKGND)) == BST_CHECKED)
				{
					ScrollBar_SetPos(GetDlgItem(hWnd, IDC_SCROLLBAR_R), GetRValue(cBackground) / WEBSAFE, bRedraw);
					ScrollBar_SetPos(GetDlgItem(hWnd, IDC_SCROLLBAR_G), GetGValue(cBackground) / WEBSAFE, bRedraw);
					ScrollBar_SetPos(GetDlgItem(hWnd, IDC_SCROLLBAR_B), GetBValue(cBackground) / WEBSAFE, bRedraw);

					UpdateDisplayColor(hWnd, &cBackground, &cBackground, &cForeground);
				}
				else
				{
					ScrollBar_SetPos(GetDlgItem(hWnd, IDC_SCROLLBAR_R), GetRValue(cForeground) / WEBSAFE, bRedraw);
					ScrollBar_SetPos(GetDlgItem(hWnd, IDC_SCROLLBAR_G), GetGValue(cForeground) / WEBSAFE, bRedraw);
					ScrollBar_SetPos(GetDlgItem(hWnd, IDC_SCROLLBAR_B), GetBValue(cForeground) / WEBSAFE, bRedraw);

					UpdateDisplayColor(hWnd, &cForeground, &cBackground, &cForeground);
				}
			}
			else
			{
				bool bRedraw;

				// update the scrollbars
				SetScrollbarRange(hWnd, SCROLL_PAGE_NORMAL, false);

				// don't redraw if scrollbars are disabled
				bRedraw = (Button_GetCheck(GetDlgItem(hWnd, IDC_CHECK_GRABCLR)) == BST_CHECKED) ? false : true;

				// update the display output depending on background or foreground mode
				if(Button_GetCheck(GetDlgItem(hWnd, IDC_RADIO_BACKGND)) == BST_CHECKED)
				{
					ScrollBar_SetPos(GetDlgItem(hWnd, IDC_SCROLLBAR_R), GetRValue(cBackground), bRedraw);
					ScrollBar_SetPos(GetDlgItem(hWnd, IDC_SCROLLBAR_G), GetGValue(cBackground), bRedraw);
					ScrollBar_SetPos(GetDlgItem(hWnd, IDC_SCROLLBAR_B), GetBValue(cBackground), bRedraw);

					UpdateDisplayColor(hWnd, &cBackground, &cBackground, &cForeground);
				}
				else
				{
					ScrollBar_SetPos(GetDlgItem(hWnd, IDC_SCROLLBAR_R), GetRValue(cForeground), bRedraw);
					ScrollBar_SetPos(GetDlgItem(hWnd, IDC_SCROLLBAR_G), GetGValue(cForeground), bRedraw);
					ScrollBar_SetPos(GetDlgItem(hWnd, IDC_SCROLLBAR_B), GetBValue(cForeground), bRedraw);

					UpdateDisplayColor(hWnd, &cForeground, &cBackground, &cForeground);
				}
			}
			break;

		case IDC_CHECK_ONTOP:

			// toggle on/off whether or not the window is to stay always on top
			if(Button_GetCheck(hWndCtl) == BST_CHECKED)
				SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE|SWP_NOMOVE);
			else
				SetWindowPos(hWnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOSIZE|SWP_NOMOVE);
			break;

		case IDC_CHECK_GRABCLR:

			ToggleGrabColor(hWnd, hWndCtl, &cBackground, &cForeground);
			break;

		case IDC_RADIO_DECIMAL:
		case IDC_RADIO_FLOAT:
		case IDC_RADIO_MARKUP:

			// update the display output
			if(Button_GetCheck(GetDlgItem(hWnd, IDC_RADIO_BACKGND)) == BST_CHECKED)
				UpdateDisplayColor(hWnd, &cBackground, &cBackground, &cForeground);
			else
				UpdateDisplayColor(hWnd, &cForeground, &cBackground, &cForeground);
			break;

		case IDC_RADIO_BACKGND:	// do not use an auto button for this message

			// update scroll bars to reflect selected color
			if(Button_GetCheck(GetDlgItem(hWnd, IDC_CHECK_WEBSAFE)) == BST_CHECKED)
			{
				// web safe
				ScrollBar_SetPos(GetDlgItem(hWnd, IDC_SCROLLBAR_R), GetRValue(cBackground) / WEBSAFE, true);
				ScrollBar_SetPos(GetDlgItem(hWnd, IDC_SCROLLBAR_G), GetGValue(cBackground) / WEBSAFE, true);
				ScrollBar_SetPos(GetDlgItem(hWnd, IDC_SCROLLBAR_B), GetBValue(cBackground) / WEBSAFE, true);
			}
			else
			{
				// normal
				ScrollBar_SetPos(GetDlgItem(hWnd, IDC_SCROLLBAR_R), GetRValue(cBackground), true);
				ScrollBar_SetPos(GetDlgItem(hWnd, IDC_SCROLLBAR_G), GetGValue(cBackground), true);
				ScrollBar_SetPos(GetDlgItem(hWnd, IDC_SCROLLBAR_B), GetBValue(cBackground), true);
			}

			Button_SetCheck(GetDlgItem(hWnd, IDC_RADIO_FOREGND), BST_UNCHECKED);
			Button_SetCheck(hWndCtl, BST_CHECKED);

			// update the display output
			UpdateDisplayColor(hWnd, &cBackground, &cBackground, &cForeground);
			break;

		case IDC_RADIO_FOREGND:	// do not use an auto button for this message

			// update scroll bars to reflect selected color
			if(Button_GetCheck(GetDlgItem(hWnd, IDC_CHECK_WEBSAFE)) == BST_CHECKED)
			{
				// web safe
				ScrollBar_SetPos(GetDlgItem(hWnd, IDC_SCROLLBAR_R), GetRValue(cForeground) / WEBSAFE, true);
				ScrollBar_SetPos(GetDlgItem(hWnd, IDC_SCROLLBAR_G), GetGValue(cForeground) / WEBSAFE, true);
				ScrollBar_SetPos(GetDlgItem(hWnd, IDC_SCROLLBAR_B), GetBValue(cForeground) / WEBSAFE, true);
			}
			else
			{
				// normal
				ScrollBar_SetPos(GetDlgItem(hWnd, IDC_SCROLLBAR_R), GetRValue(cForeground), true);
				ScrollBar_SetPos(GetDlgItem(hWnd, IDC_SCROLLBAR_G), GetGValue(cForeground), true);
				ScrollBar_SetPos(GetDlgItem(hWnd, IDC_SCROLLBAR_B), GetBValue(cForeground), true);
			}

			Button_SetCheck(GetDlgItem(hWnd, IDC_RADIO_BACKGND), BST_UNCHECKED);
			Button_SetCheck(hWndCtl, BST_CHECKED);

			// update the display output
			UpdateDisplayColor(hWnd, &cForeground, &cBackground, &cForeground);
			break;

		case IDC_CHECK_AUTOCOPY:

			// force output control to repaint (because we highlight it)
			InvalidateRgn(GetDlgItem(hWnd, IDC_STATIC_OUTPUT), NULL, true);
			break;

		case IDCANCEL:

			// dismiss the Colorz dialog box
			SaveWindowPosition(hWnd);
			EndDialog(hWnd, true);
			break;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*/
/ / Determines the colors of a static control.
/*/

static HBRUSH
Dialog_OnCtlColorStatic (HWND hWnd, HDC hDC, HWND hChild, int type)
{
	if(hChild == GetDlgItem(hWnd, IDC_STATIC_PREVIEW))
	{
		// create a brush for the background color
		if(hBackground) DeleteObject(hBackground);
		hBackground = CreateSolidBrush(cBackground);
		SetBkMode(hDC, TRANSPARENT);

		// set the foreground color
		SetTextColor(hDC, cForeground);
		return hBackground;
	}

	else if(hChild == GetDlgItem(hWnd, IDC_STATIC_OUTPUT))
	{
		// create a brush for the background color
		if(hBackground) DeleteObject(hBackground);
		hBackground = CreateSolidBrush(GetSysColor(COLOR_3DFACE));
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

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*/
/ / Handle the scroll bar notifications and update the other controls as needed.
/*/

static void
Dialog_OnHScroll (HWND hWnd, HWND hCtl, UINT code, int pos)
{
	HWND hR = NULL, hG = NULL, hB = NULL;

	int nPos = 0;				// used to track current position
	COLORREF cCurrent;			// used to set the current color

	// get handles to all the scrollbars
	hR = GetDlgItem(hWnd, IDC_SCROLLBAR_R);
	hG = GetDlgItem(hWnd, IDC_SCROLLBAR_G);
	hB = GetDlgItem(hWnd, IDC_SCROLLBAR_B);

	switch(code)
	{
		/*/
		/ / This code handles common notifications from the scroll bars
		/ / and if the user wants to keep his/her values in grayscale, it
		/ / makes sure the bars are in uniform. (same value for all)
		/*/

		case SB_THUMBPOSITION:

			// user releases thumb
			if(Button_GetCheck(GetDlgItem(hWnd, IDC_CHECK_GRAYSCALE)) == BST_CHECKED)
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
			if(Button_GetCheck(GetDlgItem(hWnd, IDC_CHECK_GRAYSCALE)) == BST_CHECKED)
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

			if(Button_GetCheck(GetDlgItem(hWnd, IDC_CHECK_GRAYSCALE)) == BST_CHECKED)
			{
				nPos += SCROLL_LINE;
				ScrollBar_SetPos(hR, nPos, true);
				ScrollBar_SetPos(hG, nPos, true);
				ScrollBar_SetPos(hB, nPos, true);
			}
			else
			{
				nPos += SCROLL_LINE;
				ScrollBar_SetPos(hCtl, nPos, true);
			}
			break;
			
		case SB_LINELEFT:

			// user clicks left arrow
			nPos = ScrollBar_GetPos(hCtl);

			if(Button_GetCheck(GetDlgItem(hWnd, IDC_CHECK_GRAYSCALE)) == BST_CHECKED)
			{
				nPos -= SCROLL_LINE;
				ScrollBar_SetPos(hR, nPos, true);
				ScrollBar_SetPos(hG, nPos, true);
				ScrollBar_SetPos(hB, nPos, true);
			}
			else
			{
				nPos -= SCROLL_LINE;
				ScrollBar_SetPos(hCtl, nPos, true);
			}
			break;

		case SB_PAGERIGHT:

			// user pages right
			nPos = ScrollBar_GetPos(hCtl);

			if(Button_GetCheck(GetDlgItem(hWnd, IDC_CHECK_WEBSAFE)) == BST_CHECKED)
				nPos += SCROLL_PAGE_WEBSAFE;
			else
				nPos += SCROLL_PAGE_NORMAL;

			// for grayscale all scrollbars must match
			if(Button_GetCheck(GetDlgItem(hWnd, IDC_CHECK_GRAYSCALE)) == BST_CHECKED)
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

			if(Button_GetCheck(GetDlgItem(hWnd, IDC_CHECK_WEBSAFE)) == BST_CHECKED)
				nPos -= SCROLL_PAGE_WEBSAFE;
			else
				nPos -= SCROLL_PAGE_NORMAL;

			// for grayscale all scrollbars must match
			if(Button_GetCheck(GetDlgItem(hWnd, IDC_CHECK_GRAYSCALE)) == BST_CHECKED)
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
	if(Button_GetCheck(GetDlgItem(hWnd, IDC_CHECK_WEBSAFE)) == BST_CHECKED)
	{
		cCurrent = RGB((unsigned short)(ScrollBar_GetPos(hR) * WEBSAFE),
			(unsigned short)(ScrollBar_GetPos(hG) * WEBSAFE),
			(unsigned short)(ScrollBar_GetPos(hB) * WEBSAFE));
	}
	else
	{
		cCurrent = RGB((unsigned short)ScrollBar_GetPos(hR),
			(unsigned short)ScrollBar_GetPos(hG),
			(unsigned short)ScrollBar_GetPos(hB));
	}

	// update the display
	UpdateDisplayColor(hWnd, &cCurrent, &cBackground, &cForeground);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*/
/ / Function to handle any initialization necessary before the newly created window is shown.
/*/

static bool
Dialog_OnInitDialog (HWND hWnd, HWND hWndFocus, LPARAM lParam)
{
	HINSTANCE hInst = GetWindowInstance(hWnd);
	HDC hDC = GetDC(hWnd);

	AlignWindow(hWnd, NULL, ALIGN_CENTER|ALIGN_MIDDLE);

	// create a fixed width font for Arial Narrow at 11 points (use a truetype if possible)
	hFontOutput = CreateFont(-MulDiv(11, GetDeviceCaps(hDC, LOGPIXELSY), 72),
		0, 0, 0, FW_NORMAL, 0, 0, 0, ANSI_CHARSET, OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS,
		ANTIALIASED_QUALITY, FF_DONTCARE, "Arial Narrow");

	SetWindowFont(GetDlgItem(hWnd, IDC_STATIC_PREVIEW), NULL, true);
	SetWindowFont(GetDlgItem(hWnd, IDC_STATIC_OUTPUT), hFontOutput, true);

	SetScrollbarRange(hWnd, SCROLL_PAGE_NORMAL, false);

	Button_SetCheck(GetDlgItem(hWnd, IDC_RADIO_BACKGND), BM_SETCHECK);
	Button_SetCheck(GetDlgItem(hWnd, IDC_RADIO_DECIMAL), BM_SETCHECK);

	hIconLarge = (HICON)LoadImage(hInst, MAKEINTRESOURCE(IDR_MAINFRAME), IMAGE_ICON,
		GetSystemMetrics(SM_CXICON), GetSystemMetrics(SM_CYICON), LR_DEFAULTCOLOR);

	hIconSmall = (HICON)LoadImage(hInst, MAKEINTRESOURCE(IDR_MAINFRAME), IMAGE_ICON,
		GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CYSMICON), LR_DEFAULTCOLOR);

	SendMessage(hWnd, WM_SETICON, (WPARAM)ICON_BIG, (LPARAM)hIconLarge);
	SendMessage(hWnd, WM_SETICON, (WPARAM)ICON_SMALL, (LPARAM)hIconSmall);

	ReleaseDC(hWnd, hDC);

	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*/
/ / Receives timer notifications. This handles the Grab Color processing.
/*/

static void
Dialog_OnTimer (HWND hWnd, UINT id)
{
	bool bControlKey = false, bShiftKey = false;

	// is the user pressing the control or shift keys (the most significant bit if the key was down)?
	bControlKey = (bool)(GetAsyncKeyState(VK_CONTROL) & 0x8000);
	bShiftKey = (bool)(GetAsyncKeyState(VK_SHIFT) & 0x8000);

	ShowMagnifier(bShiftKey);

	if(bControlKey)
	{
		static POINT spoint;
		POINT cursor;

		GetCursorPos(&cursor);

		// don't bother if the cursor hasn't moved since the last tick
		if((cursor.x != spoint.x) && (cursor.y != spoint.y))
		{
			HDC hDC = NULL;
			HWND hWndxy = NULL, hPreview = NULL;
			COLORREF rgb = 0;

			spoint.x = cursor.x;
			spoint.y = cursor.y;

			// get a handle to the window under the cursor
			hWndxy = WindowFromPoint(spoint);

			// update the magnifier's position if it's misplaced
			SetMagnifierPos(hWndxy, cursor.x - 50, cursor.y - 37);

			// get a handle to the preview pane
			hPreview = GetDlgItem(hWnd, IDC_STATIC_PREVIEW);

			// get the dc for the window in question
			hDC = GetDC(hWndxy);

			// GetPixel() requires client coordinates
			ScreenToClient(hWndxy, &cursor);
			rgb = GetPixel(hDC, cursor.x, cursor.y);

			/*/
			/ / The following is a workaround for the GetPixel API. If the cursor is over a title bar or other
			/ / non-client areas of the window it'll return -1 for the RGB intensities. In this case we use BitBlt() to
			/ / copy the color to our control and extract the the intensities from that.
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

			// set the correct value if in web safe mode
			if(Button_GetCheck(GetDlgItem(hWnd, IDC_CHECK_WEBSAFE)) == BST_CHECKED) rgb = GetWebSafeColor(&rgb);

			// update the display
			UpdateDisplayColor(hWnd, &rgb, &cBackground, &cForeground);
		}
	}
}