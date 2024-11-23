#ifndef PACKET_EXCEPTION_H
#define PACKET_EXCEPTION_H

#include <stdexcept>
#include <string>

using namespace std;
/*
 * PacketException.h - A custom exception class for the PacketHolder class.
 * Author: Erich Gutierrez Chavez
 * Description:
 * - This class is used to handle errors in the PacketHolder class, such as 
 *   invalid data type
 */

class PacketException : public runtime_error {
private:
    string message;

public:
    // Constructor to initialize the exception message
    explicit PacketException(const string& msg) : runtime_error(message){
        message = msg;
    }

    const char* what() const noexcept override {
        return message.c_str();
    }
};

#endif // PACKET_EXCEPTION_H

