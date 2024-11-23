/**
 * @file PacketTest.h
 * @author Erich Gutierrez
 * @brief A test suite for the Packet class.
 *
 * This class tests the functionality of the Packet class, including:
 * - Creation and validation of valid and invalid packets.
 * - Accessing data stored in packets using the `get<T>()` method.
 * - Ensuring type safety and exception handling for incorrect type access.
 * - Validating move semantics to confirm proper ownership transfer of packet data.
 * - Verifying the timestamp functionality to ensure packets have unique, ordered timestamps.
 *
 * Each test case demonstrates a specific feature of the Packet class with output indicating whether the test passed or failed.
 */

#ifndef PACKETTEST_H
#define PACKETTEST_H

#include <iostream>
#include <memory>
#include <cassert>
#include "../src/packet.h"  

using namespace std;



class PacketTest {
public:
    /**
     * @brief Tests the packet move constructor.
     *
     */
    static void testPacketCopyContructor() {
        double  expectedValue = 12.0;
        Packet p1 = Packet(expectedValue);
        Packet p2 = Packet(std::move(p1));
        assert(!p1.isValid());
        assert(p2.isValid());
        cout << "p1 timestamp: " << p1  <<endl;
        cout << "p2 timestamp: " << p2 << endl;
        cout << "Packet Copy Constructor PASS " << endl;
    }
 
    /**
     * @brief Tests the creation and validity of Packet instances.
     *
     * This test verifies:
     * - A packet created with data is valid.
     * - A default-constructed packet is invalid.
     */
    static void testCreation() {
        cout << "Testing Packet creation and isValid()...\n";

        // Create a valid packet
        Packet validPacket(42.0);
        assert(validPacket.isValid() == true);
        cout << "Valid packet created: " << (validPacket.isValid() ? "PASS" : "FAIL") << "\n";

        // Create an invalid packet
        Packet invalidPacket;
        assert(invalidPacket.isValid() == false);
        cout << "Invalid packet created: " << (!invalidPacket.isValid() ? "PASS" : "FAIL") << "\n";
    }

    /**
     * @brief Tests the `get<T>()` method of the Packet class.
     *
     * This test verifies:
     * - Correct retrieval of data from the packet.
     * - Exception handling for incorrect type access.
     */
    static void testGet() {
        cout << "\nTesting Packet get<T>()...\n";

        try {
            auto expectedValue = 42.0;
            Packet packet(expectedValue);  // Packet with a double
            double value = packet.get<double>();
            cout << "Retrieved double value: " << value << " (PASS)\n";
            assert(value == expectedValue);

            // Attempt to retrieve with wrong type
            packet.get<int>();
        } catch (const PacketException& e) {
            cout << "Type mismatch test: PASS (exception: " << e.what() << ")\n";
        }
    }

    /**
     * @brief Tests move semantics of the Packet class.
     *
     * This test verifies:
     * - A packet becomes invalid after being moved.
     * - The moved-to packet retains validity and correct data.
     */
    static void testMoveSemantics() {
        cout << "\nTesting Packet move semantics...\n";
        auto expectedValue = 99.99;

        Packet packet1(expectedValue);  // Original packet
        Packet packet2 = std::move(packet1);  // Move to another packet

        cout << "Original packet isValid after move: " << (!packet1.isValid() ? "PASS" : "FAIL") << "\n";
        cout << "Moved packet isValid: " << (packet2.isValid() ? "PASS" : "FAIL") << "\n";
    }

    /**
     * @brief Tests the timestamp functionality of the Packet class.
     *
     * This test verifies:
     * - Packets have valid timestamps.
     * - Timestamps are unique and ordered for packets created sequentially.
     */
    static void testTimestamp() {
        cout << "\nTesting Packet timestamp...\n";

        Packet packet1(42);
        Packet packet2(43);

        long long ts1 = packet1.getTimestamp();
        long long ts2 = packet2.getTimestamp();

        cout << "Timestamp of packet1: " << ts1 << "\n";
        cout << "Timestamp of packet2: " << ts2 << "\n";

        cout << "Timestamp comparison: " << (ts2 > ts1 ? "PASS" : "FAIL") << "\n";
    }

    /**
     * @brief Runs all test cases for the Packet class.
     *
     * This method sequentially calls each test case and outputs the results.
     */
    static void run() {
        cout << "Starting Packet tests...\n";
        testPacketCopyContructor();
        testCreation();
        testGet();
        testMoveSemantics();
        testTimestamp();
        cout << "\nAll Packet tests completed.\n";
    }
};

#endif // PACKETTEST_H

