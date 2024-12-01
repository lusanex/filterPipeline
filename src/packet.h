/**********************************
 * @file packet.h
 * @author Erich Gutierrez Chavez
 * @brief Defines the Packet class for managing packets with timestamped ownership.
 *
 * @details
 * - Templated class for managing ownership of dynamically allocated data of type `T`.
 * - Includes support for timestamps in microseconds since epoch for unique identification.
 * - Handles dynamic casting to ensure type safety when accessing data.
 *
 * Constraints:
 * - The template type `T` must be copyable and movable for deep copies and moves.
 * - The timestamp must be unique and sequential for proper packet ordering.
 **********************************/

#ifndef PACKET_H
#define PACKET_H

#include <iostream>
#include <string>
#include <sys/time.h>
#include "packetholder.h"
#include "packetexception.h"
#include <memory>

using namespace std;

/**********************************
 * @class Packet
 * @brief A generic class for managing timestamped packets of data.
 * @details The Packet class handles polymorphic storage for any type 
 *          and ensures proper ownership and timestamping.
 **********************************/
class Packet {
private:
    unique_ptr<PacketHolderBase> holder;  // Polymorphic storage for any type
    long long timestamp;                  // Timestamp in microseconds since epoch

public:
    /**********************************
     * Constructs a Packet with a unique pointer to a PacketHolder.
     * @tparam T The type of data managed by the PacketHolder.
     * @param packetHolder A unique pointer to a PacketHolder<T>.
     **********************************/
    template <typename T>
    explicit Packet(unique_ptr<PacketHolder<T>> packetHolder)
        : holder(std::move(packetHolder)), timestamp(currentTimestamp()) {}

    /**********************************
     * Default constructor initializes an empty Packet.
     **********************************/
    Packet() {
        holder = nullptr;
        timestamp = Packet::kInvalidTimestamp;
    }

    /**********************************
     * Constructs a Packet by creating a PacketHolder for the given value.
     * @tparam T The type of the provided value.
     * @param value The value to be stored in the Packet.
     **********************************/
    template <typename T>
    Packet(const T value) {
        holder = make_unique<PacketHolder<T>>(PacketHolder<T>(value));
        timestamp = currentTimestamp();
    }

    /**********************************
     * Move constructor transfers ownership of the holder and timestamp.
     * @param other The Packet to move from.
     **********************************/
    Packet(Packet&& other) {
        holder = std::move(other.holder);
        timestamp = other.timestamp;
        other.holder = nullptr;
        other.timestamp = Packet::kInvalidTimestamp;
    }

    /**********************************
     * Move assignment operator transfers ownership of the holder and timestamp.
     * @param other The Packet to move from.
     * @return A reference to the current object.
     **********************************/
    Packet& operator=(Packet&& other) {
        if (this != &other) {
            holder = std::move(other.holder);
            timestamp = other.timestamp;
            other.holder = nullptr;
            other.timestamp = Packet::kInvalidTimestamp;
        }
        return *this;
    }

    /**********************************
     * Overloaded output operator for printing the Packet's timestamp.
     * @param os The output stream.
     * @param packet The Packet to be printed.
     * @return The output stream with the Packet's details.
     **********************************/
    friend ostream& operator<<(ostream& os, const Packet& packet) {
        os << "\nPacket : {"
           << " timestamp: " << packet.timestamp << " }\n";
        return os;
    }

    /**********************************
     * Compares timestamps of two Packets for "greater than".
     * @param other The Packet to compare with.
     * @return True if this Packet's timestamp is greater.
     **********************************/
    bool operator>(const Packet& other) const {
        return (this->timestamp > other.timestamp);
    }

    /**********************************
     * Compares timestamps of two Packets for "less than".
     * @param other The Packet to compare with.
     * @return True if this Packet's timestamp is less.
     **********************************/
    bool operator<(const Packet& other) const {
        return this->timestamp < other.timestamp;
    }

    /**********************************
     * Compares timestamps of two Packets for equality.
     * @param other The Packet to compare with.
     * @return True if both Packets have the same timestamp.
     **********************************/
    bool operator==(const Packet& other) const {
        return this->timestamp == other.timestamp;
    }

    /**********************************
     * Checks if the Packet is valid (has a valid timestamp and data).
     * @return True if the Packet is valid.
     **********************************/
    bool isValid() const {
        return timestamp != Packet::kInvalidTimestamp && holder != nullptr;
    }

    /**********************************
     * Retrieves the data as a constant reference.
     * @tparam T The type of the data.
     * @return A constant reference to the data.
     * @throws PacketException if the data type does not match or the Packet is empty.
     **********************************/
    template <typename T>
    const T& get() const {
        if (!holder) {
            throw PacketException("get<T> Packet is empty");
        }
        PacketHolder<T>* typedHolder = dynamic_cast<PacketHolder<T>*>(holder.get());
        if (!typedHolder) {
            throw PacketException("get<T> Invalid T type access in Packet");
        }
        return typedHolder->get();
    }

    /**********************************
     * Retrieves the data as a mutable reference.
     * @tparam T The type of the data.
     * @return A mutable reference to the data.
     * @throws PacketException if the data type does not match or the Packet is empty.
     **********************************/
    template <typename T>
    T& get() {
        if (!holder) {
            throw PacketException("get<T> Packet is empty");
        }
        PacketHolder<T>* typedHolder = dynamic_cast<PacketHolder<T>*>(holder.get());
        if (!typedHolder) {
            throw PacketException("get<T> Invalid T type access in Packet");
        }
        return typedHolder->get();
    }

    /***********************************
     *  Returns the timestamp of the packet
     ***********************************/
    long long getTimestamp() const {
        return timestamp;
    }

private:
    /**********************************
     * Generates a unique, monotonic timestamp in microseconds.
     * This method ensures that timestamp is unique for each
     * packet. It is generated using a static lastTimestamp to 
     * record the last timestamp ensuring that timestamp is not
     * returned twice.
     * @return A unique timestamp.
     **********************************/
    long long currentTimestamp() {
        struct timespec ts;
        clock_gettime(CLOCK_MONOTONIC, &ts);
        long long current = static_cast<long long>(ts.tv_sec) * 10000000LL + ts.tv_nsec;
        if (current <= lastTimestamp) {
            current = lastTimestamp + 1;
        }
        lastTimestamp = current;
        return current;
    }

public:
    static long long lastTimestamp;                  // Tracks the last used timestamp
    static const long long kInvalidTimestamp = -11111111;  // Constant for invalid timestamp
};

#endif  // PACKET_H

