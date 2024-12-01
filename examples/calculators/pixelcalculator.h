/**********************************
 * @file pixelshapecalculator.h
 * @brief Defines the PixelShapeCalculator class, 
 * which applies pixelation effects on images.
 *
 * @details
 * - Supports two pixel shapes (e.g., square, triangle).
 * - Processes image data by grouping pixels into blocks and reassigning their values.
 * - Utilizes side packets for pixel size and shape settings.
 **********************************/

#ifndef PIXEL_SHAPE_CALCULATOR_H
#define PIXEL_SHAPE_CALCULATOR_H

#include "../../src/calculatorbase.h"
#include "../../src/image.h"
#include "../../src/imageutils.h"
#include <sstream>
#include <cmath>
#include <cassert>

/**********************************
 * @class PixelShapeCalculator
 * @brief A calculator class for applying pixelation effects to images.
 **********************************/
class PixelShapeCalculator : public CalculatorBase {
private:
    const string kOutputPixel = "ImagePixel";         // Output port tag for pixelated image
    const string kOutputGrayscale = "ImageGrayscale"; // Output port tag for grayscale image
    const string kPixelSize = "pixelSize";            // Side packet tag for pixel size
    const string kPixelShape = "pixeShape";           // Side packet tag for pixel shape

public:
    /**********************************
     * @brief Constructor.
     * Initializes the calculator with its name.
     **********************************/
    PixelShapeCalculator() : CalculatorBase("PixelShapeCalculator") {}

    /**********************************
     * @brief Registers input and output ports.
     * @param newSidePacket Optional map of side packets.
     * @return A unique pointer to the calculator context.
     **********************************/
    unique_ptr<CalculatorContext> registerContext(const shared_ptr<map<string, Packet>>& newSidePacket = make_shared<map<string, Packet>>()) override {
        auto context = make_unique<CalculatorContext>(newSidePacket);
        context->addOutputPort(kOutputPixel, Port()); 
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
     * Applies pixelation effects to the input image.
     * @param cc Pointer to the calculator context.
     * @param delta Time elapsed since the last frame.
     **********************************/
    void process(CalculatorContext* cc, float delta) override {
        // Retrieve the input port
        Port& inputPort = cc->getInputPort(cc->kTagInput);

        // Check if there is data in the input port
        if (inputPort.size() == 0) return;

        // Read the input packet and extract the image
        Packet inputPacket = inputPort.read();
        Image outputImage = inputPacket.get<Image>();
        int pixSizeFilter = cc->getSidePacket(kPixelSize).get<int>();
        int pixelShape = cc->getSidePacket(kPixelShape).get<int>();

        // Access image data
        std::vector<uint8_t>& pixelData = outputImage.getData();
        size_t pixelSize = Image::bitsPerPixel(outputImage.getFormat()) / 8;
        if (pixelSize < 1) return;

        size_t width = outputImage.getWidth();
        size_t height = outputImage.getHeight();
        size_t realStride = pixelSize * width;

        // Apply pixelation
        for (size_t x = 0; x < width; ++x) {
            for (size_t y = 0; y < height; ++y) {
                size_t i = (y * realStride) + (x * pixelSize); // Calculate 1D index

                size_t pixelatedUV[2] {x, y};
                size_t imageSize[2] {width, height};

                // Apply the appropriate pixelation function
                if (pixelShape == 0) {
                    getSquareUV(pixelatedUV, pixSizeFilter, imageSize);
                } else if (pixelShape == 1) {
                    getTriangleUV(pixelatedUV, pixSizeFilter, imageSize);
                } else {
                    getSquareUV(pixelatedUV, pixSizeFilter, imageSize);
                }

                size_t newRow = pixelatedUV[1];
                size_t newCol = pixelatedUV[0];

                if (newRow >= height || newCol >= width) continue;
                size_t newIndex = (newRow * realStride) + (newCol * pixelSize);

                // Copy pixel data
                uint8_t red = pixelData[newIndex];
                uint8_t green = pixelData[newIndex + 1];
                uint8_t blue = pixelData[newIndex + 2];
                uint8_t alpha = pixelData[newIndex + 3];

                pixelData[i] = red;
                pixelData[i + 1] = green;
                pixelData[i + 2] = blue;
                pixelData[i + 3] = alpha;
            }
        }

        // Write the processed image to the output port
        cc->getOutputPort(kOutputPixel).write(Packet(std::move(outputImage)));
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
     * @brief Applies square pixelation to UV coordinates.
     * @param uv The UV coordinates to modify.
     * @param pixelSize The size of the square pixel block.
     * @param imageSize The size of the image.
     **********************************/
    void getSquareUV(size_t uv[2], float pixelSize, size_t imageSize[2]) {
        uv[0] = floor(uv[0] / pixelSize) * pixelSize;
        uv[1] = floor(uv[1] / pixelSize) * pixelSize;
        if (uv[0] >= imageSize[0]) uv[0] = imageSize[0] - 1;
        if (uv[1] >= imageSize[1]) uv[1] = imageSize[1] - 1;
    }

    /**********************************
     * @brief Applies triangle pixelation to UV coordinates.
     * @param uv The UV coordinates to modify.
     * @param pixelSize The size of the triangle pixel block.
     * @param imageSize The size of the image.
     **********************************/
    void getTriangleUV(size_t uv[2], size_t pixelSize, size_t imageSize[2]) {
        float blockCoords[2] = {
            static_cast<float>(uv[0]) / pixelSize,
            static_cast<float>(uv[1]) / pixelSize
        };
        size_t blockFloor[2] = {
            static_cast<size_t>(floor(blockCoords[0])),
            static_cast<size_t>(floor(blockCoords[1])),
        };

        float isUpperTriangle = (fmod(blockCoords[0], 1.0f) + fmod(blockCoords[1], 1.0f)) < 1.0f ? 0.0f : 1.0f;

        int triangleAnchor[2];
        if (isUpperTriangle == 1.0f) {
            triangleAnchor[0] = blockCoords[0] * pixelSize;
            triangleAnchor[1] = blockCoords[1] * pixelSize;
        } else {
            triangleAnchor[0] = (blockFloor[0] + 1) * pixelSize - 1;
            triangleAnchor[1] = (blockFloor[1] + 1) * pixelSize - 1;
        }
        uv[0] = min(triangleAnchor[0], (int)imageSize[0] - 1);
        uv[1] = min(triangleAnchor[1], (int)imageSize[1] - 1);
    }
};

#endif // PIXEL_SHAPE_CALCULATOR_H

