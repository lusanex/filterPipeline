/**********************************
 * @file calculatorbase.h
 * @author Erich Gutierrez Chavez
 * @brief Defines the CalculatorBase class, 
 * the abstract base class for calculators in the framework.
 *
 * @details
 * - The CalculatorBase class provides the basic 
 *   interface for implementing calculators.
 * - Defines lifecycle methods 
 *   (`registerContext`, `enter`, `process`, `close`) for processing data.
 * - Integrates with the CalculatorContext to manage input, output, and side packets.
 * - Supports assigning a name to each calculator for identification.
 *
 * Usage:
 * - Derive from this class to implement custom calculators.
 * - Override the virtual methods to define specific behavior 
 *   for each phase of the calculator's lifecycle.
 *
 * Constraints:
 * - Calculators must implement all virtual methods to be instantiated.
 * - Proper interaction with the CalculatorContext 
 *   is required for managing ports and packets.
 **********************************/

#ifndef CALCULATOR_BASE_H
#define CALCULATOR_BASE_H
#include <iostream>
#include "port.h"
#include "calculatorcontext.h"

using namespace std;

/**********************************
 * CalculatorBase class.
 * An abstract base class for calculators, providing the basic interface.
 **********************************/
class CalculatorBase{
    protected:
        string name;
    public:
        /**********************************
         * Constructor.
         * Initializes the calculator with a given name.
         * @param calcName The name of the calculator.
         **********************************/
        CalculatorBase(const string& calcName)
            :name(calcName){}

        /**********************************
         * Registers a new context.
         * @param newSidePacket An optional side packet map.
         * @return A unique pointer to the new CalculatorContext.
         **********************************/
        virtual unique_ptr<CalculatorContext> registerContext(const shared_ptr<map<string,Packet>>& newSidePacket = make_shared<map<string,Packet>>()) = 0;

        /**********************************
         * Enter method.
         * Called when entering the calculator context.
         * @param cc The calculator context.
         * @param delta The delta time.
         **********************************/
        virtual void enter(CalculatorContext* cc, float delta) = 0;

        /**********************************
         * Process method.
         * Processes data within the calculator context.
         * @param cc The calculator context.
         * @param delta The delta time.
         **********************************/
        virtual void process(CalculatorContext* cc, float delta) = 0;

        /**********************************
         * Close method.
         * Called when closing the calculator context.
         * @param cc The calculator context.
         * @param delta The delta time.
         **********************************/
        virtual void close(CalculatorContext* cc, float delta) =0;

        /**********************************
         * Gets the name of the calculator.
         * @return The name of the calculator.
         **********************************/
        const string getName() const {
            return name;
        }

        /**********************************
         * Sets the name of the calculator.
         * @param calcName The new name of the calculator.
         **********************************/
        void setName(const string calcName){
            name = calcName;
        }
};

#endif
