This is a part of the ALP application programming interface.
Copyright (C) 2004-2014 ViALUX GmbH
All rights reserved.

Please always consult the ALP-4 API description when customizing
this program. It contains a detailled specification of all Alp... functions.


The BlackWhiteSample application primarily serves to illustrate the ALP
application programming interface features and programming techniques:

  1. Initialization of one ALP device.

  2. Inquiry of DMD type.

  3. Allocation of ALP sequence memory and transfer of image data into device
	 memory. (very simple sequence consisting of one black and one white
	 picture)

  4. Adjustment of timing properties for the sequence display.

  5. Start of sequence display.

  6. Stop the device and clean up.


All of the code resides in the _tmain() function in BlackWhiteSample.cpp.

ViALUX has supplied build settings by means of project files for different
versions of the Microsoft Visual Studio Software. However do not mix them.
Always clean the directory in the case of switching to another version, for
example use complete rebuild instead of incremental build.

Project settings are available for Microsoft Visual Studio 2005 (8.0;
BlackWhiteSample8.sln and BlackWhiteSample8.vcproj) and for
Visual Studio 2010 (10.0; BlackWhiteSample.sln, BlackWhiteSample.vcxproj, and
BlackWhiteSample.vcxproj.filters)

For other build systems make sure to have the ALP API header file alp.h in the
include path. Link the application with alp4395.lib.

The BlackWhiteSample.exe depends upon alp4395.dll. So make sure to
run it from a working directory that contains alp4395.dll.
This DLL file must match the built EXE (32 or 64 bit). Else it 
will not start, and instead shows error 0xc000007b.

