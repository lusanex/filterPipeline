#ifndef TYPE_ID_H
#define TYPE_ID_H

#include <string>
#include <iostream>

using namespace std;


class TypeId {
public:
    template <typename T>
    static TypeId Of() {
        static const char uniqueTag = 0; // Unique tag per type.
        return TypeId(&uniqueTag);
    }

    bool operator==(const TypeId& other) const { return id == other.id; }
    bool operator!=(const TypeId& other) const { return id != other.id; }

    string getId() const{
        return to_string(reinterpret_cast<uintptr_t>(id));
    }


private:
    explicit TypeId(const void* id) : id(id) {}
    const void* id;
};


#endif // TYPE_ID_H



