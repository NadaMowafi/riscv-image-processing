#ifndef IMAGE_WRITER_CPP
#define IMAGE_WRITER_CPP

#include "ImageWriter.hpp"
#include <fstream>

template class ImageWriter<uint8_t>;
template class ImageWriter<uint16_t>;
template class ImageWriter<uint32_t>;
template class ImageWriter<uint64_t>;

template <typename T>
ImageWriter<T>::ImageWriter() {}

template <typename T>
ImageStatus ImageWriter<T>::writeImage(const string &filePath, const Image<T> &image)
{
    switch (image.metadata.format)
    {
    case ImageFormat::PGM:
        return writePGM(filePath, image);
    case ImageFormat::PNG:
        return writePNG(filePath, image);
    case ImageFormat::JPEG:
        return writeJPEG(filePath, image);
    case ImageFormat::BMP:
        return writeBMP(filePath, image);
    default:
        return ImageStatus::UNSUPPORTED_FORMAT;
    }
}

template <typename T>
ImageStatus ImageWriter<T>::writePGM(const string &filePath, const Image<T> &image)
{
    ofstream file(filePath, ios::binary);
    if (!file.is_open())
    {
        return ImageStatus::FILE_WRITE_ERROR;
    }

    // Write PGM header
    file << "P5\n";
    file << image.metadata.width << " " << image.metadata.height << "\n";
    file << image.metadata.maxValue << "\n";

    // Write pixel data from pixelMatrix
    if (image.metadata.maxValue <= 255)
    {
        for (const auto &row : image.pixelMatrix)
        {
            file.write(reinterpret_cast<const char *>(row.data()), row.size());
        }
    }
    else
    {
        for (const auto &row : image.pixelMatrix)
        {
            for (const auto &pixel : row)
            {
                file.write(reinterpret_cast<const char *>(&pixel), sizeof(T));
            }
        }
    }

    file.close();
    if (!file)
    {
        return ImageStatus::FILE_WRITE_ERROR;
    }

    return ImageStatus::SUCCESS;
}

template <typename T>
ImageStatus ImageWriter<T>::writePNG(const string &, const Image<T> &)
{
    return ImageStatus::UNIMPLEMENTED_FEATURE; // To be implemented later
}

template <typename T>
ImageStatus ImageWriter<T>::writeJPEG(const string &, const Image<T> &)
{
    return ImageStatus::UNIMPLEMENTED_FEATURE; // To be implemented later
}

template <typename T>
ImageStatus ImageWriter<T>::writeBMP(const string &, const Image<T> &)
{
    return ImageStatus::UNIMPLEMENTED_FEATURE; // To be implemented later
}

#endif // IMAGE_WRITER_CPP
