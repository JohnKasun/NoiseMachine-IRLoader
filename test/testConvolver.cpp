#pragma once

#include <stdlib.h>
#include <time.h>

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
	SECTION("Error Checking") {
		REQUIRE(conv.init(ir.get(), 1) == Error_t::kMemError);
		ir.reset(new float[10]{});
		REQUIRE(conv.init(ir.get(), -1) == Error_t::kFunctionInvalidArgsError);
		REQUIRE(conv.init(ir.get(), 10, 3) == Error_t::kFunctionInvalidArgsError);
		REQUIRE(conv.init(ir.get(), 10, 0) == Error_t::kFunctionInvalidArgsError);
		REQUIRE(conv.init(ir.get(), 10, -1) == Error_t::kFunctionInvalidArgsError);
		REQUIRE(conv.process(inputBuffer.get(), outputBuffer.get(), 10) == Error_t::kNotInitializedError);
		REQUIRE(conv.init(ir.get(), 10) == Error_t::kNoError);
		REQUIRE(conv.process(inputBuffer.get(), outputBuffer.get(), 10) == Error_t::kMemError);
		inputBuffer.reset(new float[10]{});
		REQUIRE(conv.process(inputBuffer.get(), outputBuffer.get(), 10) == Error_t::kMemError);
		outputBuffer.reset(new float[10]{});
		REQUIRE(conv.process(inputBuffer.get(), outputBuffer.get(), 10) == Error_t::kNoError);
		REQUIRE(conv.process(inputBuffer.get(), outputBuffer.get(), -10) == Error_t::kFunctionInvalidArgsError);
		REQUIRE(conv.flushBuffer(tailBuffer.get()) == Error_t::kMemError);
		tailBuffer.reset(new float[conv.getTailLength()]{});
		REQUIRE(conv.flushBuffer(tailBuffer.get()) == Error_t::kNoError);
		conv.reset();
		REQUIRE(conv.flushBuffer(tailBuffer.get()) == Error_t::kNotInitializedError);
	}
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
	SECTION("Varying Input Buffer Sizes") {
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
		std::vector<int> bufferSizes{ 1, 45, 123 ,560, 6, 265 };
		int offset = 0;
		for (int& bufferSize : bufferSizes) {
			conv.process(&inputBuffer.get()[offset], &outputBuffer.get()[offset], bufferSize);
			offset += bufferSize;
		}
		CatchUtil::compare(outputBuffer.get(), groundBuffer.get(), numSamples);
	}
	SECTION("Various Block Sizes") {
		srand(time(NULL));
		const int numSamples = 10;
		const int lengthOfIr = 5;
		const int delay = 2;

		inputBuffer.reset(new float[numSamples] {});
		outputBuffer.reset(new float[numSamples] {});
		groundBuffer.reset(new float[numSamples] {});
		ir.reset(new float[lengthOfIr] {});

		ir.get()[delay] = 1;

		for (int i = 0; i < numSamples; i++) {
			inputBuffer.get()[i] = rand() % 10 + 1;
		}

		std::vector<int> blockSizes{ 2, 8, 16, 256, 512, 1024, 2048, 8192 };
		for (int& blockSize : blockSizes) {
			conv.init(ir.get(), lengthOfIr, blockSize);
			conv.process(inputBuffer.get(), outputBuffer.get(), numSamples);

			tailBuffer.reset(new float[conv.getTailLength()]{});
			conv.flushBuffer(tailBuffer.get());
			CatchUtil::compare(inputBuffer.get(), outputBuffer.get() + delay, numSamples - delay);
			CatchUtil::compare(inputBuffer.get() + numSamples - delay, tailBuffer.get(), delay);
			conv.reset();
		}

	}
}