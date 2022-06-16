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
	CRingBuffer<float> irCopy(lengthOfIr);
	irCopy.put(ir, lengthOfIr);

	// Init fft
	mFft->initInstance(blockSize, 2, CFft::kWindowHann, CFft::kNoWindow);
	mBlockSize = mFft->getLength(CFft::kLengthData);
	mFftSize = mFft->getLength(CFft::kLengthFft);
	mProcessBuffer.reset(new float[mFftSize]{});
	mDelayLine.reset(new CRingBuffer<float>(mFftSize));
	mDelayLine->setWriteIdx(mBlockSize);

	int numIrBlocks = static_cast<int>(ceil(lengthOfIr / mBlockSize));
	for (int block = 0; block < numIrBlocks; block++) {

	}
	// TODO: precompute ir fft 
	return Error_t::kNoError;
}

Error_t Convolver::reset()
{
	if (mIsInitialized) {
		mFft->resetInstance();
		mDelayLine.reset();
		mBlockSize = 0;
		mFftSize = 0;
		mProcessBuffer.reset();
		mIsInitialized = false;
	}
	return Error_t::kNoError;
}

Error_t Convolver::process(const float* inputBuffer, float* outputBuffer, int numSamples)
{
	return Error_t();
}
