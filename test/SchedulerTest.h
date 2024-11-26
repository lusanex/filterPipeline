#ifndef SCHEDULER_TEST_H
#define SCHEDULER_TEST_H

#include <sstream>
#include <iostream>
#include <cassert>
#include "../src/scheduler.h"
#include "../src/calculatorbase.h"
#include "../src/calculatorcontext.h"

const string kTagInput = "kTagInput"; 
const string kTagOutput = "kTagOutput"; 


const int kFrameRate = 60;

const int kNumberOfPacketsToPush = kFrameRate;

using namespace std;
class Calculator1 : public CalculatorBase {
    private:
        int current_index = 0;
        const string kCalc1Tag = "C1Output";

public:
    Calculator1() : CalculatorBase("Calculator1") {}

    unique_ptr<CalculatorContext> registerContext() override {
        unique_ptr<CalculatorContext> context = make_unique<CalculatorContext>();
        context->addOutputPort( kCalc1Tag,Port());
        return context;
    }

    void enter(CalculatorContext* cc, float delta) override {
        assert(cc->hasInput(kTagInput) && "Calculator1 requires kTagInput");
        //cout << "[Calculator1] Delta Time: " << delta << " seconds\n";
        //cout << "[Calculator1] Input Tags: ";
        assert(cc->getInputPortTags().size() > 0);
        //cout << "\n";
        //cout << "[Calculator1] Output Tags: ";
        assert(cc->getOutputPortTags().size() > 0 && "output port should be > 0");
            //cout << tag << " ";
        //cout << "\n";
    }

    void process(CalculatorContext* cc, float delta) override { 
        //check if port has updated
        Port& port = cc->getInputPort(kTagInput);
        if ( port.size() == 0 ) return;

        if ( current_index < kNumberOfPacketsToPush ){


            Packet p = port.read();
            //cout << "[Calculator1 ] p.get<> " << p.get<int>() << endl;
            p.get<int>() += 1;

            cc->getOutputPort(kCalc1Tag).write(std::move(p));
            //cout << "Pushing packets to output port  " << getName() << endl;
            current_index++;
        }

    }

    void close(CalculatorContext* cc, float delta) override {}
};

class Calculator2 : public CalculatorBase {
    private:

        int current_index = 0;
        const string kCalc2Tag = "C2Output";
        const string kCalc1Tag = "C1Output";

public:
    Calculator2() : CalculatorBase("Calculator2") {}

    unique_ptr<CalculatorContext> registerContext() override {
        auto context = make_unique<CalculatorContext>();
        context->addInputPort(kCalc2Tag, Port());
        context->addOutputPort(kTagOutput, Port());
        return context;
    }

    void enter(CalculatorContext* cc, float delta) override {
        assert(cc->hasOutput(kTagOutput) && "Calculator2 requires kTagOutput");
        //cout << "[Calculator2] Delta Time: " << delta << " seconds\n";
        //cout << "[Calculator2] Input Tags: ";
        /**
        for (const string& tag : cc->getInputPortTags()) {
            //cout << tag << " ";
        }
        //cout << "\n";
        //cout << "[Calculator2] Output Tags: ";
        for (const string& tag : cc->getOutputPortTags()) {
            //cout << tag << " ";
        }
        **/
        //cout << "\n";
    }

    void process(CalculatorContext* cc, float delta) override {

        if ( current_index < kNumberOfPacketsToPush ){
            //cout << "Pussing packet to output port "  << getName() << endl;
            //we should foward the packet calculator1 has update the data
            Port& port = cc->getInputPort(kCalc1Tag);
            if ( port.size() == 0 ) return;

           // cout << "inpurt port tag " << kCalc1Tag << " size : " << port.size() << endl;
            Packet p = port.read();
            //cout << p << endl;
            cc->getOutputPort(kTagOutput).write(std::move(p));
            current_index++;
        }
    }

    void close(CalculatorContext* cc, float delta) override {}
};
    

class SchedulerTest {
public:
    /**
     * Run all the Scheduler tests.
     */
    static void run(){
        cout << "Starting Scheduler Tests...\n";

        testBasicScheduler();
        testMultipleCalculators();

        cout << "All Scheduler Tests Completed.\n";
    }


private:
    static void testBasicScheduler(){
        cout << "\n--- Test: Basic Scheduler ---\n";

        Scheduler scheduler;
        auto packet = Packet(42); // Create a sample packet

        scheduler.writeToInputPort(std::move(packet));
        try {
            Packet readPacket = scheduler.readFromOutputPort();
        }catch(const PacketException& e){
            cout << "Output Port is empty " << e.what() << " test PASSED" << endl;

        }
        try  {
            scheduler.connectCalculators(); 
        }catch(const CalculatorException& e){
            cout << "Empty Scheduler : " << e.what() << " test PASSED " << endl;
        }
            
    }


    static void testMultipleCalculators(){
        cout << "\n--- Test: Multiple Calculators ---\n";

        Scheduler scheduler;

        // Add calculators
        Calculator1* calculator1 = new Calculator1();
        Calculator2* calculator2 = new Calculator2();
        scheduler.registerCalculator(calculator1);
        scheduler.registerCalculator(calculator2);
        calculator1 = nullptr;  
        calculator2 = nullptr;  

        scheduler.connectCalculators();

        // Write a packet to the input port


        for(int i = 0 ; i < kNumberOfPacketsToPush; i++){
            scheduler.writeToInputPort(std::move(Packet(i)));
        }


        // Run the scheduler for 1 frame
        for ( int i = 0 ; i < kFrameRate ; i++ ){
            scheduler.run();
        }

        cout << "Checking processed packets " << endl;
        for (int i = 0 ; i < kFrameRate ; i++ ) {
            Packet outputPacket = scheduler.readFromOutputPort();
            assert(outputPacket.isValid() && "output packet shoud be valid");
            ostringstream oss;
            oss << "packet data should  be i: " << (i+1) << endl; 
            assert(outputPacket.get<int>() == i + 1 && oss.str().c_str());
        }
        cout << "Testing packets updated PASSED" << endl;

        // Validate the output


    }


};


#endif // SCHEDULER_TEST_H

