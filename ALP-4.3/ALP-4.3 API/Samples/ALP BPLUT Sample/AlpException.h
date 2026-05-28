#pragma once

#include <string>
#include "alp.h"

/// Exception class thrown on ALP error (by AlpError())
class CAlpException
{
	/// hide default constructor
	CAlpException(void);	// = delete;
	/// hide assignment operator
	CAlpException& operator=(const CAlpException& rhs);	// = delete;
public:
	CAlpException(const long alpApiReturnCode, const std::string &operation, int exitCode=1);
	~CAlpException(void);

	const long mAlpApiReturnCode;	///< error code returned by an Alp...() function call
	const std::string mOperation;	///< description of the operation intented by this function call
	const int mExitCode;			///< suggested exit code if the exception causes the program to stop
};

/// if apiResult!=ALP_OK, throw a CAlpException, else silently return
void AlpError( long const apiResult, const std::string &operation );
