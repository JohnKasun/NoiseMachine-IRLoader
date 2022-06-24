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
	if (lengthOfIr < 0 || blockSize <= 0)
		return Error_t::kFunctionInvalidArgsError;

	reset();

	// Init fft
	Error_t err = mFft->initInstance(blockSize, 2, CFft::kWindowHann, CFft::kNoWindow);
	if (err != Error_t::kNoError)
		return err;

	// Allocate buffers
	mBlockSize = mFft->getLength(CFft::kLengthData);
	mFftSize = mFft->getLength(CFft::kLengthFft);
	mProcessBuffer.reset(new float[mFftSize]{});
	mProcessReal.reset(new float[mFftSize / 2 + 1]{});
	mProcessImag.reset(new float[mFftSize / 2 + 1]{});
	mProcessRealCopy.reset(new float[mFftSize / 2 + 1]{});
	mProcessImagCopy.reset(new float[mFftSize / 2 + 1]{});

	// Precompute Ir fft
	mNumIrBlocks = static_cast<int>(ceil(static_cast<float>(lengthOfIr) / mBlockSize));
	for (int block = 0; block < mNumIrBlocks; block++) {
		int irStartIndex = block * mBlockSize;
		int irEndIndex = std::min<int>(irStartIndex + mBlockSize, lengthOfIr);
		mIrReal.emplace_back(new float[mFftSize / 2 + 1]{});
		mIrImag.emplace_back(new float[mFftSize / 2 + 1]{});
		getSpectrum(ir + irStartIndex, irEndIndex - irStartIndex, mIrReal.back().get(), mIrImag.back().get());
	}

	// Allocate tail buffer
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
		mProcessImagCopy.reset();
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
	CVectorFloat::setZero(mTail.get(), copyLength);
	if (remainder > 0) {
		assert(remainder <= mLengthOfTail);
		CVectorFloat::moveInMem(mTail.get(), 0, copyLength, remainder);
	}

	// Main process block for fft
	int numInputBlocks = static_cast<int>(ceil(static_cast<float>(numSamples) / mBlockSize));
	for (int inputBlock = 0; inputBlock < numInputBlocks; inputBlock++) {
		int inputStartIndex = inputBlock * mBlockSize;
		int inputEndIndex = std::min<int>(inputStartIndex + mBlockSize, numSamples);
		getSpectrum(inputBuffer + inputStartIndex, inputEndIndex - inputStartIndex, mProcessReal.get(), mProcessImag.get());
		CVectorFloat::copy(mProcessRealCopy.get(), mProcessReal.get(), mFftSize / 2 + 1);
		CVectorFloat::copy(mProcessImagCopy.get(), mProcessImag.get(), mFftSize / 2 + 1);
		for (int irBlock = 0; irBlock < mNumIrBlocks; irBlock++) {
			for (int m = 0; m < mFftSize / 2 + 1; m++){
				mProcessReal.get()[m] = (mProcessRealCopy.get()[m] * mIrReal.at(irBlock).get()[m] - mProcessImagCopy.get()[m] * mIrImag.at(irBlock).get()[m]);
				mProcessImag.get()[m] = (mProcessRealCopy.get()[m] * mIrImag.at(irBlock).get()[m] + mProcessImagCopy.get()[m] * mIrReal.at(irBlock).get()[m]);
			}
			mFft->mergeRealImag(mProcessBuffer.get(), mProcessReal.get(), mProcessImag.get());
			mFft->doInvFft(mProcessBuffer.get(), mProcessBuffer.get());
			CVectorFloat::mulC_I(mProcessBuffer.get(), 1.0f / mFftSize, mFftSize);

			// Place values into appropriate buffers
			int irStartIndex = inputStartIndex + irBlock * mBlockSize;
			int irEndIndex = irStartIndex + mFftSize;
			if (irStartIndex < numSamples) {
				int amountToOutput = std::min<int>(irEndIndex, numSamples) - irStartIndex;
				assert(irStartIndex + amountToOutput <= numSamples);
				CVectorFloat::add_I(outputBuffer + irStartIndex, mProcessBuffer.get(), amountToOutput);
				int amountToTail = irEndIndex - numSamples;
				if (amountToTail > 0) {
					assert(amountToTail <= mLengthOfTail);
					assert(amountToOutput + amountToTail <= mFftSize);
					CVectorFloat::add_I(mTail.get(), mProcessBuffer.get() + amountToOutput, amountToTail);
				}
			}
			else {
				// places all values into tail
				assert(irStartIndex - numSamples + irEndIndex - irStartIndex <= mLengthOfTail);
				CVectorFloat::add_I(mTail.get() + irStartIndex - numSamples, mProcessBuffer.get(), irEndIndex - irStartIndex);
			}
		}
	}
	return Error_t::kNoError;
}

void Convolver::getSpectrum(const float const* inputBuffer, const int numSamples, float* realSpec, float* imagSpec)
{
	// Zero pad
	CVectorFloat::setZero(mProcessBuffer.get(), mFftSize);
	CVectorFloat::copy(mProcessBuffer.get(), inputBuffer, numSamples);

	
	mFft->doFft(mProcessBuffer.get(), mProcessBuffer.get());
	CVectorFloat::mulC_I(mProcessBuffer.get(), mFftSize, mFftSize);
	mFft->splitRealImag(realSpec, imagSpec, mProcessBuffer.get());
}
