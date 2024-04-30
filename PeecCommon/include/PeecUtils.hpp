#pragma once

#include <Windows.h>

#include "PeecCommonHeaders.hpp"
#include "PeecMessageStatus.hpp"

namespace Utils 
{
	UINT GetCountCPU();

	std::string ConvertStatusToStr(MessageStatus _status);
}