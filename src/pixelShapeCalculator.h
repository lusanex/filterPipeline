#ifndef PIXEL_SHAPE_CALCULATOR_H
#define PIXEL_SHAPE_CALCULATOR_H

#include "calculatorbase.h"
#include "image.h"
#include "imageutils.h"
#include <sstream>
#include <cmath>
#include <cassert>

class PixelShapeCalculator : public CalculatorBase {
private:
    const string kOutputPixel = "ImagePixel";
    const string kOutputGrayscale = "ImageGrayscale";
    const string kPixelSize = "pixelSize";
    const string kImageSizeX = "imageSizeX";
    const string kImageSizeY = "imageSizeY";
    const string kPixelShape = "pixeShape";

public:
    PixelShapeCalculator() : CalculatorBase("PixelShapeCalculator") {}

    // Register input/output ports
    unique_ptr<CalculatorContext> registerContext(const shared_ptr<map<string,Packet>>& newSidePacket = make_shared<map<string,Packet>>()) override {
        auto context = make_unique<CalculatorContext>(newSidePacket);
        context->addOutputPort(kOutputPixel, Port()); 
        return context;
    }

    // Enter method (initialization if needed)
    void enter(CalculatorContext* cc, float delta) override {
        //assert(cc->hasOutput(kOutputGrayscale) && "ImageToGrayscaleCalculator requires an ImageOutput port");
    }

    // Process method: Perform the grayscale transformation
    void process(CalculatorContext* cc, float delta) override {
        //cout << "CALCULATOR  " << endl;
        //Port& inputPort = cc->getInputPort(kOutputGrayscale);

        Port& inputPort = cc->getInputPort(cc->kTagInput);

        // Check if there is data in the input port
        if (inputPort.size() == 0) return;

        // Read the input packet
        Packet inputPacket = inputPort.read();
        Image outputImage = inputPacket.get<Image>();
        int pixSizeFilter = cc->getSidePacket(kPixelSize).get<int>();
        int imageSizeX = cc->getSidePacket(kImageSizeX).get<int>();
        int imageSizeY = cc->getSidePacket(kImageSizeY).get<int>();
        int pixelShape = cc->getSidePacket(kPixelShape).get<int>();


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
        cout << "realStride " << realStride << endl;
        cout << "width " << width << endl;
        cout << "height " << height << endl;
        cout << "pixel size " << pixelSize << endl;
        cout << "data size " << dataSize << endl;
        cout << "bit depth " << Image::bitsPerPixel(outputImage.getFormat()) << endl;
        //assert(realStride == 128 && "real stride should be 128"); 
        
        // Leave alpha channel (if present) unchanged
        for (size_t x = 0; x < width; ++x) {
            for (size_t y = 0; y < height; ++y) { // Fix inner loop to increment y

                // Calculate the current pixel's 1D index
                size_t i = (y * realStride) + (x * pixelSize);

                // Debug output for pixel coordinates
                //cout << "x " << x << " y " << y << endl;

                // Set up pixelated UVs
                size_t pixelatedUV[2] {x, y};
                int imageSize[2] {imageSizeX, imageSizeY};

                // Choose pixelation function based on pixelShape
                if (pixelShape == 0) {
                    getSquareUV(pixelatedUV, pixSizeFilter, imageSize);
                } else if (pixelShape == 1) {
                    getTriangleUV(pixelatedUV, pixSizeFilter, imageSize);
                } else {
                    getSquareUV(pixelatedUV, pixSizeFilter, imageSize);
                }

                // Calculate new row and column
                size_t newRow = pixelatedUV[1]; // Y corresponds to row
                size_t newCol = pixelatedUV[0]; // X corresponds to column

                // Calculate new index
                if (newRow >= height || newCol >= width) {
                    continue; // Ensure we don't go out of bounds
                }
                size_t newIndex = (newRow * realStride) + (newCol * pixelSize);

                // Access pixel data
                uint8_t blue = pixelData[newIndex];
                uint8_t green = pixelData[newIndex + 1];
                uint8_t red = pixelData[newIndex + 2];
                uint8_t alpha = pixelData[newIndex + 3];

                // Write updated pixel values back
                pixelData[i] = blue;
                pixelData[i + 1] = green;
                pixelData[i + 2] = red;
                pixelData[i + 3] = alpha;
            }
        }

            // Leave alpha channel unchanged

            //ostringstream error;
            //Image outPixel = outputImage.clone();
            //error << "Output port should be " << cc->kTagOutput << endl;
            //assert(cc->hasOutput(cc->kTagOutput) && error.str().c_str());
            //cc->getOutputPort(cc->kTagOutput).write(Packet(std::move(outputImage)));
            cc->getOutputPort(kOutputPixel).write(Packet(std::move(outputImage)));

            //cout <<  "CALCULATOR END " << endl;

        }

        // Close method (cleanup if needed)
        void close(CalculatorContext* cc, float delta) override {}


        void getSquareUV(size_t uv[2], float pixelSize, int imageSize[2]){
            uv[0] = floor(uv[0]/ pixelSize) * pixelSize;
            uv[1] = floor(uv[1]/pixelSize) * pixelSize;
            if (uv[0] >= imageSize[0]) uv[0] = imageSize[0] -1;
            if (uv[1] >= imageSize[1]) uv[1] = imageSize[0] -1;
        }

        void getTriangleUV(size_t uv[2] , size_t pixelSize, int imageSize[2]){

            float blockCoords[2] = {
                static_cast<float>(uv[0])/pixelSize,
                static_cast<float>(uv[1])/pixelSize
            };
            size_t blockFloor[2] = {
                static_cast<size_t>(floor(blockCoords[0])),
                static_cast<size_t>(floor(blockCoords[1])),
            };

            float isUpperTriangle = 
                (fmod(blockCoords[0], 1.0f) 
                 + fmod(blockCoords[1], 1.0f)) < 1.0f ? 0.0f : 1.0f;

            int triangleAnchor[2];
            if(isUpperTriangle == 1.0f){
                triangleAnchor[0] = blockCoords[0] * pixelSize;
                triangleAnchor[1] = blockCoords[1] * pixelSize;
            }else{
                triangleAnchor[0] = (blockFloor[0] + 1) * pixelSize -1;
                triangleAnchor[1] = (blockFloor[1] + 1) * pixelSize -1;

            }
            uv[0] = min(triangleAnchor[0], imageSize[0] -1);
            uv[1] = min(triangleAnchor[1], imageSize[1] -1);
        }
        

        
    };

#endif
