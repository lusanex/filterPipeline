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
#include <sys/time.h> 
#include "packetholder.h"
#include "packetexception.h"

using namespace std;


#include <memory>
#include "packetholder.h"  // Include your PacketHolder classes

class Packet {
private:
    unique_ptr<PacketHolderBase> holder;  // Polymorphic storage for any type
    long long timestamp;     // Timestamp in microseconds since epoch

public:
    // Constructor accepting PacketHolder<T>
    template <typename T>
    explicit Packet(unique_ptr<PacketHolder<T>> packetHolder)
    : holder(std::move(packetHolder)) , timestamp(currentTimestamp()){

    }

    Packet(){
        holder = nullptr;
        timestamp = Packet::kInvalidTimestamp;
    }

    template <typename T>
    Packet(const T value){
        holder = make_unique<PacketHolder<T>>(PacketHolder(value));
        timestamp  = currentTimestamp();
    }

    Packet(Packet&& other){
        holder = std::move(other.holder);
        timestamp = other.timestamp;
        other.holder = nullptr;
        other.timestamp = Packet::kInvalidTimestamp;

    }


    Packet& operator=(Packet&& other){
        if ( this != &other ){
            holder = std::move(other.holder);
            timestamp = other.timestamp;
            other.holder = nullptr;
            other.timestamp = Packet::kInvalidTimestamp;
        }
        return *this;
    }

    friend ostream& operator<<(ostream& os, const Packet& packet) {
        os << "\nPacket : {"
           << " timestamp: " << packet.timestamp << " }\n";
        return os;
    }

    //this methods compares timestamp packet not data
    //should be uses to compare datatimstamp of the packets
    bool operator>(const Packet& other) const {
        return (this->timestamp > other.timestamp);
    }

    bool operator<(const Packet& other) const{
        return this->timestamp < other.timestamp;
    }

    bool operator==(const Packet& other) const {
        return this->timestamp == other.timestamp;
    }



    bool isValid() const {
        //cout << "timestap  " << timestamp  << endl;
        //cout << "holder  " << holder << endl;
        return timestamp != Packet::kInvalidTimestamp && holder != nullptr;
    }


    // Templated get<T> method to access data
    template <typename T>
    const T& get() {
        if(!holder){
            throw PacketException("Packet is empty");
        }
        // Downcast the base holder to the concrete type
        auto typedHolder = dynamic_cast<PacketHolder<T>*>(holder.get());
        
        if (!typedHolder) {
            throw PacketException("Invalid type access in Packet");
        }
        return typedHolder->get();
    }

    long long getTimestamp() const {
        return timestamp;
    }



public:
    static long long lastTimestamp;
    static const long long kInvalidTimestamp = -11111111;

    long long currentTimestamp() {
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

#endif  // PACKET_H


