#pragma once

#include <iostream>
#include <string>

/// Wait until user presses a key. Print prompt (to outStream) before waiting, and response afterwards.
void pause( const std::string& prompt = "Press a key...", const std::string& response = "\n", std::ostream &outStream = std::cout );

/// Print prompt (to outStream) and read keys until one from the set of acceptableCharacters. Return the key code.
char choice( const std::string &prompt = "Select yes or no ", const std::string &acceptableCharacters = "yn", const std::string &response = "\n",
	bool const ignoreCase = true, bool const echo = true, std::ostream& outStream = std::cout );
