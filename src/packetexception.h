#ifndef PORT_EXCEPTION_H
#define PORT_EXCEPTION_H

#include <stdexcept>
#include <string>

/*
 * PacketException.h - A custom exception class for the PacketHolder class.
 * Author: Erich Gutierrez Chavez
 * Description:
 * - This class is used to handle errors in the PacketHolder class, such as 
 *   invalid data type
 */

class PacketException : public std::runtime_error {
private:
    std::string message;

public:
    // Constructor to initialize the exception message
    explicit PacketException(const std::string& msg) : std::runtime_error(message){}

};

#endif // PORT_EXCEPTION_H

