#ifndef BANNER_H
#define BANNER_H

#include "../../src/calculatorbase.h"
#include "../../src/image.h"
#include "../../src/imageutils.h"
#include <sstream>
#include <cassert>

class BannerCalculator: public CalculatorBase {
private:
    const string kOutputGrayscale = "ImageGrayscale";
    const string kOutputBanner = "ImageBanner";
    const string kTagBanner = "ImageBanner";
    const string kTagOverlayStartX = "OverlayStartX";
    const string kTagOverlayStartY = "OverlayStartY";


public:
    BannerCalculator() : CalculatorBase("BannerCalculator") {}

    // Register input/output ports
    unique_ptr<CalculatorContext> registerContext(const shared_ptr<map<string,Packet>>& newSidePacket = make_shared<map<string,Packet>>()) override {
        auto context = make_unique<CalculatorContext>(newSidePacket);
        context->addOutputPort(kOutputBanner, Port()); 
        return context;
    }

    // Enter method (initialization if needed)
    void enter(CalculatorContext* cc, float delta) override {
        //assert(cc->hasOutput(kOutputGrayscale) && "ImageToGrayscaleCalculator requires an ImageOutput port");
    }

    // Process method: Perform the grayscale transformation
    void process(CalculatorContext* cc, float delta) override {
        //Port& inputPort = cc->getInputPort(cc->kTagInput);
        Port& inputPort = cc->getInputPort(kOutputGrayscale);


        // Check if there is data in the input port
        if (inputPort.size() == 0) return;

        // Read the input packet
        Packet inputPacket = inputPort.read();
        Image outputImage = inputPacket.get<Image>();

        // Create a new image for the output
        //Image outputImage = inputImage.clone(); // Clone the input image
        std::vector<uint8_t>& outputData = outputImage.getData();
        Image banner = cc->getSidePacket(kTagBanner).get<Image>();


        // Convert to grayscale
        size_t outputPixelSize = Image::bitsPerPixel(outputImage.getFormat()) / 8;
        if (outputPixelSize < 1) return;
        size_t width = outputImage.getWidth();
        size_t height = outputImage.getHeight();
        size_t outputStride = outputPixelSize * width;
        size_t dataSize = outputData.size();
        /*
        cout << "realStride " << outputStride << endl;
        cout << "width " << width << endl;
        cout << "height " << height << endl;
        cout << "pixel size " << outputPixelSize << endl;
        cout << "data size " << dataSize << endl;
        cout << "bit depth " << Image::bitsPerPixel(outputImage.getFormat()) << endl;
        //assert(realStride == 128 && "real stride should be 128"); 
        
        */


        const int widthBanner = banner.getWidth();
        const int heightBanner = banner.getHeight();
        const int overlayStartX = cc->getSidePacket(kTagOverlayStartX).get<int>();
        const int overlayStartY = cc->getSidePacket(kTagOverlayStartY).get<int>();

        size_t offsetSize = Image::bitsPerPixel(banner.getFormat()) / 8;
        size_t bannerStride = offsetSize * widthBanner;
        vector<uint8_t>& bannerData = banner.getData();

        for (size_t by = 0; by < (size_t)heightBanner; ++by) {
            // Calculate the actual row in the banner data (bottom-to-top)
            size_t bannerRow = heightBanner - 1 - by; 
            size_t oy = overlayStartY + by;
            if (oy >= height) continue;

            for (size_t bx = 0; bx < widthBanner; ++bx) {
                size_t ox = overlayStartX + bx;
                if (ox >= width) continue;

                // Calculate the index in the banner data (BGRA format)
                size_t bannerIndex = (bannerRow * bannerStride) + (bx * offsetSize);
                uint8_t bannerBlue = bannerData[bannerIndex];
                uint8_t bannerGreen = bannerData[bannerIndex + 1];
                uint8_t bannerRed = bannerData[bannerIndex + 2];
                uint8_t bannerAlpha = bannerData[bannerIndex + 3];

                // Calculate the index in the output data (RGBA format)
                size_t outputIndex = (oy * outputStride) + (ox * outputPixelSize);

                if (bannerAlpha != 0) {
                    // Write the swapped (RGBA) data to the output
                    outputData[outputIndex] = bannerRed;
                    outputData[outputIndex + 1] = bannerGreen;
                    outputData[outputIndex + 2] = bannerBlue;
                    outputData[outputIndex + 3] = bannerAlpha;
                }
            }
        }
        
        // Leave alpha channel (if present) unchanged

        //ostringstream error;
        //Image outGray = outputImage.clone();
        //error << "Output port should be " << cc->kTagOutput << endl;
        //assert(cc->hasOutput(cc->kTagOutput) && error.str().c_str());
        cc->getOutputPort(cc->kTagOutput).write(Packet(std::move(outputImage)));
        //cc->getOutputPort(kOutputGrayscale).write(Packet(std::move(outputImage)));

        //cout <<  "CALCULATOR END " << endl;

    }

    // Close method (cleanup if needed)
    void close(CalculatorContext* cc, float delta) override {}
};

#endif
