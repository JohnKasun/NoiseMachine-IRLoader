#pragma once

#include <vector>
#include <memory>

#include "Vector.h"
#include "RingBuffer.h"
#include "ErrorDef.h"
#include "Fft.h"

class Convolver {
public:
	Convolver();
	~Convolver();

	Error_t init(float* ir, int lengthOfIr, int blockSize = 8192);
	Error_t reset();

	Error_t process(const float* inputBuffer, float* outputBuffer, int numSamples);
private:
	CFft* mFft = nullptr;;
	std::unique_ptr<CRingBuffer<float>> mDelayLine = nullptr;
	std::unique_ptr<float> mIr = nullptr;

	int mBlockSize = 0;
	int mFftSize = 0;
	bool mIsInitialized = false;

	// TODO: add buffers for fft computations
};