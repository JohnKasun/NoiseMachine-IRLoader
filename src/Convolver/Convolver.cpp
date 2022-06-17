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

Error_t Convolver::init(const float const* ir, const int lengthOfIr, const int blockSize)
{
	if (!ir)
		return Error_t::kMemError;
	if (lengthOfIr < 0 || blockSize < 0)
		return Error_t::kFunctionInvalidArgsError;

	reset();

	// Init fft
	mFft->initInstance(blockSize, 2, CFft::kWindowHann, CFft::kNoWindow);
	mBlockSize = mFft->getLength(CFft::kLengthData);
	mFftSize = mFft->getLength(CFft::kLengthFft);
	mProcessBuffer.reset(new float[mFftSize]{});

	// Precompute Ir fft
	int numIrBlocks = static_cast<int>(ceil(static_cast<float>(lengthOfIr) / mBlockSize));
	for (int block = 0; block < numIrBlocks; block++) {
		int irStartIndex = block * mBlockSize;
		int irEndIndex = std::min<int>(irStartIndex + mBlockSize, lengthOfIr);
		CVectorFloat::setZero(mProcessBuffer.get(), mFftSize);
		CVectorFloat::copy(mProcessBuffer.get(), ir + irStartIndex, irEndIndex - irStartIndex);
		mFft->doFft(mProcessBuffer.get(), mProcessBuffer.get());
		CVectorFloat::mulC_I(mProcessBuffer.get(), mFftSize, mFftSize);

		mIrReal.emplace_back(new float[mFftSize / 2 + 1]{});
		mIrImag.emplace_back(new float[mFftSize / 2 + 1]{});
		mFft->splitRealImag(mIrReal.back().get(), mIrImag.back().get(), mProcessBuffer.get());
	}
	return Error_t::kNoError;
}

Error_t Convolver::reset()
{
	if (mIsInitialized) {
		mFft->resetInstance();
		mBlockSize = 0;
		mFftSize = 0;
		mProcessBuffer.reset();
		mIrReal.clear();
		mIrImag.clear();
		mIsInitialized = false;
	}
	return Error_t::kNoError;
}

Error_t Convolver::process(const float* inputBuffer, float* outputBuffer, int numSamples)
{
	return Error_t();
}
