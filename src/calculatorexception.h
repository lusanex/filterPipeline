/**********************************
 * CalculatorException class.
 * A custom exception class for handling errors in the calculator framework.
 * Author: Erich Gutierrez Chavez
 * 
 * Features:
 * - Provides a mechanism to handle specific errors, such as accessing 
 *   non-existent ports or packets in the calculator context.
 **********************************/

#ifndef CALCULATOR_CONTEXT_EXCEPTION_H
#define CALCULATOR_CONTEXT_EXCEPTION_H

#include <exception>
#include <string>

class CalculatorException : public std::exception {
private:
    std::string message; // Exception message

public:
    /**********************************
     * Constructor.
     * Initializes the exception with a custom message.
     * @param msg The custom message describing the exception.
     **********************************/
    explicit CalculatorException(const std::string& msg) : message(msg) {}

    /**********************************
     * Retrieve the exception message.
     * Overrides the standard what() method from std::exception.
     * @return A C-style string containing the exception message.
     **********************************/
    const char* what() const noexcept override {
        return message.c_str();
    }
};

#endif // CALCULATOR_CONTEXT_EXCEPTION_H

