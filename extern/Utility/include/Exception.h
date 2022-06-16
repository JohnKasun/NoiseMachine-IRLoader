#pragma once

#include <iostream>
#include <exception>
#include <iomanip>

class Exception : public std::exception {
public:
	Exception(const char* message) : mMessage(message) {};
	const char* what() const override{
		return mMessage;
	}
private:
	const char* mMessage;
};