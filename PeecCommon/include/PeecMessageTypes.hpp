#pragma once

#include <stdint.h>

enum class MessageTypes : uint32_t 
{
	ServerPing = 200,
	FileRegistration,
	FileLocation,
	GetFileList,
	DownloadFile
};