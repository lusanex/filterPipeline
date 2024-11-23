#ifndef PORT_H
#define PORT_H

#include <queue>
#include <string>
#include "portexception.h"
#include "packet.h"


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
        queue<Packet> dataQueue; //Queue for holding packets
        long long latestTimestamp;  //Tracks the latest timestamp
        const size_t MAX_QUEUE_SIZE = 100;

    public:

        Port(size_t maxQueueSize = 100)
        : latestTimestamp(0), MAX_QUEUE_SIZE(maxQueueSize) {}

        Port(Port&& other)
            : dataQueue(std::move(other.dataQueue)),
            latestTimestamp(other.latestTimestamp){
                other.latestTimestamp = 0;

            }
             
        Port& operator=(Port&& other) {
            if(this != &other){
                dataQueue = std::move(other.dataQueue);
                latestTimestamp = other.latestTimestamp;
                other.latestTimestamp = 0;
            }
            return *this;
        }


        void write(const Packet& packet) {
            if(packet.getTimestamp() > latestTimestamp){
                if(dataQueue.size() >= MAX_QUEUE_SIZE){
                    dataQueue.pop();
                }
                dataQueue.push(packet);
                latestTimestamp = packet.getTimestamp();
            }
        }
        
        Packet read() {
            if(dataQueue.empty()){
                //Costum exception class
                throw PortException("Port is empty") ;
            }
            Packet packet = &dataQueue.front();
            dataQueue.pop();
            return packet;
        }

        size_t size() const {
            return dataQueue.size();
        }

       
};

#endif
