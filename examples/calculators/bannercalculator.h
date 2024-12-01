/**********************************
 * @file banner.h
 * @brief Defines the BannerCalculator class, which overlays a banner on an image.
 *
 * @details
 * - Reads an input image and overlays a banner image at specified coordinates.
 * - Supports RGBA images for input, output, and banner data.
 * - Utilizes side packets for providing the banner image and overlay positions.
 **********************************/

#ifndef BANNER_H
#define BANNER_H

#include "../../src/calculatorbase.h"
#include "../../src/image.h"
#include "../../src/imageutils.h"
#include <sstream>
#include <cassert>

/**********************************
 * @class BannerCalculator
 * @brief A calculator class for overlaying a banner onto an image.
 **********************************/
class BannerCalculator: public CalculatorBase {
private:
    const string kOutputGrayscale = "ImageGrayscale";  // Output port tag for grayscale image
    const string kOutputBanner = "ImageBanner";        // Output port tag for banner image
    const string kTagBanner = "ImageBanner";           // Side packet tag for the banner image
    const string kTagOverlayStartX = "OverlayStartX";  // Side packet tag for X position
    const string kTagOverlayStartY = "OverlayStartY";  // Side packet tag for Y position

public:
    /**********************************
     * @brief Constructor.
     * Initializes the calculator with its name.
     **********************************/
    BannerCalculator() : CalculatorBase("BannerCalculator") {}

    /**********************************
     * @brief Registers input and output ports.
     * @param newSidePacket Optional map of side packets.
     * @return A unique pointer to the calculator context.
     **********************************/
    unique_ptr<CalculatorContext> registerContext(const shared_ptr<map<string,Packet>>& newSidePacket = make_shared<map<string,Packet>>()) override {
        auto context = make_unique<CalculatorContext>(newSidePacket);
        context->addOutputPort(kOutputBanner, Port()); 
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
     * Overlays the banner image onto the input image.
     * @param cc Pointer to the calculator context.
     * @param delta Time elapsed since the last frame.
     **********************************/
    void process(CalculatorContext* cc, float delta) override {
        // Retrieve the input port
        Port& inputPort = cc->getInputPort(kOutputGrayscale);

        // Check if there is data in the input port
        if (inputPort.size() == 0) return;

        // Read the input packet and extract the image
        Packet inputPacket = inputPort.read();
        Image outputImage = inputPacket.get<Image>();

        // Retrieve the banner image and overlay positions from side packets
        Image banner = cc->getSidePacket(kTagBanner).get<Image>();
        const int overlayStartX = cc->getSidePacket(kTagOverlayStartX).get<int>();
        const int overlayStartY = cc->getSidePacket(kTagOverlayStartY).get<int>();

        // Retrieve image and banner properties
        size_t outputPixelSize = Image::bitsPerPixel(outputImage.getFormat()) / 8;
        if (outputPixelSize < 1) return;

        size_t width = outputImage.getWidth();
        size_t height = outputImage.getHeight();
        size_t outputStride = outputPixelSize * width;
        vector<uint8_t>& outputData = outputImage.getData();

        size_t offsetSize = Image::bitsPerPixel(banner.getFormat()) / 8;
        size_t bannerStride = offsetSize * banner.getWidth();
        vector<uint8_t>& bannerData = banner.getData();

        // Overlay the banner onto the image
        for (size_t by = 0; by < (size_t)banner.getHeight(); ++by) {
            size_t oy = overlayStartY + by;
            if (oy >= height) continue;

            for (size_t bx = 0; bx < banner.getWidth(); ++bx) {
                size_t ox = overlayStartX + bx;
                if (ox >= width) continue;

                // Calculate banner pixel and output pixel indices
                size_t bannerIndex = (by * bannerStride) + (bx * offsetSize);
                uint8_t bannerRed = bannerData[bannerIndex];
                uint8_t bannerGreen = bannerData[bannerIndex + 1];
                uint8_t bannerBlue = bannerData[bannerIndex + 2];
                uint8_t bannerAlpha = bannerData[bannerIndex + 3];

                size_t outputIndex = (oy * outputStride) + (ox * outputPixelSize);

                // Copy banner pixel data if alpha is non-zero
                if (bannerAlpha != 0) {
                    outputData[outputIndex] = bannerRed;
                    outputData[outputIndex + 1] = bannerGreen;
                    outputData[outputIndex + 2] = bannerBlue;
                    outputData[outputIndex + 3] = bannerAlpha;
                }
            }
        }

        // Write the modified image to the output port
        cc->getOutputPort(cc->kTagOutput).write(Packet(std::move(outputImage)));
    }

    /**********************************
     * @brief Close method.
     * Called at the end of the calculator lifecycle for cleanup.
     * @param cc Pointer to the calculator context.
     * @param delta Time elapsed since the last frame.
     **********************************/
    void close(CalculatorContext* cc, float delta) override {}
};

#endif // BANNER_H

