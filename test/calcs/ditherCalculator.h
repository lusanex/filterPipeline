#ifndef DITHER_CALCULATOR_H
#define DITHER_CALCULATOR_H

#include "../../src/calculatorbase.h"
#include "../../src/image.h"
#include "../../src/imageutils.h"
#include <sstream>
#include <cassert>
#include <cmath>

class DitherCalculator : public CalculatorBase {
private:
    const string kInputGrayscale = "ImageGrayscale";
    const string kOutputDither = "ImageDither";
    const string kOutputPixel = "ImagePixel";
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

    // Function to retrieve Bayer matrix value based on level
    float getBayerValue(int x, int y, int level) {
        if (level == 0) {
            return float(bayer2[(x % 2) + (y % 2) * 2]) / 4.0f - 0.5f;
        } else if (level == 1) {
            return float(bayer4[(x % 4) + (y % 4) * 4]) / 16.0f - 0.5f;
        } else {
            return float(bayer8[(x % 8) + (y % 8) * 8]) / 64.0f - 0.5f;
        }
    }

        const string kRedLevels = "redCount";
        const string kGreenLevels = "greenCount";
        const string kBlueLevels = "blueCount";
        const string kSpread = "spread";
        const string kBayerLevel = "bayerLevel";


    public:
        DitherCalculator() : CalculatorBase("DitherCalculator") {}

        // Register input/output ports
        unique_ptr<CalculatorContext> registerContext(
                const shared_ptr<map<string,Packet>>& newSidePacket = 
                make_shared<map<string,Packet>>()) override {
            auto context = make_unique<CalculatorContext>(newSidePacket);
            context->addOutputPort(kOutputDither, Port()); 
            return context;
        }

        // Enter method (initialization if needed)
        void enter(CalculatorContext* cc, float delta) override {
            //assert(cc->hasOutput(kOutputGrayscale) && "ImageToGrayscaleCalculator requires an ImageOutput port");
        }

        // Process method: Perform the grayscale transformation
        void process(CalculatorContext* cc, float delta) override {
            //cout << "CALCULATOR  " << endl;
            Port& inputPort = cc->getInputPort(kOutputPixel);

            const int redLevels = cc->getSidePacket(kRedLevels).get<int>();
            const int greenLevels = cc->getSidePacket(kGreenLevels).get<int>();
            const int blueLevels = cc->getSidePacket(kBlueLevels).get<int>();
            const int spread = cc->getSidePacket(kSpread).get<int>();
            const int bayerLevel = cc->getSidePacket(kBayerLevel).get<int>();

            // Check if there is data in the input port
            if (inputPort.size() == 0) return;

            // Read the input packet
            Packet inputPacket = inputPort.read();
            Image outputImage = inputPacket.get<Image>();

            // Create a new image for the output
            //Image outputImage = inputImage.clone(); // Clone the input image
            std::vector<uint8_t>& pixelData = outputImage.getData();


            // Convert to grayscale
            size_t pixelSize = Image::bitsPerPixel(outputImage.getFormat()) / 8;
            if (pixelSize < 1) return;
            size_t rowStride = outputImage.getStride();
            size_t width = outputImage.getWidth();
            size_t height = outputImage.getHeight();
            size_t realStride = pixelSize * width;
            size_t dataSize = pixelData.size();
            /*
            cout << "realStride " << realStride << endl;
            cout << "width " << width << endl;
            cout << "height " << height << endl;
            cout << "pixel size " << pixelSize << endl;
            cout << "data size " << dataSize << endl;
            cout << "bit depth " << Image::bitsPerPixel(outputImage.getFormat()) << endl;
            //assert(realStride == 128 && "real stride should be 128"); 
            
            cout << "Red Levels: " << redLevels
              << ", Green Levels: " << greenLevels
              << ", Blue Levels: " << blueLevels
              << ", Spread: " << spread
              << ", Bayer Level: " << bayerLevel
              << endl;
              */

            for ( size_t i = 0 ; i < dataSize; i += pixelSize ){
                size_t row = i / realStride;
                size_t col = (i % realStride) / pixelSize;
                //cout << "i " << i << " row " << row << endl;
                //cout << "row " << row << " col " << col << endl;
                if ( row > height ) break;
                uint8_t red = pixelData[i];
                uint8_t green = pixelData[i+1];
                uint8_t blue = pixelData[i+2];
                uint8_t alpha = pixelData[i+3];
                float bayerValue = getBayerValue(row,col,bayerLevel);

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

        // Leave alpha channel (if present) unchanged

        //ostringstream error;
        //Image outImage = outputImage.clone();
        //error << "Output port should be " << cc->kTagOutput << endl;
        //assert(cc->hasOutput(cc->kTagOutput) && error.str().c_str());
        //cc->getOutputPort(cc->kTagOutput).write(Packet(std::move(outputImage)));
        cc->getOutputPort(kOutputDither).write(Packet(std::move(outputImage)));
        //cout <<  "CALCULATOR END " << endl;

    }

    // Close method (cleanup if needed)
    void close(CalculatorContext* cc, float delta) override {}

    private:
        uint8_t clamp(uint8_t value, uint8_t min, uint8_t max) {
            if (value < min) return min;
            if (value > max) return max;
            return value;
        }

};

#endif
