#include "../src/packetholder.h"

int main() {
    try {
        // Test with integer
        PacketHolder<int> intHolder(42);
        std::cout << "Stored integer: " << intHolder.get() << std::endl;

        // Test with string
        PacketHolder<std::string> stringHolder("Hello, World!");
        std::cout << "Stored string: " << stringHolder.get() << std::endl;

        // Test type validation
        try {
            std::cout << "Attempting to access integer as string:" << std::endl;
            stringHolder.get<int>();  // This should throw an exception
        } catch (const PacketException& e) {
            std::cout << "Caught exception: " << e.what() << std::endl;
        }

        // Test copy constructor
        PacketHolder<int> copiedHolder = intHolder;
        std::cout << "Copied integer: " << copiedHolder.get() << std::endl;

        // Test move constructor
        PacketHolder<std::string> movedHolder = std::move(stringHolder);
        std::cout << "Moved string: " << movedHolder.get() << std::endl;

        // Test move assignment
        PacketHolder<int> assignedHolder(100);
        assignedHolder = std::move(intHolder);
        std::cout << "Assigned integer after move: " << assignedHolder.get() << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "Unhandled exception: " << e.what() << std::endl;
    }

    return 0;
}
