#if !defined (UTILITY_H_080C6E90_E9F8_437B_AF93_8BA8C6823811_)
#define UTILITY_H_080C6E90_E9F8_437B_AF93_8BA8C6823811_

// constants for use with AlignWindow()
#define ALIGN_BOTTOM	0x0001
#define ALIGN_CENTER	0x0002
#define ALIGN_LEFT		0x0004
#define ALIGN_MIDDLE	0x0008
#define ALIGN_RIGHT		0x0010
#define ALIGN_TOP		0x0020

bool	 AddClipboardText	(HWND hWnd, const char *szText);
bool	 AlignWindow		(HWND hWnd, HWND hWndParent, unsigned short nOrientation);
COLORREF GetWebSafeColor	(LPCOLORREF pInput);
void	 ToggleGrabColor	(HWND hWnd, HWND hWndCtl, LPCOLORREF pcBackground, LPCOLORREF pcForeground);
void	 SaveWindowPosition	(HWND hWnd);
void	 SetScrollbarRange	(HWND hWnd, unsigned int nPageSize, bool bWebSafe);
void	 UpdateDisplayColor	(HWND hWnd, LPCOLORREF pcColor, LPCOLORREF pcBackground, LPCOLORREF pcForeground);

#endif  // UTILITY.H
