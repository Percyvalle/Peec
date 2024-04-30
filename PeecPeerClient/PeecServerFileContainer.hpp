#pragma once

#include <PeecCommonHeaders.hpp>

#include <unordered_map>

struct FileContainer
{
	std::string fileName;
	std::string filePath;
	std::size_t fileLength;

	explicit FileContainer(const std::string& _fileName, const std::string& _filePath, const std::size_t& _fileLength)
		: fileName(_fileName), filePath(_filePath), fileLength(_fileLength) {}

	bool operator==(const FileContainer& _other) const
	{
		return filePath == _other.filePath;
	}
};

// Meybe unused
//struct FileContainerHash
//{
//	std::size_t operator()(const FileContainer& _fc) const
//	{
//		return std::hash<std::string>{}(_fc.filePath);
//	}
//};

using FileContainerMap = std::unordered_map<std::string, FileContainer>;