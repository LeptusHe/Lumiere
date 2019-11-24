#include "LumiereFileStream.h"
#include <fstream>
#include <sstream>
#include "Common/LumiereAssert.h"
#include "Common/LumiereMemory.h"
#include "Logging/LumiereLogManager.h"

BEGIN_LUMIERE_NAMESPACE

FileStream::FileStream(const std::string& fileName, FileAccessMode accessMode)
    : DataStream(fileName)
    , mFileStream(nullptr)
    , mAccessMode(accessMode)
{
    std::ios::openmode openMode = std::ios::binary;
    switch (accessMode) {
        case FileAccessMode::READ: openMode |= std::ios::in; break;
        case FileAccessMode::WRITE: openMode |= std::ios::out; break;
    }
    mFileStream = LUMIERE_NEW std::fstream(fileName, openMode);

    if (accessMode == FileAccessMode::READ) {
        mFileStream->seekg(0, std::ios_base::end);
        setSize(static_cast<size_t>(mFileStream->tellg()));
        mFileStream->seekg(0, std::ios_base::beg);
    }
    LUMIERE_ENSURE(mFileStream);
    LUMIERE_ENSURE(mAccessMode == accessMode);
}


FileStream::~FileStream()
{
    LUMIERE_EXPECT(mFileStream);
    close();
    LUMIERE_ENSURE(!mFileStream);
}


size_t FileStream::read(void *buffer, size_t byteSize)
{
    LUMIERE_EXPECT(buffer);
    if (!isReadable()) {
        LUMIERE_DEBUG_FMT("fail to read file stream [{}] which is not readable", getName());
        return 0;
    }

    mFileStream->read(static_cast<char*>(buffer), static_cast<std::streamsize>(byteSize));
    return static_cast<size_t>(mFileStream->gcount());
}


size_t FileStream::write(const void *buffer, size_t byteSize)
{
    LUMIERE_EXPECT(buffer);
    if (!isWriteable()) {
        LUMIERE_DEBUG_FMT("fail to write file stream [{}] which is not writeable", getName());
        return 0;
    }
    mFileStream->write(static_cast<const char*>(buffer), static_cast<std::streamsize>(byteSize));
    return byteSize;
}


std::string FileStream::getLine()
{
    if (!isReadable()) {
        LUMIERE_DEBUG_FMT("fail to read line from file stream [{}] which is not readable", getName());
        return {};
    }

    std::string line;
    std::getline(*mFileStream, line);
    return line;
}


bool FileStream::getLine(std::string& line)
{
    if (!isReadable()) {
        LUMIERE_DEBUG_FMT("fail to read line from file stream [{}] which is not readable", getName());
        return {};
    }
    return static_cast<bool>(std::getline(*mFileStream, line));
}


std::string FileStream::getAsString()
{
    if (!isReadable()) {
        LUMIERE_DEBUG_FMT("fail to read line from file stream [{}] which is not readable", getName());
        return {};
    }

    std::stringstream characterStream;
    characterStream << mFileStream->rdbuf();
    seek(getSize());
    return characterStream.str();
}


std::vector<char> FileStream::getAsByteArray()
{
    if (!isReadable()) {
        LUMIERE_DEBUG_FMT("fail to read line from file stream [{}] which is not readable", getName());
        return {};
    }
    auto size = getSize();
    std::vector<char> byteArray(size);
    mFileStream->read(byteArray.data(), size);
    return byteArray;
}


void FileStream::skip(uint64_t byteSize)
{
    LUMIERE_EXPECT(isReadable());
    if (!isReadable()) {
        LUMIERE_DEBUG_FMT("fail to call skip function in file stream [{}] because it is not readable", getName());
        return;
    }

    mFileStream->clear();
    mFileStream->seekg(static_cast<std::ifstream::pos_type>(byteSize), std::ios_base::cur);
}


void FileStream::seek(size_t pos)
{
    LUMIERE_EXPECT(isReadable());
    if (!isReadable()) {
        LUMIERE_DEBUG_FMT("fail to call seek function in file stream [{}] because it is not readable", getName());
        return;
    }

    mFileStream->clear();
    mFileStream->seekg(static_cast<std::streamoff>(pos), std::ios_base::beg);
}


size_t FileStream::tell() const
{
    LUMIERE_EXPECT(isReadable());
    if (!isReadable()) {
        LUMIERE_DEBUG_FMT("fail to call tell function in file stream [{}] because it is not readable", getName());
        return 0;
    }

    mFileStream->clear();
    return  static_cast<size_t>(mFileStream->tellg());
}


bool FileStream::eof() const
{
    LUMIERE_EXPECT(isReadable());
    if (!isReadable()) {
        LUMIERE_DEBUG_FMT("fail to call eof function in file stream [{}] because it is not readable", getName());
        return false;
    }
    return mFileStream->eof();
}


void FileStream::close()
{
    if (mFileStream) {
        mFileStream->flush();
        mFileStream->close();
        LUMIERE_DELETE mFileStream;
        mFileStream = nullptr;
    }
    LUMIERE_ENSURE(!mFileStream);
}


bool FileStream::isReadable() const
{
    return mAccessMode == FileAccessMode::READ;
}


bool FileStream::isWriteable() const
{
    return mAccessMode == FileAccessMode::WRITE;
}

END_LUMIERE_NAMESPACE
