#include "PeecUtils.hpp"

UINT Utils::GetCountCPU()
{
	SYSTEM_INFO sysinfo;
	GetSystemInfo(&sysinfo);
	return sysinfo.dwNumberOfProcessors;
}

std::string Utils::ConvertStatusToStr(MessageStatus _status)
{
	return _status == MessageStatus::SUCCESS ? "SUCCESS" : "FAILURE";
}