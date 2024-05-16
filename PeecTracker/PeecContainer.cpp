#include "PeecContainer.hpp"

void ContainerTackerServer::AddFile(const FileInfo& _file, const PeerInfo& _location)
{
	std::lock_guard<std::mutex> lock(contanerMtx);

	if (!FileExistsOnServer(_file.fileName))
	{
		files[_file.fileName] = _file;
		filesList.push_front(_file.fileName);
	}

	if (!LocationIsExist(_file.fileName, _location));
	{
		AddFileLocation(_file.fileName, _location);
	}
}

bool ContainerTackerServer::FileExistsOnServer(const std::string& _filename)
{
	return files.find(_filename) != files.end();
}

bool ContainerTackerServer::LocationIsExist(const std::string& _filename, const PeerInfo& _location)
{
	std::function eqPeer = [&_location](const PeerInfo& peer) { return peer.address == _location.address; };
	
	FileLocationVector& fileLocations = GetFileLocation(_filename);
	return std::find_if(fileLocations.begin(), fileLocations.end(), eqPeer) != fileLocations.end();
}

JSON ContainerTackerServer::GetFileLocationJSON(const std::string& _filename)
{
	std::lock_guard<std::mutex> lock(contanerMtx);

	JSON jsonLocation;
	for (const PeerInfo& i : files[_filename].fileLocation)
	{
		jsonLocation.push_back(JSON::object());
		jsonLocation.back()["ADDRESS"] = i.address;
		jsonLocation.back()["PORT"] = i.port;
	}

	return jsonLocation;
}

JSON ContainerTackerServer::GetFilesJSON()
{
	std::lock_guard<std::mutex> lock(contanerMtx);

	JSON jsonFilesList;
	for (const auto& i : filesList)
	{
		jsonFilesList.push_back(JSON::object());
		jsonFilesList.back()["FILENAME"] = i;
		jsonFilesList.back()["FILELENGTH"] = files[i].fileLength;
		jsonFilesList.back()["NUMREGISTR"] = files[i].fileLocation.size();
	}

	return jsonFilesList;
}
