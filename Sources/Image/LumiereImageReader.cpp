#include "LumiereImageReader.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>
#include "Common/LumiereAssert.h"
#include "Common/LumiereMemory.h"
#include "Exception/LumiereException.h"

BEGIN_LUMIERE_NAMESPACE

ImageReader::ImageReader(std::unique_ptr<FileSystem>&& fileSystem)
    : mFileSystem(std::move(fileSystem))
{
    LUMIERE_ENSURE(mFileSystem);
    LUMIERE_ENSURE(!fileSystem);
}


Image ImageReader::read(const std::string& filePath, ImageFormat format)
{
    LUMIERE_EXPECT(!filePath.empty());
    const std::string canonicalFilePath = mFileSystem->weaklyCanonical(filePath);
    if (!mFileSystem->fileExist(canonicalFilePath)) {
        LUMIERE_THROW_EXCEPTION_FMT(ExceptionCode::FileNotFound,
                "unable to find file [{}] in filePath [{}]", filePath, canonicalFilePath);
    }

    if (format._value == ImageFormat::RGB8) {
        return loadRGB8ImageFromFile(canonicalFilePath);
    } else if (format._value == ImageFormat::RGBA8) {
        return loadRGBA8ImageFromFile(canonicalFilePath);
    } else if (format._value == ImageFormat::RGBF) {
        return loadRGBFImageFromFile(canonicalFilePath);
    } else if (format._value == ImageFormat::RGBAF) {
        return loadRGBAFImageFromFile(canonicalFilePath);
    } else {
        LUMIERE_ASSERT(false && "undefined image format");
        return Image(ImageFormat::RGBA8, 0, 0, nullptr);
    }
}


Image ImageReader::loadRGBA8ImageFromFile(const std::string& path)
{
    int channels = 0, width = 0, height = 0;
    uint8_t *data = stbi_load(path.c_str(), &width, &height, &channels, 4);
    checkImageState(path, ImageFormat::RGBA8, data, channels, 4);
    return Image(path, ImageFormat::RGBA8, width, height, data);
}


Image ImageReader::loadRGB8ImageFromFile(const std::string& path)
{
    int channels = 0, width = 0, height = 0;
    uint8_t *data = stbi_load(path.c_str(), &width, &height, &channels, 3);
    checkImageState(path, ImageFormat::RGB8, data, channels, 3);
    return Image(path, ImageFormat::RGB8, width, height, data);
}


Image ImageReader::loadRGBAFImageFromFile(const std::string& path)
{
    int channels = 0, width = 0, height = 0;
    float *source = stbi_loadf(path.c_str(), &width, &height, &channels, 4);
    auto *data = reinterpret_cast<uint8_t *>(source);
    checkImageState(path, ImageFormat::RGBAF, data, channels, 4);
    return Image(path, ImageFormat::RGBAF, width, height, data);
}


Image ImageReader::loadRGBFImageFromFile(const std::string& path) noexcept(false)
{
    int channels = 0, width = 0, height = 0;
    float *source = stbi_loadf(path.c_str(), &width, &height, &channels, 3);
    auto *data = reinterpret_cast<uint8_t*>(source);
    checkImageState(path, ImageFormat::RGBF, data, channels, 3);
    return Image(path, ImageFormat::RGBF, width, height, data);
}


void ImageReader::checkImageState(const std::string& name, ImageFormat format, uint8_t *data, int actualChannels, int requiredChannels) const
{
    LUMIERE_EXPECT(!name.empty());
    LUMIERE_EXPECT(actualChannels > 0 && requiredChannels > 0);
    if (!data) {
        LUMIERE_THROW_EXCEPTION_FMT(ExceptionCode::ImageLoadError,
                                   "fail to load image [name={}, format={}] because {}", name, format._to_string(), stbi_failure_reason());
    }

    if (actualChannels != requiredChannels) {
        LUMIERE_DELETE_ARRAY(data);
        LUMIERE_THROW_EXCEPTION_FMT(ExceptionCode::InvalidParams,
                                   "fail to load image [path={}, format={}] because image channels is invalid [actual-channels={}, required-channels={}]",
                                   name,
                                   format._to_string(),
                                   actualChannels,
                                   requiredChannels);
    }
}

END_LUMIERE_NAMESPACE
