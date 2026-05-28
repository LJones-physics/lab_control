This Programming Sample is part of the ALP API.
It is provided as-is, without any warranty.
Copyright (C) 2018 ViALUX GmbH
All rights reserved.

Please always consult the ALP-4 API description when customizing
this program. It contains a detailed specification of all Alp... functions.
A description of the used formulas can be found in the 
"ALP-4.3 Shear Scrolling.pdf" documentation.

-----------------------------------------------------------------------
ALP Shearing Sample:


Overview:
The intention of this program is to provide a simple code-example how the shearing-extension 
can be used in combination with scrolling to manipulate the frame-data without touching the 
SDRAM-Data and how rotation can be achieved by shearing previously shifted frames.


Program-description:
The program consists of two stages:
In the first stage, a scrolling chess-board-pattern is displayed. By pressing the spacebar-key,
the user can switch between three different shearing-modes:

1. no shearing
2. linear shearing (i.e. the shearing value grows linear from 0 to 511 with increasing line number) 
3. sinus shearing (i.e. the shearing values oscillate in a way that creates a sinus-shaped curve)

The user can proceed to the second stage by pressing any other key except spacebar.


In the second stage, the chess-board is vertically shifted in a way that the activation of 
linear shearing will cause the frame to look rotated.
By pressing the spacebar-key, the user can switch between two different shearing-modes:

1. no shearing
2. linear shearing (the displayed frame looks rotated)

Now the user can exit the program by hitting any key.


------------------------------------------------------------------------
Below follows an automatic comment of Microsoft Visual Studio 2010,
generated when creating this project.

========================================================================
    CONSOLE APPLICATION : ALP Shearing Sample Project Overview
========================================================================

AppWizard has created this ALP Shearing Sample application for you.

This file contains a summary of what you will find in each of the files that
make up your ALP Shearing Sample application.


ALP Shearing Sample.vcxproj
    This is the main project file for VC++ projects generated using an Application Wizard.
    It contains information about the version of Visual C++ that generated the file, and
    information about the platforms, configurations, and project features selected with the
    Application Wizard.

ALP Shearing Sample.vcxproj.filters
    This is the filters file for VC++ projects generated using an Application Wizard. 
    It contains information about the association between the files in your project 
    and the filters. This association is used in the IDE to show grouping of files with
    similar extensions under a specific node (for e.g. ".cpp" files are associated with the
    "Source Files" filter).

ALP Shearing Sample.cpp
    This is the main application source file.

/////////////////////////////////////////////////////////////////////////////
Other standard files:

StdAfx.h, StdAfx.cpp
    These files are used to build a precompiled header (PCH) file
    named ALP Shearing Sample.pch and a precompiled types file named StdAfx.obj.

/////////////////////////////////////////////////////////////////////////////
Other notes:

AppWizard uses "TODO:" comments to indicate parts of the source code you
should add to or customize.

/////////////////////////////////////////////////////////////////////////////
