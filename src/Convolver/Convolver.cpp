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
	mTail.reset(new CRingBuffer<float>(mNumIrBlocks * mFftSize + 2 * mBlockSize));
	mTail->setWriteIdx(mBlockSize);
	return Error_t::kNoError;
}

Error_t Convolver::reset()
{
	if (mIsInitialized) {
		mFft->resetInstance();
		mBlockSize = 0;
		mFftSize = 0;
		mNumIrBlocks = 0;
		mProcessBuffer.reset();
		mProcessReal.reset();
		mProcessRealCopy.reset();
		mProcessImag.reset();
		mTail.reset();
		mIrReal.clear();
		mIrImag.clear();
		mIsInitialized = false;
	}
	return Error_t::kNoError;
}

Error_t Convolver::process(const float* inputBuffer, float* outputBuffer, int numSamples)
{
	mTail->getPostInc(outputBuffer, std::min<int>(numSamples, mBlockSize));
	int oldWriteIdx = mTail->getWriteIdx();
	int numInputBlocks = static_cast<int>(ceil(static_cast<float>(numSamples / mBlockSize)));
	for (int inputBlock = 0; inputBlock < numInputBlocks; inputBlock++) {
		int inputStartIndex = inputBlock * mBlockSize;
		int inputEndIndex = std::min<int>(inputStartIndex + mBlockSize, numSamples);
		CVectorFloat::setZero(mProcessBuffer.get(), mFftSize);
		CVectorFloat::copy(mProcessBuffer.get(), inputBuffer + inputStartIndex, inputEndIndex - inputStartIndex);
		mFft->doFft(mProcessBuffer.get(), mProcessBuffer.get());
		CVectorFloat::mulC_I(mProcessBuffer.get(), mFftSize, mFftSize);
		mFft->splitRealImag(mProcessReal.get(), mProcessImag.get(), mProcessBuffer.get());
		CVectorFloat::copy(mProcessRealCopy.get(), mProcessReal.get(), mFftSize / 2 + 1);
		mTail->setWriteIdx(oldWriteIdx + inputBlock * mBlockSize);
		for (int irBlock = 0; irBlock < mNumIrBlocks; irBlock++) {
			for (int m = 0; m < mFftSize / 2 + 1; m++){
				mProcessReal.get()[m] = (mProcessRealCopy.get()[m] * mIrReal.at(irBlock).get()[m] - mProcessImag.get()[m] * mIrImag.at(irBlock).get()[m]);
				mProcessImag.get()[m] = (mProcessRealCopy.get()[m] * mIrImag.at(irBlock).get()[m] + mProcessImag.get()[m] * mIrReal.at(irBlock).get()[m]);
			}
			mFft->mergeRealImag(mProcessBuffer.get(), mProcessReal.get(), mProcessImag.get());
			mFft->doInvFft(mProcessBuffer.get(), mProcessBuffer.get());
			CVectorFloat::mulC_I(mProcessBuffer.get(), 1.0f / mFftSize, mFftSize);
			mTail->add(mProcessBuffer.get(), mFftSize);
			mTail->setWriteIdx(mTail->getWriteIdx() + mBlockSize);
		}
	}
	return Error_t::kNoError;
}
