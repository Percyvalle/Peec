#pragma once

#include <PeecCommonHeaders.hpp>

#include "PeecFileInfo.hpp" 

struct ContainerTackerServer {
	std::unordered_map<std::string, FileInfo> files;
	std::forward_list<std::string> filesList;
	std::mutex contanerMtx;

	void AddFile(const FileInfo& _file,const PeerInfo& _location);

	bool FileExistsOnServer(const std::string& _filename);
	bool LocationIsExist(const std::string& _filename, const PeerInfo& _location);

	JSON GetFileLocationJSON(const std::string& _filename);
	JSON GetFilesJSON();

private:
	void AddFileLocation(const std::string& _filename, const PeerInfo& _location)
	{
		files[_filename].fileLocation.push_back(_location);
	}

	const FileLocationVector& GetFileLocation(const std::string& _filename)
	{
		return files[_filename].fileLocation;
	}
};