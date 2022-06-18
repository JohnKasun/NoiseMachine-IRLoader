#pragma once

#include "catch.hpp"
#include "RingBuffer.h"
#include "Vector.h"
#include "CatchUtil.h"

TEST_CASE("add") {
	const static int bufferSize = 10;
	CRingBuffer<float> ringBuffer(bufferSize);
	float input[bufferSize]{};
	float output[bufferSize]{};
	float ground[bufferSize]{};
	SECTION("Value") {
		const int offset = 3;
		CVectorFloat::setValue(input, 1.0f, bufferSize);
		ringBuffer.put(input, bufferSize);
		ringBuffer.setWriteIdx(offset);
		ringBuffer.add(1.0f);

		CVectorFloat::setValue(ground, 1.0f, bufferSize);
		ground[offset] = 2.0f;

		ringBuffer.get(output, bufferSize);

		CatchUtil::compare(output, ground, bufferSize, 0);
	}
	SECTION("BufferNoOffset") {
		CVectorFloat::setValue(input, 1.0f, bufferSize);
		ringBuffer.put(input, bufferSize);
		CVectorFloat::setValue(input, -1.0f, bufferSize);
		ringBuffer.add(input, bufferSize);
		ringBuffer.get(output, bufferSize);
		CVectorFloat::setZero(ground, bufferSize);
		CatchUtil::compare(output, ground, bufferSize, 0);
	}
	SECTION("BufferOffset") {
		static const int offset = 3;
		static const int cancelBufferSize = bufferSize - offset;
		float cancelBuffer[cancelBufferSize]{};
		CVectorFloat::setValue(input, 1.0f, bufferSize);
		ringBuffer.put(input, bufferSize);
		ringBuffer.setWriteIdx(offset);
		CVectorFloat::setValue(cancelBuffer, -1.0f, cancelBufferSize);
		ringBuffer.add(cancelBuffer, cancelBufferSize);
		ringBuffer.get(output, bufferSize);
		CVectorFloat::setValue(ground, 1.0f, offset);
		CatchUtil::compare(output, ground, bufferSize, 0);
	}
}
