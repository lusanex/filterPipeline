#include <iostream>
#include <string>
#include "../src/packet.h"
#include "../src/packetholder.h"

int main() {
    try {
        // Create a PacketHolder<int>
        auto intHolder = std::make_shared<PacketHolder<int>>(42);

        // Create a Packet using the intHolder
        Packet intPacket(std::move(intHolder));

        // Access the stored value
        std::cout << "Stored integer: " << intPacket.get<int>() << std::endl;

        // Create a PacketHolder<std::string>
        auto stringHolder = std::make_shared<PacketHolder<std::string>>("Hello, World!");

        // Create a Packet using the stringHolder
        Packet stringPacket(std::move(stringHolder));

        // Access the stored value
        std::cout << "Stored string: " << stringPacket.get<std::string>() << std::endl;

        // Attempt to access the wrong type (throws exception)
        std::cout << "Attempting to access integer as string:" << std::endl;
        stringPacket.get<int>();  // This will throw an exception
    } catch (const PacketException& e) {
        std::cerr << "Caught exception: " << e.what() << std::endl;
    }

    return 0;
}

