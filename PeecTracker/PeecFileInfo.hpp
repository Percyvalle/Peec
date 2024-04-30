#pragma once

#include <PeecCommonHeaders.hpp>

constexpr std::uint16_t CHUNK_SIZE = 1024;

inline size_t CalculateChunkCount(const std::uint32_t& _fileLength)
{
	int chunkNumber = _fileLength / CHUNK_SIZE;
	if ((chunkNumber * CHUNK_SIZE) < _fileLength) {
		chunkNumber += 1;
	}
	return chunkNumber;
}

struct PeerInfo
{
	std::string address;
	std::uint16_t port;
};

using FileLocationVector = std::vector<PeerInfo>;

struct FileInfo {
	std::string fileName;
	std::size_t countChunks;
	std::uint32_t fileLength;
	FileLocationVector fileLocation;
};