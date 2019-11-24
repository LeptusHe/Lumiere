#include "LumiereFileWatcher.h"
#include "Common/LumierePlatform.h"
#include "Common/LumiereAssert.h"
#include "Exception/LumiereException.h"
#include "Logging/LumiereLogManager.h"

BEGIN_LUMIERE_NAMESPACE

FileWatcher::FileWatcher(const std::string& filePath, std::unique_ptr<FileSystem>&& fileSystem)
    : mFilePath(filePath)
    , mIsWatching(false)
    , mLastWriteTime()
    , mListenerList()
    , mFileSystem(std::move(fileSystem))
{
    LUMIERE_ENSURE(!mFilePath.empty());
    LUMIERE_ENSURE(mFilePath == filePath);
    LUMIERE_ENSURE(!mIsWatching);
    LUMIERE_ENSURE(mLastWriteTime.time_since_epoch().count() == 0);
    LUMIERE_ENSURE(mListenerList.empty());
    LUMIERE_ENSURE(mFileSystem);
    LUMIERE_ENSURE(!fileSystem);

    mFilePath = mFileSystem->weaklyCanonical(filePath);
    if (!mFileSystem->fileExist(mFilePath)) {
        LUMIERE_THROW_EXCEPTION_FMT(ExceptionCode::FileNotFound, "fail to find file [{}]", mFilePath);
    }
}


void FileWatcher::addListener(const FileWatcher::Listener& listener)
{
    mListenerList.push_back(listener);
}


const std::string& FileWatcher::getFilePath() const
{
    return mFilePath;
}


const FileWatcher::ListenerList& FileWatcher::getListenerList() const
{
    return mListenerList;
}


bool FileWatcher::isWatching() const
{
    return mIsWatching;
}


void FileWatcher::startToWatch()
{
    LUMIERE_EXPECT(!isWatching());
    mIsWatching = true;
    mLastWriteTime = mFileSystem->getLastWriteTime(mFilePath);
    LUMIERE_ENSURE(isWatching());
}


void FileWatcher::stopWatching()
{
    LUMIERE_EXPECT(isWatching());
    mIsWatching = false;
    LUMIERE_ENSURE(!isWatching());
}



bool FileWatcher::isModified()
{
    if (!isWatching()) {
        return false;
    }

    FileTime lastWriteTime = mFileSystem->getLastWriteTime(mFilePath);
    bool isModified = lastWriteTime.time_since_epoch().count() > mLastWriteTime.time_since_epoch().count();
    if (isModified) {
        mLastWriteTime = lastWriteTime;
#ifdef LUMIERE_OS_WINDOWS
        LUMIERE_INFO_FMT("file [{}] is modified", getFilePath());
#else
        constexpr std::chrono::seconds EpochDiff{6437664000};
        std::chrono::system_clock::time_point lastWriteTimePoint(mLastWriteTime.time_since_epoch() + EpochDiff);
        std::time_t time = std::chrono::system_clock::to_time_t(lastWriteTimePoint);
        LUMIERE_INFO_FMT("file [{}] is modified in time [{}]", getFilePath(), std::ctime(&time));
#endif
    }
    return isModified;
}

END_LUMIERE_NAMESPACE
