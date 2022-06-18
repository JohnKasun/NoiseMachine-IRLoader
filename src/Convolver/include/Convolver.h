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

	int getTailLength() const;
	Error_t flushBuffer(float* outputBuffer) const;
	Error_t process(const float* inputBuffer, float* outputBuffer, int numSamples);
private:
	CFft* mFft = nullptr;;

	int mBlockSize = 0;
	int mFftSize = 0;
	int mNumIrBlocks = 0;
	int mLengthOfTail = 0;
	bool mIsInitialized = false;

	std::unique_ptr<float> mProcessBuffer = nullptr;
	std::unique_ptr<float> mProcessReal = nullptr;
	std::unique_ptr<float> mProcessRealCopy = nullptr;
	std::unique_ptr<float> mProcessImag = nullptr;
	std::unique_ptr<float> mTail = nullptr;
	std::vector<std::unique_ptr<float>> mIrReal;
	std::vector<std::unique_ptr<float>> mIrImag;
};