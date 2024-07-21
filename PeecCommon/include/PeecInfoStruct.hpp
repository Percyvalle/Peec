#pragma once

#include <PeecCommonHeaders.hpp>

namespace Info {

	struct PeerInfo {
		std::string address;
		std::uint16_t port;
	};

	using FileLocationVector = std::vector<PeerInfo>;

	struct FileInfo {
		std::string fileName;
		std::uint32_t fileLength;
		FileLocationVector fileLocation;
	};

	inline void to_json(JSON& _json, const PeerInfo& _info) {
		_json = JSON{ {"ADDRESS", _info.address}, {"PORT", _info.port} };
	}

	inline void to_json(JSON& _json, const FileInfo& _info) {
		_json = JSON{ {"FILENAME", _info.fileName}, {"FILELENGTH", _info.fileLength}, {"LOCATION", JSON::array()}};

		for (const PeerInfo& i : _info.fileLocation) {
			JSON peerJson = i;

			_json["LOCATION"].push_back(peerJson);
		}
	}

	inline void from_json(const JSON& _json, FileInfo& _info) {
		_json.at("FILENAME").get_to(_info.fileName);
		_json.at("FILELENGTH").get_to(_info.fileLength);
	}

	inline void from_json(const JSON& _json, PeerInfo& _info) {
		_json.at("ADDRESS").get_to(_info.address);
		_json.at("PORT").get_to(_info.port);
	}
}