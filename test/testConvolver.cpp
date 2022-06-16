#pragma once

#include "catch.hpp"
#include "Convolver.h"
#include "ErrorDef.h"

TEST_CASE("Trial") {
	Convolver conv;
	float arr[]{ 1, 1 ,1 , 1, 1, 1, 1, 1, 1 };
	conv.init(arr, 9, 4);
	conv.reset();
}