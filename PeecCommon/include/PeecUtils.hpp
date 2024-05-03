#pragma once

#include <Windows.h>

#include "PeecCommonHeaders.hpp"
#include "PeecMessageStatus.hpp"

namespace Utils 
{
	DWORD GetCountCPU();

	std::string ConvertStatusToStr(MessageStatus _status);
}