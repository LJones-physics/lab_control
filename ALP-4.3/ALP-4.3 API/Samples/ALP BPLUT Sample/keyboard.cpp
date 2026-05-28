#include "keyboard.h"
#include <conio.h>
#include <Windows.h>

using namespace std;

void pause( const std::string &prompt, const std::string &response, ostream& outStream )
{
	// clear keyboard buffer
	while (_kbhit()) _getch();
	// wait for key stroke
	outStream << prompt.c_str() << flush;
	_getch();
	outStream << response.c_str() << flush;
}

char choice( const std::string &prompt, const std::string &acceptableCharacters, const std::string &response,
	bool const ignoreCase, bool const echo, std::ostream& outStream )
{
	// compare only lower-case, if ignoreCase
	string filterKeys;
	if (ignoreCase)
	{
		for (auto it = acceptableCharacters.begin(); it != acceptableCharacters.end(); it++)
			filterKeys += (char) tolower(*it);
	} else
	{
		filterKeys = acceptableCharacters;
	}

	// read keys until one of the set of filterKeys is detected
	size_t matchIndex;
	outStream << prompt.c_str() << flush;
	do
	{
		int button(0);
		if (echo)
			button = _getche();
		else
			button = _getch();
		if (ignoreCase)
			button = tolower(button);
		matchIndex = filterKeys.rfind((char) button);
	} while (matchIndex == string::npos);
	outStream << response.c_str() << flush;

	// return original character code, which could be different from button, if ignoreCase==true
	return acceptableCharacters.at(matchIndex);
}
