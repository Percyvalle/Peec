#pragma once

#include <stdint.h>

enum class MessageTypes : uint32_t 
{
	ServerPing		 = 201,
	FileRegistration = 202,
	FileLocation	 = 203,
	GetFileList		 = 204,
	DownloadFile	 = 205
};