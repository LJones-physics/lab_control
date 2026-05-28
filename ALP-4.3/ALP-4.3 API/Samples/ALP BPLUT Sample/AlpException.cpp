#include "AlpException.h"

CAlpException::CAlpException(long alpApiReturnCode, const std::string &operation, int exitCode) :
	mAlpApiReturnCode(alpApiReturnCode),
	mOperation(operation),
	mExitCode(exitCode)
{
}

CAlpException::~CAlpException(void)
{
}

void AlpError( long const apiResult, const std::string &operation )
{
	if (apiResult == ALP_OK)
		return;
	throw CAlpException(apiResult, operation);
}
