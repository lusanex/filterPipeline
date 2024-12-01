#ifndef PORT_H
#define PORT_H

#include <queue>
#include <string>
#include "packet.h"
#include "portexception.h"




using namespace std;

/*
 * Port.h - A Port class for managing Packet transmission
 * Author: Erich Gutierrez Chavez
 * Project: Packet and Port Management
 * Description:
 * - The Port class works with Packet<T> objects.
 * - Allows writing, reading, and managing Packet objects in a queue.
 * - Ensures that the queue only accepts Packets with increasing timestamps.
 * - Includes a configurable MAX_SIZE to limit the size of the queue.
 */

class Port {
private:
    deque<Packet> dataQueue;  // Store Packet objects directly
    long long latestTimestamp;
    const size_t MAX_QUEUE_SIZE = 100;

public:

    Port(size_t maxQueueSize = 100)
        : dataQueue(deque<Packet>()), latestTimestamp(0), MAX_QUEUE_SIZE(maxQueueSize) {}



    // Move constructor
    Port(Port&& other)
        : dataQueue(std::move(other.dataQueue)),
          latestTimestamp(other.latestTimestamp) {
        other.latestTimestamp = 0;
    }

    // Move assignment operator
    Port& operator=(Port&& other) {
        if (this != &other) {
            dataQueue = std::move(other.dataQueue);
            latestTimestamp = other.latestTimestamp;
            other.latestTimestamp = 0;
        }
        return *this;
    }

    // Write a packet to the queue
    void write(Packet&& packet) {
        if (packet.getTimestamp() > latestTimestamp) {
            if (dataQueue.size() >= MAX_QUEUE_SIZE) {
                dataQueue.pop_front();  // Remove the oldest packet
            }
            latestTimestamp = packet.getTimestamp();
            dataQueue.push_back(std::move(packet));  // Move the packet into the queue
        }
    }

    // Read a packet from the queue
    Packet read() {
        if (dataQueue.empty()) {
            //throw PortException("Port read: Queue is empty");
            return Packet();
        }
        Packet packet = std::move(dataQueue.front());  // Move the packet out
        dataQueue.pop_front();
        return packet;
    }


    bool operator==(const Port& other){
        if (dataQueue == other.dataQueue) return true;
        if (dataQueue.size() != other.dataQueue.size()) return false;
        return false;
    }
        

    // Get the size of the queue
    size_t size() const {
        return dataQueue.size();
    }
};

#endif
