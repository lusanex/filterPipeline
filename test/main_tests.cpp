
#include "PortTest.h"
#include "PacketTest.h"
#include "CalculatorContextTest.h"

long long Packet::lastTimestamp = 0;
int main() {
    PacketTest::run();
    PortTest::run();
    CalculatorContextTest::run();
    //TypeIdTest::run();
    return 0;
}

