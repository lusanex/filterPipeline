#ifndef CALCULATOR_BASE_H
#define CALCULATOR_BASE_H
#include <iostream>
#include "port.h"
#include "calculatorcontext.h"

using namespace std;

class CalculatorBase{
    protected:
        string name;
    public:
        CalculatorBase(const string& calcName)
            :name(calcName){}
        virtual unique_ptr<CalculatorContext> registerContext() = 0;
        virtual void enter(CalculatorContext* cc, float delta) = 0;
        virtual void process(CalculatorContext* cc, float delta) = 0;
        virtual void close(CalculatorContext* cc, float delta) =0;
        const string getName() const {
            return name;
        }

        void setName(const string calcName){
            name = calcName;
        }
};

#endif
