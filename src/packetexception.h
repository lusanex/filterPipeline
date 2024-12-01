/**********************************
 * PacketException class.
 * A custom exception class for handling errors in the PacketHolder class.
 * Author: Erich Gutierrez Chavez
 * 
 * Features:
 * - Used to manage exceptions such as invalid data types in packets.
 **********************************/

#ifndef PACKET_EXCEPTION_H
#define PACKET_EXCEPTION_H

#include <stdexcept>
#include <string>

using namespace std;

class PacketException : public runtime_error {
private:
    string message; // Exception message

public:
    /**********************************
     * Constructor.
     * Initializes the exception with a custom message.
     * @param msg The custom message describing the exception.
     **********************************/
    explicit PacketException(const string& msg) 
        : runtime_error(msg), message(msg) {}

    /**********************************
     * Retrieve the exception message.
     * Overrides the standard what() method from std::runtime_error.
     * @return A C-style string containing the exception message.
     **********************************/
    const char* what() const noexcept override {
        return message.c_str();
    }
};

#endif // PACKET_EXCEPTION_H

