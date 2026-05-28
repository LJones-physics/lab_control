/** (c) 2023 ViALUX GmbH. All rights reserved. */

#pragma once

#include <Windows.h>
#include <GdiPlus.h>

/** manage initialization of Windows GDI+ library.

- EXE files: define a global variable of this type anywhere to initialize GDI+ for the whole run-time of the program.
- DLL files: do not use it globally. Use locally, take care to destroy all GDI+ objects before this instance.

Recommendation: Use the macro "GDIPLUS_APP" to create a variable - either in a local scope or globally, as discussed above.
*/

#define	GDIPLUS_APP	GdiplusApplication __theGdiplusApp

class GdiplusApplication
{
public:
	/// call GdiplusStartup()
	GdiplusApplication(void);
	/// call GdiplusShutdown()
	~GdiplusApplication(void);
private:
	Gdiplus::GdiplusStartupInput	gdiplusStartupInput;
	ULONG_PTR						gdiplusToken;
};
