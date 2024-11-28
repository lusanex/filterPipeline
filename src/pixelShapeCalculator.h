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
        

        for ( size_t i = 0 ; i < dataSize; i += pixelSize ){
            size_t row = i / realStride;
            //cout << "i " << i << " row " << row << endl;
            size_t col = (i % realStride) / pixelSize;


            if ( row > height ) break;


            size_t pixelatedUV[2] { row,col };
            size_t uv[2] {row,col}; 
            int imageSize[2] { imageSizeX,imageSizeY };
            getSquareUV(pixelatedUV,pixelSize,imageSize);

            size_t newRow = pixelatedUV[0];
            size_t newCol = pixelatedUV[1];
            size_t newIndex = (newRow * realStride) + (newCol * pixelSize);

            uint8_t blue = pixelData[newIndex];
            uint8_t green = pixelData[newIndex+1];
            uint8_t red = pixelData[newIndex+2];
            uint8_t alpha = pixelData[newIndex+3];

            //cout << "gray " << static_cast<int>(gray) << endl;
            //cout << " alpha " << static_cast<int>(alpha) << endl;

            pixelData[i] = blue;       
            pixelData[i + 1] = green;   
            pixelData[i + 2] = red;   
            pixelData[i + 3] = alpha;
        }

        // Leave alpha channel (if present) unchanged

        ostringstream error;
        Image outPixel = outputImage.clone();
        error << "Output port should be " << cc->kTagOutput << endl;
        assert(cc->hasOutput(cc->kTagOutput) && error.str().c_str());
        cc->getOutputPort(cc->kTagOutput).write(Packet(std::move(outputImage)));
        cc->getOutputPort(kOutputPixel).write(Packet(std::move(outPixel)));

        //cout <<  "CALCULATOR END " << endl;

    }

    // Close method (cleanup if needed)
    void close(CalculatorContext* cc, float delta) override {}


    void getSquareUV(size_t uv[2], float pixelSize, int imageSize[2]){
        uv[0] = floor(uv[0]/ pixelSize) * pixelSize / imageSize[0];
        uv[1] = floor(uv[1]/pixelSize) * pixelSize / imageSize[1];
    }
    

       

    
};

#endif
