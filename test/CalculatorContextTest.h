#include "../src/calculatorcontext.h"
#include <iostream>
#include <cassert>


const string kTagImages = "images";
const string kTagColors = "colors";
class CalculatorContextTest {
public:
    static void run(){
        testDefaultConstructor();
        testReadAndWritePortsWithTags();
        testInvalidTag();
        testAddSidePacket();

    }

private:
    static void testDefaultConstructor(){

        CalculatorContext cc;
        Port imagesPort, colorsPort;
        cc.addInputPort(kTagImages,std::move(imagesPort));
        cc.addInputPort(kTagColors,std::move(colorsPort));
        cout << cc.getInputs(kTagImages)->size() << endl;
        assert(cc.getInputs(kTagImages)->size() == 0);
        assert(cc.getInputs(kTagColors)->size() == 0);
        cout << "Test Default constructor passed " << endl;
    }

    static void testReadAndWritePortsWithTags(){
        CalculatorContext cc;
        Port imagesPort, colorsPort;
        size_t expectedSize = 12;
        for (size_t i = 0 ; i < expectedSize; i++){
            imagesPort.write(std::move(Packet(i)));
            colorsPort.write(std::move(Packet("Color " + to_string(i))));
        }
        cc.addInputPort(kTagImages,std::move(imagesPort));
        cc.addInputPort(kTagColors,std::move(colorsPort));

        assert(cc.getInputs(kTagImages)->size() == expectedSize);
        assert(cc.getInputs(kTagColors)->size() == expectedSize);

        Port *inputPortPtr = cc.getInputs(kTagImages);
        size_t i = 0;
        while( inputPortPtr->size() != 0 ){
            Packet p = inputPortPtr->read();
            assert(p.get<size_t>() == i++);
        }


        assert(inputPortPtr->size() == 0);
        cout << "Test add ports with Tags and packets passed" << endl;

    }
    static void testInvalidTag(){
        CalculatorContext cc;
        Port imagesPort, colorsPort;
        size_t expectedSize = 12;
        for (size_t i = 0 ; i < expectedSize; i++){
            imagesPort.write(std::move(Packet(i)));
            colorsPort.write(std::move(Packet("Color " + to_string(i))));
        }
        const string kTagInvalid = "invalidTagName";
        cc.addInputPort(kTagImages,std::move(imagesPort));
        cc.addInputPort(kTagColors,std::move(colorsPort));
        try{
            cc.getInputs(kTagInvalid);
        }
        catch(const CalculatorException& e){
            cout << "Test invalid tag Error: "<<  e.what() << " PASSED" <<  endl;
        }

        assert(cc.getInputs(kTagImages)->size() == expectedSize);
        assert(cc.getInputs(kTagColors)->size() == expectedSize);
        cout << "Test invalid tag name passed" << endl;

    }

    static void testAddSidePacket() {
        CalculatorContext cc;
        int expectedValue = 12;
        Packet p(expectedValue);
        const string kTagName = "number";
        cc.addSidePacket(kTagName, p);
        const Packet& p1 = cc.getSidePacket(kTagName);
        assert(p1.isValid());
        assert(p1.get<int>() == expectedValue);
        cout << "Test SidePacket PASSED" << endl;
    }

};
