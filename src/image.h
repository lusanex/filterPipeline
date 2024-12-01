/**********************************
 * @file image.h
 * @author Erich Gutierrez Chavez
 * @brief Defines the Image class for managing image data and metadata.
 *
 * @details
 * - Provides constructors for creating Image objects with or without data.
 * - Supports deep copy and move semantics for efficient memory management.
 * - Ensures image validity through dimension, format, and data size checks.
 * - Includes static utility methods for mapping pixel formats and bit depth.
 *
 * Constraints:
 * - The width, height, and format must be valid for the Image to be considered valid.
 * - The data size must match the expected buffer size based on image dimensions and stride.
 **********************************/

#ifndef IMAGE_H
#define IMAGE_H

#include <memory>
#include <vector>
#include <cstdint>
#include <iostream>
#include <stdexcept>
#include <map>

using namespace std;

/**********************************
 * @class ImageException
 * @brief Custom exception class for handling Image-related errors.
 **********************************/
class ImageException : public exception {
private:
    std::string message;

public:
    /**********************************
     * Constructs an ImageException with the specified error message.
     * @param msg The error message.
     **********************************/
    explicit ImageException(const std::string& msg) : message(msg) {}

    /**********************************
     * Retrieves the error message.
     * @return A C-string containing the error message.
     **********************************/
    const char* what() const noexcept override {
        return message.c_str();
    }
};

/**********************************
 * @enum PixelFormat
 * @brief Enumerates supported pixel formats for images.
 **********************************/
enum class PixelFormat {
    UNKNOWN = 0,
    GRAYSCALE1,
    GRAYSCALE2,
    GRAYSCALE4,
    GRAYSCALE8,
    RGB24,
    RGBA32,
    JPEG,
};

/**********************************
 * @class Image
 * @brief Manages image data, dimensions, and pixel formats.
 **********************************/
class Image {
private:
    int32_t width;                      // Image width in pixels
    int32_t height;                     // Image height in pixels
    PixelFormat format;                 // Pixel format of the image
    int32_t stride;                     // Number of bytes per row
    vector<uint8_t> buffer;             // Pixel data buffer
    bool isValid;                       // Indicates whether the image is valid

public:
    /**********************************
     * Maps bit depths to corresponding pixel formats.
     **********************************/
    static const map<int, PixelFormat> kBitDepthPixelMap;

    /**********************************
     * Converts a bit depth to its corresponding PixelFormat.
     * @param bitDepth The bit depth of the image.
     * @return The corresponding PixelFormat or UNKNOWN if not found.
     **********************************/
    static PixelFormat toPixelFormat(int32_t bitDepth) {
        auto it = kBitDepthPixelMap.find(bitDepth);
        if (it != kBitDepthPixelMap.end()) {
            return it->second;
        }
        return PixelFormat::UNKNOWN;
    }

    /**********************************
     * Retrieves the bits per pixel for a given PixelFormat.
     * @param format The pixel format.
     * @return The number of bits per pixel for the format, or 0 if unknown.
     **********************************/
    static int32_t bitsPerPixel(PixelFormat format) {
        for (const auto& pair : kBitDepthPixelMap) {
            if (pair.second == format) {
                return pair.first;
            }
        }
        return 0;
    }

    /**********************************
     * Default destructor.
     **********************************/
    ~Image() = default;

    /**********************************
     * Constructs an Image without data.
     * @param width The width of the image.
     * @param height The height of the image.
     * @param format The pixel format of the image.
     * @throws ImageException if dimensions or format are invalid.
     **********************************/
    Image(int32_t width, int32_t height, PixelFormat format)
        : width(width),
          height(height),
          format(format),
          stride(width * bytesPerStride(bitsPerPixel(format))),
          buffer(width * height * bytesPerStride(bitsPerPixel(format)), 0),
          isValid(false) {
        if (width <= 0 || height <= 0 || format == PixelFormat::UNKNOWN) {
            throw ImageException("Invalid image dimensions or format");
        }
    }

    /**********************************
     * Constructs an Image with data (deep copy).
     * @param width The width of the image.
     * @param height The height of the image.
     * @param format The pixel format of the image.
     * @param data The pixel data to be copied into the image.
     * @throws ImageException if dimensions, format, or data size are invalid.
     **********************************/
    Image(int32_t width, int32_t height, PixelFormat format, const vector<uint8_t>& data)
        : width(width),
          height(height),
          format(format),
          stride(width * bytesPerStride(bitsPerPixel(format))),
          buffer(data),
          isValid(true) {
        if (width <= 0 || height <= 0 || format == PixelFormat::UNKNOWN || 
            data.size() != static_cast<size_t>(height * stride)) {
            throw ImageException("Invalid image dimensions, format, or data size");
        }
    }

    /**********************************
     * Constructs an Image with data (move).
     * @param width The width of the image.
     * @param height The height of the image.
     * @param format The pixel format of the image.
     * @param data The pixel data to be moved into the image.
     * @throws ImageException if dimensions, format, or data size are invalid.
     **********************************/
    Image(int32_t width, int32_t height, PixelFormat format, vector<uint8_t>&& data)
        : width(width),
          height(height),
          format(format),
          stride(width * bytesPerStride(bitsPerPixel(format))),
          buffer(std::move(data)),
          isValid(true) {
        if (width <= 0 || height <= 0 || format == PixelFormat::UNKNOWN || 
            buffer.size() != static_cast<size_t>(height * stride)) {
            throw ImageException("Invalid image dimensions, format, or data size");
        }
    }

    /**********************************
     * Copy constructor for deep copying another Image.
     * @param other The Image to be copied.
     **********************************/
    Image(const Image& other)
        : width(other.width),
          height(other.height),
          format(other.format),
          stride(other.stride),
          buffer(other.buffer),
          isValid(other.isValid) {}

    /**********************************
     * Assignment operator for deep copying another Image.
     * @param other The Image to be copied.
     * @return A reference to the current object after the copy.
     **********************************/
    Image& operator=(const Image& other) {
        if (this == &other) {
            return *this;
        }
        width = other.width;
        height = other.height;
        format = other.format;
        stride = other.stride;
        buffer = other.buffer;
        isValid = other.isValid;
        return *this;
    }

    /**********************************
     * Move constructor to transfer ownership of another Image's data.
     * @param other The Image to be moved.
     **********************************/
    Image(Image&& other)
        : width(other.width),
          height(other.height),
          format(other.format),
          stride(other.stride),
          buffer(std::move(other.buffer)),
          isValid(other.isValid) {
        other.isValid = false;
    }

    /**********************************
     * Move assignment operator to transfer ownership of another Image's data.
     * @param other The Image to be moved.
     * @return A reference to the current object after the move.
     **********************************/
    Image& operator=(Image&& other) noexcept {
        if (this == &other) {
            return *this;
        }
        width = other.width;
        height = other.height;
        format = other.format;
        stride = other.stride;
        buffer = std::move(other.buffer);
        isValid = other.isValid;
        other.isValid = false;
        return *this;
    }

    /**********************************
     * Retrieves the image width.
     * @return The width of the image.
     **********************************/
    int32_t getWidth() const { return width; }

    /**********************************
     * Retrieves the image height.
     * @return The height of the image.
     **********************************/
    int32_t getHeight() const { return height; }

    /**********************************
     * Retrieves the image stride.
     * @return The stride of the image (bytes per row).
     **********************************/
    int32_t getStride() const { return stride; }

    /**********************************
     * Retrieves the pixel format of the image.
     * @return The PixelFormat of the image.
     **********************************/
    PixelFormat getFormat() const { return format; }

    /**********************************
     * Retrieves the image data as a constant reference.
     * @return A constant reference to the pixel data buffer.
     **********************************/
    const vector<uint8_t>& getData() const { return buffer; }

    /**********************************
     * Retrieves the image data as a mutable reference.
     * @return A mutable reference to the pixel data buffer.
     **********************************/
    vector<uint8_t>& getData() { return buffer; }

    /**********************************
     * Sets the image data (deep copy).
     * @param data The data to be copied into the image.
     * @throws ImageException if the data size does not match the image dimensions.
     **********************************/
    void setData(const vector<uint8_t>& data) {
        if (data.size() != static_cast<size_t>(height * stride)) {
            throw ImageException("Image setData size mismatch");
        }
        buffer = data;
        isValid = true;
    }

    /**********************************
     * Sets the image data (move).
     * @param data The data to be moved into the image.
     * @throws ImageException if the data size does not match the image dimensions.
     **********************************/
    void setData(vector<uint8_t>&& data) {
        if (data.size() != static_cast<size_t>(height * stride)) {
            throw ImageException("Image setData size mismatch");
        }
        buffer = std::move(data);
        isValid = true;
    }

    /**********************************
     * Checks if the image is valid.
     * @return True if the image is valid; false otherwise.
     **********************************/
    bool isImageValid() const { return isValid; }

private:
    /**********************************
     * Calculates the number of bytes per stride for the given bit depth.
     * @param bitsPerLine The number of bits per line.
     * @return The number of bytes per stride.
     **********************************/
    uint32_t bytesPerStride(uint32_t bitsPerLine) const {
        return (((bitsPerLine + 31) / 32) * 32) / 8;
    }

    /**********************************
     * Calculates the number of bytes per line for the given bit depth.
     * @param bitsPerLine The number of bits per line.
     * @return The number of bytes per line.
     **********************************/
    uint32_t bytesPerLine(uint32_t bitsPerLine) {
        return ((bitsPerLine + 7) / 8);
    }
};

const map<int, PixelFormat> Image::kBitDepthPixelMap = {
    {1, PixelFormat::GRAYSCALE1},
    {2, PixelFormat::GRAYSCALE2},
    {4, PixelFormat::GRAYSCALE4},
    {8, PixelFormat::GRAYSCALE8},
    {24, PixelFormat::RGB24},
    {32, PixelFormat::RGBA32},
};

#endif

