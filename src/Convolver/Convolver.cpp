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
	mProcessReal.reset(new float[mFftSize / 2 + 1]{});
	mProcessRealCopy.reset(new float[mFftSize / 2 + 1]{});
	mProcessImag.reset(new float[mFftSize / 2 + 1]{});

	// Precompute Ir fft
	mNumIrBlocks = static_cast<int>(ceil(static_cast<float>(lengthOfIr) / mBlockSize));
	for (int block = 0; block < mNumIrBlocks; block++) {
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
	mLengthOfTail = mNumIrBlocks * mBlockSize + mBlockSize;
	mTail.reset(new float[mLengthOfTail] {});
	mIsInitialized = true;
	return Error_t::kNoError;
}

Error_t Convolver::reset()
{
	if (mIsInitialized) {
		mFft->resetInstance();
		mBlockSize = 0;
		mFftSize = 0;
		mNumIrBlocks = 0;
		mLengthOfTail = 0;
		mProcessBuffer.reset();
		mProcessReal.reset();
		mProcessRealCopy.reset();
		mProcessImag.reset();
		mIrReal.clear();
		mIrImag.clear();
		mTail.reset();
		mIsInitialized = false;
	}
	return Error_t::kNoError;
}

int Convolver::getTailLength() const
{
	return mLengthOfTail;
}

Error_t Convolver::flushBuffer(float* outputBuffer) const
{
	if (!mIsInitialized)
		return Error_t::kNotInitializedError;
	if (!outputBuffer)
		return Error_t::kMemError;

	CVectorFloat::copy(outputBuffer, mTail.get(), mLengthOfTail);
	return Error_t::kNoError;
}

Error_t Convolver::process(const float* inputBuffer, float* outputBuffer, int numSamples)
{
	if (!mIsInitialized)
		return Error_t::kNotInitializedError;
	if (!inputBuffer || !outputBuffer)
		return Error_t::kMemError;
	if (numSamples < 0)
		return Error_t::kFunctionInvalidArgsError;

	// Copy over leftover values
	int copyLength = std::min<int>(numSamples, mLengthOfTail);
	int remainder = mLengthOfTail - copyLength;
	CVectorFloat::copy(outputBuffer, mTail.get(), copyLength);
	if (remainder > 0)
		CVectorFloat::moveInMem(mTail.get(), 0, copyLength, remainder);

	// Main process block for fft
	int numInputBlocks = static_cast<int>(ceil(static_cast<float>(numSamples) / mBlockSize));
	for (int inputBlock = 0; inputBlock < numInputBlocks; inputBlock++) {
		int inputStartIndex = inputBlock * mBlockSize;
		int inputEndIndex = std::min<int>(inputStartIndex + mBlockSize, numSamples);
		CVectorFloat::setZero(mProcessBuffer.get(), mFftSize);
		CVectorFloat::copy(mProcessBuffer.get(), inputBuffer + inputStartIndex, inputEndIndex - inputStartIndex);
		mFft->doFft(mProcessBuffer.get(), mProcessBuffer.get());
		CVectorFloat::mulC_I(mProcessBuffer.get(), mFftSize, mFftSize);
		mFft->splitRealImag(mProcessReal.get(), mProcessImag.get(), mProcessBuffer.get());
		CVectorFloat::copy(mProcessRealCopy.get(), mProcessReal.get(), mFftSize / 2 + 1);
		for (int irBlock = 0; irBlock < mNumIrBlocks; irBlock++) {
			for (int m = 0; m < mFftSize / 2 + 1; m++){
				mProcessReal.get()[m] = (mProcessRealCopy.get()[m] * mIrReal.at(irBlock).get()[m] - mProcessImag.get()[m] * mIrImag.at(irBlock).get()[m]);
				mProcessImag.get()[m] = (mProcessRealCopy.get()[m] * mIrImag.at(irBlock).get()[m] + mProcessImag.get()[m] * mIrReal.at(irBlock).get()[m]);
			}
			mFft->mergeRealImag(mProcessBuffer.get(), mProcessReal.get(), mProcessImag.get());
			mFft->doInvFft(mProcessBuffer.get(), mProcessBuffer.get());
			CVectorFloat::mulC_I(mProcessBuffer.get(), 1.0f / mFftSize, mFftSize);

			// Place values into appropriate buffers
			int irStartIndex = inputStartIndex + irBlock * mBlockSize;
			int irEndIndex = irStartIndex + mFftSize;
			int amountToOutput = std::min<int>(irEndIndex, numSamples) - irStartIndex;
			int amountToTail = irEndIndex - numSamples;
			CVectorFloat::add_I(outputBuffer + irStartIndex, mProcessBuffer.get(), amountToOutput);
			if (amountToTail > 0) {
				CVectorFloat::add_I(mTail.get(), mProcessBuffer.get() + amountToOutput, amountToTail);
			}
		}
	}
	return Error_t::kNoError;
}
