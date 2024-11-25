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
        testBindPort();

    }

private:
    static void testDefaultConstructor(){

        CalculatorContext cc;
        Port imagesPort, colorsPort;
        cc.addInputPort(kTagImages,std::move(imagesPort));
        cc.addInputPort(kTagColors,std::move(colorsPort));
        cout << cc.getInputPort(kTagImages).size() << endl;
        assert(cc.getInputPort(kTagImages).size() == 0);
        assert(cc.getInputPort(kTagColors).size() == 0);
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

        assert(cc.getInputPort(kTagImages).size() == expectedSize);
        assert(cc.getInputPort(kTagColors).size() == expectedSize);

        Port& inputPortPtr = cc.getInputPort(kTagImages);
        size_t i = 0;
        while( inputPortPtr.size() != 0 ){
            Packet p = inputPortPtr.read();
            assert(p.get<size_t>() == i++);
        }


        assert(inputPortPtr.size() == 0);
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
            cc.getInputPort(kTagInvalid);
        }
        catch(const CalculatorException& e){
            cout << "Test invalid tag Error: "<<  e.what() << " PASSED" <<  endl;
        }

        assert(cc.getInputPort(kTagImages).size() == expectedSize);
        assert(cc.getInputPort(kTagColors).size() == expectedSize);
        cout << "Test invalid tag name PASSED" << endl;

    }

    static void testAddSidePacket() {
        // Create and populate a side packets map
        int expectedValue = 12;
        string expectedName = "CONST_NAME";
        const string kTagName1 = "number";
        const string kTagName2 = "name";

        // Create a map for side packets
        shared_ptr<map<string, Packet>> sidePackets = make_shared<map<string, Packet>>();
        (*sidePackets)[kTagName1] = Packet(expectedValue); // Add an integer packet
        (*sidePackets)[kTagName2] = Packet(expectedName);  // Add a string packet
        CalculatorContext cc(sidePackets);
        CalculatorContext cc2(sidePackets);

        // Set the side packets map in the CalculatorContext

        // Access packets and validate their contents
        const Packet& p1 = cc.getSidePacket(kTagName1);
        const Packet& p2 = cc.getSidePacket(kTagName2);


        const Packet& p3 = cc.getSidePacket(kTagName1);
        const Packet& p4 = cc.getSidePacket(kTagName2);

        // Print and validate the packets
        cout << p1 << endl;
        cout << p2 << endl;

        cout << p3 << endl;
        cout << p4 << endl;

        assert(p1.isValid());
        assert(p2.isValid());
        assert(p3.isValid());
        assert(p4.isValid());
    
        assert(p1.get<int>() == expectedValue);
        assert(p3.get<int>() == expectedValue);

        assert(p2.get<string>() == expectedName);
        assert(p4.get<string>() == expectedName);
        cout << "Test SidePacket PASSED" << endl;
    }

    static void testBindPort(){
        CalculatorContext cc;
        CalculatorContext cc2;
        Port imagesPort;
        size_t expectedSize = 12;
        for (size_t i = 0 ; i < expectedSize; i++){
            imagesPort.write(std::move(Packet(i)));
        }
        cc.addOutputPort(kTagImages,std::move(imagesPort));
        for(const string& tag: cc.getOutputPortTags()){
            cout << "Tag name " << tag << endl;
            assert(tag == kTagImages);
            //Port& port = cc.getOutputPort(tag);
            
        }
        cc2.bindInputPort(kTagImages,cc.getOutputPort(kTagImages));
        assert(cc.getOutputPort(kTagImages) == cc2.getInputPort(kTagImages));
        cout << "Test SidePacket PASSED" << endl;
    }

  };
