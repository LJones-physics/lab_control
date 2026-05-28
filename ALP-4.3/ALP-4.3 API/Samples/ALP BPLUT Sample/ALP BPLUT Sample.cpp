/**
\file ALP BPLUT Sample.cpp
Entry point of the ViALUX Bitplane Lookup Table Sample.
*/

/*
	This sample is provided as-is, without any warranty.

	Please always consult the ALP-4.3 API description when
	customizing this program. It contains a detailled specification
	of all Alp... functions.

	A comprehensive explanation of the algorithms and approaches
	for gray-scale generation can be found in the document
	"ALP-4 Bitplane LUT Grayscale App Note.pdf".

	(c) 2023 ViALUX GmbH. All rights reserved.
*/

#define NOMINMAX
#include <Windows.h>

#include "alp.h"
#include "AlpSimpleApi.h"
#include "AlpException.h"
#include "AlpImage.h"
#include "keyboard.h"

#include <iostream>
using namespace std;

/// ALP standard gray-scale generation; default timing; step frame by frame
void demoGrayScale( CAlpSimpleApi &alp ) throw(...);

/// ALP standard gray-scale generation; scroll line by line; maximum frame rate
void demoGrayScaleScrolling( CAlpSimpleApi &alp, long const scrollToRow ) throw(...);

/// reduce number of bit-planes; no BPLUT; pictureTime=-1: maximum frequency, else µs
void demoBinaryScaleScrolling( CAlpSimpleApi &alp, long const scrollToRow, long const pictureTime ) throw(...);

/// use BPLUT = FRAME; pictureTime=-1: maximum frequency, else µs
void demoBinaryBplutFrame( CAlpSimpleApi &alp, long const scrollToRow, long const pictureTime ) throw(...);

/// use BPLUT = ROW; pictureTime=-1: maximum frequency, else µs
void demoBinaryBplutRow( CAlpSimpleApi &alp, long const scrollToRow, long const pictureTime ) throw(...);

/// each demo shows a description first, then starts ALP display.
/// The ALP sequence could sometimes be short, so the program could wait for another key stroke before starting.
/// Note: Single Demo cases can be repeated, anyway.
void waitForKeyToStart()
{
	// disabled: pause("Hit a key to start...");
}

/**
Entry point of the program.

main() does one-time ALP initialization, handles image files,
and implements the user interface for navigating through demo cases.
*/
int main(int argc, char* argv[])
{
	cout << "ALP BPLUT Sample" << endl << "(c) 2023 ViALUX GmbH" << endl;

	LPCSTR sImageFilename = "SampleImage.png";
	if (argc>=2)
		sImageFilename = argv[1];
		// ignore further arguments

	const long bitplanes = 4;
	const long maxGrayValue = (1<<bitplanes)-1;	///< 2^BPL - 1: maximum gray values, counted from 0..nMaxGrayValue

	try
	{
		/***** INITIALIZATION *****/
#pragma region init_device
		CAlpSimpleApi alp;
		alp.devOpen();

		cout << "Using ALP #" << alp.devInquire(ALP_DEVICE_NUMBER)
			<< " (" << alp.getDmdWidth() << "*" << alp.getDmdHeight() << " pixels)" << endl
			<< "Bitplanes to be tested: " << bitplanes << " (gray values 0.." << maxGrayValue << ")" << endl
			<< "Load image from file: " << sImageFilename << endl;

		// Restrict active DMD area to an integer multiple of nMaxGrayValue (for scrolling with BPLUT).
		// Hint: reduce the active area further for even faster scrolling speed.
		// This can be enabled/disabled per ALP sequence, and only works for binary display.
		long const nAoiHeight = alp.getDmdHeight() / maxGrayValue * maxGrayValue;
		alp.setBinaryAOI( 0, nAoiHeight );

		// Mandatory adjustment for using ALP_BITPLANE_LUT_ROW; acceptable for other modes as well
		alp.devControl( ALP_SEQ_CONFIG, ALP_SEQ_CONFIG_BITPLANE_LUT_ROW );

#pragma endregion

#pragma region init_image
		// Open image file, load data, scale to ALP sequence format, and add black fence areas
		CAlpImage img(alp.getDmdWidth(), alp.getDmdHeight());
		// Note: bottom fence would usually require only nAoiHeight, but if we do not use AOI,
		// then there should be a full DMD of black rows.
		img.setTopFence(nAoiHeight).setBottomFence(alp.getDmdHeight());
		if (! img.load(sImageFilename))
		{
			cerr << "ERROR loading image from file " << sImageFilename << endl;
			pause("Press a key to quit...");
			return 2;
		}
		long const scrollLastTopRow = img.getTopFence()+img.getPayloadHeight();
		cout << "scaled image to ALP sequence of " << img.getDmdWidth() << "*" << img.getPayloadHeight() << " pixels." << endl
			<< endl;
#pragma endregion

#pragma region init_sequence
		// Prepare sequence and download image data to ALP on-board memory
		alp.seqAlloc(bitplanes, img.getSequencePictures());
		alp.seqPut(img.getSequenceData());
		const long minPictureTimeGrayscale(alp.seqInquire(ALP_MIN_PICTURE_TIME));
#pragma endregion

		/***** RUN DEMOS *****/
#pragma region run_demos
		enum tDemoCase {
			DC_QUIT = 0,
			DC_GRAY,
			DC_GRAY_SCROLL,
			DC_BIN_SCROLL,
			DC_BIN_BPLUT_FRAME,
			DC_BIN_BPLUT_ROW,
			DC_UNSUPPORTED
		} demoCase = DC_GRAY;
		bool maximumSpeed(false);	///< only processed by binary display demos

		while (demoCase != DC_QUIT)
		{
			// Start a Demo:
			long const pictureTime = maximumSpeed?-1:minPictureTimeGrayscale;
			switch (demoCase)
			{
			case DC_GRAY: demoGrayScale(alp); break;
			case DC_GRAY_SCROLL: demoGrayScaleScrolling(alp, scrollLastTopRow); break;
			case DC_BIN_SCROLL: demoBinaryScaleScrolling(alp, scrollLastTopRow, pictureTime); break;
			case DC_BIN_BPLUT_FRAME: demoBinaryBplutFrame(alp, scrollLastTopRow, pictureTime); break;
			case DC_BIN_BPLUT_ROW: demoBinaryBplutRow(alp, scrollLastTopRow, pictureTime); break;
			default: cout << "Invalid selection, demo " << (int) demoCase << " not supported." << endl; break;
			}

			// Let the user choose next Demo:
			char button = choice( "Select demo: Space=repeat, Return=next, 1..9=jump, f=toggle frame rate, q=quit ", "f \r123456789q", "-> ", true, false);
			switch (button)
			{
			case 'q' :
				demoCase = DC_QUIT;
				cout << "quit" << endl << endl;
				break;
			case VK_RETURN :	// '\r'
				demoCase = (tDemoCase) (demoCase+1);
				if (DC_UNSUPPORTED==demoCase)
				{
					cout << "finished, quit." << endl << endl;
					demoCase = DC_QUIT;
				} else
				{
					cout << "next (demo " << demoCase << ")" << endl << endl;
				}
				break;
			case ' ' :
				cout << "repeat (demo " << demoCase << ")" << endl << endl;
				break;
			case 'f' :
				if (maximumSpeed)
				{
					cout << "reduce frame rate" << endl << endl;
					maximumSpeed = false;
				} else
				{
					cout << "maximum frame rate" << endl << endl;
					maximumSpeed = true;
				}
				break;
			case '1': case '2': case '3':
			case '4': case '5': case '6':
			case '7': case '8': case '9':
				demoCase = (tDemoCase) (button-'0');
				cout << "demo " << button << endl << endl;
				break;
			}
		}
#pragma endregion

	} catch (CAlpException exception)
	{
		cerr << "ERROR " << exception.mAlpApiReturnCode << " in " << exception.mOperation.c_str() << endl;
		pause("Press a key to quit...");
		return exception.mExitCode;
	}

	cout << "Finished successfully" << endl;
	pause("Press a key to quit...");
	return 0;
}

void demoGrayScale( CAlpSimpleApi &alp ) throw(...)
{
	// Display the sequence step by step with ALP standard grayscale generation mode.
	cout << "Standard ALP grayscale display:" << endl
		<< "What to expect?" << endl
		<< "- step through the sequence one DMD per frame" << endl
		<< "  that means " << alp.seqInquire(ALP_PICNUM) << " frames per sequence" << endl
		<< "- use standard grayscale algorithm with " << alp.getSeqBitplanes() << " bit-planes" << endl
		<< "- use default display timing: " << alp.seqInquire(ALP_PICTURE_TIME)/1000 << "ms per frame" << endl
		<< "- sequence is repeated" << endl;

	waitForKeyToStart();
	alp.projStartCont();
	pause("Running. Hit a key to stop...");
	alp.projStop();

	cout << "Standard ALP grayscale display done." << endl << endl;
}

void demoGrayScaleScrolling( CAlpSimpleApi &alp, long const scrollToRow  ) throw(...)
{
	long const frameCount = scrollToRow+1;	///< display one more frame because the scrolling starts at row 0

	// Enable scrolling display and increase display speed
	long const minPictureTimeGrayscale(alp.seqInquire(ALP_MIN_PICTURE_TIME));
	alp.seqTiming(minPictureTimeGrayscale);
	alp.setSeqScroll(1, 0, scrollToRow);

	cout << "Scrolling with standard ALP grayscale display:" << endl
		<< "What to expect?" << endl
		<< "- the first frame shows the black top fence area" << endl
		<< "- then payload scrolls into the DMD from the bottom edge" << endl
		<< "- finally, the bottom fence area scrolls in" << endl
		<< "- use standard grayscale algorithm with " << alp.getSeqBitplanes() << " bit-planes" << endl
		<< "- use fastest display speed: " << minPictureTimeGrayscale*frameCount/1000 << "ms per sequence" << endl
		<< "- sequence is started for one pass only" << endl;

	waitForKeyToStart();
	alp.projStart(1);
	alp.projWait();
	cout << "Scrolling in grayscale mode done." << endl << endl;

	// Revert settings to default:
	alp.setSeqNoScroll();
	alp.seqTiming(ALP_DEFAULT);
}

/// Select from maximum frame rate (-1) and specified frame rate (1/pictureTime). Common code for several demo cases.
void setTiming( CAlpSimpleApi &alp, long const pictureTime, long const frameCount )
{
	if (pictureTime == -1)
	{
		const long minPictureTime(alp.seqInquire(ALP_MIN_PICTURE_TIME));
		cout << "- use fastest display speed: " << minPictureTime*frameCount/1000 << "ms per sequence" << endl;
		alp.seqTiming(minPictureTime);
	} else
	{
		cout << "- use reduced display speed: " << pictureTime*frameCount/1000 << "ms per sequence" << endl;
		alp.seqTiming(pictureTime);
	}
}

void demoBinaryScaleScrolling( CAlpSimpleApi &alp, long const scrollToRow, long const pictureTime ) throw(...)
{
	// Use binary display; the required AlpSeqTiming() call is done by setTiming() below
	alp.seqControl(ALP_BITNUM, 1);
	alp.seqControl(ALP_BIN_MODE, ALP_BIN_UNINTERRUPTED);
	alp.setSeqScroll(1, 0, scrollToRow);

	cout << "Scrolling in binary uninterrupted mode, only MSB:" << endl
		<< "What to expect?" << endl
		<< "- only the most-significant bit plane (MSB) is displayed" << endl;
	setTiming(alp, pictureTime, scrollToRow+1);
	cout << "- sequence is started for one pass only" << endl;

	waitForKeyToStart();
	alp.projStart(1);
	alp.projWait();
	cout << "Scrolling in binary uninterrupted mode, only MSB, done." << endl << endl;

	// Revert settings to default:
	alp.seqControl(ALP_BITNUM, ALP_DEFAULT);
	alp.seqTiming(ALP_DEFAULT);
	alp.setSeqNoScroll();
}

void demoBinaryBplutFrame( CAlpSimpleApi &alp, long const scrollToRow, long const pictureTime ) throw(...)
{
	// Generate a high-frequency bit-plane lookup table:
	CAlpBplut bplutData;
	bplutData.generateDeltaSigma(alp.getSeqBitplanes());
	// Hint: also try out e.g. bplutData.generateAlpStandard

	// If wanted, the bitplane switch frequency can be reduced:
	// bplutData.distribute( alp.getBinaryAoiHeight()/bplutData.getSize() );

	// Use binary display, AOI, BPLUT; the required seqTiming() call is below
	alp.seqControl(ALP_BITNUM, 1);
	alp.seqControl(ALP_BIN_MODE, ALP_BIN_UNINTERRUPTED);
	alp.seqControl(ALP_SEQ_DMD_LINES, alp.getBinaryAOI());
	alp.seqControl(ALP_BITPLANE_LUT_MODE, ALP_BITPLANE_LUT_FRAME);
	alp.seqControl(ALP_BITPLANE_LUT_ENTRIES, bplutData.getSize());
	alp.setBplut(bplutData);
	alp.setSeqScroll(1, 0, scrollToRow);

	cout << "Scrolling with bit-plane lookup per DMD FRAME (binary uninterrupted display):" << endl
		<< "What to expect?" << endl
		<< "- BPLUT causes bit-planes to be displayed in the following order:" << endl
		<< "  " << bplutData.tostring(35) << endl;
	setTiming(alp, pictureTime, scrollToRow+1);
	cout << "- sequence is started for one pass only" << endl;

	waitForKeyToStart();
	alp.projStart(1);
	alp.projWait();
	cout << "Scrolling with bit-plane lookup per DMD FRAME, done." << endl << endl;

	// Revert settings to default:
	alp.seqControl(ALP_SEQ_DMD_LINES, ALP_DEFAULT);
	alp.seqControl(ALP_BITPLANE_LUT_MODE, ALP_DEFAULT);
	alp.seqControl(ALP_BITNUM, ALP_DEFAULT);
	alp.seqTiming(ALP_DEFAULT);
	alp.setSeqNoScroll();
}

void demoBinaryBplutRow( CAlpSimpleApi &alp, long const scrollToRow, long const pictureTime ) throw(...)
{
	// Generate a high-frequency bit-plane lookup table:
	CAlpBplut bplutData;
	bplutData.generateDeltaSigma(alp.getSeqBitplanes());
	// Hint: also try out e.g. bplutData.generateAlpStandard

	// If wanted, the bitplane switch frequency can be reduced:
	// bplutData.distribute( alp.getBinaryAoiHeight()/bplutData.getSize() );

	// Use binary display, AOI, BPLUT; the required seqTiming() call is below
	alp.seqControl(ALP_BITNUM, 1);
	alp.seqControl(ALP_BIN_MODE, ALP_BIN_UNINTERRUPTED);
	alp.seqControl(ALP_SEQ_DMD_LINES, alp.getBinaryAOI());
	alp.seqControl(ALP_BITPLANE_LUT_MODE, ALP_BITPLANE_LUT_ROW);
	// alp.seqControl(ALP_BITPLANE_LUT_ENTRIES, not required)		// LUT_ROW mode: the LUT size always matches the DMD height (or AOI height, if enabled)
	alp.setBplut(bplutData.duplicate(alp.getBinaryAoiHeight()));	// Duplicate: There must be one valid bitplane entry per active DMD row!
	alp.setSeqScroll(1, 0, scrollToRow);

	cout << "Scrolling with bit-plane lookup per DMD ROW (binary uninterrupted display):" << endl
		<< "What to expect?" << endl
		<< "- BPLUT causes bit-planes to be combined to frames row-by-row in the following order:" << endl
		<< "  " << bplutData.tostring(35) << endl;
	setTiming(alp, pictureTime, scrollToRow+1);
	cout << "- sequence is started for one pass only" << endl;
	
	waitForKeyToStart();
	alp.projStart(1);
	alp.projWait();
	cout << "Scrolling with bit-plane lookup per DMD ROW done." << endl << endl;

	// Revert settings to default:
	alp.seqControl(ALP_SEQ_DMD_LINES, ALP_DEFAULT);
	alp.seqControl(ALP_BITPLANE_LUT_MODE, ALP_DEFAULT);
	alp.seqControl(ALP_BITNUM, ALP_DEFAULT);
	alp.seqTiming(ALP_DEFAULT);
	alp.setSeqNoScroll();
}
