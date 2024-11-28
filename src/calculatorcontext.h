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
    map<string, shared_ptr<Port>> inputs;            // Input ports
    map<string, shared_ptr<Port>> outputs;           // Output ports
    const shared_ptr<map<string, Packet>> sidePackets;     // Side packets
                                                           //
public:
    const string kTagInput = "kTagInput"; 
    const string kTagOutput = "kTagOutput"; 


public:

    CalculatorContext() {
        inputs = map<string, shared_ptr<Port>>();
        outputs = map<string, shared_ptr<Port>>();
    }

    CalculatorContext(const shared_ptr<map<string,Packet>>& newSidePacket)
    :sidePackets(newSidePacket)
    {
        inputs = map<string, shared_ptr<Port>>();
        outputs = map<string, shared_ptr<Port>>();
    }


    // Add a new input port by tag
    void addInputPort(const string& tag, Port&& port) {
        if (inputs.find(tag) == inputs.end()) {
            inputs[tag] = make_shared<Port>(std::move(port));
        }
    }


    // Add a new output port by tag
    void addOutputPort(const string& tag, Port&& port) {
        if (outputs.find(tag) == outputs.end()) {
            outputs[tag] = make_shared<Port>(std::move(port));  
        }

    }

    //Bind an input port by tag (shared ref)
    void bindInputPort(const string& tag, Port& port){
        inputs[tag] = shared_ptr<Port>(&port,[](Port*){});
    }
    //Bind an output port by tag (shared ref)
    void bindOutputPort(const string& tag, Port& port){
        outputs[tag] = shared_ptr<Port>(&port,[](Port*){});
    }


 

    // Access input port by tag
    Port& getInputPort(const string& tag) {
        auto it = inputs.find(tag);
        if (it == inputs.end()) {
            throw CalculatorException("No such input port: " + tag);
        }
        return *(it->second);
    }


    // Access output port by tag
    Port& getOutputPort(const string& tag) {
        auto it = outputs.find(tag);
        if (it == outputs.end()) {
            throw CalculatorException("No such output port: " + tag);
        }
        return *(it->second);
    }

    // Access side packet by tag
    const Packet& getSidePacket(const string& tag) const{
        auto it = sidePackets->find(tag);
        if (it == sidePackets->end()) {
            throw CalculatorException("No such side packet: " + tag);
        }
        return it->second;
    }

     vector<string> getInputPortTags() const {
        vector<string> names;
        for (map<string, shared_ptr<Port>>::const_iterator it = inputs.begin(); it != inputs.end(); ++it) {
            names.push_back(it->first);
        }
        return names;
    }

    // Get names of all output ports
    vector<string> getOutputPortTags() const {
        vector<string> names;
        for (map<string, shared_ptr<Port>>::const_iterator it = outputs.begin(); it != outputs.end(); ++it) {
            names.push_back(it->first);
        }
        return names;
    }

    // Add a new side packet
    //void addSidePacket(const string& tag, Packet&& packet) {
    //    sidePackets[tag] = make_unique<Packet>(std::move(packet));  
    //}

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
        return sidePackets->find(tag) != sidePackets->end();
    }

};

#endif // CALCULATOR_CONTEXT_H

