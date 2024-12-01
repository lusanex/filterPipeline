/**********************************
 * @file grayscalecalculator.h
 * @brief Defines the GrayscaleCalculator 
 * class, which converts images to grayscale.
 *
 * @details
 * - Processes input image data and computes grayscale values.
 * - Uses a weighted formula to calculate grayscale based on RGB values.
 * - Outputs the grayscale image through a specified port.
 *   https://en.wikipedia.org/wiki/Grayscale
 **********************************/

#ifndef GRAYSCALE_CALCULATOR_H
#define GRAYSCALE_CALCULATOR_H

#include "../../src/calculatorbase.h"
#include "../../src/image.h"
#include "../../src/imageutils.h"
#include "../../src/packet.h"
#include <sstream>
#include <cassert>

/**********************************
 * @class GrayscaleCalculator
 * @brief A calculator class for converting images to grayscale.
 **********************************/
class GrayscaleCalculator : public CalculatorBase {
private:
    const string kOutputGrayscale = "ImageGrayscale"; // Output port tag for grayscale image
    const string kOutputDither = "ImageDither";       // Output port tag for dithered image
    const string kOutputPixel = "ImagePixel";         // Output port tag for pixelated image

public:
    /**********************************
     * @brief Constructor.
     * Initializes the calculator with its name.
     **********************************/
    GrayscaleCalculator() : CalculatorBase("GrayscaleCalculator") {}

    /**********************************
     * @brief Registers input and output ports.
     * @param newSidePacket Optional map of side packets.
     * @return A unique pointer to the calculator context.
     **********************************/
    unique_ptr<CalculatorContext> registerContext(const shared_ptr<map<string, Packet>>& newSidePacket = make_shared<map<string, Packet>>()) override {
        auto context = make_unique<CalculatorContext>(newSidePacket);
        context->addOutputPort(kOutputGrayscale, Port());
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
     * Converts the input image to grayscale.
     * @param cc Pointer to the calculator context.
     * @param delta Time elapsed since the last frame.
     **********************************/
    void process(CalculatorContext* cc, float delta) override {
        Port& inputPort = cc->getInputPort(kOutputDither);

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


        // Convert to grayscale
        for (size_t x = 0; x < width; ++x) {
            for (size_t y = 0; y < height; ++y) {
                size_t i = (y * realStride) + (x * pixelSize);

                uint8_t red = pixelData[i];
                uint8_t green = pixelData[i + 1];
                uint8_t blue = pixelData[i + 2];
                uint8_t alpha = pixelData[i + 3];

                uint8_t gray = static_cast<uint8_t>(
                    0.2126 * red +
                    0.7152 * green +
                    0.0722 * blue
                );

                pixelData[i] = gray;
                pixelData[i + 1] = gray;
                pixelData[i + 2] = gray;
                pixelData[i + 3] = alpha;
            }
        }

        // Write the grayscale image to the output port
        cc->getOutputPort(kOutputGrayscale).write(Packet(std::move(outputImage)));
    }

    /**********************************
     * @brief Close method.
     * Called at the end of the calculator lifecycle for cleanup.
     * @param cc Pointer to the calculator context.
     * @param delta Time elapsed since the last frame.
     **********************************/
    void close(CalculatorContext* cc, float delta) override {}

};

#endif // GRAYSCALE_CALCULATOR_H

