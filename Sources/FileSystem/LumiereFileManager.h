#pragma once
#include <memory>
#include <vector>
#include <string>
#include "Common/LumiereSingleton.h"
#include "FileSystem/LumiereFileSystem.h"
#include "Streaming/LumiereFileStream.h"

BEGIN_LUMIERE_NAMESPACE

class FileManager {
public:
    using SearchPathList = std::vector<std::string>;

public:
    explicit FileManager(std::unique_ptr<FileSystem>&& fileSystem = std::make_unique<FileSystem>());
    virtual ~FileManager() = default;

    void addSearchPath(const std::string& path) noexcept(false);
    void removeSearchPath(const std::string& path) noexcept(false);
    virtual std::pair<bool, std::string> findFile(const std::string& fileName) const;
    virtual std::pair<bool, std::string> findDirectory(const std::string& path) const;
    virtual std::unique_ptr<DataStream> openFile(const std::string& fileName, FileAccessMode accessMode) const;
    virtual FileSystem* getFileSystem() const;

private:
    SearchPathList mSearchPathList;
    std::unique_ptr<FileSystem> mFileSystem;
};

END_LUMIERE_NAMESPACE
