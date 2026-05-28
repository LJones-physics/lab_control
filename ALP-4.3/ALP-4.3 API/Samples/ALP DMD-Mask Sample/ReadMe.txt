This Programming Sample is part of the ALP API.
It is provided as-is, without any warranty.
Copyright (C) 2017-2021 ViALUX GmbH
All rights reserved.

Please always consult the ALP-4 API description when customizing
this program. It contains a detailed specification of all Alp... functions.

------------------------------------------------------------------------
ALP DMD-Mask Sample:


Overview:
The intention of this program is to show the user a simple code example how 
the DMD mask provided by the ALP API can be used.
It also demonstrates the ability to turn the mask on or off while a projection
is running on the DMD and that the mask is unaffected by the ALP Scrolling extension.


Program description:
After the initialization of the ALP Device, a mask will be set up, which has the form of a
rectangle and is bounded by 1/4 of the DMD columns/rows to the left/top and 1/4 of the 
DMD columns/rows to the right/bottom.
The mask is laid over a white picture, containing two vertical and two horizontal black lines, which
are indicating the boundaries of the mask.  

i.e.:

               1/4    2/4    1/4

             --------------------
             |    |        |    |   1/4
             |----|--------|----|
             |    |########|    |   2/4
             |    |########|    |
             |----|--------|----|
             |    |        |    |   1/4
             --------------------

# = Pixels covered by the DMD mask

The mask can be turned on and off without stopping the projection by using the spacebar key.

After the user as pressed any key except spacebar, the program will turn on the mask permanently and
project a second sequence which contains multiple frames, showing chess board patterns.
Scrolling will be activated, which will affect the displayed frames but leaves the mask untouched. 

Now the user can exit the program by hitting any key.

------------------------------------------------------------------------
History:
05/2021	Translate ALP API functions from "legacy" 2kB mask to high-resolution 16kB mask
	(ALP_DMD_MASK_XY, ALP_DMD_MASK_WRITE_16K)
06/2019	Add Visual Studio 2019 Solution file
11/2017	Initial creation

------------------------------------------------------------------------
Below follows an automatic comment of Microsoft Visual Studio 2010,
generated when creating this project.

========================================================================
    CONSOLE APPLICATION : ALP DMD-Mask Sample Project Overview
========================================================================

AppWizard has created this ALP DMD-Mask Sample application for you.

This file contains a summary of what you will find in each of the files that
make up your ALP DMD-Mask Sample application.


ALP DMD-Mask Sample.vcxproj
    This is the main project file for VC++ projects generated using an Application Wizard.
    It contains information about the version of Visual C++ that generated the file, and
    information about the platforms, configurations, and project features selected with the
    Application Wizard.

ALP DMD-Mask Sample.vcxproj.filters
    This is the filters file for VC++ projects generated using an Application Wizard. 
    It contains information about the association between the files in your project 
    and the filters. This association is used in the IDE to show grouping of files with
    similar extensions under a specific node (for e.g. ".cpp" files are associated with the
    "Source Files" filter).

ALP DMD-Mask Sample.cpp
    This is the main application source file.

/////////////////////////////////////////////////////////////////////////////
Other standard files:

StdAfx.h, StdAfx.cpp
    These files are used to build a precompiled header (PCH) file
    named ALP DMD-Mask Sample.pch and a precompiled types file named StdAfx.obj.

/////////////////////////////////////////////////////////////////////////////
Other notes:

AppWizard uses "TODO:" comments to indicate parts of the source code you
should add to or customize.

/////////////////////////////////////////////////////////////////////////////
