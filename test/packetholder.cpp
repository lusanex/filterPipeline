#include "../src/packetholder.h"
#include <iostream>
using namespace std;

int main() {
    try {
        // Test with integer
        PacketHolder<int> intHolder(42);
        cout << "Stored integer: " << intHolder.get() << endl;

        // Test with string
        PacketHolder<string> stringHolder("Hello, World!");
        cout << "Stored string: " << stringHolder.get() << endl;

        // Test type validation
        try {
            cout << "Attempting to access integer as string:" << endl;
            stringHolder.get<int>();  // This should throw an exception
        } catch (const PacketException& e) {
            cout << "Caught exception: " << e.what() << endl;
        }

        // Test copy constructor
        PacketHolder<int> copiedHolder = intHolder;
        cout << "Copied integer: " << copiedHolder.get() << endl;

        // Test move constructor
        PacketHolder<string> movedHolder = std::move(stringHolder);
        cout << "Moved string: " << movedHolder.get() << endl;

        // Test move assignment
        PacketHolder<int> assignedHolder(100);
        assignedHolder = std::move(intHolder);
        cout << "Assigned integer after move: " << assignedHolder.get() << endl;

    } catch (const exception& e) {
        cerr << "Unhandled exception: " << e.what() << endl;
    }

    return 0;
}
