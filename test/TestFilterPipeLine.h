#ifndef FILTER_PIPELINE_TEST_H
#define FILTER_PIPELINE_TEST_H

#include <sstream>
#include <iostream>
#include <cassert>
#include "../src/scheduler.h"
#include "../src/calculatorbase.h"
#include "../src/calculatorcontext.h"
#include "../src/pixelShapeCalculator.h"
#include "../src/dithercalculator.h"
#include "../src/grayscalecalculator.h"
#include "../src/bannercalculator.h"
#include "../src/image.h"
#include "../src/imageutils.h"
#include "../src/packet.h"


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


            const string nameFile = "assets/chapo.bmp";
            const string bannerName = "assets/banner.bmp";
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
            const string kPixelShape = "pixeShape";

            const string kTagBanner = "ImageBanner";
            const string kTagOverlayStartX = "OverlayStartX";
            const string kTagOverlayStartY = "OverlayStartY";

            Image img = ImageUtils::readBMP(nameFile);
            Image banner = ImageUtils::readBMP(bannerName);

            shared_ptr<map<string,Packet>> sidePackets = 
                make_shared<map<string,Packet>>();
            //pixel config calc

            (*sidePackets)[kPixelSize] = Packet(4);
            (*sidePackets)[kImageSizeX] = Packet(img.getWidth());
            (*sidePackets)[kImageSizeY] = Packet(img.getHeight());
            (*sidePackets)[kPixelShape] = Packet(1);

            // dither config calc
            (*sidePackets)[kRedLevels] = Packet(5);
            (*sidePackets)[kGreenLevels] = Packet(11);
            (*sidePackets)[kBlueLevels] = Packet(9);
            (*sidePackets)[kSpread] = Packet(4);
            (*sidePackets)[kBayerLevel] = Packet(1);

            // banner image
            (*sidePackets)[kTagBanner] = Packet(banner);
            int w = img.getWidth();
            int h = img.getHeight();
            int hb = banner.getHeight();
            cout << "BANEER H " << hb << endl;

            (*sidePackets)[kTagOverlayStartX] = Packet(0);
            (*sidePackets)[kTagOverlayStartY] = Packet(h-hb * 2);

            scheduler.writeToInputPort(Packet(img));
            // Add calculators
            GrayscaleCalculator* grayscale = new GrayscaleCalculator();
            DitherCalculator* dither = new DitherCalculator();
            PixelShapeCalculator* pixel = new PixelShapeCalculator();
            BannerCalculator* bannerCalculator = new BannerCalculator();
            scheduler.registerCalculator(pixel,sidePackets);
            scheduler.registerCalculator(dither,sidePackets);
            scheduler.registerCalculator(grayscale,sidePackets);
            scheduler.registerCalculator(bannerCalculator,sidePackets);
            grayscale = nullptr;
            dither = nullptr;
            pixel  = nullptr;

            scheduler.connectCalculators();
            for(int i = 0 ; i < scheduler.size() ; i++){
                scheduler.run();
            }

            Packet p = scheduler.readFromOutputPort();
            ImageUtils::writeBMP(outFile,p.get<Image>());
            //Image i = ImageUtils::imageReader(outFile);
            //cout << "Header size " << i.getHeader().size() << endl;
            //cout << "header dump after writing \n" << ImageUtils::hexdump(i.getHeader()) << endl;
            
            
        }

};



#endif
