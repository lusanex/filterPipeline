#ifndef PORT_TEST_H
#define PORT_TEST_H

#include <iostream>
#include <cassert>
#include <vector>
#include "../src/port.h"
#include "../src/packet.h"

using namespace std;

class PortTest {
public:
    static void run() {
        cout << "Testing PortTest class" << endl;
        testPushAndReadIntegerPackets();
        testPushAndReadStringPackets();
        testDefaultConstructor();
        cout << "All Port tests passed successfully!" << endl;
    }

private:

    static void testDefaultConstructor(){
        Port port;
        assert(port.size() == 0);
        auto expectedValue = 12;
        port.write(std::move(Packet(expectedValue)));
        assert(port.size() == 1);
        assert(port.read().get<int>() == expectedValue );
        cout << "Test default constructor PASSED" << endl;
    }
    // Test pushing and reading integer packets
    static void testPushAndReadIntegerPackets() {
        Port port;


        // Push 25 packets containing integers
        for (int i = 1; i <= 25; i++) {
            Packet packet(i); // Packet with data i
            port.write(std::move(packet));
        }

        // Ensure the queue size is 25
        cout << "port size  " << port.size() << endl;
        assert(port.size() == 25);

        // Read and verify packets
        for (int i = 1; i <= 25; i++) {
            Packet packet = port.read();
            assert(packet.get<int>() == i); // Data should match what was pushed
        }

        // Ensure the queue is empty after reading
        assert(port.size() == 0);

        cout << "Push and Read Integer Packets test passed!" << endl;
    }

    // Test pushing and reading string packets
    static void testPushAndReadStringPackets() {
        Port port;

        // Array of test strings
        vector<string> testStrings = {
            "Alice", "Bob", "Charlie", "David", "Eve", "Frank", "Grace", "Hank",
            "Ivy", "Judy", "Kevin", "Laura", "Mona", "Nina", "Oscar", "Paul",
            "Quincy", "Rita", "Steve", "Tina", "Uma", "Victor", "Wendy", "Xander", "Yvonne"
        };

        // Push 25 packets containing strings
        for (const string& name : testStrings) {
            Packet packet(name);
            port.write(std::move(packet));
        }

        // Ensure the queue size is 25
        assert(port.size() == testStrings.size());

        // Read and verify packets
        for (const string& expectedName : testStrings) {
            Packet packet = port.read();
            assert(packet.get<string>() == expectedName); // Data should match what was pushed
        }

        // Ensure the queue is empty after reading
        assert(port.size() == 0);
        cout << "Push and Read String Packets test passed!" << endl;
    }
};

#endif // PORT_TEST_H

