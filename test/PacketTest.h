#include "../src/packet.h"
#include <iostream>
#include <string>
#include <cassert> // For assertions
#include <vector>

using namespace std;

template <typename T>
long long Packet<T>::lastTimestamp = 0;

class PacketTest {
public:
    static void runAllTests() {
        testDefaultConstructor();
        testParameterizedConstructor();
        testCopyConstructor();
        testCopyAssignmentOperator();
        testMoveConstructor();
        testMoveAssignmentOperator();
        testTimestamp();
        testArrayOfPackets();
        cout << "All tests passed successfully!" << endl;
    }

private:
    static void testDefaultConstructor() {
        Packet<int> packet(0);
        assert(packet.getData() == 0); // Default constructed int is 0
        cout << "Default Constructor test passed!" << endl;
    }

    static void testParameterizedConstructor() {
        Packet<int> packet(42);
        assert(packet.getData() == 42); // Ensure data is set correctly
        cout << "Parameterized Constructor test passed!" << endl;
    }

    static void testCopyConstructor() {
        Packet<int> packet1(42);
        Packet<int> packet2(packet1); // Use copy constructor
        assert(packet2.getData() == 42);
        assert(packet2.getData() == packet1.getData());
        cout << "Copy Constructor test passed!" << endl;
    }

    static void testCopyAssignmentOperator() {
        Packet<int> packet1(42);
        Packet<int> packet2(0);
        packet2 = packet1; // Use copy assignment operator
        assert(packet2.getData() == 42);
        assert(packet2.getData() == packet1.getData());
        cout << "Copy Assignment Operator test passed!" << endl;
    }

    static void testMoveConstructor() {
        Packet<int> packet1(42);
        Packet<int> packet2(std::move(packet1)); // Use move constructor
        assert(packet2.getData() == 42);
        cout << "Move Constructor test passed!" << endl;
    }

    static void testMoveAssignmentOperator() {
        Packet<int> packet1(42);
        Packet<int> packet2(0);
        packet2 = std::move(packet1); // Use move assignment operator
        assert(packet2.getData() == 42);
        cout << "Move Assignment Operator test passed!" << endl;
    }

    static void testTimestamp() {
        Packet<int> packet1(42);
        Packet<int> packet2(42);
        assert(packet1.getTimestamp() != packet2.getTimestamp());
        cout << "Timestamp test passed!" << endl;
    }

    static void testArrayOfPackets() {
        vector<Packet<int>> packets;

        // Create 25 packets with increasing integer values
        for (int i = 1; i <= 25; i++) {
            Packet<int> packet(i);
            packets.push_back(packet); // Add packet to the vector
        }

        // Assert that timestamps are in strictly increasing order
        for (size_t i = 1; i < packets.size(); i++) {
            assert(packets[i].getTimestamp() > packets[i - 1].getTimestamp()); // Ensure increasing timestamps
        }

        /**
        for (const auto& packet : packets) {
            cout << packet << endl;
        }
        **/

        cout << "Array of Packets test passed!" << endl;
    }
};
