#ifndef GRAYSCALE_CALCULATOR_H
#define GRAY

#include "calculatorbase.h"
#include "image.h"
#include "imageutils.h"
#include <sstream>
#include <cassert>

class GrayscaleCalculator : public CalculatorBase {
private:
    const string kOutputGrayscale = "ImageGrayscale";

public:
    GrayscaleCalculator() : CalculatorBase("GrayscaleCalculator") {}

    // Register input/output ports
    unique_ptr<CalculatorContext> registerContext() override {
        auto context = make_unique<CalculatorContext>();
        //context->addOutputPort(kOutputGrayscale, Port()); 
        return context;
    }

    // Enter method (initialization if needed)
    void enter(CalculatorContext* cc, float delta) override {
        //assert(cc->hasOutput(kOutputGrayscale) && "ImageToGrayscaleCalculator requires an ImageOutput port");
    }

    // Process method: Perform the grayscale transformation
    void process(CalculatorContext* cc, float delta) override {
        //cout << "CALCULATOR  " << endl;
        Port& inputPort = cc->getInputPort(cc->kTagInput);

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
        cout << "realStride " << realStride << endl;
        cout << "width " << width << endl;
        cout << "height " << height << endl;
        cout << "pixel size " << pixelSize << endl;
        cout << "data size " << dataSize << endl;
        cout << "bit depth " << Image::bitsPerPixel(outputImage.getFormat()) << endl;
        //assert(realStride == 128 && "real stride should be 128"); 
        

        for ( size_t i = 0 ; i < dataSize; i += pixelSize ){
            size_t row = i / realStride;
            cout << "i " << i << " row " << row << endl;
            if ( row > height ) break;
            uint8_t blue = pixelData[i];
            uint8_t green = pixelData[i+1];
            uint8_t red = pixelData[i+2];
            uint8_t alpha = pixelData[i+3];

            uint8_t gray = static_cast<uint8_t>(
                        0.299 * blue +
                        0.587 * green +
                        0.114 * red
                    );
            cout << "gray " << static_cast<int>(gray) << endl;
            cout << " alpha " << static_cast<int>(alpha) << endl;

            uint8_t temp = blue;
            blue = red;
            red = temp;
            pixelData[i] = gray;       
            pixelData[i + 1] = gray;   
            pixelData[i + 2] = gray;   
            pixelData[i + 3] = alpha;
        }

        // Leave alpha channel (if present) unchanged

        ostringstream error;
        error << "Output port should be " << cc->kTagOutput << endl;
        assert(cc->hasOutput(cc->kTagOutput) && error.str().c_str());
        cc->getOutputPort(cc->kTagOutput).write(Packet(std::move(outputImage)));
        //cout <<  "CALCULATOR END " << endl;

    }

    // Close method (cleanup if needed)
    void close(CalculatorContext* cc, float delta) override {}
};

#endif
