/**********************************
 * @file imageutils.h
 * @author Erich Gutierrez Chavez
 * @brief Provides utility functions for 
 * handling BMP image files and related operations.
 *
 * @details
 * - Defines functions for reading and writing BMP files.
 * - Supports 24-bit (RGB) and 32-bit (RGBA) BMP files.
 * - Includes utilities for converting pixel formats and validating headers.
 * - Contains a hexdump function for debugging byte arrays.
 * - Provides a function to print BMP headers for detailed inspection.
 *
 * Key Features:
 * - Read and convert BMP files into an Image object with pixel format correction
 *   form BGRA to RGBA or BGR to RGB.
 * - Row order is changed form top to bottom when Image is create
 * - Row order is changed from bottom to top when converte to BMP format 
 * - Write Image objects to BMP files, converting to appropriate BMP format.
 * - Validate BMP file structure, including file, info, and color headers.
 * - Debugging support with hexdump and BMP header printing utilities.
 *
 * Constraints:
 * - Only 24-bit and 32-bit uncompressed BMP files are supported.
 * - Header validation must pass for BMP files to be processed correctly.
 * - Pixel data is assumed to be bottom-up as per BMP standard.
 *
 * Algorithm adapted and used from: 
 *
 * https://solarianprogrammer.com/2018/11/19/cpp-reading-writing-bmp-images/
 *
 **********************************/

#ifndef IMAGE_UTILS_H
#define IMAGE_UTILS_H

#include <fstream>
#include <vector>
#include <string>
#include <cmath>
#include <cassert>
#include <iomanip>
#include "image.h" 
#include <stdexcept>

using namespace std;

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


class ImageUtils {

public:

    /**********************************
     * Generates a hexadecimal representation of a byte array.
     * Displays the data in both hex and ASCII formats.
     * @param data The data to generate a hexdump for.
     * @return A formatted string representing the hexdump.
     **********************************/
    static string hexdump(const vector<uint8_t>& data) {
        ostringstream hexdump;
        const size_t bytesPerLine = 16; // Number of bytes to display per line
        size_t dataSize = data.size();

        for (size_t i = 0; i < dataSize; i += bytesPerLine) {
            // Print the offset in the first column
            hexdump << setw(8) << setfill('0') << hex << i << " ";

            // Print the bytes in hexadecimal
            for (size_t j = 0; j < bytesPerLine; ++j) {
                if (i + j < dataSize) {
                    hexdump << setw(2) << setfill('0') << hex
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
                    if (isprint(byte)) {
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


public:

    /**********************************
     * Reads a BMP file and creates an Image object.
     * Supports only 32-bit (RGBA) and 24-bit (RGB) BMP files.
     * Converts pixel data from BGRA to RGBA or BGR to RGB.
     * @param filename The path to the BMP file.
     * @return An Image object representing the BMP file.
     * @throws ImageException if the BMP file is invalid or unsupported.
     **********************************/
    static Image readBMP(const std::string& filename) {
        BMPFileHeader fileHeader;
        BMPInfoHeader infoHeader;
        BMPColorHeader colorHeader;
        vector<uint8_t> data;

        ifstream file(filename, ios::binary);
        if (!file.is_open()) {
            throw runtime_error("Error readBMP: Unable to open file " + filename);
        }

        // Read BMP file header
        file.read(reinterpret_cast<char*>(&fileHeader), sizeof(fileHeader));
        if (fileHeader.file_type != 0x4D42) {
            throw runtime_error("Error readBMP: File is not a valid BMP format.");
        }

        // Read BMP info header
        file.read(reinterpret_cast<char*>(&infoHeader), sizeof(infoHeader));

        // Validate bit depth
        if (infoHeader.bit_count != 32 && infoHeader.bit_count != 24) {
            throw ImageException("Error: Only 32-bit and 24-bit BMP files are supported.");
        }

        // Handle 32-bit BMPs with color masks
        if (infoHeader.bit_count == 32) {
            if (infoHeader.size >= (sizeof(BMPInfoHeader) + sizeof(BMPColorHeader))) {
                file.read(reinterpret_cast<char*>(&colorHeader), sizeof(colorHeader));
                validateColorHeader(colorHeader);
            } else {
                throw ImageException("Error: 32-bit BMP file lacks color masks.");
            }
        }

        // Move to pixel data location
        file.seekg(fileHeader.offset_data, file.beg);

        // Read pixel data
        data.resize(infoHeader.width * infoHeader.height * (infoHeader.bit_count / 8));
        file.read(reinterpret_cast<char*>(data.data()), data.size());

        // Convert pixel data to RGBA or RGB
        PixelFormat format = 
            infoHeader.bit_count == 32 ? PixelFormat::RGBA32 : PixelFormat::RGB24;

        if (infoHeader.bit_count == 32) {
            // Allocate a new vector to store the corrected data
            vector<uint8_t> correctedData(data.size());

            // Calculate row size in bytes, including any padding
            size_t rowSize = infoHeader.width * 4;

            for (size_t row = 0; row < infoHeader.height; ++row) {
                // Calculate the start index for the bottom row and the target top row
                size_t sourceRowIndex = (infoHeader.height - 1 - row) * rowSize;
                size_t targetRowIndex = row * rowSize;

                // Copy the row while swapping the blue and red channels
                for (size_t col = 0; col < infoHeader.width; ++col) {
                    size_t sourceIndex = sourceRowIndex + col * 4;
                    size_t targetIndex = targetRowIndex + col * 4;

                    correctedData[targetIndex] = data[sourceIndex + 2];       // Red
                    correctedData[targetIndex + 1] = data[sourceIndex + 1];   // Green
                    correctedData[targetIndex + 2] = data[sourceIndex];       // Blue
                    correctedData[targetIndex + 3] = data[sourceIndex + 3];   
                }
            }

            // Replace original data with the corrected data
            data = std::move(correctedData);

        } else if (infoHeader.bit_count == 24) {
            // Allocate a new vector to store the corrected data
            vector<uint8_t> correctedData(data.size());

            // Calculate row size in bytes, including any padding
            size_t rowSize = infoHeader.width * 3;

            for (size_t row = 0; row < infoHeader.height; ++row) {
                // Calculate the start index for the bottom row and the target top row
                size_t sourceRowIndex = (infoHeader.height - 1 - row) * rowSize;
                size_t targetRowIndex = row * rowSize;

                // Copy the row while swapping the blue and red channels
                for (size_t col = 0; col < infoHeader.width; ++col) {
                    size_t sourceIndex = sourceRowIndex + col * 3;
                    size_t targetIndex = targetRowIndex + col * 3;

                    correctedData[targetIndex] = data[sourceIndex + 2];       // Red
                    correctedData[targetIndex + 1] = data[sourceIndex + 1];   // Green
                    correctedData[targetIndex + 2] = data[sourceIndex];       // Blue
                }
            }

            data = std::move(correctedData);
        }

        return Image(infoHeader.width, infoHeader.height, format, std::move(data));
    }

    /**********************************
     * Writes an Image object to a BMP file.
     * Converts pixel data from RGBA to BGRA or RGB to BGR.
     * @param filename The path to save the BMP file.
     * @param image The Image object to be saved.
     * @throws runtime_error if the file cannot be opened.
     **********************************/
    static void writeBMP(const std::string& filename, const Image& image) {
        BMPFileHeader fileHeader;
        BMPInfoHeader infoHeader;
        BMPColorHeader colorHeader;

        // Populate headers based on the Image object
        infoHeader.compression = 0;
        fileHeader.offset_data = sizeof(BMPFileHeader) + sizeof(BMPInfoHeader);
        infoHeader.size = sizeof(BMPInfoHeader);

        if (image.getFormat() == PixelFormat::RGBA32) {
            infoHeader.size = sizeof(BMPInfoHeader) + sizeof(BMPColorHeader);
            fileHeader.offset_data += sizeof(BMPColorHeader);
            infoHeader.compression = 3;
        }

        fileHeader.file_size = fileHeader.offset_data + image.getData().size();
        infoHeader.width = image.getWidth();
        infoHeader.height = image.getHeight();
        infoHeader.planes = 1;
        infoHeader.bit_count = image.getFormat() == PixelFormat::RGBA32 ? 32 : 24;
        infoHeader.size_image = static_cast<uint32_t>(image.getData().size());

        vector<uint8_t> data = image.getData();
        if (infoHeader.bit_count == 32) {
            // Allocate a new vector to store the corrected data
            std::vector<uint8_t> correctedData(data.size());

            // Calculate row size in bytes, including any padding
            size_t rowSize = infoHeader.width * 4;

            for (size_t row = 0; row < infoHeader.height; ++row) {
                // Calculate the start index for the top row and the target bottom row
                size_t sourceRowIndex = row * rowSize;
                size_t targetRowIndex = (infoHeader.height - 1 - row) * rowSize;

                // Copy the row while swapping the red and blue channels
                for (size_t col = 0; col < infoHeader.width; ++col) {
                    size_t sourceIndex = sourceRowIndex + col * 4;
                    size_t targetIndex = targetRowIndex + col * 4;

                    correctedData[targetIndex] = data[sourceIndex + 2];       // Blue
                    correctedData[targetIndex + 1] = data[sourceIndex + 1];   // Green
                    correctedData[targetIndex + 2] = data[sourceIndex];       // Red
                    correctedData[targetIndex + 3] = data[sourceIndex + 3];   
                }
            }

            // Replace original data with the corrected data
            data = std::move(correctedData);

        } else if (infoHeader.bit_count == 24) {
            // Allocate a new vector to store the corrected data
            std::vector<uint8_t> correctedData(data.size());

            // Calculate row size in bytes, including any padding
            size_t rowSize = infoHeader.width * 3;

            for (size_t row = 0; row < infoHeader.height; ++row) {
                // Calculate the start index for the top row and the target bottom row
                size_t sourceRowIndex = row * rowSize;
                size_t targetRowIndex = (infoHeader.height - 1 - row) * rowSize;

                // Copy the row while swapping the red and blue channels
                for (size_t col = 0; col < infoHeader.width; ++col) {
                    size_t sourceIndex = sourceRowIndex + col * 3;
                    size_t targetIndex = targetRowIndex + col * 3;

                    correctedData[targetIndex] = data[sourceIndex + 2];       // Blue
                    correctedData[targetIndex + 1] = data[sourceIndex + 1];   // Green
                    correctedData[targetIndex + 2] = data[sourceIndex];       // Red
                }
            }

            // Replace original data with the corrected data
            data = std::move(correctedData);
        }
       // Write BMP file
        ofstream file(filename, ios::binary);
        if (!file.is_open()) {
            throw runtime_error("Error: Unable to open file " + filename);
        }

        file.write(reinterpret_cast<const char*>(&fileHeader), sizeof(fileHeader));
        file.write(reinterpret_cast<const char*>(&infoHeader), sizeof(infoHeader));
        if (image.getFormat() == PixelFormat::RGBA32) {
            file.write(reinterpret_cast<const char*>(&colorHeader), sizeof(colorHeader));
        }
        file.write(reinterpret_cast<const char*>(data.data()), data.size());
    }


   static void printBMPHeaders(const BMPFileHeader& fileHeader, const BMPInfoHeader& infoHeader, const BMPColorHeader& colorHeader) {
        // Read BMP File Header
        cout << "BMP File Header:" << endl;
        cout << "File Type: " << hex << setw(4) << setfill('0') << fileHeader.file_type << endl;
        cout << "File Size: " << dec << fileHeader.file_size << " bytes" << endl;
        cout << "Reserved1: " << fileHeader.reserved1 << endl;
        cout << "Reserved2: " << fileHeader.reserved2 << endl;
        cout << "Offset Data: " << fileHeader.offset_data << " bytes" << endl;
        cout << endl;

        // Read BMP Info Header
        cout << "BMP Info Header:" << endl;
        cout << "Header Size: " << infoHeader.size << " bytes" << endl;
        cout << "Width: " << infoHeader.width << " pixels" << endl;
        cout << "Height: " << infoHeader.height << " pixels" << endl;
        cout << "Planes: " << infoHeader.planes << endl;
        cout << "Bit Count: " << infoHeader.bit_count << " bits per pixel" << endl;
        cout << "Compression: " << infoHeader.compression << endl;
        cout << "Image Size: " << infoHeader.size_image << " bytes" << endl;
        cout << "X Pixels per Meter: " << infoHeader.x_pixels_per_meter << endl;
        cout << "Y Pixels per Meter: " << infoHeader.y_pixels_per_meter << endl;
        cout << "Colors Used: " << infoHeader.colors_used << endl;
        cout << "Important Colors: " << infoHeader.colors_important << endl;
        cout << endl;

        // Read BMP Color Header
        cout << "BMP Color Header:" << endl;
        cout << "Red Mask: " << hex << colorHeader.red_mask << endl;
        cout << "Green Mask: " << hex << colorHeader.green_mask << endl;
        cout << "Blue Mask: " << hex << colorHeader.blue_mask << endl;
        cout << "Alpha Mask: " << hex << colorHeader.alpha_mask << endl;
        cout << "Color Space Type: " << hex << colorHeader.color_space_type << endl;
        for (int i = 0; i < 16; ++i) {
            cout << "Unused[" << i << "]: " << hex << colorHeader.unused[i] << endl;
        }
        cout << endl;
        cout << dec << endl;
    }

private:

    /**********************************
     * Validates the color header for 32-bit BMP files.
     * @param colorHeader The BMPColorHeader to validate.
     * @throws runtime_error if the color header is invalid.
     **********************************/
    static void validateColorHeader(const BMPColorHeader& colorHeader) {
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

};

            
#endif
