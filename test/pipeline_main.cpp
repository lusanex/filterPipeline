#include "TestFilterPipeLine.h"

long long Packet::lastTimestamp = 0;

int main() {
    TestFilterPipeline::run();
}
