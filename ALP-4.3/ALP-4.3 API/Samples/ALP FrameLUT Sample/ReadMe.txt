This Programming Sample is part of the ALP API.
It is provided as-is, without any warranty.
Copyright (C) 2017 ViALUX GmbH
All rights reserved.

Please always consult the ALP-4 API description when customizing
this program. It contains a detailed specification of all Alp... functions.

------------------------------------------------------------------------
ALP FLUT Sample:


Overview:
The intention of this program is to show the user a simple code-example how 
the frame-look-up-table provided by the ALP-API can be used.
It also demonstrates the ability to write the FLUT while a projection
is running on the dmd.


Program-description:
After the initialization of the ALP-Device, a sequence of 10 frames will be allocated.
Each frame contains a vertical stripe, which covers a tenth of the dmd-width.

i.e.:

first frame: ------------       second frame: ------------      last frame: ------------
             |#         |                     | #        |                  |         #|
             |#         |                     | #        |                  |         #|
             |#         |                     | #        |                  |         #|
             |#         |                     | #        |                  |         #|  
             ------------                     ------------                  ------------

In the first projection-step, the sequence of frames is shown in a linear order, i.e.:
frame 1, frame 2 .... frame 10.

After the user has pressed any key, the program will load and activate a FLUT, which will 
cause the frames to be displayed in a different order after projection was restarted.
The order is as follows: 1, 6, 2, 7, 3, 8, 4, 9, 5, 10.

When the user hits a key again, the program will write 'frame 5' to all FLUT-entries and
restart the projection. This way, a vertical stripe at the fifth "position" is permanently shown.
After that, the program waits for the user to press the left <- and right -> arrow-keys, which
will increase or decrease the FLUT-value and will shift the stripe on the dmd in the respective direction immediately.

The user can exit the program by hitting spacebar.


------------------------------------------------------------------------
Below follows an automatic comment of Microsoft Visual Studio 2010,
generated when creating this project.

========================================================================
    CONSOLE APPLICATION : ALP FLUT Sample Project Overview
========================================================================

AppWizard has created this ALP FLUT Sample application for you.

This file contains a summary of what you will find in each of the files that
make up your ALP FLUT Sample application.


ALP FLUT Sample.vcxproj
    This is the main project file for VC++ projects generated using an Application Wizard.
    It contains information about the version of Visual C++ that generated the file, and
    information about the platforms, configurations, and project features selected with the
    Application Wizard.

ALP FLUT Sample.vcxproj.filters
    This is the filters file for VC++ projects generated using an Application Wizard. 
    It contains information about the association between the files in your project 
    and the filters. This association is used in the IDE to show grouping of files with
    similar extensions under a specific node (for e.g. ".cpp" files are associated with the
    "Source Files" filter).

ALP FLUT Sample.cpp
    This is the main application source file.

/////////////////////////////////////////////////////////////////////////////
Other standard files:

StdAfx.h, StdAfx.cpp
    These files are used to build a precompiled header (PCH) file
    named ALP FLUT Sample.pch and a precompiled types file named StdAfx.obj.

/////////////////////////////////////////////////////////////////////////////
Other notes:

AppWizard uses "TODO:" comments to indicate parts of the source code you
should add to or customize.

/////////////////////////////////////////////////////////////////////////////
