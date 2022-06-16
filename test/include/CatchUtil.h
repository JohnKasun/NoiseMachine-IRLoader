#pragma once

#include <vector>

#include "catch.hpp"
#include "ErrorDef.h"

namespace CatchUtil {
	Error_t compare(float* buff1, float* buff2, int numSamples, float tolerance = 1E-3);
	Error_t compare(float** buff1, float** buff2, int numChannels, int numSamples, float tolerance = 1E-3);
	Error_t compare(float* buff1, std::vector<float>& buff2, int numSamples, float tolerance = 1E-3);
}