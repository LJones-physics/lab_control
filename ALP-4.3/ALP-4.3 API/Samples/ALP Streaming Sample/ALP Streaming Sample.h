#include "stdafx.h"
#include "atlstr.h"
#include "alp.h"
#include "AlpUserInterface.h"
#include <crtdbg.h>
#include <memory>
#include <iostream>
#include <fstream>
#include "shlwapi.h"

// Error handling policy: Quit, whenever an ALP error happens.
// VERIFY_ALP also echoes each successful ALP API call (in contrast to VERIFY_ALP_NO_ECHO)
#define VERIFY_ALP( AlpApiCall ) \
	if (AlpError(AlpApiCall, _T(#AlpApiCall), true)) { Pause(); return 1; }
#define VERIFY_ALP_NO_ECHO( AlpApiCall ) \
	if (AlpError(AlpApiCall, _T(#AlpApiCall), false)) { Pause(); return 1; }

DWORD WINAPI ThreadFunctionLoadFramesFromFile( LPVOID lpThreadParameter );
DWORD WINAPI ThreadFunctionAlpSeqPut( LPVOID lpThreadParameter );
CString getExeDir();

struct tThreadFunctionVariables {
	
	tThreadFunctionVariables(
		ALP_ID AlpDevId, 
		ALP_ID* AlpSeqId,
		bool* bBufferLoaded,
		bool* bBufferCommitted, 
		bool* bExit, 
		bool* bError, 
		long nBufferSize,
		long nFramesPerBuffer,
		long nBufferNumber, 
		UCHAR* pFrameData,
		CString directoryNameLOAD,
		HANDLE* eBufferLoadDone,
		HANDLE* eBufferCommitDone,
		HANDLE* eBufferLoadCleared,
		HANDLE* eBufferCommitCleared	):

		AlpDevId(AlpDevId),
		AlpSeqId(AlpSeqId),
		bBufferLoaded(bBufferLoaded),
		bBufferCommitted(bBufferCommitted),
		bExit(bExit),
		bError(bError), 
		nBufferSize(nBufferSize),
		nFramesPerBuffer(nFramesPerBuffer),
		nBufferNumber(nBufferNumber),
		pFrameData(pFrameData),
		directoryNameLOAD(directoryNameLOAD),
		eBufferLoadDone(eBufferLoadDone),
		eBufferCommitDone(eBufferCommitDone),
		eBufferLoadCleared(eBufferLoadCleared),
		eBufferCommitCleared(eBufferCommitCleared)
	{};
	
	ALP_ID	AlpDevId;
	ALP_ID* AlpSeqId;
	bool* bBufferLoaded;
	bool* bBufferCommitted;
	bool* bExit;
	bool* bError;
	long nBufferSize;
	long nFramesPerBuffer;
	long nBufferNumber;
	UCHAR* pFrameData;
	CString directoryNameLOAD;
	HANDLE* eBufferLoadDone;
	HANDLE* eBufferCommitDone;
	HANDLE* eBufferLoadCleared;
	HANDLE* eBufferCommitCleared;
};