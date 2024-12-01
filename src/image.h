#ifndef IMAGE_H
#define IMAGE_H

#include <memory>
#include <vector>
#include <cstdint>
#include <iostream>
#include <stdexcept>
#include <map>

using namespace std;

class ImageException : public exception {
private:
    std::string message;

public:
    explicit ImageException(const std::string& msg) : message(msg) {}
    const char* what() const noexcept override {
        return message.c_str();
    }
};

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

class Image {
private:
    int32_t width;
    int32_t height;
    PixelFormat format;
    int32_t stride;
    vector<uint8_t> buffer;
    bool isValid;
 
 
public:

    static const map<int, PixelFormat> kBitDepthPixelMap;
   
    static PixelFormat toPixelFormat(int32_t bitDepth){
        auto it = kBitDepthPixelMap.find(bitDepth);
        if (it != kBitDepthPixelMap.end()) {
            return it->second;
        }
        return PixelFormat::UNKNOWN;
    }

    static int32_t bitsPerPixel(PixelFormat format) {
        for (const auto& pair : kBitDepthPixelMap) {
            if (pair.second == format) {
                return pair.first;
            }
        }
        return 0;
    }

public:

    ~Image() = default;

    // Constructor without data
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

    // Constructor with data (copy)
    Image(int32_t width, int32_t height, PixelFormat format, const vector<uint8_t>& data)
        : width(width),
          height(height),
          format(format),
          stride(width * bytesPerStride(bitsPerPixel(format))),
          buffer(data), // Deep copy
          isValid(true) {
        if (width <= 0 || height <= 0 || format == PixelFormat::UNKNOWN || 
            data.size() != static_cast<size_t>(height * stride)) {
            throw ImageException("Invalid image dimensions, format, or data size");
        }
    }

    // Constructor with data (move)
    Image(int32_t width, int32_t height, PixelFormat format, vector<uint8_t>&& data)
        : width(width),
          height(height),
          format(format),
          stride(width * bytesPerStride(bitsPerPixel(format))),
          buffer(std::move(data)), // Move ownership
          isValid(true) {
        if (width <= 0 || height <= 0 || format == PixelFormat::UNKNOWN || 
            buffer.size() != static_cast<size_t>(height * stride)) {
            throw ImageException("Invalid image dimensions, format, or data size");
        }
    }

    // Copy Constructor (Deep Copy)
    Image(const Image& other)
        : width(other.width),
          height(other.height),
          format(other.format),
          stride(other.stride),
          buffer(other.buffer), // Deep copy
          isValid(other.isValid) {}

    // Assignment Operator (Deep Copy)
    Image& operator=(const Image& other) {
        if (this == &other) {
            return *this; // Handle self-assignment
        }
        width = other.width;
        height = other.height;
        format = other.format;
        stride = other.stride;
        buffer = other.buffer; // Deep copy
        isValid = other.isValid;
        return *this;
    }

    // Move Constructor
    Image(Image&& other)
        : width(other.width),
          height(other.height),
          format(other.format),
          stride(other.stride),
          buffer(std::move(other.buffer)), // Take ownership
          isValid(other.isValid) {
        other.isValid = false;
    }

    // Move Assignment Operator
    Image& operator=(Image&& other) noexcept {
        if (this == &other) {
            return *this;
        }
        width = other.width;
        height = other.height;
        format = other.format;
        stride = other.stride;
        buffer = std::move(other.buffer); // Take ownership
        isValid = other.isValid;
        other.isValid = false;
        return *this;
    }

    // Accessors
    int32_t getWidth() const { return width; }
    int32_t getHeight() const { return height; }
    int32_t getStride() const { return stride; }
    PixelFormat getFormat() const { return format; }
    const vector<uint8_t>& getData() const { return buffer; }
    vector<uint8_t>& getData() {
        return buffer;
    }

    // Set Data (Deep Copy)
    void setData(const vector<uint8_t>& data) {
        if (data.size() != static_cast<size_t>(height * stride)) {
            throw ImageException("Image setData size mismatch");
        }
        buffer = data; // Deep copy
        isValid = true;
    }

    // Set Data (Move)
    void setData(vector<uint8_t>&& data) {
        if (data.size() != static_cast<size_t>(height * stride)) {
            throw ImageException("Image setData size mismatch");
        }
        buffer = std::move(data); // Move ownership
        isValid = true;
    }

    // Check Validity
    bool isImageValid() const { return isValid; }

private:
    
    uint32_t bytesPerStride(uint32_t bitsPerLine) const {
        return (((bitsPerLine + 31) / 32) * 32) / 8;
    }

    uint32_t bytesPerLine(uint32_t bitsPerLine){
        return ((bitsPerLine +7) / 8);
    }


};

const  map<int, PixelFormat> Image::kBitDepthPixelMap = {
        {1, PixelFormat::GRAYSCALE1},
        {2, PixelFormat::GRAYSCALE2},
        {4, PixelFormat::GRAYSCALE4},
        {8, PixelFormat::GRAYSCALE8},
        {24, PixelFormat::RGB24},
        {32, PixelFormat::RGBA32},
    };

 

#endif
