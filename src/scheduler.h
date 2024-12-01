/**********************************
 * @file scheduler.h
 * @author Erich Gutierrez Chavez
 * @brief Defines the Scheduler class for managing the execution of calculators.
 *
 * @details
 * - Manages a sequence of calculators and their contexts.
 * - Handles input/output data flow through ports.
 * - Supports callbacks for external input/output operations.
 * - Enforces frame rate and delta time calculations for synchronized execution.
 * - Provides utilities for managing calculator registration, connections, and runtime control.
 *
 * Key Features:
 * - Register and connect multiple calculators dynamically.
 * - Input and output ports for external data handling.
 * - Callback mechanisms for input and output processing.
 * - High-resolution frame timing using `clock_gettime`.
 *
 * Constraints:
 * - Calculators must be registered before running the scheduler.
 * - Proper connections must be established between calculators and ports.
 * - Frame rate and delta time calculations depend on the system clock accuracy.
 **********************************/

#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <iostream>
#include <vector>
#include <map>
#include <memory>
#include <ctime>
#include "calculatorbase.h"
#include "calculatorcontext.h"
#include "image.h"

using namespace std;

class Scheduler {
private:
    vector<unique_ptr<CalculatorBase>> calculators; // List of calculators
    map<string, unique_ptr<CalculatorContext>> contexts; // Calculator contexts
    bool running; // Scheduler running state
    int current_index; // Current calculator index
    const int FRAME_RATE; // Target frame rate
    const float FRAME_DURATION; // Frame duration in seconds
    const unsigned long long FRAME_RATE_MS; // Frame duration in microseconds
    const string kTagInput = "kTagInput"; // Input port tag
    const string kTagOutput = "kTagOutput"; // Output port tag
    unsigned long long startTimeScheduler = 0; // Scheduler start time
    unsigned long long startTimeFrame = 0; // Frame start time
    unsigned long long numOfFrames = 0; // Number of processed frames

    Port inputPort; // Input port for external data
    Port outputPort; // Output port for external data

    unique_ptr<void (*)(const Packet&)> callbackWrite; // Output callback
    unique_ptr<Packet (*)(void*)> callbackRead; // Input callback
    unique_ptr<void*> context; // Context for input callback

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

    /**
     * Constructor to initialize the Scheduler with a specified frame rate.
     * @param frameRate Target frame rate in frames per second.
     */
    Scheduler(int frameRate)
        : running(false),
          current_index(0),
          FRAME_RATE(frameRate),
          FRAME_DURATION(1.0f / FRAME_RATE),
          FRAME_RATE_MS(static_cast<unsigned long long>(FRAME_DURATION * 1000000.0f)),
          inputPort(Port()),
          outputPort(Port()),
          callbackRead(nullptr){}


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
    void registerCalculator(CalculatorBase* calculator,const shared_ptr<map<string,Packet>>& newSidePacket = make_shared<map<string,Packet>>()) {
        calculators.push_back(unique_ptr<CalculatorBase>(calculator));
        unique_ptr<CalculatorContext> context = calculator->registerContext(newSidePacket);
        contexts[calculator->getName()] = std::move(context);
    }

    /**
     * Registers an output callback function for the Scheduler.
     * @param cb Function pointer for the output callback.
     */
    void registerOutputCallback(void (*cb)(const Packet&)){
        callbackWrite = make_unique<void (*)(const Packet&)>(cb);
    }

    /**
     * Registers an input callback function and its context for the Scheduler.
     * @param cb Function pointer for the input callback.
     * @param ctx Context pointer for the callback.
     */
    void registerInputCallback(Packet (*cb)(void*), void* ctx){
        callbackRead = make_unique<Packet(*)(void*)>(cb);
        context = make_unique<void *>(ctx);
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

        // Connect the first calculator to the scheduler's input port
        CalculatorContext* firstContext = getCCByCalculatorName(calculators[0]->getName());
        firstContext->bindInputPort(kTagInput, inputPort);

        // Connect the last calculator to the scheduler's output port
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
        try {
          p = outputPort.read();
        } catch(const PortException& e) {
            cout << e.what() << endl;
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

        if (!running) {
            running = true;
            startTimeScheduler = getCurrentTime();
        }

        float delta = calculateDeltaTime(startTimeFrame);
        startTimeFrame = getCurrentTime(); 

        while (running) {

            if (callbackRead && *callbackRead) {
                Packet newPacket = (*callbackRead)(*context);
                inputPort.write(std::move(newPacket));
            }

            // Get the current calculator and context
            CalculatorBase* currentCalc = calculators[current_index].get();
            CalculatorContext* currentCC = getCCByCalculatorName(currentCalc->getName());

            // Enter, process, and close the calculator
            currentCalc->enter(currentCC, delta);
            currentCalc->process(currentCC, delta);
            currentCalc->close(currentCC, delta);

            // Frame duration enforcement
            unsigned long long endTimeFrame = getCurrentTime();
            unsigned long long elapsedTimeFrame = endTimeFrame - startTimeFrame;
            numOfFrames++;

            if (callbackWrite && *callbackWrite) {
                (*callbackWrite)(readFromOutputPort());
            }

            if (elapsedTimeFrame >= FRAME_RATE_MS) {
                current_index = (current_index + 1) % calculators.size();
                return;
            }
            current_index = (current_index + 1) % calculators.size();
        }
    }

    /**
     * Retrieves the elapsed time since the scheduler started.
     * @return Elapsed time in seconds.
     */
    double getElapsedTime() const {
        if (startTimeScheduler == 0) {
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

    /**
     * Retrieves the number of registered calculators.
     * @return Number of calculators.
     */
    int size() const {
        return calculators.size();
    }

private:
    /**
     * Retrieves the current system time in microseconds.
     * @return Current time in microseconds.
     */
    unsigned long long getCurrentTime() const {
        struct timespec ts;
        clock_gettime(CLOCK_MONOTONIC, &ts);
        return static_cast<unsigned long long>(ts.tv_sec) * 1000000LL + ts.tv_nsec / 1000;
    }

    /**
     * Calculates the delta time (time elapsed since the last frame).
     * @param lastTime The time of the last frame in microseconds.
     * @return Delta time in seconds.
     */
    float calculateDeltaTime(unsigned long long& lastTime) {
        unsigned long long now = getCurrentTime(); 
        float deltaTime = static_cast<float>(now - lastTime) / 1000000.f;
        return deltaTime;
    }

};

#endif // SCHEDULER_H

