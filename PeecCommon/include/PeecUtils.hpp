#pragma once

#include <Windows.h>

#include "PeecCommonHeaders.hpp"
#include "PeecMessageStatus.hpp"

#define JsonMSG(key, text) JSON::parse("{\"" + std::string(key) + "\":\"" + std::string(text) + "\"}")
#define JsonMSGDump(key, text) JSON::parse("{\"" + std::string(key) + "\":\"" + std::string(text) + "\"}").dump()

namespace Utils 
{
	DWORD GetCountCPU();

	std::string ConvertStatusToStr(MessageStatus _status);

	bool ValidateExistsVarJSON(JSON _json, ...);
}