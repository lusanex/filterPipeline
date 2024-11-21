#ifndef PACKET_H
#define PACKET_H

/*
 * Packet.h - A generic Packet class
 * Author: Erich Gutierrez Chavez
 * Project: Packet Ownership Management
 * Description:
 * - The class is templated to hold any data type T, managing ownership 
 *   of dynamically allocated data.
 * - Includes a timestamp in microseconds since epoch, stored as a long long
 *   The timestamp methods ensure that each time a packet is created, the timestamp
 *   is unique.
 * - Implements the Rule of Five for proper memory management using smart pointers.
 */

#include <iostream>
#include <string>
#include <memory> // For std::unique_ptr
#include <sys/time.h> 

using namespace std;

template <typename T>
class Packet {
private:
    std::unique_ptr<T> data; // Smart pointer to the data
    long long timestamp;     // Timestamp in microseconds since epoch

public:
    // Default Constructor
    Packet() : data(std::make_unique<T>()), timestamp(currentTimestamp()) {}

    // Parameterized Constructor
    Packet(const T& value) : data(std::make_unique<T>(value)), timestamp(currentTimestamp()) {}

    // Copy Constructor
    Packet(const Packet<T>& other) : data(std::make_unique<T>(*other.data)), timestamp(other.timestamp) {}

    // Move Constructor
    Packet(Packet<T>&& other) noexcept : data(std::move(other.data)), timestamp(other.timestamp) {}

    // Destructor
    ~Packet() = default; // Smart pointer automatically handles memory cleanup

    // Copy Assignment Operator
    Packet<T>& operator=(const Packet<T>& other) {
        if (this != &other) {
            data = std::make_unique<T>(*other.data); // Deep copy
            timestamp = other.timestamp;
        }
        return *this;
    }

    // Move Assignment Operator
    Packet<T>& operator=(Packet<T>&& other) noexcept {
        if (this != &other) {
            data = std::move(other.data); // Transfer ownership
            timestamp = other.timestamp;
        }
        return *this;
    }

    // Getter for data
    const T& getData() const {
        return *data; // Dereference the smart pointer to get the data
    }

    // Getter for timestamp
    long long getTimestamp() const {
        return timestamp;
    }

    // Overload << operator for displaying the Packet
    friend std::ostream& operator<<(std::ostream& os, const Packet<T>& packet) {
        os << "Packet { data: " << *packet.data
           << ", timestamp: " << packet.timestamp << " }";
        return os;
    }

    static long long lastTimestamp;

    // Helper to get the current timestamp in microseconds
    static long long currentTimestamp() {
        struct timeval tv;
        gettimeofday(&tv, nullptr);
        auto current = static_cast<long long>(tv.tv_sec) * 1000000LL + tv.tv_usec;
        if (current <= lastTimestamp) {
            current = lastTimestamp + 1;
        }
        lastTimestamp = current;
        return current;
    }
};

#endif // PACKET_H

