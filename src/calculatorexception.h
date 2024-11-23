#ifndef CALCULATOR_CONTEXT_EXCEPTION_H
#define CALCULATOR_CONTEXT_EXCEPTION_H

#include <exception>
#include <string>

/*
 * CalcultatorException.h - A custom exception class for the Port class.
 * Author: Erich Gutierrez Chavez
 * Description:
 * - This class is used to handle errors in the Port class, such as reading 
 *   from an empty queue.
 */

class CalculatorException : public std::exception {
private:
    std::string message;

public:
    // Constructor to initialize the exception message
    explicit CalculatorException(const std::string& msg) : message(msg) {}

    // Override the what() method to return the exception message
    const char* what() const noexcept override {
        return message.c_str();
    }
};

#endif // CALCULATOR_CONTEXT_EXCEPTION_H