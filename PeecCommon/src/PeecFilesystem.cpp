#include "PeecFilesystem.hpp"

FileS::PathStruct::PathStruct(const std::string& _path) : path(_path)
{
    pathW.resize(path.size());
    PathProcessing();
}

void FileS::PathStruct::Normalize()
{
    auto outputIterator = path.begin();
    auto inputIterator = path.cbegin();

    if (path.end() - inputIterator >= 3 && IsPathSeparator(*inputIterator) &&
        IsPathSeparator(*(inputIterator + 1)) && !IsPathSeparator(*(inputIterator + 2))) {
        *(outputIterator++) = kPathSeparator;
        *(outputIterator++) = kPathSeparator;
    }

    while (inputIterator != path.end()) {
        const char character = *inputIterator;
        if (!IsPathSeparator(character)) {
            *(outputIterator++) = character;
        }
        else if (outputIterator == path.begin() || *std::prev(outputIterator) != kPathSeparator) {
            *(outputIterator++) = kPathSeparator;
        }
        ++inputIterator;
    }

    path.erase(outputIterator, path.end());
}

const std::string& FileS::PathStruct::GetPath() const
{
    return path;
}

void FileS::PathStruct::SetPath(const char* _path)
{
    path = _path;
    PathProcessing();
}

const std::wstring& FileS::PathStruct::GetPathW() const
{
    return pathW;
}

const std::size_t& FileS::PathStruct::GetFileLenght() const
{
    return fileLength;
}

std::string FileS::PathStruct::GetPathFileName() const
{
    if (!IsDirectory())
    {
        const std::wstring fileNameW = PathFindFileNameW(pathW.c_str());

        std::string fileNameA;
        fileNameA.resize(fileNameW.size());

        ConvertUNICODEtoUTF8(fileNameA.data(), fileNameW.c_str(), MAX_PATH);

        return fileNameA;
    }
    return "";
}

bool FileS::PathStruct::IsAbsolutePath() const
{
    return !PathIsRelativeW(pathW.c_str());
}

bool FileS::PathStruct::IsDirectory() const
{
    return PathIsDirectoryW(pathW.c_str());
}

bool FileS::PathStruct::IsRootDirectory() const
{
    return PathIsRootW(pathW.c_str());
}

void FileS::PathStruct::PathProcessing()
{
    ConvertUTF8toUNOCODE(pathW.data(), path.c_str(), path.size());

    Normalize();
    GettingFileLength();
}

void FileS::PathStruct::GettingFileLength()
{
    HANDLE hFile = CreateFileW(pathW.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (hFile == INVALID_HANDLE_VALUE)
    {
        return;
    }

    LARGE_INTEGER fileSize;
    if (!GetFileSizeEx(hFile, &fileSize))
    {
        return;
    }

    fileLength = fileSize.QuadPart;

    CloseHandle(hFile);
}

FileS::PathStruct FileS::FileManager::GetWorkingDir() const
{
    std::wstring pathW;
    pathW.resize(MAX_PATH);
    DWORD writeLen = GetCurrentDirectoryW(MAX_PATH, pathW.data());

    std::string pathA;
    pathA.resize(writeLen);
    ConvertUNICODEtoUTF8(pathA.data(), pathW.c_str(), writeLen);

    return FileS::PathStruct(pathA);
}

bool FileS::FileManager::SetWorkingDir(const PathStruct& _path) const
{
    return SetCurrentDirectoryW(_path.GetPathW().c_str());
}

bool FileS::FileManager::DirectoryExists(const PathStruct& _path)
{
    return PathIsDirectoryW(_path.GetPathW().c_str());
}
                                                        
bool FileS::FileManager::FileExists(const PathStruct& _path)
{
    return !_path.IsDirectory() && PathFileExistsW(_path.GetPathW().c_str());
}
