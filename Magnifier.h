#if !defined (MAGNIFIER_H_A66634B4_E49F_4FA4_8ED8_314DFCE741DF_)
#define MAGNIFIER_H_A66634B4_E49F_4FA4_8ED8_314DFCE741DF_

bool CreateMagnifier	(const HWND hParent);
void DestroyMagnifier	(void);
void ShowMagnifier		(const bool bShow);
void SetMagnifierPos	(const HWND hWndxy, const int x, const int y);

#endif  // MAGNIFIER.H
