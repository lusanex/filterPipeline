#ifndef FILTER_PIPELINE_TEST_H
#define FILTER_PIPELINE_TEST_H

#include <sstream>
#include <iostream>
#include <cassert>
#include "../src/scheduler.h"
#include "../src/calculatorbase.h"
#include "../src/calculatorcontext.h"
#include "../src/grayscalecalculator.h"
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

            const string nameFile = "assets/lena_color.bmp";
            const string outFile = "out/out_lena.bmp";
            
            Image img = ImageUtils::readBMP(nameFile);
            scheduler.writeToInputPort(Packet(img));
            // Add calculators
            GrayscaleCalculator* grayscale = new GrayscaleCalculator();
            scheduler.registerCalculator(grayscale);
            grayscale = nullptr;
            

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
