#pragma once

#include "catch.hpp"
#include "Convolver.h"
#include "ErrorDef.h"
#include "CatchUtil.h"

TEST_CASE("Convolver") {

	std::unique_ptr<float> inputBuffer;
	std::unique_ptr<float> ir;
	std::unique_ptr<float> outputBuffer;
	std::unique_ptr<float> groundBuffer;
	std::unique_ptr<float> tailBuffer;
	Convolver conv;
	SECTION("Delay") {
		const int numSamples = 16;
		const int blockSize = 4;
		const int lengthOfIr = 4;
		const int delay = 2;

		inputBuffer.reset(new float[numSamples] {});
		outputBuffer.reset(new float[numSamples] {});
		groundBuffer.reset(new float[numSamples] {});
		ir.reset(new float[lengthOfIr] {});

		ir.get()[delay] = 1;

		for (int i = 0; i < numSamples; i++) {
			inputBuffer.get()[i] = i + 1;
		}

		conv.init(ir.get(), lengthOfIr, blockSize);
		conv.process(inputBuffer.get(), outputBuffer.get(), numSamples);

		tailBuffer.reset(new float[conv.getTailLength()]{});
		conv.flushBuffer(tailBuffer.get());
		CatchUtil::compare(inputBuffer.get(), outputBuffer.get() + delay, numSamples - delay);
		CatchUtil::compare(inputBuffer.get() + numSamples - delay, tailBuffer.get(), delay);
	}
	SECTION("Varying Blocksize") {
		const int numSamples = 1000;
		const int blockSize = 128;
		const int lengthOfIr = 10;
		const int delay = 2;

		inputBuffer.reset(new float[numSamples] {});
		outputBuffer.reset(new float[numSamples] {});
		groundBuffer.reset(new float[numSamples] {});
		ir.reset(new float[lengthOfIr] {});

		ir.get()[delay] = 1;
		
		for (int i = 0; i < numSamples; i++) {
			inputBuffer.get()[i] = i + 1;
		}
		CVectorFloat::copy(&groundBuffer.get()[delay], inputBuffer.get(), numSamples - delay);

		conv.init(ir.get(), lengthOfIr, blockSize);
		std::vector<int> blockSizes{ 1, 45, 123 ,560, 6, 265 };
		int offset = 0;
		for (int& block : blockSizes) {
			conv.process(&inputBuffer.get()[offset], &outputBuffer.get()[offset], block);
			offset += block;
		}
		CatchUtil::compare(outputBuffer.get(), groundBuffer.get(), numSamples);

	}
}