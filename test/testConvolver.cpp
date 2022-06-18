#pragma once

#include "catch.hpp"
#include "Convolver.h"
#include "ErrorDef.h"
#include "CatchUtil.h"

TEST_CASE("Trial") {

	std::unique_ptr<float> inputBuffer;
	std::unique_ptr<float> ir;
	std::unique_ptr<float> outputBuffer;
	std::unique_ptr<float> groundBuffer;
	Convolver conv;
	SECTION("Delay") {
		const int numSamples = 16;
		const int blockSize = 4;
		const int lengthOfIr = 4;
		const int delay = 4;

		inputBuffer.reset(new float[numSamples] {});
		outputBuffer.reset(new float[numSamples] {});
		groundBuffer.reset(new float[numSamples] {});
		ir.reset(new float[lengthOfIr] {});

		ir.get()[delay] = 1;

		CVectorFloat::setValue(inputBuffer.get(), 1, numSamples);

		conv.init(ir.get(), lengthOfIr, blockSize);
		conv.process(inputBuffer.get(), outputBuffer.get(), numSamples);
		CatchUtil::compare(inputBuffer.get(), outputBuffer.get() + delay, numSamples - delay);
	}
}