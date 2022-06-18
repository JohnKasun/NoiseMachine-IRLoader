#include "CatchUtil.h"

Error_t CatchUtil::compare(float* buff1, float* buff2, int numSamples, float tolerance)
{
	if (!buff1 || !buff2 || numSamples < 0)
		return Error_t::kFunctionInvalidArgsError;

	for (int i = 0; i < numSamples; i++) {
		REQUIRE(abs(buff1[i] - buff2[i]) <= tolerance);
	}
	return Error_t::kNoError;
}

Error_t CatchUtil::compare(float** buff1, float** buff2, int numChannels, int numSamples, float tolerance)
{
	if (numChannels < 0)
		return Error_t::kFunctionInvalidArgsError;

	for (int c = 0; c < numChannels; c++) {
		compare(buff1[c], buff2[c], numSamples, tolerance);
	}
	return Error_t::kNoError;
}

Error_t CatchUtil::compare(float* buff1, std::vector<float>& buff2, int numSamples, float tolerance)
{
	if (!buff1 || numSamples < 0)
		return Error_t::kFunctionInvalidArgsError;

	for (int i = 0; i < numSamples; i++) {
		REQUIRE(abs(buff1[i] - buff2.at(i)) <= tolerance);
	}
	return Error_t::kNoError;
}

