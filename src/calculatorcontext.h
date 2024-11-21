#ifndef CALCULATOR_CONTEXT_H
#define CALCULATOR_CONTEXT_H

#include <map>
#include <string>
#include "port.h"
#include "packet.h"

/*
 * CalculatorContext.h - A context class for calculators
 * Author: Erich Gutierrez Chavez
 * Project: Calculator Framework
 * Description:
 * - Holds input, output, and side packets for a calculator.
 * - Side packets use a map for storing and retrieving tagged packets.
 * - Provides getter and setter methods for accessing and modifying ports and side packets.
 */

using namespace std;

template <typename T>
class CalculatorContext {
private:
    Port<T> inputs;                           // Input port for the calculator
    Port<T> outputs;                          // Output port for the calculator
    map<string, Packet<T>> sidePackets;       // Map for tagged packets (key: string, value: Packet<T>)

public:
    // Default Constructor
    CalculatorContext() {}

    // Getter for inputs port
    Port<T>& getInputs() {
        return inputs;
    }

    // Setter for inputs port
    void setInputs(const Port<T>& inputPort) {
        inputs = inputPort;
    }

    // Getter for outputs port
    Port<T>& getOutputs() {
        return outputs;
    }

    // Setter for outputs port
    void setOutputs(const Port<T>& outputPort) {
        outputs = outputPort;
    }

    // Add a side packet with a tag
    void addSidePacket(const string& tag, const Packet<T>& packet) {
        sidePackets[tag] = packet;
    }

    // Retrieve a side packet by tag
    Packet<T> getSidePacket(const string& tag) const {
        auto it = sidePackets.find(tag);
        if (it != sidePackets.end()) {
            return it->second;
        }
        throw runtime_error("Side packet with tag '" + tag + "' not found.");
    }

    // Check if a side packet exists by tag
    bool hasSidePacket(const string& tag) const {
        return sidePackets.find(tag) != sidePackets.end();
    }

    // Remove a side packet by tag
    void removeSidePacket(const string& tag) {
        sidePackets.erase(tag);
    }

    // Get all tags in sidePackets
    vector<string> getAllTags() const {
        vector<string> tags;
        for (const auto& pair : sidePackets) {
            tags.push_back(pair.first);
        }
        return tags;
    }
};

#endif // CALCULATOR_CONTEXT_H

