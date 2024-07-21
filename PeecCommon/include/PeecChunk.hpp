#pragma once

#include <stdint.h>

constexpr size_t CHUNK_SIZE = 1024;

inline size_t CalculateChunkCount(const size_t& _fileLength)
{
	size_t chunkNumber = _fileLength / CHUNK_SIZE;
	if ((chunkNumber * CHUNK_SIZE) < _fileLength) {
		chunkNumber += 1;
	}
	return chunkNumber;
}
