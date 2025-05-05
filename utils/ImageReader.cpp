#ifndef IMAGE_READER_CPP
#define IMAGE_READER_CPP

#include "ImageReader.hpp"
#include <fstream>
#include <sstream>

template class ImageReader<uint8_t>;
template class ImageReader<uint16_t>;
template class ImageReader<uint32_t>;
template class ImageReader<uint64_t>;

template <typename T>
ImageReader<T>::ImageReader() {}

template <typename T>
ImageStatus ImageReader<T>::readImage(const string &filePath, Image<T> &image)
{
    ifstream file(filePath, ios::binary);
    if (!file.is_open())
    {
        return ImageStatus::FILE_NOT_FOUND;
    }

    vector<T> rawData((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
    file.close();

    if (rawData.empty())
    {
        return ImageStatus::FILE_READ_ERROR;
    }

    image.metadata.format = detectFormat(rawData);
    switch (image.metadata.format)
    {
    case ImageFormat::PGM:
        return parsePGM(rawData, image);
    case ImageFormat::PNG:
        return parsePNG(rawData, image);
    case ImageFormat::JPEG:
        return parseJPEG(rawData, image);
    case ImageFormat::BMP:
        return parseBMP(rawData, image);
    default:
        return ImageStatus::UNSUPPORTED_FORMAT;
    }
}

template <typename T>
ImageFormat ImageReader<T>::detectFormat(const vector<T> &rawData)
{
    if (rawData.size() >= 2 && rawData[0] == 'P' && rawData[1] == '5')
    {
        return ImageFormat::PGM;
    }
    else if (rawData.size() >= 8 &&
             rawData[0] == 0x89 && rawData[1] == 'P' && rawData[2] == 'N' &&
             rawData[3] == 'G' && rawData[4] == 0x0D &&
             rawData[5] == 0x0A && rawData[6] == 0x1A &&
             rawData[7] == 0x0A)
    {
        return ImageFormat::PNG;
    }
    else if (rawData.size() >= 2 && rawData[0] == 0xFF && rawData[1] == 0xD8)
    {
        return ImageFormat::JPEG;
    }
    else if (rawData.size() >= 2 && rawData[0] == 'B' && rawData[1] == 'M')
    {
        return ImageFormat::BMP;
    }
    return ImageFormat::UNKNOWN;
}

template <typename T>
ImageStatus ImageReader<T>::parseMetadata(const vector<T> &, ImageMetadata &)
{
    return ImageStatus::UNIMPLEMENTED_FEATURE; // Reserved for shared metadata logic if needed.
}

template <typename T>
ImageStatus ImageReader<T>::parsePGM(const vector<T> &rawData, Image<T> &image)
{
    istringstream stream(string(rawData.begin(), rawData.end()));
    string line;
    string magicNumber;
    uint32_t width = 0, height = 0, maxValue = 0;

    // Read magic number
    getline(stream, line);
    if (line == "P2")
    {
        return ImageStatus::UNIMPLEMENTED_FEATURE;
    }
    else if (line != "P5")
    {
        return ImageStatus::PARSE_ERROR;
    }
    magicNumber = line;

    // Read width, height, and maxValue while skipping comments
    while (getline(stream, line))
    {
        if (line.empty() || line[0] == '#')
            continue;
        istringstream dimensions(line);
        dimensions >> width >> height;
        if (width > 0 && height > 0)
            break;
    }
    while (getline(stream, line))
    {
        if (line.empty() || line[0] == '#')
            continue;
        istringstream maxValStream(line);
        maxValStream >> maxValue;
        if (maxValue > 0)
            break;
    }

    if (width == 0 || height == 0 || maxValue == 0)
    {
        return ImageStatus::PARSE_ERROR;
    }

    image.metadata.width = width;
    image.metadata.height = height;
    image.metadata.maxValue = maxValue;

    // Get position of pixel data after header
    size_t headerSize = stream.tellg();
    if (headerSize == -1)
    {
        return ImageStatus::PARSE_ERROR;
    }

    if (rawData.size() < headerSize + width * height)
    {
        return ImageStatus::FILE_READ_ERROR;
    }

    // Fill pixelData
    if (maxValue <= 255)
    {
        if (sizeof (T) < 1)
        {
            return ImageStatus::INVALID_DATASIZE;
        }
        image.pixelData.assign(rawData.begin() + headerSize, rawData.begin() + headerSize + width * height);
    }
    else if (maxValue <= 65535)
    {
        if (sizeof (T) < 2)
        {
            return ImageStatus::INVALID_DATASIZE;
        }
        image.pixelData.resize(width * height);
        for (uint32_t i = 0; i < width * height; ++i)
        {
            image.pixelData[i] = (rawData[headerSize + 2 * i] << 8) | rawData[headerSize + 2 * i + 1];
        }
    }
    else
    {
        return ImageStatus::UNSUPPORTED_FORMAT;
    }

    // Fill pixelMatrix
    image.pixelMatrix.resize(height, vector<T>(width));
    for (uint32_t i = 0; i < height; ++i)
    {
        for (uint32_t j = 0; j < width; ++j)
        {
            image.pixelMatrix[i][j] = image.pixelData[i * width + j];
        }
    }

    return ImageStatus::SUCCESS;
}

// Placeholder implementations for future formats
template <typename T>
ImageStatus ImageReader<T>::parsePNG(const vector<T> &, Image<T> &)
{
    return ImageStatus::UNIMPLEMENTED_FEATURE; // Implement later
}

template <typename T>
ImageStatus ImageReader<T>::parseJPEG(const vector<T> &, Image<T> &)
{
    return ImageStatus::UNIMPLEMENTED_FEATURE; // Implement later
}

template <typename T>
ImageStatus ImageReader<T>::parseBMP(const vector<T> &, Image<T> &)
{
    return ImageStatus::UNIMPLEMENTED_FEATURE; // Implement later
}


#endif // IMAGE_READER_CPP
