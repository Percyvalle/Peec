#include "PeecUtils.hpp"

DWORD Utils::GetCountCPU()
{
	SYSTEM_INFO sysinfo;
	GetSystemInfo(&sysinfo);
	return sysinfo.dwNumberOfProcessors;
}

std::string Utils::ConvertStatusToStr(MessageStatus _status)
{
	return _status == MessageStatus::SUCCESS ? "SUCCESS" : "FAILURE";
}

bool Utils::ValidateExistsVarJSON(JSON _json, ...)
{
	va_list args;
	va_start(args, _json);

	const char* variable;
	while ((variable = va_arg(args, const char*)) != nullptr) {
		if (!_json.contains(variable)) {
			va_end(args);
			return false;
		}
	}

	va_end(args);

	return true;
}
