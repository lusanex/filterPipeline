#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <iostream>
#include <vector>
#include <map>
#include <memory>
#include <ctime>
#include "calculatorbase.h"
#include "calculatorcontext.h"

using namespace std;

class Scheduler {
private:
    vector<unique_ptr<CalculatorBase>> calculators; 
    map<string, unique_ptr<CalculatorContext>> contexts; 
    bool running; 
    int current_index; 
    const int FRAME_RATE; 
    const float FRAME_DURATION; 
    const unsigned long long FRAME_RATE_MS;
    const string kTagInput = "kTagInput"; 
    const string kTagOutput = "kTagOutput"; 
    unsigned long long startTimeScheduler = 0;
    unsigned long long startTimeFrame = 0;


    Port inputPort; 
    Port outputPort; 

    /**
     * Calculates the delta time (time elapsed since the last frame) using `ctime`.
     * @param lastTime The time of the last frame in seconds.
     * @return Delta time in seconds.
     */
   
public:
    /**
     * Constructor to initialize the Scheduler with default settings.
     */
    Scheduler() 
        : running(false), 
          current_index(0), 
          FRAME_RATE(60), 
          FRAME_DURATION(1.0f / FRAME_RATE), 
          FRAME_RATE_MS(static_cast<unsigned long long>(FRAME_DURATION * 1000000.0f)),
          inputPort(Port()), 
          outputPort(Port()) {}

    Scheduler(int frameRate)
        : running(false),
          current_index(0),
          FRAME_RATE(frameRate),
          FRAME_DURATION(1.0f / FRAME_RATE),
          FRAME_RATE_MS(static_cast<unsigned long long>(FRAME_DURATION * 1000000.0f)),
          inputPort(Port()),
          outputPort(Port()) {}


    /**
     * Retrieves a CalculatorContext by its associated calculator's name.
     * @param calcName The name of the calculator.
     * @return Pointer to the corresponding CalculatorContext.
     */
    CalculatorContext* getCCByCalculatorName(const string& calcName) {
        auto it = contexts.find(calcName);
        if (it == contexts.end()) {
            throw CalculatorException("No context found for calculator: " + calcName);
        }
        return it->second.get();
    }

    /**
     * Registers a calculator and its context.
     * @param calculator The calculator to register.
     */
    void registerCalculator(CalculatorBase* calculator) {
        calculators.push_back(unique_ptr<CalculatorBase>(calculator));
        unique_ptr<CalculatorContext> context = calculator->registerContext();
        contexts[calculator->getName()] = std::move(context);
    }

    /**
     * Connects the calculators and manages internal input and output ports.
     */
    void connectCalculators() {
        if (calculators.empty()) {
            throw CalculatorException("Error: No calculators registered to connect.");
        }

              // Connect calculators in sequence
        for (size_t i = 0; i < calculators.size() - 1; ++i) {
            CalculatorContext* currentContext = 
                getCCByCalculatorName(calculators[i]->getName());
            CalculatorContext* nextContext = 
                getCCByCalculatorName(calculators[i + 1]->getName());

            vector<string> outputPortNames = currentContext->getOutputPortTags();
            for (size_t j = 0; j < outputPortNames.size(); ++j) {
                const string& tag = outputPortNames[j];
                Port& sharedPort = currentContext->getOutputPort(tag);
                nextContext->bindInputPort(tag, sharedPort);
            }
        }

        // Connect the last calculator to the scheduler's output port
        // Connect the scheduler's input port to the first calculator
        CalculatorContext* firstContext = getCCByCalculatorName(calculators[0]->getName());
        firstContext->bindInputPort(kTagInput, inputPort);


        CalculatorContext* lastContext = 
            getCCByCalculatorName(calculators[calculators.size() - 1]->getName());
        lastContext->bindOutputPort(kTagOutput, outputPort);
    }

    /**
     * Writes a packet to the input port.
     * @param packet The packet to write.
     */
    void writeToInputPort(Packet&& packet) {
        inputPort.write(std::move(packet));
    }

    /**
     * Reads a packet from the output port.
     * @return The packet read from the output port.
     */
    Packet readFromOutputPort() {
        Packet p;
        try{
          p = outputPort.read();
        }catch(const PortException& e){
            //cout << e.what() << endl;
        }
        return p;
    }

    /**
     * Starts the main loop to run all calculators.
     */
    void run() {

        if (calculators.empty()) {
            throw CalculatorException("No calculators registered to run.");
        }

        if (!running){
            running = true;
            startTimeScheduler= getCurrentTime();
            cout << "Starting time sche " << startTimeFrame << endl;
        }

        cout << "Last start time frame "  << startTimeFrame << endl;
        float delta = calculateDeltaTime(startTimeFrame);
        cout << "Delta time : " << delta << endl;
        startTimeFrame = getCurrentTime(); 
        cout << "Current start time frame " << startTimeFrame << endl;

        while (running) {

            // Get the current calculator and context
            CalculatorBase* currentCalc = calculators[current_index].get();
            CalculatorContext* currentCC = getCCByCalculatorName(currentCalc->getName());

            // Enter, process, and close the calculator
            currentCalc->enter(currentCC, delta);
            currentCalc->process(currentCC, delta);
            currentCalc->close(currentCC, delta);

            // Move to the next calculator


            // Frame duration enforcement
            unsigned long long  endTimeFrame = getCurrentTime();
            cout << "endTimeFrame : " << endTimeFrame << endl;
            unsigned long long  elapsedTimeFrame = endTimeFrame - startTimeFrame;

            if (elapsedTimeFrame >= FRAME_RATE_MS) {
                cout << "One frame elapse " << elapsedTimeFrame << endl;
                current_index = (current_index + 1) % calculators.size();
                return;
            }
            current_index = (current_index + 1) % calculators.size();
        }
    }

    double getElapsedTime() const{
        if( startTimeScheduler == 0 ){
            return 0.0;
        }
        unsigned long long currentTime = getCurrentTime();
        return static_cast<double>(currentTime - startTimeScheduler) / 1000000.0;
    }
    /**
     * Stops the scheduler.
     */
    void stop() {
        running = false;
    }

    private:
    unsigned long long getCurrentTime() const {
        struct timeval tv;
        gettimeofday(&tv,nullptr);
        return static_cast<unsigned long long>(tv.tv_sec) * 1000000LL + tv.tv_usec;

    }
    float calculateDeltaTime(unsigned long long& lastTime) {
        unsigned long long now = getCurrentTime(); 
        float deltaTime = static_cast<float>(now - lastTime) / 1000000.f;
        return deltaTime;
    }

};

#endif // SCHEDULER_H
