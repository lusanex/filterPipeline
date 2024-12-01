/**********************************
 * @file port.h
 * @author Erich Gutierrez Chavez
 * @brief Defines the Port class for managing 
 *  Packet transmission in a queue.
 *
 * @details
 * - The Port class works with Packet objects.
 * - Provides functionality to write, read, and manage Packets in a queue.
 * - Ensures that only Packets with increasing timestamps are added to the queue.
 * - Includes a configurable `MAX_QUEUE_SIZE` to limit the number of Packets stored.
 **********************************/

#ifndef PORT_H
#define PORT_H

#include <queue>
#include <string>
#include "packet.h"
#include "portexception.h"

using namespace std;

/**********************************
 * @class Port
 * @brief A class for managing the transmission of Packet objects in a queue.
 **********************************/
class Port {
private:
    deque<Packet> dataQueue;                /* Queue to store Packet objects */
    long long latestTimestamp;              /* Tracks the latest timestamp in the queue */
    const size_t MAX_QUEUE_SIZE = 100;      /* Maximum size of the queue */

public:
    /**********************************
     * Constructor for initializing the Port with a specified maximum queue size.
     * @param maxQueueSize The maximum number of Packets the queue can hold.
     **********************************/
    Port(size_t maxQueueSize = 100)
        : dataQueue(deque<Packet>()), latestTimestamp(0), MAX_QUEUE_SIZE(maxQueueSize) {}

    /**********************************
     * Move constructor.
     * Transfers ownership of the queue and timestamp from another Port.
     * @param other The Port to move from.
     **********************************/
    Port(Port&& other)
        : dataQueue(std::move(other.dataQueue)),
          latestTimestamp(other.latestTimestamp) {
        other.latestTimestamp = Packet::kInvalidTimestamp;
    }

    /**********************************
     * Move assignment operator.
     * Transfers ownership of the queue and timestamp from another Port.
     * @param other The Port to move from.
     * @return A reference to the current object.
     **********************************/
    Port& operator=(Port&& other) {
        if (this != &other) {
            dataQueue = std::move(other.dataQueue);
            latestTimestamp = other.latestTimestamp;
            other.latestTimestamp = Packet::kInvalidTimestamp;
        }
        return *this;
    }

    /**********************************
     * Writes a Packet to the queue if it has a newer timestamp than the latest.
     * Removes the oldest Packet if the queue exceeds the maximum size.
     * @param packet The Packet to write to the queue.
     **********************************/
    void write(Packet&& packet) {
        if (packet.getTimestamp() > latestTimestamp) {
            if (dataQueue.size() >= MAX_QUEUE_SIZE) {
                dataQueue.pop_front();  /* Remove the oldest Packet */
            }
            latestTimestamp = packet.getTimestamp();
            dataQueue.push_back(std::move(packet));  /* Move the Packet into the queue */
        }
    }

    /**********************************
     * Reads and removes a Packet from the front of the queue.
     * @return If the queue is empty returns a invalid and 
     * empty packet otherwise returns the packet read from the
     * queue.
     **********************************/
    Packet read() {
        if (dataQueue.empty()) {
            return Packet();
        }
        Packet packet = std::move(dataQueue.front());  /* Move the Packet out */
        dataQueue.pop_front();
        return packet;
    }

    /**********************************
     * Compares two Ports for equality based on their data queues direction.
     * @param other The Port to compare with.
     * @return True if the queues are identical, false otherwise.
     **********************************/
    bool operator==(const Port& other) {
        if (dataQueue == other.dataQueue) return true;
        if (dataQueue.size() != other.dataQueue.size()) return false;
        return false;
    }

    /**********************************
     * Retrieves the size of the queue.
     * @return The number of Packets in the queue.
     **********************************/
    size_t size() const {
        return dataQueue.size();
    }
};

#endif  // PORT_H

