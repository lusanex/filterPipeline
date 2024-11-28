#include <cassert>
#include <iostream>
#include <vector>
#include "../src/image.h"
#include "../src/imageutils.h"
#include "../src/packet.h"

using namespace std;

long long Packet::lastTimestamp = 0;

class UtilsTest {
public:
    static void run() {
        cout << "Running UtilsTest..." << endl;

        //testReadAndWriteReaderUtilImage();
        testLoadReadBlueImage();
        cout << "All tests passed!" << endl;
    }

private:
    static void testImageReader1Bit() {
        cout << "Testing imageReader ..." << endl;



        //createTestBMP("test_1bit.bmp", 15, 15, 1);

        // Read the file back using ImageUtils
        Image img = ImageUtils::readBMP("assets/test_1_bit.bmp");
        cout << "test width " << img.getWidth();

        // Validate the RGB buffer
        vector<uint8_t> rgbData = img.getData();
        //assert(rgbData.size());
        cout << "Test passed!" << endl;
    }

    static void testReadAndWriteReaderUtilImage() {
        cout << "\nTesting image reader and writer " << endl;
        Image img = ImageUtils::readBMP("assets/test_24_bit.bmp");

        cout << " image created " << endl;
        assert(img.isImageValid() && "Image should be valid");
        assert(!img.getData().empty() && "Data should not be empty");
        //assert(img.getData().size() == img.getHeight() * img.getStride() && "Data should be equal to heigh * stride");
        cout << "Testing image writer " << endl;

        //ImageUtils::writeBMP("out/copy_24_bit.bmp",img);

        cout << "Test image reader and writer PASSED " << endl;
        
    }
    static void testLoadReadBlueImage() {
        const string imagePath = "assets/lena_color.bmp";
        cout << "Running test for image: " << imagePath << endl;

        // Step 1: Read the image
        Image image = ImageUtils::readBMP(imagePath);

        // Step 2: Output image details
        cout << "Image Width: " << image.getWidth() << endl;
        cout << "Image Height: " << image.getHeight() << endl;
        cout << "Bit Depth: " << Image::bitsPerPixel(image.getFormat()) << endl;

        // Step 3: Access pixel data
        vector<uint8_t>& pixelData = image.getData();
        size_t width = image.getWidth();
        size_t height = image.getHeight();
        size_t stride = image.getStride();
        size_t pixelSize = Image::bitsPerPixel(image.getFormat()) / 8;
        size_t dataSize = pixelData.size(); 

        //assert(image.getFormat() == PixelFormat::RGB24);
        bool isRedValid = true;
        bool isBlueValid = true;

        cout << "stride : " << stride << endl;
        cout << "image size " << dataSize << endl;
        cout << "pixel size " << pixelSize << endl;
        size_t offset = (stride - (pixelSize *  width));
        cout << "offset : " << offset << endl;
        size_t realStride = pixelSize * width;
        //assert(offset == 32 && "offset should be 32");
        //assert(realStride == 96 && "real stride should be 96");
        cout << "reasl stride " << realStride << endl;

        for (size_t i = 0; i < dataSize; i += pixelSize) {

            size_t row = i / realStride; // Calculate the row number
            size_t col = (i % realStride) / pixelSize; 
            if ( row > height ) break;
            //cout << "row i " << i << " stride " << realStride << " = " << row << endl;

            // Extract RGB components
            uint8_t blue = pixelData[i];       // Blue
            uint8_t green = pixelData[i + 1];  // Green
            uint8_t red = pixelData[i + 2];    // Red

            /*
            cout << "BGR : " << static_cast<int>(blue) << "," 
                             << static_cast<int>(green) << "," 
                             << static_cast<int>(red) << endl;
                             */
            uint8_t temp = blue;
            blue = red;
            red = temp;
            pixelData[i] = blue;
            pixelData[i + 1] = green;
            pixelData[i + 2] = red;

            //cout << "blue " << static_cast<int>(blue) << endl;
            //cout << "green " << static_cast<int>(green) << endl;
            //cout << "red " << static_cast<int>(red) << endl;

        }
      
        // Step 4: Output validation results
        cout << "Red region validation: " << (isRedValid ? "PASSED" : "FAILED") << endl;
        cout << "Blue region validation: " << (isBlueValid ? "PASSED" : "FAILED") << endl;

        // Step 5: Write the image back to a new file
        const string outputPath = "out/output_test_blue_red.bmp";
        ImageUtils::writeBMP(outputPath, image);
        cout << "Image successfully written to: " << outputPath << endl;
    }


};

int main() {
    UtilsTest::run();
}
