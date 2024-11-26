#ifndef IMAGE_UTILS_H
#define IMAGE_UTILS_H

#include <fstream>
#include <vector>
#include <cstdint>
#include <string>
#include <cmath>
#include <cassert>
#include <iomanip>
#include "image.h" // Include your Image class here

using namespace std;


class ImageUtils {
    private:
        const static int kHeaderSize = 54;
        const static int kColorTableSize = 1024;
        const static int kWidhtOffset = 18;
        const static int kHeightOffset = 22;
        const static int kBitDepth = 28;

    public:
        static Image imageReader(const string& imageName){
            vector<uint8_t> header(kHeaderSize);
            vector<uint8_t> colorTable(kColorTableSize);
            vector<uint8_t> buffer;

            int32_t width = 0;
            int32_t height = 0;
            int32_t bitDepth = 0;
            ifstream inputFile(imageName, ios::binary);
            if(!inputFile.is_open()){
                throw ImageException("Unable to read image file " +imageName);
            }
            inputFile.read(reinterpret_cast<char*>(header.data()),kHeaderSize);
            if ( inputFile.gcount() != kHeaderSize ){
                throw ImageException("Failed to read BMP header from file " + imageName);
            }
            width =*reinterpret_cast<int32_t*>(&header[kWidhtOffset]);
            height=*reinterpret_cast<int32_t*>(&header[kHeightOffset]);
            bitDepth=*reinterpret_cast<int16_t*>(&header[kBitDepth]);

            PixelFormat format = Image::toPixelFormat(bitDepth);
            Image image(width,height,format,header);
            uint32_t stride = image.getStride();
            buffer.resize(height * stride);
            inputFile.read(reinterpret_cast<char*>(buffer.data()),buffer.size());
            std::cout << "Buffer size: " << buffer.size() << " bytes" << std::endl;

    std::cout << "Buffer content (hex):" << std::endl;
    for (size_t i = 0; i < buffer.size(); ++i) {
        std::cout << std::hex << std::setw(2) << std::setfill('0') << (int)buffer[i] << " ";
        if ((i + 1) % 16 == 0) { // Print 16 bytes per line
        std::cout << std::endl;
    }
    }
            image.setData(buffer);

            if(bitDepth <= 8){
                inputFile.read(reinterpret_cast<char*>(colorTable.data()),kColorTableSize);
                image.setColorTable(std::move(colorTable));
            }

            cout << "bitdepth : " << bitDepth << endl;
            cout << "format : " << static_cast<int>(format) << endl;
            cout << "width " << width << endl;
            cout << "heigh " << height << endl;
            cout << "stride " << stride << endl;
            cout << "size image stride * heigh " << height * stride << endl;


            inputFile.close();

            return image;

        }

        static void imageWriter(
            const string& imgName,
            const Image image
        ) {

            const vector<uint8_t>& header = image.getHeader();
            const vector<uint8_t>& colorTable = image.getColorTable();
            const vector<uint8_t>& buffer = image.getData();
            int bitDepth = Image::bitsPerPixel(image.getFormat());
            assert(buffer.size() == image.getStride() * image.getHeight());

   std::cout << "Buffer size: " << buffer.size() << " bytes" << std::endl;

    std::cout << "Buffer content (hex):" << std::endl;
    for (size_t i = 0; i < buffer.size(); ++i) {
        std::cout << std::hex << std::setw(2) << std::setfill('0') << (int)buffer[i] << " ";
        if ((i + 1) % 16 == 0) { // Print 16 bytes per line
        std::cout << std::endl;
    }
    }

            // Open the file for binary writing
            ofstream outputFile(imgName, ios::binary);
            if (!outputFile.is_open()) {
                throw std::runtime_error("Unable to open file for writing: " + imgName);
            }

            // Write the BMP header
            if (header.size() != kHeaderSize) {
                throw ImageException("ImageWriter: Invalid header size. Expected 54 bytes.");
            }
            outputFile.write(reinterpret_cast<const char*>(header.data()), header.size());

            // Write the color table if bitDepth is less than or equal to 8
            if (bitDepth <= 8) {
                if (colorTable.size() != kColorTableSize) {
                    throw ImageException("Invalid color table size. Expected 1024 bytes for indexed color BMP.");
                }
                outputFile.write(reinterpret_cast<const char*>(colorTable.data()), colorTable.size());
            }
            // Write the pixel data buffer
            outputFile.write(reinterpret_cast<const char*>(buffer.data()), buffer.size());

            // Close the file
            outputFile.close();
    }


                      
};

#endif
