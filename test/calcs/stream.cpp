
#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include <cstdint>
#include "../../src/scheduler.h"
#include "../../src/calculatorcontext.h"
#include "./grayCalculator.h"
#include "./pixelCalculator.h"
#include "./ditherCalculator.h"
#include "./bannercalculator.h"
#include "../../src/image.h"
#include "../../src/packet.h"

long long GrayscaleCalculator::lastTimestamp = 0;
long long Packet::lastTimestamp = 0;

using namespace std;


struct Context{
    int32_t width;
    int32_t height;
    PixelFormat format;
};



// Function to parse the header from stdin
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

int main() {
    //cout << "\n---- Starting Filter Pipeline Test ----\n";

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


    /*
    cout << "Header Info:" << endl;
    cout << "Width: " << width << endl;
    cout << "Height: " << height << endl;
    cout << "Pixel Format: " << static_cast<int>(format) << endl;
    cout << "FPS: " << fps << endl;
    cout << "Duration: " << duration << " seconds" << endl;
    */

    // Calculate frame size
    //
    const string bannerName = "../assets/banner.bmp";
    const string kPixelSize = "pixelSize";
    const string kPixelShape = "pixeShape";

    //options for dither filter
    const string kRedLevels = "redCount";
    const string kGreenLevels = "greenCount";
    const string kBlueLevels = "blueCount";
    const string kSpread = "spread";
    const string kBayerLevel = "bayerLevel";




    shared_ptr<map<string,Packet>> sidePackets = 
        make_shared<map<string,Packet>>();

    (*sidePackets)[kRedLevels] = Packet(4);
    (*sidePackets)[kGreenLevels] = Packet(6);
    (*sidePackets)[kBlueLevels] = Packet(4);
    (*sidePackets)[kSpread] = Packet(2);
    (*sidePackets)[kBayerLevel] = Packet(2);


    //pixel config calc

    (*sidePackets)[kPixelSize] = Packet(5);
    (*sidePackets)[kPixelShape] = Packet(1);

    const string kTagBanner = "ImageBanner";
    const string kTagOverlayStartX = "OverlayStartX";
    const string kTagOverlayStartY = "OverlayStartY";

    Image banner = ImageUtils::readBMP(bannerName);

    (*sidePackets)[kTagBanner] = Packet(banner);

    int hb = banner.getHeight();
    (*sidePackets)[kTagOverlayStartX] = Packet(64);
    (*sidePackets)[kTagOverlayStartY] = Packet(32);


    // GrayscaleCalculator setup
    PixelShapeCalculator pixelCalculator;
    DitherCalculator diteherCalculator;
    GrayscaleCalculator grayscaleCalculator;
    BannerCalculator bannerCalculator;
    Scheduler scheduler;

    scheduler.registerCalculator(&pixelCalculator,sidePackets);
    scheduler.registerCalculator(&diteherCalculator,sidePackets);
    scheduler.registerCalculator(&grayscaleCalculator,sidePackets);
    scheduler.registerCalculator(&bannerCalculator,sidePackets);

    scheduler.connectCalculators();

    int frameCount = 0;


    scheduler.registerOutputCallback([](const Packet &packet){
            if(packet.isValid()){
                //cerr << "is valid " << endl;
                Image out = packet.get<Image>();
                const vector<uint8_t>& rgbaData = out.getData();
                cout.write(reinterpret_cast<const char*>(rgbaData.data()),rgbaData.size());
                return;
            }
            //cout << "Reading invalid packe scheluder " << endl;
        }
    );

   

    scheduler.registerInputCallback([](void* ctx) -> Packet{
            struct Context* context = static_cast<Context*>(ctx);
            int32_t w = context->width;
            int32_t h = context->height;
            PixelFormat format = context->format;
            //cout << "w " << w << endl;
            //cout << "h " << h << endl;

            size_t frameSize = w * h * Image::bitsPerPixel(format) / 8;
            vector<uint8_t> frameData(frameSize);

            cin.read(reinterpret_cast<char*>(frameData.data()),frameSize);
            Image inputImage(w,h,format,frameData);
            return Packet(std::move(inputImage));
        
    },new Context{width,height,format});


    // Process frames
    while (true) //cin.read(reinterpret_cast<char*>(frameData.data()), frameSize)) {
    {
            // Create an Image object
            //Image inputImage(width,height, format, frameData);

            // Write to the scheduler
            //scheduler.writeToInputPort(Packet(inputImage));

            // Run the scheduler
        scheduler.run();

            // Read processed output
            /*
            Packet outputPacket = scheduler.readFromOutputPort();
            Image outputImage = outputPacket.get<Image>();

            //cout << "Processed Frame " << ++frameCount << endl;
            const vector<uint8_t>& rgbaData = outputImage.getData();
            cout.write(reinterpret_cast<const char*>(rgbaData.data()),rgbaData.size());
            */

            // Further processing or saving the image can be done here
            //cerr << "Error processing frame: " << e.what() << endl;
    }

    //cout << "Total Frames Processed: " << frameCount << endl;
    //cout << "---- Filter Pipeline Test Completed ----\n";

    return 0;
}
