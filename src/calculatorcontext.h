/**********************************
 * CalculatorContext class.
 * Provides a context for calculators, 
 *  managing input, output, and side packets.
 * Author: Erich Gutierrez Chavez
 *
 * Features:
 * - Dynamically adds input and output ports.
 * - Prevents overwriting of existing ports.
 * - Allows access to side packets using standard map operations.
 * - Ensures ports and packets are accessible but immutable once added.
 **********************************/

#ifndef CALCULATOR_CONTEXT_H
#define CALCULATOR_CONTEXT_H

#include <map>
#include <string>
#include "port.h"
#include "packet.h"
#include "calculatorexception.h"

using namespace std;

class CalculatorContext {
private:
    map<string, shared_ptr<Port>> inputs;             // Input ports
    map<string, shared_ptr<Port>> outputs;            // Output ports
    const shared_ptr<map<string, Packet>> sidePackets; // Side packets

public:
    const string kTagInput = "kTagInput"; 
    const string kTagOutput = "kTagOutput"; 

public:
    /**********************************
     * Default constructor.
     * Initializes empty input and output port maps.
     **********************************/
    CalculatorContext() {
        inputs = map<string, shared_ptr<Port>>();
        outputs = map<string, shared_ptr<Port>>();
    }

    /**********************************
     * Constructor with side packets.
     * Initializes with provided side packets and empty ports.
     * @param newSidePacket Shared pointer to the map of side packets.
     **********************************/
    CalculatorContext(const shared_ptr<map<string,Packet>>& newSidePacket)
    :sidePackets(newSidePacket)
    {
        inputs = map<string, shared_ptr<Port>>();
        outputs = map<string, shared_ptr<Port>>();
    }

    /**********************************
     * Add a new input port.
     * @param tag Unique tag for the port.
     * @param port The Port object to add.
     **********************************/
    void addInputPort(const string& tag, Port&& port) {
        if (inputs.find(tag) == inputs.end()) {
            inputs[tag] = make_shared<Port>(std::move(port));
        }
    }

    /**********************************
     * Add a new output port.
     * @param tag Unique tag for the port.
     * @param port The Port object to add.
     **********************************/
    void addOutputPort(const string& tag, Port&& port) {
        if (outputs.find(tag) == outputs.end()) {
            outputs[tag] = make_shared<Port>(std::move(port));  
        }
    }

    /**********************************
     * Bind an input port by tag.
     * @param tag Unique tag for the port.
     * @param port Reference to the Port object to bind.
     **********************************/
    void bindInputPort(const string& tag, Port& port){
        inputs[tag] = shared_ptr<Port>(&port,[](Port*){});
    }

    /**********************************
     * Bind an output port by tag.
     * @param tag Unique tag for the port.
     * @param port Reference to the Port object to bind.
     **********************************/
    void bindOutputPort(const string& tag, Port& port){
        outputs[tag] = shared_ptr<Port>(&port,[](Port*){});
    }

    /**********************************
     * Get input port by tag.
     * @param tag Unique tag for the port.
     * @return Reference to the requested input port.
     * @throws CalculatorException if port not found.
     **********************************/
    Port& getInputPort(const string& tag) const {
        auto it = inputs.find(tag);
        if (it == inputs.end()) {
            throw CalculatorException("No such input port: " + tag);
        }
        return *(it->second);
    }

    /**********************************
     * Get output port by tag.
     * @param tag Unique tag for the port.
     * @return Reference to the requested output port.
     * @throws CalculatorException if port not found.
     **********************************/
    Port& getOutputPort(const string& tag) const {
        auto it = outputs.find(tag);
        if (it == outputs.end()) {
            throw CalculatorException("No such output port: " + tag);
        }
        return *(it->second);
    }

    /**********************************
     * Get side packet by tag.
     * @param tag Unique tag for the side packet.
     * @return Reference to the requested side packet.
     * @throws CalculatorException if side packet not found.
     **********************************/
    const Packet& getSidePacket(const string& tag) const {
        auto it = sidePackets->find(tag);
        if (it == sidePackets->end()) {
            throw CalculatorException("No such side packet: " + tag);
        }
        return it->second;
    }

    /**********************************
     * Get tags of all input ports.
     * @return Vector containing all input port tags.
     **********************************/
    vector<string> getInputPortTags() const {
        vector<string> names;
        for (const auto& pair : inputs) {
            names.push_back(pair.first);
        }
        return names;
    }

    /**********************************
     * Get tags of all output ports.
     * @return Vector containing all output port tags.
     **********************************/
    vector<string> getOutputPortTags() const {
        vector<string> names;
        for (const auto& pair : outputs) {
            names.push_back(pair.first);
        }
        return names;
    }

    /**********************************
     * Check if an input port exists.
     * @param tag Unique tag for the port.
     * @return True if the input port exists, false otherwise.
     **********************************/
    bool hasInput(const string& tag) const {
        return inputs.find(tag) != inputs.end();
    }

    /**********************************
     * Check if an output port exists.
     * @param tag Unique tag for the port.
     * @return True if the output port exists, false otherwise.
     **********************************/
    bool hasOutput(const string& tag) const {
        return outputs.find(tag) != outputs.end();
    }

    /**********************************
     * Check if a side packet exists.
     * @param tag Unique tag for the side packet.
     * @return True if the side packet exists, false otherwise.
     **********************************/
    bool hasSidePacket(const string& tag) const {
        return sidePackets->find(tag) != sidePackets->end();
    }
};

#endif // CALCULATOR_CONTEXT_H

