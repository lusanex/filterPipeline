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
    shared_ptr<vector<uint8_t>> buffer;
    bool isValid;
    shared_ptr<vector<uint8_t>> colorTable;
    vector<uint8_t> header;


   
 
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

    ~Image() { buffer.reset(); }
    // Constructor without data
    Image(int32_t width, int32_t height, PixelFormat format, vector<uint8_t> header = {})
        : width(width),
          height(height),
          format(format),
          stride(width * bytesPerStride(bitsPerPixel(format))),
          buffer(make_shared<vector<uint8_t>>(height * stride, 0)),
          isValid(false),
          colorTable(make_shared<vector<uint8_t>>()),
          header(header)
        { 
        if (width <= 0 || height <= 0 || format == PixelFormat::UNKNOWN) {
            throw ImageException("Invalid image dimensions or format");
        }
    }

    // Constructor with data (copy)
    Image(int32_t width, int32_t height, PixelFormat format,
          int32_t stride, const vector<uint8_t>& data,const vector<uint8_t>& colorTable, vector<uint8_t> header = {})
        : width(width),
          height(height),
          format(format),
          stride(stride),
          buffer(make_shared<vector<uint8_t>>(data.begin(), data.end())),
          isValid(true),
          colorTable(make_shared<vector<uint8_t>>(colorTable.begin(),colorTable.end())),
          header(vector<uint8_t>(header.begin(), header.end()))

        {
        if (width <= 0 || height <= 0 || format == PixelFormat::UNKNOWN || 
            data.size() != static_cast<size_t>(height * stride)) {
            throw ImageException("Invalid image dimensions, format, or data size");
        }
    }

    // Constructor with data (move)
    Image(int32_t width, int32_t height, PixelFormat format,
          int32_t stride, vector<uint8_t>&& data, vector<uint8_t>&& colorTable = {}, vector<uint8_t>&& header = {})
        : width(width),
          height(height),
          format(format),
          stride(stride),
          buffer(make_shared<vector<uint8_t>>(std::move(data))),
          isValid(true),
          colorTable(make_shared<vector<uint8_t>>(std::move(colorTable))),
          header(vector<uint8_t>(std::move(header)))
          {

            if (width <= 0 || height <= 0 || format == PixelFormat::UNKNOWN || 
                data.size() != static_cast<size_t>(height * stride)) {
                throw ImageException("Invalid image dimensions, format, or data size");
            }
    }

    // Copy Constructor
    Image(const Image& other)
        : width(other.width),
          height(other.height),
          format(other.format),
          stride(other.stride),
          buffer(other.buffer),
          isValid(other.isValid),
          colorTable(other.colorTable),
          header(other.header){}



    // Clone Method
    Image clone() const {
        if (!isValid) {
            throw ImageException("Cannot clone an invalid image");
        }
        return Image(width, height, format, stride, *buffer,*colorTable,header); // Deep copy of buffer
    }

    // Accessors
    int32_t getWidth() const { return width; }
    int32_t getHeight() const { return height; }
    int32_t getStride() const { return stride; }
    vector<uint8_t> getHeader() const { return header;}
    PixelFormat getFormat() const { return format; }
    vector<uint8_t> getColorTable() const { return *colorTable; }

    void setColorTable(vector<uint8_t>&& nColorTable){
        colorTable.reset();
        colorTable = make_shared<vector<uint8_t>>(std::move(nColorTable));
    }

    void setHeader(vector<uint8_t> nHeader) {
        header = vector<uint8_t>(nHeader.begin(), nHeader.end());
    }
    void setStride(int32_t newStride) { stride = newStride; };
    void setFormat(PixelFormat newFormat){
        format = newFormat; 
        stride = bytesPerStride(bitsPerPixel(format));
    }

    vector<uint8_t>& getData() const {
        if (!isValid) {
            throw ImageException("Image is invalid");
        }
        return *buffer;
    }

    // Set Data
    void setData(const vector<uint8_t>& data) {
        if (data.size() != static_cast<size_t>(height * stride)) {
            throw ImageException("Image setData size mismatch got: " +to_string( data.size()) + " expected " + to_string(height * stride));
        }
        buffer = make_shared<vector<uint8_t>>(data.begin(), data.end()); // Deep copy
        isValid = true;
    }
    void setData(const vector<uint8_t>&& data) {
        if (data.size() != static_cast<size_t>(height * stride)) {

            throw ImageException("Image setData size mismatch got: " +to_string( data.size()) + " expected " + to_string(height * stride));
        }
        buffer = make_shared<vector<uint8_t>>(std::move(data)); // move opeartion take owenership of bufferj
        isValid = true;
    }
    // Check Validity
    bool isImageValid() const { return isValid; }

private:   

    uint32_t bytesPerStride(uint32_t bitsPerLine) const {
        return (((bitsPerLine + 31) / 32) * 32) / 8;
    }

    uint32_t bytesPerLine(uint32_t bitsPerLine){
        return ((bitsPerLine + 7) / 8);
    }


};


const std::map<int, PixelFormat> Image::kBitDepthPixelMap = {
    {1, PixelFormat::GRAYSCALE1},
    {2, PixelFormat::GRAYSCALE2},
    {4, PixelFormat::GRAYSCALE4},
    {8, PixelFormat::GRAYSCALE8},
    {24, PixelFormat::RGB24},
    {32, PixelFormat::RGBA32},
};

#endif // IMAGE_H

