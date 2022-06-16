#include "Convolver.h"

Convolver::Convolver()
{
	CFft::createInstance(mFft);
}

Convolver::~Convolver()
{
	reset();
	CFft::destroyInstance(mFft);
}

Error_t Convolver::init(float* ir, int lengthOfIr, int blockSize)
{
	if (!ir)
		return Error_t::kMemError;
	if (lengthOfIr < 0 || blockSize < 0)
		return Error_t::kFunctionInvalidArgsError;

	reset();

	// Copy over Ir
	mIr.reset(new float[lengthOfIr]);
	CVectorFloat::copy(mIr.get(), ir, lengthOfIr);

	// Init fft
	mFft->initInstance(blockSize, 2, CFft::kWindowHann, CFft::kNoWindow);
	mBlockSize = mFft->getLength(CFft::kLengthData);
	mFftSize = mFft->getLength(CFft::kLengthFft);

	// Init delay line
	mDelayLine.reset(new CRingBuffer<float>(mFftSize));
	mDelayLine->setWriteIdx(mBlockSize);

	// TODO: precompute ir fft 
	return Error_t::kNoError;
}

Error_t Convolver::reset()
{
	if (mIsInitialized) {
		mFft->resetInstance();
		mIr.reset();
		mDelayLine.reset();
		mBlockSize = 0;
		mFftSize = 0;
		mIsInitialized = false;
	}
	return Error_t::kNoError;
}

Error_t Convolver::process(const float* inputBuffer, float* outputBuffer, int numSamples)
{
	return Error_t();
}
