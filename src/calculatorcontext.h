#ifndef CALCULATOR_CONTEXT_H
#define CALCULATOR_CONTEXT_H

#include <map>
#include <string>
#include "calculatorexception.h"
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
    map<string,Port> inputs;            // Input port for the calculator
    map<string,Port> outputs;           // Output port for the calculator    
    map<string, Packet> sidePackets;    // sidePackets 

public:
    // Default Constructor
    CalculatorContext() = delete;

    Port& getInputPort(const string& tag) {
        auto it  = inputs.find(tag);
        if(it == inputs.end()){
            throw CalculatorException("No such tag input port: " + tag);
        }
        return it->second;
    }

    // Setter for inputs port
    void setInputPacket(const string& tag,const Packet packet) {
        Port &port = getInputPort(tag);
        port.write(packet);
    }

    Packet popInputPacket(const string& tag){
        Port& port = getInputPort(tag);
        return port.read(); 
    }

    // Getter for outputs port
    Port& getOutputPacket(string tag) {
    }

    // Setter for outputs port
    void setOutputPakcet(const string tag, Packet packet) {
        //use the proivate getOutput and then set the   
    }

    // Add a side packet with a tag
    void addSidePacket(const string& tag, const Packet& packet) {
        sidePackets[tag] = &packet;
    }

    // Retrieve a side packet by tag
    // fix this i get a call to delte contructor of Packet
    Packet getSidePacket(const string& tag) const {
        auto it = sidePackets.find(tag);
        if (it != sidePackets.end()) {
            return it->second;
        }
        throw runtime_error("Side packet with tag '" + tag + "' not found.");
    }

    // Check if a side packet exists by tag
    // similar contructor for inputs an doutps
    bool hasSidePacket(const string& tag) const {
        return sidePackets.find(tag) != sidePackets.end();
    }

};

#endif // CALCULATOR_CONTEXT_H

