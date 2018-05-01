#if !defined (UTILITY_H_080C6E90_E9F8_437B_AF93_8BA8C6823811_)
#define UTILITY_H_080C6E90_E9F8_437B_AF93_8BA8C6823811_

/*/
/ / HWND_BOTTOM, HWND_TOP, etc. are already defined in Winuser.h, but
/ / I don't want to cuase possible compatibility issues in the future.
/ / So, I'm defining my own constants for use with MoveWnd().
/*/
#define MV_BOTTOM	0x0001
#define MV_CENTER	0x0002
#define MV_LEFT		0x0004
#define MV_MIDDLE	0x0008
#define MV_RIGHT	0x0010
#define MV_TOP		0x0020

bool AddClipboardText (HWND hWnd, const char *szText);
bool MoveWnd		  (HWND hwnd, unsigned short nOrientation, HWND hwndPreferred);

#endif  // UTILITY.H
