
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <cstdint>
#include "../src/image.h"

using namespace std;

// Function to parse the header from stdin
void parseHeader(istream& input, int32_t& width, int32_t& height, PixelFormat& format, int32_t& fps, double& duration) {
    string line;
    while (getline(input, line)) {
        if (line == "HEADER_END") break; // Stop at the end of the header

        stringstream ss(line);
        string key, value;
        getline(ss, key, ':');
        getline(ss, value);


        cout << "key " << key << " value " << value << endl;
        if (key == "WIDTH") {
            width = stoi(value);
        } else if (key == "HEIGHT") {
            height = stoi(value);
        } else if (key == "PIX_FMT") {

            cout << "HERE " << value << endl;

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

// Main entry point
int main() {
    // Variables to store header information
    int32_t width = 0, height = 0, fps = 0;
    double duration = 0.0;
    PixelFormat format = PixelFormat::RGB24;

    // Parse the header from stdin
    parseHeader(cin, width, height, format, fps, duration);

    // Output the header information
    cout << "Header Info:" << endl;
    cout << "Width: " << width << endl;
    cout << "Height: " << height << endl;
    cout << "Pixel Format: " << static_cast<int>(format) << endl;
    cout << "FPS: " << fps << endl;
    cout << "Duration: " << duration << " seconds" << endl;

    // Validate header
    if (width <= 0 || height <= 0 || format == PixelFormat::UNKNOWN) {
        cerr << "Invalid header information. Exiting." << endl;
        return 1;
    }

    // Calculate frame size (width * height * bytes_per_pixel)
    size_t frameSize = width * height * Image::bitsPerPixel(format) / 8;

    // Vector to store frame data
    vector<uint8_t> frameData(frameSize);

    // Read frames from stdin and store them in the Image object
    int frameCount = 0;
    while (cin.read(reinterpret_cast<char*>(frameData.data()), frameSize)) {
        try {
            Image image(width, height, format, frameData);
            cout << "Frame " << ++frameCount << " read successfully." << endl;

            // Process the frame or store it as needed
            // (e.g., save to file, process pixels, etc.)
        } catch (const ImageException& e) {
            cerr << "Error creating image: " << e.what() << endl;
            return 1;
        }
    }

    cout << "Total Frames Processed: " << frameCount << endl;

    return 0;

}
