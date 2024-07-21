#pragma once

#include <PeecInfoStruct.hpp>

struct ContainerTackerServer {
	std::unordered_map<std::string, Info::FileInfo> files;
	std::forward_list<std::string> filesList;
	std::mutex contanerMtx;

	void AddFile(const Info::FileInfo & _file, const Info::PeerInfo& _location);

	bool FileExistsOnServer(const std::string& _filename);
	bool LocationIsExist(const std::string& _filename, const Info::PeerInfo& _location);

	JSON GetFileLocationJSON(const std::string& _filename);
	JSON GetFilesJSON();

private:
	void AddFileLocation(const std::string& _filename, const Info::PeerInfo& _location);
	const Info::FileLocationVector& GetFileLocation(const std::string& _filename);
};