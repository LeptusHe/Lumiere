#include "LumiereImage.h"
#include "Common/LumiereAssert.h"
#include "Common/LumiereMemory.h"

BEGIN_LUMIERE_NAMESPACE

size_t Image::getSizeOfImageFormat(ImageFormat format)
{
    switch (format) {
        case ImageFormat::RGB8:  return 3 * sizeof(uint8_t);
        case ImageFormat::RGBA8: return 4 * sizeof(uint8_t);
        case ImageFormat::RGBF:  return 3 * sizeof(float);
        case ImageFormat::RGBAF: return 4 * sizeof(float);
        default: LUMIERE_ASSERT(false && "undefined image format");
    }
    return 0;
}


int Image::getChannelNumberOfImageFormat(ImageFormat format)
{
    switch (format) {
        case ImageFormat::RGB8: return 3;
        case ImageFormat::RGBA8: return 4;
        case ImageFormat::RGBF: return 3;
        case ImageFormat::RGBAF: return 4;
        default: LUMIERE_ASSERT(false && "undefined image format");
    }
    return 0;
}


Image::Image(const std::string& name, ImageFormat format, int width, int height, uint8_t *data)
    : mName(name)
    , mFormat(format)
    , mWidth(width)
    , mHeight(height)
    , mData(nullptr)
{
    LUMIERE_ENSURE(mWidth > 0 && mWidth == width);
    LUMIERE_ENSURE(mHeight > 0 && mHeight == height);
    LUMIERE_ENSURE(mFormat._value == format._value);

    LUMIERE_EXPECT(!mData);
    auto byteSize = getSizeOfImageFormat(mFormat) * mWidth * mHeight;
    mData = LUMIERE_NEW uint8_t[byteSize];
    std::memcpy(mData, data, byteSize);
    LUMIERE_ENSURE(mData);
}


Image::Image(ImageFormat format, int width, int height, uint8_t *data) : Image("", format, width, height, data)
{
    LUMIERE_ENSURE(mName.empty());
}


Image::Image(Image&& image) noexcept
    : mName(std::move(image.mName))
    , mWidth(image.mWidth)
    , mHeight(image.mHeight)
    , mFormat(image.mFormat)
    , mData(image.mData)
{
    LUMIERE_ENSURE(mHeight > 0 && mWidth > 0);
    LUMIERE_ENSURE(mFormat._value == image.mFormat._value);
    LUMIERE_ENSURE(mData && mData == image.mData);

    image.mWidth = image.mHeight = 0;
    image.mData = nullptr;
    image.mFormat = ImageFormat::RGBA8;
    LUMIERE_ENSURE(image.mWidth == 0 && image.mHeight == 0);
    LUMIERE_ENSURE(image.mFormat._value == ImageFormat::RGBA8);
    LUMIERE_ENSURE(!image.mData);
}


Image::~Image()
{
    LUMIERE_DELETE mData;
};


const std::string& Image::getName() const
{
    return mName;
}


uint32_t Image::getWidth() const
{
    return static_cast<uint32_t>(mWidth);
}


uint32_t Image::getHeight() const
{
    return static_cast<uint32_t>(mHeight);
}


ImageFormat Image::getFormat() const
{
    return mFormat;
}


bool Image::isHDRImage() const
{
    const std::string imageFormatString = getFormat()._to_string();
    return imageFormatString.find('F') != std::string::npos;
}

END_LUMIERE_NAMESPACE
