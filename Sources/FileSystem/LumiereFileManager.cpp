#include "LumiereFileManager.h"
#include "Exception/LumiereException.h"
#include "Logging/LumiereLogManager.h"

BEGIN_LUMIERE_NAMESPACE

FileManager::FileManager(std::unique_ptr<FileSystem>&& fileSystem) : mFileSystem(std::move(fileSystem))
{
    LUMIERE_ENSURE(mFileSystem);
    LUMIERE_ENSURE(!fileSystem);
}


void FileManager::addSearchPath(const std::string& path)
{
    std::string absolutePath = mFileSystem->weaklyCanonical(path);
    try {
        if (!mFileSystem->directoryExist(absolutePath)) {
            LUMIERE_THROW_EXCEPTION_FMT(ExceptionCode::FileSystemError,"path [{}] is not a directory", absolutePath);
        }
    } catch (std::exception& e) {
        LUMIERE_ERROR_FMT("fail to add [{}] as a search path because [{}]", absolutePath, e.what());
        throw;
    }
    mSearchPathList.push_back(absolutePath);
}


void FileManager::removeSearchPath(const std::string& path)
{
    auto iter = std::find(std::begin(mSearchPathList), std::end(mSearchPathList), path);
    if (iter != std::end(mSearchPathList)) {
        mSearchPathList.erase(iter);
    }
}


std::pair<bool, std::string> FileManager::findFile(const std::string& fileName) const
{
    if (mFileSystem->fileExist(fileName)) {
        return { true, mFileSystem->canonical(fileName) };
    }

    for (const auto& searchPath : mSearchPathList) {
        auto [fileExist, filePath] = mFileSystem->findFileRecursivelyInDirectory(fileName, searchPath);
        if (fileExist) {
            return {true, filePath};
        }
    }
    return {false, ""};
}


std::pair<bool, std::string> FileManager::findDirectory(const std::string& path) const
{
    if (mFileSystem->directoryExist(path)) {
        return {true, mFileSystem->canonical(path) };
    }

    for (const auto& searchPath : mSearchPathList) {
        auto dir = mFileSystem->combine(searchPath, path);
        if (mFileSystem->directoryExist(dir)) {
            return {true, dir};
        }
    }
    return {false, ""};
}


std::unique_ptr<DataStream> FileManager::openFile(const std::string& fileName, FileAccessMode accessMode) const
{
    LUMIERE_EXPECT(!fileName.empty());
    if (accessMode == FileAccessMode::READ) {
        if (auto [exist, filePath] = findFile(fileName); exist) {
            return std::make_unique<FileStream>(filePath, accessMode);
        }
        return nullptr;
    } else {
        return std::make_unique<FileStream>(fileName, accessMode);
    }
}


FileSystem* FileManager::getFileSystem() const
{
    LUMIERE_EXPECT(mFileSystem);
    return mFileSystem.get();
}

END_LUMIERE_NAMESPACE
