#ifndef GRAYSCALE_CALCULATOR_H
#define GRAY

#include "calculatorbase.h"
#include "image.h"
#include <cassert>

class ImageToGrayscaleCalculator : public CalculatorBase {
private:
    const string kOutputGrayscale = "ImageGrayscale";

public:
    ImageToGrayscaleCalculator() : CalculatorBase("ImageToGrayscaleCalculator") {}

    // Register input/output ports
    unique_ptr<CalculatorContext> registerContext() override {
        auto context = make_unique<CalculatorContext>();
        context->addOutputPort(kOutputGrayscale, Port()); 
        return context;
    }

    // Enter method (initialization if needed)
    void enter(CalculatorContext* cc, float delta) override {
        //assert(cc->hasOutput(kOutputGrayscale) && "ImageToGrayscaleCalculator requires an ImageOutput port");
    }

    // Process method: Perform the grayscale transformation
    void process(CalculatorContext* cc, float delta) override {
        Port& inputPort = cc->getInputPort(cc->kTagInput);

        // Check if there is data in the input port
        if (inputPort.size() == 0) return;

        // Read the input packet
        Packet inputPacket = inputPort.read();
        const Image& inputImage = inputPacket.get<Image>();

        // Create a new image for the output
        Image outputImage = inputImage.clone(); // Clone the input image
        std::vector<uint8_t>& pixelData = outputImage.getData();

        // Convert to grayscale
        if (outputImage.getFormat() == PixelFormat::RGB24 || outputImage.getFormat() == PixelFormat::RGBA32) {
            for (size_t i = 0; i < pixelData.size(); i += (outputImage.getFormat() == PixelFormat::RGBA32 ? 4 : 3)) {
                // Calculate grayscale value as the average of R, G, and B
                uint8_t gray = static_cast<uint8_t>(0.299 * pixelData[i] + 
                                                    0.587 * pixelData[i + 1] + 
                                                    0.114 * pixelData[i + 2]);
                pixelData[i] = gray;       // Red channel
                pixelData[i + 1] = gray;   // Green channel
                pixelData[i + 2] = gray;   // Blue channel
                // Keep alpha unchanged for RGBA
            }
        }

        // Write the modified image to the output port
        cc->getOutputPort(kOutputImageTag).write(Packet(std::move(outputImage)));
    }

    // Close method (cleanup if needed)
    void close(CalculatorContext* cc, float delta) override {}
};

#endif
