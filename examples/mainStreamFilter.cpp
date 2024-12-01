/**********************************
 * @file mainStreamFilter.cpp
 * @brief Entry point for the video stream filter pipeline.
 *
 * @details
 * - Parses video metadata from stdin.
 * - Sets up a series of image processing calculators.
 * - Processes video frames in real-time.
 * - Outputs processed frames to stdout.
 **********************************/

#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include <cstdint>
#include "calculators/graycalculator.h"
#include "calculators/pixelcalculator.h"
#include "calculators/dithercalculator.h"
#include "calculators/bannercalculator.h"
#include "../src/scheduler.h"
#include "../src/image.h"
#include "../src/packet.h"

long long Packet::lastTimestamp = 0;

using namespace std;

/**********************************
 * @struct Context
 * @brief Holds video metadata including width, height, and pixel format.
 **********************************/
struct Context {
    int32_t width;
    int32_t height;
    PixelFormat format;
};

/**********************************
 * @brief Parses video metadata from stdin.
 * @param input The input stream to read metadata from.
 * @param width Reference to store the video width.
 * @param height Reference to store the video height.
 * @param format Reference to store the pixel format.
 * @param fps Reference to store the frame rate.
 * @param duration Reference to store the video duration.
 **********************************/
void parseHeader(istream& input, int32_t& width, int32_t& height, PixelFormat& format, int32_t& fps, double& duration) {
    string line;
    while (getline(input, line)) {
        if (line == "HEADER_END") break;

        stringstream ss(line);
        string key, value;
        getline(ss, key, ':');
        getline(ss, value);

        if (key == "WIDTH") {
            width = stoi(value);
        } else if (key == "HEIGHT") {
            height = stoi(value);
        } else if (key == "PIX_FMT") {
            if (value == "rgba") {
                format = PixelFormat::RGBA32;
            } else if (value == "rgb") {
                format = PixelFormat::RGB24;
            } else {
                format = PixelFormat::UNKNOWN;
            }
        } else if (key == "FPS") {
            fps = stoi(value);
        } else if (key == "DURATION") {
            duration = stod(value);
        }
    }
}

/**********************************
 * @brief Main function.
 * Parses video metadata, 
 * sets up the scheduler and calculators, 
 * and processes frames in real-time.
 **********************************/
int main() {
    int32_t width = 0, height = 0, fps = 0;
    double duration = 0.0;
    PixelFormat format = PixelFormat::UNKNOWN;

    // Parse the header from stdin
    parseHeader(cin, width, height, format, fps, duration);

    // Validate header
    if (width <= 0 || height <= 0 || format == PixelFormat::UNKNOWN) {
        cerr << "Invalid header information. Exiting.\n";
        return 1;
    }

    //pixel tags config 
    const string kPixelSize = "pixelSize";
    const string kPixelShape = "pixeShape";

    //dither tags configuration
    const string kRedLevels = "redCount";
    const string kGreenLevels = "greenCount";
    const string kBlueLevels = "blueCount";
    const string kSpread = "spread";
    const string kBayerLevel = "bayerLevel";

    // Banner tags configuration
    const string bannerName = "../assets/banner.bmp";
    const string kTagBanner = "ImageBanner";
    const string kTagOverlayStartX = "OverlayStartX";
    const string kTagOverlayStartY = "OverlayStartY";

    shared_ptr<map<string, Packet>> sidePackets = make_shared<map<string, Packet>>();

    // Configure side packets for dithering and pixel effects
    (*sidePackets)[kRedLevels] = Packet(3);
    (*sidePackets)[kGreenLevels] = Packet(6);
    (*sidePackets)[kBlueLevels] = Packet(3);
    (*sidePackets)[kSpread] = Packet(3);
    (*sidePackets)[kBayerLevel] = Packet(2);
    (*sidePackets)[kPixelSize] = Packet(4);
    (*sidePackets)[kPixelShape] = Packet(1);

    // Load banner image and set its position
    Image banner = ImageUtils::readBMP(bannerName);
    (*sidePackets)[kTagBanner] = Packet(banner);
    (*sidePackets)[kTagOverlayStartX] = Packet(64);
    (*sidePackets)[kTagOverlayStartY] = Packet(32);

    // Initialize calculators and scheduler
    PixelShapeCalculator pixelCalculator;
    DitherCalculator ditheringCalculator;
    GrayscaleCalculator grayscaleCalculator;
    BannerCalculator bannerCalculator;
    Scheduler scheduler;

    // Register calculators with the scheduler
    scheduler.registerCalculator(&pixelCalculator, sidePackets);
    scheduler.registerCalculator(&ditheringCalculator, sidePackets);
    scheduler.registerCalculator(&grayscaleCalculator, sidePackets);
    scheduler.registerCalculator(&bannerCalculator, sidePackets);

    scheduler.connectCalculators();

    // Register output callback for processed frames
    scheduler.registerOutputCallback([](const Packet& packet) {
        if (packet.isValid()) {
            Image out = packet.get<Image>();
            const vector<uint8_t>& rgbaData = out.getData();
            cout.write(reinterpret_cast<const char*>(rgbaData.data()), rgbaData.size());
        }
    });

    // Register input callback to feed frames into the scheduler
    scheduler.registerInputCallback([](void* ctx) -> Packet {
        Context* context = static_cast<Context*>(ctx);
        int32_t w = context->width;
        int32_t h = context->height;
        PixelFormat format = context->format;

        size_t frameSize = w * h * Image::bitsPerPixel(format) / 8;
        vector<uint8_t> frameData(frameSize);

        cin.read(reinterpret_cast<char*>(frameData.data()), frameSize);
        Image inputImage(w, h, format, frameData);
        return Packet(std::move(inputImage));
    }, new Context{width, height, format});

    // Process video frames in real-time
    while (true) {
        scheduler.run();
    }

    return 0;
}

