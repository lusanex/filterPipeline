#ifndef CALCULATOR_CONTEXT_H
#define CALCULATOR_CONTEXT_H

#include <map>
#include <string>
#include "port.h"
#include "packet.h"
#include "calculatorexception.h"

/*
 * CalculatorContext.h - A context class for calculators
 * Author: Erich Gutierrez Chavez
 * Project: Calculator Framework
 * Description:
 * - Holds input, output, and side packets for a calculator.
 * - Allows adding ports dynamically without overwriting existing ports.
 * - Users can access but not modify ports once added.
 * - Manages side packets with standard map operations.
 */

using namespace std;

class CalculatorContext {
private:
    map<string, Port> inputs;            // Input ports
    map<string, Port> outputs;           // Output ports
    map<string, Packet> sidePackets;     // Side packets

public:

    CalculatorContext() {
        inputs = map<string, Port>();
        outputs = map<string, Port>();
        sidePackets = map<string, Packet>();
    }

    // Add a new input port by tag
    void addInputPort(const string& tag, Port&& port) {
        if (inputs.find(tag) == inputs.end()) {
            inputs[tag] = std::move(port);  
        }
    }

    // Add a new output port by tag
    void addOutputPort(const string& tag, Port&& port) {
        if (outputs.find(tag) == outputs.end()) {
            outputs[tag] = std::move(port);  // Add only if the tag doesn't exist
        }

    }

    // Access input port by tag
    Port* const getInputs(const string& tag) {
        auto it = inputs.find(tag);
        if (it == inputs.end()) {
            throw CalculatorException("No such input port: " + tag);
        }
        return &(it->second);
    }

    // Access output port by tag
    Port* const getOutputs(const string& tag) {
        auto it = outputs.find(tag);
        if (it == outputs.end()) {
            throw CalculatorException("No such output port: " + tag);
        }
        return &(it->second);
    }

    // Access side packet by tag
    const Packet& getSidePacket(const string& tag) {
        auto it = sidePackets.find(tag);
        if (it == sidePackets.end()) {
            throw CalculatorException("No such side packet: " + tag);
        }
        return it->second;
    }

    // Add a new side packet
    void addSidePacket(const string& tag, Packet& packet) {
        sidePackets[tag] = std::move(packet);  // Overwrite existing side packets if tag exists
    }

    // Check if an input port exists
    bool hasInput(const string& tag) const {
        return inputs.find(tag) != inputs.end();
    }

    // Check if an output port exists
    bool hasOutput(const string& tag) const {
        return outputs.find(tag) != outputs.end();
    }

    // Check if a side packet exists
    bool hasSidePacket(const string& tag) const {
        return sidePackets.find(tag) != sidePackets.end();
    }

    // Remove a side packet by tag
    void removeSidePacket(const string& tag) {
        sidePackets.erase(tag);
    }
};

#endif // CALCULATOR_CONTEXT_H

