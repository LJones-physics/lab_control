This Programming Sample is part of the ALP API.
It is provided as-is, without any warranty.
Copyright (C) 2018 ViALUX GmbH
All rights reserved.

Please always consult the ALP-4 API description when customizing
this program. It contains a detailed specification of all Alp... functions.

------------------------------------------------------------------------
ALP Area of Interest Sample:

This sample demonstrates how to correctly combine Area of Interest with data download via AlpSeqPutEx.

The program allocates three AOI-Frames (AOI-size is half the dmd-size) within an sequence of two frames.
The pictures are containing an rectangle, moving from the left of the dmd across the middle to the right.  

The frame rate of the sequence-display and the highest available frame rate for the sequence (with respect to the selected AOI) is also displayed.

------------------------------------------------------------------------
Below follows an automatic comment of Microsoft Visual Studio 2010,
generated when creating this project.

========================================================================
    CONSOLE APPLICATION : ALP Area of Interest Sample Project Overview
========================================================================

AppWizard has created this ALP Area of Interest Sample application for you.

This file contains a summary of what you will find in each of the files that
make up your ALP Area of Interest Sample application.


ALP Area of Interest Sample.vcxproj
    This is the main project file for VC++ projects generated using an Application Wizard.
    It contains information about the version of Visual C++ that generated the file, and
    information about the platforms, configurations, and project features selected with the
    Application Wizard.

ALP Area of Interest Sample.vcxproj.filters
    This is the filters file for VC++ projects generated using an Application Wizard. 
    It contains information about the association between the files in your project 
    and the filters. This association is used in the IDE to show grouping of files with
    similar extensions under a specific node (for e.g. ".cpp" files are associated with the
    "Source Files" filter).

ALP Area of Interest Sample.cpp
    This is the main application source file.

/////////////////////////////////////////////////////////////////////////////
Other standard files:

StdAfx.h, StdAfx.cpp
    These files are used to build a precompiled header (PCH) file
    named ALP Area of Interest Sample.pch and a precompiled types file named StdAfx.obj.

/////////////////////////////////////////////////////////////////////////////
Other notes:

AppWizard uses "TODO:" comments to indicate parts of the source code you
should add to or customize.

/////////////////////////////////////////////////////////////////////////////
