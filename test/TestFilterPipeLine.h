#ifndef FILTER_PIPELINE_TEST_H
#define FILTER_PIPELINE_TEST_H

#include <sstream>
#include <iostream>
#include <cassert>
#include "../src/scheduler.h"
#include "../src/calculatorbase.h"
#include "../src/calculatorcontext.h"
#include "../src/grayscalecalculator.h"
#include "../src/dithercalculator.h"
#include "../src/image.h"
#include "../src/imageutils.h"
#include "../src/packet.h"
#include "../src/pixelShapeCalculator.h"

using namespace std;

class TestFilterPipeline {
    public:
        static void run() {
            cout << "\n----Starting Filter pipeline Tests ----\n" ;
            testFilterpipeline();
            cout<< "Pipeline filter Completed\n";

        }

    private:
        static void testFilterpipeline(){

            Scheduler scheduler;


            const string nameFile = "assets/lena_color.bmp";
            const string outFile = "out/final_filter.bmp";

            //options for dither filter
            const string kRedLevels = "redCount";
            const string kGreenLevels = "greenCount";
            const string kBlueLevels = "blueCount";
            const string kSpread = "spread";
            const string kBayerLevel = "bayerLevel";


            //options for pixel filter
            const string kPixelSize = "pixelSize";
            const string kImageSizeX = "imageSizeX";
            const string kImageSizeY = "imageSizeY";

            shared_ptr<map<string,Packet>> sidePackets = 
                make_shared<map<string,Packet>>();

            (*sidePackets)[kRedLevels] = Packet(4);
            (*sidePackets)[kGreenLevels] = Packet(4);
            (*sidePackets)[kBlueLevels] = Packet(4);
            (*sidePackets)[kSpread] = Packet(2);
            (*sidePackets)[kBayerLevel] = Packet(1);


            (*sidePackets)[kPixelSize] = Packet(4);
            
            Image img = ImageUtils::readBMP(nameFile);

            (*sidePackets)[kImageSizeX] = Packet(img.getWidth());
            (*sidePackets)[kImageSizeY] = Packet(img.getHeight());
            
            scheduler.writeToInputPort(Packet(img));
            // Add calculators
            GrayscaleCalculator* grayscale = new GrayscaleCalculator();
            DitherCalculator* dither = new DitherCalculator();
            PixelShapeCalculator* pixel = new PixelShapeCalculator();
            //scheduler.registerCalculator(dither,sidePackets);
            //scheduler.registerCalculator(grayscale,sidePackets);
            scheduler.registerCalculator(pixel,sidePackets);
            grayscale = nullptr;
            dither = nullptr;
            

            scheduler.connectCalculators();
            scheduler.run();
            Packet p = scheduler.readFromOutputPort();
            ImageUtils::writeBMP(outFile,p.get<Image>());
            //Image i = ImageUtils::imageReader(outFile);
            //cout << "Header size " << i.getHeader().size() << endl;
            //cout << "header dump after writing \n" << ImageUtils::hexdump(i.getHeader()) << endl;
            
            
        }

};



#endif
