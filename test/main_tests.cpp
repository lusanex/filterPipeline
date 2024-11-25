
#include "PortTest.h"
#include "PacketTest.h"
#include "CalculatorContextTest.h"
#include "SchedulerTest.h"

long long Packet::lastTimestamp = 0;
int main() {
    PacketTest::run();
    PortTest::run();
    CalculatorContextTest::run();
    SchedulerTest::run();
    //TypeIdTest::run();
    return 0;
}

