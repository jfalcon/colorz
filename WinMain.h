#if !defined (WINMAIN_H_5106F918_520E_482F_94BC_FBA29A9424E3_)
#define WINMAIN_H_5106F918_520E_482F_94BC_FBA29A9424E3_

// standard definitions
#define STRICT						// enable strict type-checking of Windows handles
#define WIN32_LEAN_AND_MEAN			// allow the exclusion of uncommon features
#define _CRT_SECURE_NO_DEPRECATE	// maintain compatibility with older platform SDKs

// app specific definitions
#define WEBSAFE	51					// 0x33

// standard includes
#include <windows.h>				// fundamental Windows header file
#include <windowsx.h>				// useful Windows programming extensions
#include <commctrl.h>				// windows common controls library
#include <string.h>					// includes string manipulation routines
#include <stdio.h>					// includes standard input/output routines

#pragma warning (disable:4100)		// unreferenced formal parameter
#pragma warning (disable:4306)		// conversion from 'type1' to 'type2' of greater size

#ifndef __cplusplus
	// mimic a boolean type in C
	typedef enum {false=FALSE, true=TRUE} bool;
#endif

#endif  // WINMAIN.H
