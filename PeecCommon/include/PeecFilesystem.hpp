#pragma once

#include "PeecCommonHeaders.hpp"

#include <fstream>

#include <WinBase.h>
#include <shlwapi.h>

namespace FileS
{
	const char kPathSeparator = '/';

	static bool IsPathSeparator(const char& c) { return c == kPathSeparator; }

	static void ConvertUTF8toUNOCODE(wchar_t* _target, const char* _source, const DWORD& _len)
	{
		MultiByteToWideChar(CP_ACP, 0, _source, -1, _target, _len);
	}

	static void ConvertUNICODEtoUTF8(char* _target, const wchar_t* _source, const DWORD& _len)
	{
		WideCharToMultiByte(CP_UTF8, 0, _source, -1, _target, _len, NULL, NULL);
	}

	/// <summary>
	/// / 
	/// </summary>
	class PathStruct
	{
	private:
		std::string path;
		std::wstring pathW;
		std::size_t fileLength = 0;

	public:
		explicit PathStruct(const std::string& _path);
		~PathStruct() = default;

		const std::string& GetPath() const; 
		void SetPath(const char* _path);

		const std::wstring& GetPathW() const;

		const std::size_t& GetFileLenght() const;

		std::string GetPathFileName() const;
		
		bool IsAbsolutePath() const;
		bool IsDirectory() const;
		bool IsRootDirectory() const;
	private:
		void PathProcessing();
		void GettingFileLength();
		void Normalize();
	};

	class FileIO
	{
	private:
		PathStruct filePath;
		std::fstream fileStream; 
	
	public:
		FileIO() = default;
		explicit FileIO(const PathStruct& _filePath, std::ios_base::openmode _mode) 
			: filePath(_filePath), fileStream(_filePath.GetPath(), _mode) {}
		
		~FileIO() { Close(); }

		bool Open(const PathStruct& _filePath, std::ios_base::openmode _mode);
		bool Close();
		bool IsOpen() const;

		std::streampos Size();
		std::istream& Read(char* _str, std::streamsize _size);
		std::ostream& Write(const char* _str, std::streamsize _size);
	};

	/// <summary>
	/// 
	/// </summary>
	class FileSystemManager
	{
	public:
		explicit FileSystemManager() = default;
		~FileSystemManager() = default;

		PathStruct GetWorkingDir() const;
		bool SetWorkingDir(const PathStruct& _path) const;

		static bool DirectoryExists(const PathStruct& _path);
		static bool FileExists(const PathStruct& _path);
	};
}