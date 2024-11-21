#include "../src/typeid.h"
#include <cassert> // For assertions
using namespace std;


class TypeIdTest{

    public:
        static void runAllTests(){
            testTypeId();
            cout << "All TypeId test passed" << endl;
        }
    private:
        static void testTypeId() {
            TypeId intTypeId = TypeId::Of<int>();
            TypeId stringTypeId = TypeId::Of<std::string>();
            assert(intTypeId == TypeId::Of<int>());
            assert(intTypeId != stringTypeId);
            cout << "TypeId Test passed" << endl;
        }

};

