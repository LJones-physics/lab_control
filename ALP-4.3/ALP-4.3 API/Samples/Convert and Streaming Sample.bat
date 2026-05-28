@echo off
:: This script call the "ALP ImageConverter Sample" and the "ALP Streaming Sample" to 
:: convert and display the images, placed in the "Sequence" subfolder.
:: ALP imageConverter will generate a file called "ConvertedFrames.ALP" containing all images stored in the folder Sequence.
:: ALP Streaming Sample will then read and project this file in a continuous loop.
:: You could also place additional ".ALP-files" in this folder. In this case, all files in the folder will be looped continuous.

:: XGA resolution
set nFrameWidth=1024
set nFrameHeight=768

:: The "ALP ImageConverter Sample" will scale the height of the frame to that value.
:: The scaled width will be computed automatically.
set nScaledHeight=384

:: Flip image vertically
set bFlipVertical=0

:: If turned on, input images will be split in tree frames containing the red, green and blue part of the input image.
:: If turned off, the converted frame will be monochrome.
:: This option is useful for projection with a RGB projector like the STAR-07 3.0 RGB.
set RgbMode=0

:: Frame rate in fps
set nFrameRate=2

:: "ALP Streaming Sample.exe" accepts only BinaryTopDown.
set bUseDataFormatBinaryTopdown=1

:: Reduce this value, if "ALP Streaming Sample.exe" reports an error due to insufficient memory.
:: Must be a multiple of 3 if RgbMode=1
set nFramesPerBuffer=3000

:: Variables for file and directory handling.
set InputDirectoryConversion="sequences\SequenceTextAndCircle-XGA"
set InputDirectoryStreaming=""
set ConversionOutputFileName="ConvertedFrames"

:: call the programs
call "ALP ImageConverter Sample.exe" %nFrameWidth% %nFrameHeight% %nScaledHeight% %bFlipVertical% %RgbMode% %bUseDataFormatBinaryTopdown% %InputDirectoryConversion% %ConversionOutputFileName%	
call "ALP Streaming Sample.exe" %nScaledHeight% %nFrameRate% %InputDirectoryStreaming% %nFramesPerBuffer%