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


#include <stdexcept>
using namespace std;


class ImageUtils {
    private:
        const static int kHeaderSize = 54;
        const static int kColorTableSize = 1024;
        const static int kImageSizeOffset = 2;
        const static int kPixelDataOffset = 10;
        const static int kWidhtOffset = 18;
        const static int kHeightOffset = 22;
        const static int kBitDepth = 28;

    public:
        static string hexdump(const std::vector<uint8_t>& data) {
            ostringstream hexdump;
            const size_t bytesPerLine = 16; // Number of bytes to display per line
            size_t dataSize = data.size();

            for (size_t i = 0; i < dataSize; i += bytesPerLine) {
                // Print the offset in the first column
                hexdump << std::setw(8) << std::setfill('0') << std::hex << i << " ";

                // Print the bytes in hexadecimal
                for (size_t j = 0; j < bytesPerLine; ++j) {
                    if (i + j < dataSize) {
                        hexdump << std::setw(2) << std::setfill('0') << std::hex
                                << static_cast<int>(data[i + j]) << " ";
                    } else {
                        hexdump << "   "; // Padding for incomplete lines
                    }
                }

                // Add a separator between hex and ASCII sections
                hexdump << " ";

                // Print the bytes in ASCII (printable characters only)
                for (size_t j = 0; j < bytesPerLine; ++j) {
                    if (i + j < dataSize) {
                        uint8_t byte = data[i + j];
                        if (std::isprint(byte)) {
                            hexdump << static_cast<char>(byte); // Printable characters
                        } else {
                            hexdump << '.'; // Non-printable characters
                        }
                    }
                }

                hexdump << "\n"; // Newline after each line

            }

            return hexdump.str();
        }



#pragma pack(push, 1)
struct BMPFileHeader {
    uint16_t file_type{0x4D42}; // File type always BM which is 0x4D42
    uint32_t file_size{0};      // Size of the file (in bytes)
    uint16_t reserved1{0};      // Reserved, always 0
    uint16_t reserved2{0};      // Reserved, always 0
    uint32_t offset_data{0};    // Start position of pixel data
};

struct BMPInfoHeader {
    uint32_t size{0};           // Size of this header (in bytes)
    int32_t width{0};           // Width of the image in pixels
    int32_t height{0};          // Height of the image in pixels
    uint16_t planes{1};         // Number of planes (always 1)
    uint16_t bit_count{0};      // Bits per pixel
    uint32_t compression{0};    // Compression type (0 = none)
    uint32_t size_image{0};     // Size of pixel data
    int32_t x_pixels_per_meter{0};
    int32_t y_pixels_per_meter{0};
    uint32_t colors_used{0};    // Number of colors in color table
    uint32_t colors_important{0}; // Important colors
};

struct BMPColorHeader {
    uint32_t red_mask{0x00ff0000};
    uint32_t green_mask{0x0000ff00};
    uint32_t blue_mask{0x000000ff};
    uint32_t alpha_mask{0xff000000};
    uint32_t color_space_type{0x73524742}; // sRGB
    uint32_t unused[16]{0};
};
#pragma pack(pop)

public:
    static Image readBMP(const std::string &filename) {
        BMPFileHeader fileHeader;
        BMPInfoHeader infoHeader;
        BMPColorHeader colorHeader;
        vector<uint8_t> data;


        ifstream file(filename, ios::binary);
        if (!file.is_open()) {
            throw runtime_error("Error: Unable to open file " + filename);
        }

        // Read BMP file header
        file.read(reinterpret_cast<char *>(&fileHeader), sizeof(fileHeader));
        if (fileHeader.file_type != 0x4D42) {
            throw runtime_error("Error: File is not a valid BMP format.");
        }

        // Read BMP info header
        file.read(reinterpret_cast<char *>(&infoHeader), sizeof(infoHeader));
        printBMPHeaders(fileHeader,infoHeader);

        // Handle 32-bit BMPs with color masks
        if (infoHeader.bit_count == 32) {
            if ( infoHeader.size >= (sizeof(BMPInfoHeader) + sizeof(BMPColorHeader))){
                file.read(reinterpret_cast<char *>(&colorHeader), sizeof(colorHeader));
                validateColorHeader(colorHeader);
            }
            else{
                cerr << "Error file " << filename << " does not contain bit mask " <<endl;
                throw ImageException("Error reading BMP unrecognized file format");
            }
        }
        // Move to pixel data location
        file.seekg(fileHeader.offset_data, file.beg);

        if(infoHeader.bit_count == 32){
            infoHeader.size = sizeof(BMPInfoHeader) + sizeof(BMPColorHeader);
            fileHeader.offset_data = sizeof(BMPFileHeader) + sizeof(BMPInfoHeader) + sizeof(BMPColorHeader);
        }
        else{
            infoHeader.size = sizeof(BMPInfoHeader);
            fileHeader.offset_data = sizeof(BMPFileHeader) + sizeof(BMPInfoHeader);
        }
        fileHeader.file_size = fileHeader.offset_data;

        if( infoHeader.height < 0 ){
            throw ImageException("The program can tread only BMP images");
        }
        data.resize(infoHeader.width * infoHeader.height * infoHeader.bit_count / 8);

        if ( infoHeader.width % 4 == 0 ){
            file.read(reinterpret_cast<char *>(data.data()), data.size());
            fileHeader.file_size += static_cast<uint32_t>(data.size());
        }
        else{
            rowStride = infoHeader.width * infoHeader.bit_count / 8;
            uint32_t newStride = makeStrideAligned(4);
            vector<uint8_t> paddingRow(newStride - rowStride);
            for ( int y = 0; y < infoHeader.height; ++y ){
                file.read(reinterpret_cast<char*>(data.data() + rowStride * y),rowStride);
                file.read(reinterpret_cast<char*>(paddingRow.data()),paddingRow.size());

                fileHeader.file_size += static_cast<uint32_t>(data.size()) + infoHeader.height * static_cast<uint32_t>(paddingRow.size());
            }
        }

        PixelFormat format = infoHeader.bit_count == 32 ? PixelFormat::RGBA32 : PixelFormat::RGB24;
        // Read pixel data
        Image image(infoHeader.width,infoHeader.height,format,data);

        //image.setData(std::move(data));

        return image;
    }

    static void writeBMP(const std::string &filename, const Image &image) {
        BMPFileHeader fileHeader;
        BMPInfoHeader infoHeader;
        BMPColorHeader colorHeader;

        // Populate headers based on the Image object
        fileHeader.file_type = 0x4D42;
        infoHeader.compression = 0;
        fileHeader.offset_data = sizeof(BMPFileHeader) + sizeof(BMPInfoHeader);
        if (image.getFormat() == PixelFormat::RGBA32) {
            infoHeader.size = sizeof(BMPInfoHeader) + sizeof(BMPColorHeader);
            fileHeader.offset_data += sizeof(BMPColorHeader);
            infoHeader.compression = 3;
            fileHeader.file_size = fileHeader.offset_data + static_cast<uint32_t>(image.getData().size());
        }
        fileHeader.file_size = fileHeader.offset_data + image.getData().size();

        infoHeader.size = sizeof(BMPInfoHeader);
        infoHeader.width = image.getWidth();
        infoHeader.height = image.getHeight();
        infoHeader.planes = 1;
        infoHeader.bit_count = image.getFormat() == PixelFormat::RGBA32 ? 32 : 24;
        infoHeader.size_image = static_cast<uint32_t>(image.getData().size());

        if (image.getFormat() == PixelFormat::RGBA32) {
            colorHeader = BMPColorHeader{};
        }

        // Write BMP file
        std::ofstream file(filename, std::ios::binary);
        if (!file.is_open()) {
            throw std::runtime_error("Error: Unable to open file " + filename);
        }

        file.write(reinterpret_cast<const char *>(&fileHeader), sizeof(fileHeader));
        file.write(reinterpret_cast<const char *>(&infoHeader), sizeof(infoHeader));
        if (image.getFormat() == PixelFormat::RGBA32) {
            file.write(reinterpret_cast<const char *>(&colorHeader), sizeof(colorHeader));
        }
        file.write(reinterpret_cast<const char *>(image.getData().data()), image.getData().size());
    }

private:

    static uint32_t rowStride;
    static void validateColorHeader(const BMPColorHeader &colorHeader) {
        BMPColorHeader expectedHeader;
        if (colorHeader.red_mask != expectedHeader.red_mask ||
            colorHeader.green_mask != expectedHeader.green_mask ||
            colorHeader.blue_mask != expectedHeader.blue_mask ||
            colorHeader.alpha_mask != expectedHeader.alpha_mask) {
            throw runtime_error("Error: Unexpected color mask format.");
        }
        if (colorHeader.color_space_type != expectedHeader.color_space_type) {
            throw runtime_error("Error: Unexpected color space type.");
        }
    }
    static uint32_t makeStrideAligned(uint32_t alignStride) {
        uint32_t newStride = rowStride;
        while (newStride % alignStride != 0) {
            newStride++;
        }
        return newStride; 
    }

    static void writeHeaders(ofstream& of, BMPFileHeader fileHeader, BMPInfoHeader infoHeader,BMPColorHeader colorHeader ){
        of.write(reinterpret_cast<char*>(&fileHeader),sizeof(fileHeader));
        of.write(reinterpret_cast<char*>(&infoHeader),sizeof(infoHeader));
        if(infoHeader.bit_count == 32){
            of.write(reinterpret_cast<char*>(&colorHeader),sizeof(colorHeader));
        }
    }


    static void writeHeadersAndData(ofstream& of, BMPFileHeader fileHeader, BMPInfoHeader infoHeader,BMPColorHeader colorHeader , vector<uint8_t> data){
        writeHeaders(of,fileHeader,infoHeader,colorHeader);
        of.write(reinterpret_cast<char*>(data.data()),data.size());
    }
    static void printBMPHeaders(const BMPFileHeader& fileHeader, const BMPInfoHeader& infoHeader) {
        // Print BMPFileHeader fields
        std::cout << "===== BMP File Header =====" << std::endl;
        std::cout << "File Type: " << std::hex << "0x" << fileHeader.file_type << std::dec << std::endl;
        std::cout << "File Size: " << fileHeader.file_size << " bytes" << std::endl;
        std::cout << "Reserved1: " << fileHeader.reserved1 << std::endl;
        std::cout << "Reserved2: " << fileHeader.reserved2 << std::endl;
        std::cout << "Pixel Data Offset: " << fileHeader.offset_data << " bytes" << std::endl;

        // Print BMPInfoHeader fields
        std::cout << "===== BMP Info Header =====" << std::endl;
        std::cout << "Header Size: " << infoHeader.size << " bytes" << std::endl;
        std::cout << "Image Width: " << infoHeader.width << " pixels" << std::endl;
        std::cout << "Image Height: " << infoHeader.height << " pixels" << std::endl;
        std::cout << "Planes: " << infoHeader.planes << std::endl;
        std::cout << "Bit Count: " << infoHeader.bit_count << " bits per pixel" << std::endl;
        std::cout << "Compression: " << infoHeader.compression << std::endl;
        std::cout << "Image Size: " << infoHeader.size_image << " bytes" << std::endl;
        std::cout << "X Pixels per Meter: " << infoHeader.x_pixels_per_meter << std::endl;
        std::cout << "Y Pixels per Meter: " << infoHeader.y_pixels_per_meter << std::endl;
        std::cout << "Colors Used: " << infoHeader.colors_used << std::endl;
        std::cout << "Important Colors: " << infoHeader.colors_important << std::endl;
    }

};

uint32_t ImageUtils::rowStride = {0};
        
#endif
