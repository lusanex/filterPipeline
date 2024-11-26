#include <cassert>
#include <iostream>
#include <vector>
#include <fstream>
#include "../src/image.h"
#include "../src/imageutils.h"

using namespace std;

class UtilsTest {
public:
    static void run() {
        cout << "Running UtilsTest..." << endl;

        testReadAndWriteReaderUtilImage();
        //testImageReader1Bit();
        //testImageReader2Bit();
        //testImageReader4Bit();
        //testImageReader8Bit();
        cout << "All tests passed!" << endl;
    }

private:
    static void testImageReader1Bit() {
        cout << "Testing imageReader ..." << endl;



        //createTestBMP("test_1bit.bmp", 15, 15, 1);

        // Read the file back using ImageUtils
        Image img = ImageUtils::imageReader("test_1_bit.bmp");
        cout << "widht " << img.getWidth();

        // Validate the RGB buffer
        vector<uint8_t> rgbData = img.getData();
        assert(rgbData.size() == img.getHeight() * img.getStride());
        cout << "Test passed!" << endl;
    }

    static void testReadAndWriteReaderUtilImage() {
        cout << "\nTesting image reader and writer " << endl;
        Image img = ImageUtils::imageReader("test_1_bit.bmp");
        assert(img.isImageValid() && "Image should be valid");
        assert(img.getColorTable().empty() && "Color table should not be empty");
        assert(!img.getData().empty() && "Data should not be empty");
        assert(img.getData().size() == img.getHeight() * img.getStride() && "Data should be equal to heigh * stride");
        cout << "Testing image writer " << endl;

        ImageUtils::imageWriter("output_test1bit.bmp",img);

        cout << "Test image reader and writer PASSED " << endl;
        
    }

    static void testImageReader2Bit() {
        cout << "Testing imageReader with 2-bit indexed grayscale..." << endl;
       // Read the file back using ImageUtils
          createTestBMP("test_2bit.bmp", 15, 15, 2);
        Image img = ImageUtils::imageReader("test_2bit.bmp");

        // Validate the RGB buffer
        vector<uint8_t> rgbData = img.getData();
        assert(rgbData.size() == img.getWidth() * img.getHeight() * 3);
        assert(rgbData[0] == 255 && rgbData[1] == 255 && rgbData[2] == 255); // White
        assert(rgbData[3] == 0 && rgbData[4] == 0 && rgbData[5] == 0);       // Black
        assert(rgbData[6] == 170 && rgbData[7] == 170 && rgbData[8] == 170); // Light Gray
        assert(rgbData[9] == 85 && rgbData[10] == 85 && rgbData[11] == 85);  // Dark Gray

        cout << "2-bit test passed!" << endl;
    }

    static void testImageReader4Bit() {
        // Read the file back using ImageUtils
        createTestBMP("test_4bit.bmp", 15, 15, 4);
        Image img = ImageUtils::imageReader("test_4bit.bmp");

        // Validate the RGB buffer
        vector<uint8_t> rgbData = img.getData();
        assert(rgbData.size() == img.getWidth() * img.getHeight() * 3);
        assert(rgbData[0] == 17 && rgbData[1] == 17 && rgbData[2] == 17); // Index 1
        assert(rgbData[3] == 34 && rgbData[4] == 34 && rgbData[5] == 34); // Index 2

        cout << "4-bit test passed!" << endl;
    }

    static void testImageReader8Bit() {
        cout << "Testing imageReader with 8-bit indexed grayscale..." << endl;
       // Read the file back using ImageUtils
       createTestBMP("test_8bit.bmp", 15, 15, 8);
        Image img = ImageUtils::imageReader("test_8bit.bmp");

        // Validate the RGB buffer
        vector<uint8_t> rgbData = img.getData();
        assert(rgbData.size() == img.getWidth() * img.getHeight() * 3);
        assert(rgbData[0] == 1 && rgbData[1] == 1 && rgbData[2] == 1); // Index 1
        assert(rgbData[3] == 2 && rgbData[4] == 2 && rgbData[5] == 2); // Index 2
        assert(rgbData[6] == 3 && rgbData[7] == 3 && rgbData[8] == 3); // Index 3
        assert(rgbData[9] == 4 && rgbData[10] == 4 && rgbData[11] == 4); // Index 4

        cout << "8-bit test passed!" << endl;
    }
    static void createTestBMP(const string& fileName, int32_t width, int32_t height, int32_t bitDepth) {
        const int kHeaderSize = 54;
        const int kColorTableSize = (bitDepth <= 8) ? (1 << bitDepth) * 4 : 0; // 4 bytes per color entry
        int32_t stride = ((width * bitDepth + 31) / 32) * 4; // Align stride to 4 bytes
        int32_t imageSize = stride * height;

        // BMP header
        vector<uint8_t> header(kHeaderSize, 0);
        header[0] = 'B';
        header[1] = 'M';
        int32_t fileSize = kHeaderSize + kColorTableSize + imageSize;
        // File size
        {
            uint8_t* fileSizePtr = reinterpret_cast<uint8_t*>(&fileSize);
            header[2] = fileSizePtr[0];
            header[3] = fileSizePtr[1];
            header[4] = fileSizePtr[2];
            header[5] = fileSizePtr[3];
        }
        header[10] = kHeaderSize + kColorTableSize; // Offset to pixel data
        header[14] = 40; // DIB header size
        // Width
        {
            uint8_t* widthPtr = reinterpret_cast<uint8_t*>(&width);
            header[18] = widthPtr[0];
            header[19] = widthPtr[1];
            header[20] = widthPtr[2];
            header[21] = widthPtr[3];
        }
        // Height
        {
            uint8_t* heightPtr = reinterpret_cast<uint8_t*>(&height);
            header[22] = heightPtr[0];
            header[23] = heightPtr[1];
            header[24] = heightPtr[2];
            header[25] = heightPtr[3];
        }
        header[26] = 1; // Planes
        // Bit depth
        {
            uint8_t* bitDepthPtr = reinterpret_cast<uint8_t*>(&bitDepth);
            header[28] = bitDepthPtr[0];
            header[29] = bitDepthPtr[1];
        }
        // Image size
        {
            uint8_t* imageSizePtr = reinterpret_cast<uint8_t*>(&imageSize);
            header[34] = imageSizePtr[0];
            header[35] = imageSizePtr[1];
            header[36] = imageSizePtr[2];
            header[37] = imageSizePtr[3];
        }

        // Color table (for bit depths <= 8)
        vector<uint8_t> colorTable(kColorTableSize, 0);
        if (bitDepth <= 8) {
            for (int i = 0; i < (1 << bitDepth); ++i) {
                colorTable[i * 4 + 0] = i * (255 / ((1 << bitDepth) - 1)); // Blue
                colorTable[i * 4 + 1] = i * (255 / ((1 << bitDepth) - 1)); // Green
                colorTable[i * 4 + 2] = i * (255 / ((1 << bitDepth) - 1)); // Red
                colorTable[i * 4 + 3] = 0; // Reserved
            }
        }

        // Pixel data
        vector<uint8_t> pixelData(imageSize, 0);
        srand(static_cast<unsigned>(time(0))); // Seed for random number generation
        for (int32_t y = 0; y < height; ++y) {
            for (int32_t x = 0; x < width; ++x) {
                int pixelIndex = y * stride + x * bitDepth / 8;

                if (bitDepth == 1) {
                    int bitOffset = x % 8;
                    if (rand() % 2 == 1) {
                        pixelData[pixelIndex] |= (1 << (7 - bitOffset));
                    }
                } else if (bitDepth == 2) {
                    int shift = (3 - (x % 4)) * 2;
                    pixelData[pixelIndex] |= (rand() % 4) << shift;
                } else if (bitDepth == 4) {
                    int shift = (1 - (x % 2)) * 4;
                    pixelData[pixelIndex] |= (rand() % 16) << shift;
                } else if (bitDepth == 8) {
                    pixelData[pixelIndex] = rand() % 256;
                } else if (bitDepth == 24 || bitDepth == 32) {
                    for (int i = 0; i < bitDepth / 8; ++i) {
                        pixelData[pixelIndex + i] = rand() % 256;
                    }
                }
            }
        }

        // Write BMP file
        ofstream outputFile(fileName, ios::binary);
        if (!outputFile.is_open()) {
            throw runtime_error("Failed to create BMP file: " + fileName);
        }
        outputFile.write(reinterpret_cast<char*>(header.data()), header.size());
        if (kColorTableSize > 0) {
            outputFile.write(reinterpret_cast<char*>(colorTable.data()), colorTable.size());
        }
        outputFile.write(reinterpret_cast<char*>(pixelData.data()), pixelData.size());
        outputFile.close();

        cout << "Created BMP file: " << fileName << endl;
    }

};


int main() {
    UtilsTest::run();
    return 0;
}

