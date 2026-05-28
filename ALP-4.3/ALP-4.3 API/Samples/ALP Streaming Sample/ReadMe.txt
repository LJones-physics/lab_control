This Programming Sample is part of the ALP API.
It is provided as-is, without any warranty.
Copyright (C) 2023 ViALUX GmbH
All rights reserved.

Please always consult the ALP-4 API description when customizing
this program. It contains a detailed specification of all Alp... functions.

------------------------------------------------------------------------
ALP Streaming Sample:

This sample uses a triple buffer to achieve a continuous projection while concurrently
loading new user specific frame data.
The user data must be packed in files with the ".alp" file extension. Use the
"ALP ImageConverter Sample" to generate such files.
The program will read and project all ".alp" files stored in a specified folder
in an infinite loop, until the user is exiting the program.
If the projection is faster than the loading of new frames, a warning will be 
displayed, asking the user to lower the frame rate.

------------------------------------------------------------------------
Below follows an automatic comment of Microsoft Visual Studio 2010,
generated when creating this project.

========================================================================
    CONSOLE APPLICATION : ALP Streaming Sample Project Overview
========================================================================

AppWizard has created this ALP Streaming Sample application for you.

This file contains a summary of what you will find in each of the files that
make up your ALP Streaming Sample application.


ALP Streaming Sample.vcxproj
    This is the main project file for VC++ projects generated using an Application Wizard.
    It contains information about the version of Visual C++ that generated the file, and
    information about the platforms, configurations, and project features selected with the
    Application Wizard.

ALP Streaming Sample.vcxproj.filters
    This is the filters file for VC++ projects generated using an Application Wizard. 
    It contains information about the association between the files in your project 
    and the filters. This association is used in the IDE to show grouping of files with
    similar extensions under a specific node (for e.g. ".cpp" files are associated with the
    "Source Files" filter).

ALP Streaming Sample.cpp
    This is the main application source file.

/////////////////////////////////////////////////////////////////////////////
Other standard files:

StdAfx.h, StdAfx.cpp
    These files are used to build a precompiled header (PCH) file
    named ALP Streaming Sample.pch and a precompiled types file named StdAfx.obj.

/////////////////////////////////////////////////////////////////////////////
Other notes:

AppWizard uses "TODO:" comments to indicate parts of the source code you
should add to or customize.

/////////////////////////////////////////////////////////////////////////////
