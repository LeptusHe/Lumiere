#pragma once
#include <filesystem>
#include <functional>
#include "Common/LumiereMacro.h"
#include "LumiereFileSystem.h"

BEGIN_LUMIERE_NAMESPACE

class FileWatcher {
public:
    using Listener = std::function<void(const std::string&)>;
    using ListenerList = std::vector<Listener>;
    using FileTime = FileSystem::FileTime;

public:
    explicit FileWatcher(const std::string& filePath, std::unique_ptr<FileSystem>&& fileSystem = std::make_unique<FileSystem>()) noexcept(false);
    void addListener(const Listener& listener);
    const std::string& getFilePath() const;
    const ListenerList& getListenerList() const;
    bool isWatching() const;
    void startToWatch();
    void stopWatching();
    bool isModified();

private:
    std::string mFilePath;
    bool mIsWatching;
    FileTime mLastWriteTime;
    ListenerList mListenerList;
    std::unique_ptr<FileSystem> mFileSystem;
};

END_LUMIERE_NAMESPACE
