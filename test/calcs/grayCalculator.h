
#ifndef GRAYSCALE_CALCULATOR_H
#define GRAYSCALE_CALCULATOR_H

#include "../../src/calculatorbase.h"
#include "../../src/image.h"
#include "../../src/imageutils.h"
#include "../../src/packet.h"
#include <sstream>
#include <cassert>

class GrayscaleCalculator : public CalculatorBase {
private:
    const string kOutputGrayscale = "ImageGrayscale";
    const string kOutputDither = "ImageDither";
    const string kOutputPixel = "ImagePixel";

public:
    GrayscaleCalculator() : CalculatorBase("GrayscaleCalculator") {}

    unique_ptr<CalculatorContext> registerContext(const shared_ptr<map<string, Packet>>& newSidePacket = make_shared<map<string, Packet>>()) override {
        auto context = make_unique<CalculatorContext>(newSidePacket);
        context->addOutputPort(kOutputGrayscale, Port());
        return context;
    }

    void enter(CalculatorContext* cc, float delta) override {}

    void process(CalculatorContext* cc, float delta) override {
        //Port& inputPort = cc->getInputPort(cc->kTagInput);
        Port& inputPort = cc->getInputPort(kOutputDither);

        //cout << "delta " << delta;

        if (inputPort.size() == 0) return;

        Packet inputPacket = inputPort.read();
        Image outputImage = inputPacket.get<Image>();

        std::vector<uint8_t>& pixelData = outputImage.getData();

        size_t pixelSize = Image::bitsPerPixel(outputImage.getFormat()) / 8;
        if (pixelSize < 1) return;
        size_t rowStride = outputImage.getStride();
        size_t width = outputImage.getWidth();
        size_t height = outputImage.getHeight();
        size_t realStride = pixelSize * width;
        size_t dataSize = pixelData.size();
        long long startTime = currentTimestamp(); 
        //cout << "start time " << startTime << endl;

        // Leave alpha channel (if present) unchanged
        for (size_t x = 0; x < width; ++x) {
            for (size_t y = 0; y < height; ++y) { 
                size_t i = (y * realStride) + (x * pixelSize);
        

                uint8_t red = pixelData[i];
                uint8_t green = pixelData[i + 1];
                uint8_t blue =  pixelData[i + 2];
                uint8_t alpha = pixelData[i + 3];

                uint8_t gray = static_cast<uint8_t>(
                    0.299 * red +
                    0.587 * green +
                    0.114 * blue
                );

                pixelData[i] = gray;
                pixelData[i + 1] = gray;
                pixelData[i + 2] = gray;
                pixelData[i + 3] = alpha;
            }

        }
        //long long endTime = currentTimestamp();
        //long long elpasedTime = endTime - startTime;
        //cout << "end time " << endTime << endl;
        //cout << "elapsed time " << elpasedTime << endl;
        cc->getOutputPort(kOutputGrayscale).write(Packet(std::move(outputImage)));
    }

    void close(CalculatorContext* cc, float delta) override {}

    static long long lastTimestamp;

    long long currentTimestamp() {
        struct timespec ts;
        clock_gettime(CLOCK_MONOTONIC, &ts);

        long long current = static_cast<long long>(ts.tv_sec) * 1000000LL + ts.tv_nsec / 1000; // Convert to microseconds
        if (current <= lastTimestamp) {
            current = lastTimestamp + 1;  // Guarantee monotonicity
        }
        lastTimestamp = current;
        return current;
    }


};
#endif // GRAYSCALE_CALCULATOR_H
