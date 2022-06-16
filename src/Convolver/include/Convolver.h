#pragma once

#include <vector>
#include <memory>

#include "Vector.h"
#include "RingBuffer.h"
#include "ErrorDef.h"
#include "Fft.h"
#include "Util.h"

class Convolver {
public:
	Convolver();
	~Convolver();

	Error_t init(const float const* ir, const int lengthOfIr, const int blockSize = 8192);
	Error_t reset();

	Error_t process(const float* inputBuffer, float* outputBuffer, int numSamples);
private:
	CFft* mFft = nullptr;;

	int mBlockSize = 0;
	int mFftSize = 0;
	bool mIsInitialized = false;

	// TODO: add buffers for fft computations
	std::unique_ptr<float> mProcessBuffer = nullptr;
};