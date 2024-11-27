#ifndef IMAGE_TEST_H
#define IMAGE_TEST_H

#include "../src/image.h"
#include <iostream>
#include <cassert>

class ImageTest {
public:
    static void run() {
        cout << "Running Image Tests...\n";

        testConstructor();
        testSetDataAndGetData();
        testClone();
        testInvalidImage();

        cout << "All tests passed!\n";
    }

private:
    static void testConstructor() {
        cout << "\nTesting Image constructor..." << endl;

        int32_t width = 10;
        int32_t height = 10;
        PixelFormat format = PixelFormat::RGB24;

        // Initialize the image
        Image img(width, height, format);

        // Assertions to validate constructor
        assert(!img.isImageValid() && "Image should initially be invalid");
        cout << "IMAGE W: " << img.getWidth() << endl;
        assert(img.getHeight() == height && "Image height mismatch");
        assert(img.getFormat() == format && "Image format mismatch");

        cout << "Constructor test PASSED!" << endl;
    }

    static void testSetDataAndGetData() {
        cout << "Testing setData and getData..." << endl;

        int32_t width = 5;
        int32_t height = 5;
        PixelFormat format = PixelFormat::RGBA32;

        // Initialize the image
        Image img(width, height, format);

        int32_t stride = img.getStride();
        vector<uint8_t> dummyData(stride * height, 255);

        // Set data and validate
        img.setData(dummyData);
        assert(img.isImageValid() && "Image should be valid after setting data");
        assert(img.getStride() == stride && "Stride mismatch");

        vector<uint8_t>& data = img.getData();
        for (size_t i = 0; i < data.size(); i++) {
            if (data[i] != 255) {
                cerr << "Data mismatch at index " << i << endl;
                assert(false);
            }
        }

        cout << "setData and getData test PASSED!" << endl;
    }

    static void testClone() {
        cout << "Testing clone functionality..." << endl;

        int32_t width = 5;
        int32_t height = 5;
        PixelFormat format = PixelFormat::RGBA32;

        Image original(width, height, format);
        vector<uint8_t> dummyData(original.getStride() * height, 128);
        original.setData(dummyData);

        // Clone the image
        Image clone = original.clone();

        // Validate clone
        assert(clone.getWidth() == original.getWidth() && "Clone width mismatch");
        assert(clone.getHeight() == original.getHeight() && "Clone height mismatch");
        assert(clone.getFormat() == original.getFormat() && "Clone format mismatch");
        assert(clone.getStride() == original.getStride() && "Clone stride mismatch");

        const vector<uint8_t>& originalData = original.getData();
        const vector<uint8_t>& cloneData = clone.getData();
        for (size_t i = 0; i < originalData.size(); i++) {
            if (originalData[i] != cloneData[i]) {
                cerr << "Clone data mismatch at index " << i << endl;
                assert(false);
            }
        }

        cout << "Clone test PASSED!" << endl;
    }

    static void testInvalidImage() {
        cout << "Testing invalid image behavior..." << endl;

        int32_t width = 0;
        int32_t height = 0;
        PixelFormat format = PixelFormat::UNKNOWN;

        try {
            Image img(width, height, format);
            assert(false && "Image with invalid dimensions should throw an exception");
        } catch (const ImageException& e) {
            cout << "Caught expected exception: " << e.what() << endl;
        }

        cout << "Invalid image test PASSED!" << endl;
    }

};

#endif // IMAGE_TEST_H

