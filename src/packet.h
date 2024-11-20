#ifndef PACKET_HOLDER_H
#define PACKET_HOLDER_H

#include <iostream>
#include <string>
#include <sys/time.h>

using namespace std;

/*
 * PacketHolder.h - Base class for type-erased packet handling
 * Description:
 * - PacketHolder: Base class for polymorphic packet behavior.
 * - Packet<T>: Derived class to hold specific type data.
 */

// Base class
class PacketHolder {
public:
    virtual ~PacketHolder() = default;

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

    // Virtual method to print the packet
    virtual void print(ostream& os) const = 0;
};

// Initialize static member
//long long PacketHolder::lastTimestamp = 0;

// Derived templated class
template <typename T>
class Packet : public PacketHolder {
private:
    T data;               // Data of type T
    long long timestamp;  // Timestamp in microseconds since epoch

public:
    // Constructor
    Packet(const T& value) : data(value), timestamp(currentTimestamp()) {}

    // Getter for data
    const T& getData() const { return data; }

    // Getter for timestamp
    long long getTimestamp() const { return timestamp; }

    template <typename U>
    Packet<U>& set() {
        data = nullptr;
        data = T(U());
        timestamp = currentTimestamp();
        return *this;
    }

    // SetSameAs: Copies the data and timestamp from another packet
    Packet<T>& setSameAs(const Packet<T>& other) {
        if (this != &other) {
            data = other.data;
            timestamp = other.timestamp;
        }
        return *this;
    }

    void print(ostream& os) const override {
        os << "{ data: " << data << ", timestamp: " << timestamp << " }";
    }

    // Overload << operator for printing Packet
    friend ostream& operator<<(ostream& os, const Packet<T>& packet) {
        packet.print(os);
        return os;
    }
};

#endif // PACKET_HOLDER_H

