/**********************************
 * @file dithercalculator.h
 * @brief Defines the DitherCalculator class, which applies dithering to images.
 *
 * @details
 * - Supports dithering with configurable Bayer matrices.
 * - Adjustable color levels and spread for red, green, and blue channels.
 * - Processes image data to create a dithered output image.
 *   https://en.wikipedia.org/wiki/Dither
 **********************************/

#ifndef DITHER_CALCULATOR_H
#define DITHER_CALCULATOR_H

#include "../../src/calculatorbase.h"
#include "../../src/image.h"
#include "../../src/imageutils.h"
#include <sstream>
#include <cassert>
#include <cmath>

/**********************************
 * @class DitherCalculator
 * @brief A calculator class for applying dithering effects to images.
 **********************************/
class DitherCalculator : public CalculatorBase {
private:
    const string kInputGrayscale = "ImageGrayscale";  // Input port tag for grayscale image
    const string kOutputDither = "ImageDither";       // Output port tag for dithered image
    const string kOutputPixel = "ImagePixel";         // Output port tag for pixelated image

    // Bayer matrices for dithering
    const int bayer2[2 * 2] = {
        0, 2,
        3, 1
    };

    const int bayer4[4 * 4] = {
        0, 8, 2, 10,
        12, 4, 14, 6,
        3, 11, 1, 9,
        15, 7, 13, 5
    };

    const int bayer8[8 * 8] = {
        0, 32, 8, 40, 2, 34, 10, 42,
        48, 16, 56, 24, 50, 18, 58, 26,
        12, 44, 4, 36, 14, 46, 6, 38,
        60, 28, 52, 20, 62, 30, 54, 22,
        3, 35, 11, 43, 1, 33, 9, 41,
        51, 19, 59, 27, 49, 17, 57, 25,
        15, 47, 7, 39, 13, 45, 5, 37,
        63, 31, 55, 23, 61, 29, 53, 21
    };

    const string kRedLevels = "redCount";    // Side packet tag for red channel levels
    const string kGreenLevels = "greenCount"; // Side packet tag for green channel levels
    const string kBlueLevels = "blueCount";   // Side packet tag for blue channel levels
    const string kSpread = "spread";          // Side packet tag for dithering spread
    const string kBayerLevel = "bayerLevel";  // Side packet tag for Bayer matrix level

    /**********************************
     * @brief Retrieves a Bayer matrix value based on level.
     * @param x X-coordinate in the image.
     * @param y Y-coordinate in the image.
     * @param level Bayer matrix level (0 for 2x2, 1 for 4x4, 2 for 8x8).
     * @return Normalized Bayer value.
     **********************************/
    float getBayerValue(int x, int y, int level) {
        if (level == 0) {
            return float(bayer2[(x % 2) + (y % 2) * 2]) / 4.0f - 0.5f;
        } else if (level == 1) {
            return float(bayer4[(x % 4) + (y % 4) * 4]) / 16.0f - 0.5f;
        } else {
            return float(bayer8[(x % 8) + (y % 8) * 8]) / 64.0f - 0.5f;
        }
    }

public:
    /**********************************
     * @brief Constructor.
     * Initializes the calculator with its name.
     **********************************/
    DitherCalculator() : CalculatorBase("DitherCalculator") {}

    /**********************************
     * @brief Registers input and output ports.
     * @param newSidePacket Optional map of side packets.
     * @return A unique pointer to the calculator context.
     **********************************/
    unique_ptr<CalculatorContext> registerContext(const shared_ptr<map<string, Packet>>& newSidePacket = make_shared<map<string, Packet>>()) override {
        auto context = make_unique<CalculatorContext>(newSidePacket);
        context->addOutputPort(kOutputDither, Port()); 
        return context;
    }

    /**********************************
     * @brief Enter method.
     * Called at the start of the calculator lifecycle for initialization.
     * @param cc Pointer to the calculator context.
     * @param delta Time elapsed since the last frame.
     **********************************/
    void enter(CalculatorContext* cc, float delta) override {}

    /**********************************
     * @brief Process method.
     * Applies dithering to the input image.
     * @param cc Pointer to the calculator context.
     * @param delta Time elapsed since the last frame.
     **********************************/
    void process(CalculatorContext* cc, float delta) override {
        Port& inputPort = cc->getInputPort(kOutputPixel);

        // Retrieve side packet values
        const int redLevels = cc->getSidePacket(kRedLevels).get<int>();
        const int greenLevels = cc->getSidePacket(kGreenLevels).get<int>();
        const int blueLevels = cc->getSidePacket(kBlueLevels).get<int>();
        const int spread = cc->getSidePacket(kSpread).get<int>();
        const int bayerLevel = cc->getSidePacket(kBayerLevel).get<int>();

        // Check if there is data in the input port
        if (inputPort.size() == 0) return;

        // Read the input packet and extract the image
        Packet inputPacket = inputPort.read();
        Image outputImage = inputPacket.get<Image>();
        std::vector<uint8_t>& pixelData = outputImage.getData();

        size_t pixelSize = Image::bitsPerPixel(outputImage.getFormat()) / 8;
        if (pixelSize < 1) return;

        size_t width = outputImage.getWidth();
        size_t height = outputImage.getHeight();
        size_t realStride = pixelSize * width;
        size_t dataSize = pixelData.size();

        // Apply dithering
        for (size_t i = 0; i < dataSize; i += pixelSize) {
            size_t row = i / realStride;
            size_t col = (i % realStride) / pixelSize;

            if (row >= height) break;

            uint8_t red = pixelData[i];
            uint8_t green = pixelData[i + 1];
            uint8_t blue = pixelData[i + 2];
            uint8_t alpha = pixelData[i + 3];
            float bayerValue = getBayerValue(row, col, bayerLevel);

            uint8_t dr = static_cast<uint8_t>(
                (floor((redLevels - 1.0) * (red / 255.0) + spread * (bayerValue + 0.5)) / (redLevels - 1.0)) * 255.0);
            dr = clamp(dr, 0, 255);

            uint8_t dg = static_cast<uint8_t>(
                (floor((greenLevels - 1.0) * (green / 255.0) + spread * (bayerValue + 0.5)) / (greenLevels - 1.0)) * 255.0);
            dg = clamp(dg, 0, 255);

            uint8_t db = static_cast<uint8_t>(
                (floor((blueLevels - 1.0) * (blue / 255.0) + spread * (bayerValue + 0.5)) / (blueLevels - 1.0)) * 255.0);
            db = clamp(db, 0, 255);

            pixelData[i] = dr;
            pixelData[i + 1] = dg;
            pixelData[i + 2] = db;
            pixelData[i + 3] = alpha;
        }

        // Write the dithered image to the output port
        cc->getOutputPort(kOutputDither).write(Packet(std::move(outputImage)));
    }

    /**********************************
     * @brief Close method.
     * Called at the end of the calculator lifecycle for cleanup.
     * @param cc Pointer to the calculator context.
     * @param delta Time elapsed since the last frame.
     **********************************/
    void close(CalculatorContext* cc, float delta) override {}

private:
    /**********************************
     * @brief Clamps a value between a minimum and maximum.
     * @param value The value to clamp.
     * @param min The minimum allowed value.
     * @param max The maximum allowed value.
     * @return The clamped value.
     **********************************/
    uint8_t clamp(uint8_t value, uint8_t min, uint8_t max) {
        if (value < min) return min;
        if (value > max) return max;
        return value;
    }
};

#endif // DITHER_CALCULATOR_H

