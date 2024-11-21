#include "PacketTest.h"
#include "PortTest.h"
#include "TypeIdTest.h"
int main() {
    PacketTest::runAllTests();
    PortTest::runAllTests();
    TypeIdTest::runAllTests();
    return 0;
}

