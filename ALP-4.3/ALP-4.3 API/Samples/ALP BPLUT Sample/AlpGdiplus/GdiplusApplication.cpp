/** (c) 2023 ViALUX GmbH. All rights reserved. */

#include "GdiplusApplication.h"
#pragma comment (lib, "gdiplus.lib")
using namespace Gdiplus;

GdiplusApplication::GdiplusApplication(void)
{
	Status m_Status = GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
	UNREFERENCED_PARAMETER( m_Status );
}

GdiplusApplication::~GdiplusApplication(void)
{
	if(gdiplusToken)
		GdiplusShutdown(gdiplusToken);
}
