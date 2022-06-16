#pragma once

#include "ErrorDef.h"

class Convolver {
public:
	Convolver();
	~Convolver();

	Error_t init(float* ir, int lengthOfIr, int blockSize = 8192);
	Error_t reset();

	Error_t process(const float* inputBuffer, float* outputBuffer, int numSamples);
private:

};